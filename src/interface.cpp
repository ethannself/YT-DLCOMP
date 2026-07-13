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
    throw std::runtime_error("popen() failed!");

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
  int minutes = 0, seconds = 0;
  if (sscanf_s(entry.timestamp.c_str(), "%d:%d", &minutes, &seconds) != 2) {
    throw std::runtime_error("Invalid timestamp");
  }
  int startSeconds = (minutes * 60 + seconds);
  int endSeconds = startSeconds + 10;
  return std::format("yt-dlp --newline "
                     "-f "
                     "\"bestvideo[height<=1080][ext=mp4]+bestaudio[ext=m4a]/"
                     "bestvideo[height<=1080]+bestaudio/best[height<=1080]\" "
                     "-o \"{}/{}.%(ext)s\" "
                     "--postprocessor-args \"ffmpeg:-ss {} -t 10 -c copy\" "
                     "\"{}\" 2>&1",
                     destPath.string(), index, startSeconds, entry.link);
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
          throw std::runtime_error("Invalid row!");
        }
        // TODO: input validation on rows

        if (isValidYoutubeURL(row[2])) {
          Entry entry = Entry{row[1], row[2], row[3]};
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
      throw std::runtime_error("Internal Server Error");
    } else {
      std::cerr << "HTTP error " << response.status_code << ": "
                << response.text << std::endl;
      throw std::runtime_error("Invalid Sheets Link!");
    }
  } else {
    std::cerr << "apiKey environment variable not set!" << std::endl;
    throw std::runtime_error("Invalid API Key!");
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

  return std::system(command.c_str());
}

std::string buildEntryLabelCommand(const Entry &entry) {
  const std::filesystem::path &destPath = wxGetApp().destPath;

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
    throw std::runtime_error("Entry is missing required field(s)");
  }

  // using a textfile for ffmpeg text overlay handles any escape characters in
  // the text label automatically
  std::filesystem::path tempDir = entry.path.parent_path() / "temp";
  std::filesystem::path outDir = entry.path.parent_path() / "out";
  std::filesystem::create_directories(tempDir);
  std::filesystem::create_directories(outDir);
  std::filesystem::path outPath;
  outPath = outDir / entry.path.filename();
  // TODO: capture song name
  std::string songLine = "SONGNAME";
  std::string submitterLine = "Submitted by " + entry.user;

  std::filesystem::path songTextPath =
      tempDir / ("song_" + entry.path.stem().string() + ".txt");
  std::filesystem::path submitterTextPath =
      tempDir / ("submitter_" + entry.path.stem().string() + ".txt");
  auto writeTextFile = [](const std::filesystem::path &p,
                          const std::string &content) {
    std::ofstream txt(p, std::ios::binary);
    if (!txt) {
      throw std::runtime_error("Failed to write label file: " + p.string());
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