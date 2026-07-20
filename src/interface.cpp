#include "interface.hpp"
#include "MyApp.hpp"

#include <array>
#include <cpr/api.h>
#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <regex>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <wx/event.h>
#include <wx/stdpaths.h>
const auto EXECUTABLE_PATH =
    std::filesystem::path{
        wxStandardPaths::Get().GetExecutablePath().ToStdString()}
        .parent_path();
const auto TEMP_DIR = EXECUTABLE_PATH / "temp";
// Layout: destPath/raw (downloads), destPath/labelled (labelled clips),
// destPath/final.mp4 (combined output)
const std::string FILE_EXTENSION = "mp4";
const std::string FINAL_FILE_NAME = std::format("final.{}", FILE_EXTENSION);

wxDEFINE_EVENT(EVT_DOWNLOAD_PROGRESS, wxThreadEvent);

static bool isValidYoutubeURL(const std::string &url) {
  static const std::regex youtube_regex(
      R"(^(?:https?:\/\/)?(?:www\.)?(?:(?:youtube\.com\/(?:watch\?v=|embed\/|v\/|shorts\/))|(?:youtu\.be\/))([a-zA-Z0-9_-]{11})(?:[&?].*)?$)");
  return std::regex_match(url.begin(), url.end(), youtube_regex);
}
std::optional<std::string> getFileExtension(const std::string &line) {
  static const std::regex fileRegex(R"(\b[\w.-]+\.([A-Za-z0-9]+)\b)");

  std::smatch match;
  if (std::regex_search(line, match, fileRegex))
    return match[1].str();

  return std::nullopt;
}

