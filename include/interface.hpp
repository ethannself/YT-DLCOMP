#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <wx/event.h>
#include <wx/thread.h>

const int COLUMN_COUNT = 4; // user, link, timestamp, file location
struct Entry {
  std::string user;
  std::string link;
  std::string timestamp;
};

std::string
executeYtDLPCommand(const char *cmd, std::string_view filename,
                    std::function<void(float, std::string)> onProgress);
std::string buildYtDlpCommand(size_t index, const Entry &entry,
                              const std::filesystem::path &destPath);
std::optional<std::vector<Entry>> getResponses(std::string spreadsheetId,
                                               std::string apiKey);
wxDECLARE_EVENT(EVT_DOWNLOAD_PROGRESS, wxThreadEvent);