std::string executeYtDLPCommand(
    const char *cmd, std::string_view filename,
    std::function<void(float, std::string)> onProgress = nullptr) {

  std::array<char, 256> buffer;
  std::string result;
  std::string currentExt;

#if defined(_WIN32)
  std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
#else
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
#endif

  if (!pipe)
    throw std::runtime_error("[executeYtDLP] popen() failed!");

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    std::cout << buffer.data();

    result += buffer.data();
    if (onProgress) {
      std::string line(buffer.data());

      if (line.find("Destination:") != std::string::npos ||
          line.find("Merging formats into") != std::string::npos) {
        auto res = getFileExtension(line);
        if (res.has_value()) {
          currentExt = res.value();
        }
      }
      float pct = -1.f;
      if (line.find("[download]") != std::string::npos &&
          sscanf(line.c_str(), " [download] %f%%", &pct) == 1) {
        onProgress(pct, currentExt);
      } else if (line.starts_with("[download]") &&
                 line.find("has already been downloaded") !=
                     std::string::npos) {
        auto res = getFileExtension(line);
        if (res.has_value()) {
          currentExt = res.value();
        }
        onProgress(100.0, currentExt);
      }
    }
  }
  return result;
}
std::string buildYtDlpCommand(size_t index, const Entry &entry,
                              const std::filesystem::path &destPath) {
  auto rawFilePath = destPath / "raw";
  std::filesystem::create_directories(rawFilePath);
  int minutes = 0, seconds = 0;
  if (sscanf_s(entry.timestamp.c_str(), "%d:%d", &minutes, &seconds) != 2) {
    throw std::runtime_error("[buildYtDLP] Invalid timestamp");
  }
  int startSeconds = (minutes * 60 + seconds);
  int endSeconds = startSeconds + 10;
  return std::format(
      "yt-dlp --newline "
      "-f "
      "\"bestvideo[height<=1080][ext=mp4]+bestaudio[ext=m4a]/"
      "bestvideo[height<=1080]+bestaudio/best[height<=1080]\" "
      "-o \"{}/{}.%(ext)s\" "
      "--postprocessor-args \"ffmpeg:-ss {} -t 10 -c:v libx264 -c:a aac\" "
      "\"{}\" 2>&1",
      rawFilePath.string(), index, startSeconds, entry.link);
}
std::string sanitizeYoutubeURL(std::string_view url) {
  std::string clean_url;
  if (url.find("youtu.be/") != std::string_view::npos) {
    size_t id_pos = url.find("youtu.be/") + 9;

    size_t query_pos = url.find_first_of("?&", id_pos);
    std::string video_id =
        (query_pos == std::string_view::npos)
            ? std::string(url.substr(id_pos))
            : std::string(url.substr(id_pos, query_pos - id_pos));
    clean_url = "https://www.youtube.com/watch?v=" + video_id;
  } else {
    clean_url = url;
  }
  size_t param_pos = clean_url.find_first_of("?&");
  if (param_pos != std::string::npos) {
    std::string base = clean_url.substr(0, param_pos);
    std::string query = clean_url.substr(param_pos + 1);

    if (query.find("v=") == 0 || query.find("v=") == 1) {
      size_t next_param = query.find('&');
      if (next_param != std::string::npos) {
        base += "?" + query.substr(0, next_param);
      } else {
        base += "?" + query;
      }
    }
    clean_url = base;
  }

  return clean_url;
}
std::optional<std::vector<Entry>> getResponses(std::string spreadsheetId,
                                               std::string apiKey) {
  size_t size = 0;
  std::vector<Entry> entries;
  static const std::string range = "Form%20Responses%201!A:Z";
  if (!apiKey.empty()) {

    auto response = cpr::Get(cpr::Url{std::format(
        "https://sheets.googleapis.com/v4/spreadsheets/{}/values/{}?key={}",
        spreadsheetId, range, apiKey)});

    if (response.status_code == 200) {
      // process json here
      auto json = nlohmann::json::parse(response.text);

      auto rows = json["values"];
      // first row is headers so start at index 1
      for (size_t i = 1; i < rows.size(); ++i) {
        auto row = rows[i];
        if (row.size() < 4) {
          throw std::runtime_error("[getResponses] Invalid row!");
        }
        // TODO: input validation on rows
        std::string link = row[2];

        if (isValidYoutubeURL(link)) {
          Entry entry = Entry{row[1], sanitizeYoutubeURL(link), row[3]};
          std::cout << "Username: " << entry.user << " Link: " << entry.link
                    << " Timestamp: " << entry.timestamp << std::endl;
          entries.emplace_back(entry);
        }
      }

      return entries;
    } else if (response.status_code == 0) {
      // status 0 means the request never got through (no internet, bad URL,
      // etc.)
      std::cerr << "Request failed: " << response.error.message << std::endl;
      throw std::runtime_error("[getResponses] Internal Server Error");
    } else {
      std::cerr << "HTTP error " << response.status_code << ": "
                << response.text << std::endl;
      throw std::runtime_error("[getResponses] Invalid Sheets Link!");
    }
  } else {
    std::cerr << "apiKey environment variable not set!" << std::endl;
    throw std::runtime_error("[getResponses] Invalid API Key!");
  }
  return std::nullopt;
}
std::string escapeFilterPath(std::string_view path) {
  std::string out;
  for (char c : path) {
    if (c == ':' || c == '\\' || c == '\'')
      out += '\\';
    out += c;
  }
  return out;
}
int executeffmpegCommand(const std::string &command) {

  const int status = std::system(command.c_str());

  if (status == -1) {
    throw std::runtime_error("[executeffmpeg] Failed to launch ffmpeg.");
  }

  return status;
}
static std::string buildEntryLabelCommand(const Entry &entry) {
  // const std::filesystem::path &destPath = wxGetApp().settings.getDestPath();

  // std::format: (original_filepath, song_txtfile, submitter_txtfile,
  // output_filepath)
  static constexpr std::string_view templateCommand =
      "ffmpeg -y -i \"{}\" -vf "
      "\"scale=1920:1080:force_original_aspect_ratio=decrease,pad=1920:1080:("
      "ow-iw)/2:(oh-ih)/2,fps=30,"
      "drawtext=textfile='{}':fontsize=32:fontcolor=white:"
      "x=(w-text_w)/2:y=h-2*th-62:box=1:boxcolor=black@0.5:boxborderw=8,"
      "drawtext=textfile='{}':fontsize=28:fontcolor=white:"
      "x=(w-text_w)/2:y=h-th-40:box=1:boxcolor=black@0.5:boxborderw=8\" "
      "-c:v libx264 -preset fast -crf 20 -c:a aac -ar 48000 -ac 2 -b:a 192k "
      "\"{}\"";
  if (entry.link.empty() || entry.path.empty() || entry.timestamp.empty() ||
      entry.user.empty()) {
    throw std::runtime_error(
        "[buildEntryLabelCommand] Entry is missing required field(s)");
  }

  // using a textfile for ffmpeg text overlay handles any escape characters in
  // the text label automatically
  auto outDir = wxGetApp().settings.getDestPath() / "labelled";
  std::filesystem::create_directories(TEMP_DIR);
  std::filesystem::create_directories(outDir);
  std::filesystem::path outPath;
  outPath = outDir / entry.path.filename();
  // TODO: capture song name
  std::string songLine = "SONGNAME";
  std::string submitterLine = "Submitted by " + entry.user;

  std::filesystem::path songTextPath =
      TEMP_DIR / ("song_" + entry.path.stem().string() + ".txt");
  std::filesystem::path submitterTextPath =
      TEMP_DIR / ("submitter_" + entry.path.stem().string() + ".txt");
  auto writeTextFile = [](const std::filesystem::path &p,
                          const std::string &content) {
    std::ofstream txt(p, std::ios::binary);
    if (!txt) {
      throw std::runtime_error(
          "[buildEntryLabelCommand] Failed to write label file: " + p.string());
    }
    txt << content;
  };
  writeTextFile(songTextPath, songLine);
  writeTextFile(submitterTextPath, submitterLine);
  // handle escape characters in filepath
  std::string escapedSongFile = escapeFilterPath(songTextPath.string());
  std::string escapedSubmitterFile =
      escapeFilterPath(submitterTextPath.string());

  std::string cmd =
      std::format(templateCommand, entry.path.string(), escapedSongFile,
                  escapedSubmitterFile, outPath.string());
  return cmd;
}
void AddEntryLabels(const std::vector<Entry> &entries) {
  for (const Entry &e : entries) {
    try {
      std::string command = buildEntryLabelCommand(e);
      const int status = executeffmpegCommand(command);
      if (status != 0) {
        std::cerr << std::format(
                         "[AddEntryLabels] ffmpeg exited with status {}\n",
                         status)
                  << std::endl;
      }

    } catch (std::runtime_error &err) {
      std::cerr << err.what() << std::endl;
    }
  }
}
void CombineEntries(const std::vector<Entry> &entries) {
  if (entries.empty())
    return;
  const auto destPath = wxGetApp().settings.getDestPath();
  const auto labelledDir = destPath / "labelled";
  const auto listFile = TEMP_DIR / "concat.txt";
  const auto output = destPath / FINAL_FILE_NAME;

  if (entries.size() == 1) {
    const auto video = labelledDir / entries[0].path.filename();
    std::filesystem::copy_file(
        video, output, std::filesystem::copy_options::overwrite_existing);
    return;
  }

  std::ofstream list(listFile);
  if (!list) {
    throw std::runtime_error("[CombineEntries] Failed to write concat file: " +
                             listFile.string());
  }
  for (const Entry &entry : entries) {
    const auto video = labelledDir / entry.path.filename();
    list << "file '" << video.string() << "'\n";
  }
  list.close();

  const std::string command =
      std::format("ffmpeg -y -f concat -safe 0 -i \"{}\" -c copy \"{}\"",
                  listFile.string(), output.string());
  const int status = std::system(command.c_str());
  if (status == -1) {
    throw std::runtime_error("[CombineEntries] ffmpeg failed to initialize");
  } else if (status != 0) {
    std::cerr << std::format("[CombineEntries] ffmpeg exited with status {}\n",
                             status)
              << std::endl;
  }
}
void cleanup() {
  auto &settings = wxGetApp().settings;
  const auto destPath = settings.getDestPath();

  std::filesystem::remove_all(TEMP_DIR);

  if (!settings.keepOriginal) {
    // the raw videos
    if (std::filesystem::exists(destPath / "raw")) {
      std::filesystem::remove_all(destPath / "raw");
    }
    // videos with submission labels rendered
    if (std::filesystem::exists(destPath / "labelled")) {
      std::filesystem::remove_all(destPath / "labelled");
    }
  }
}