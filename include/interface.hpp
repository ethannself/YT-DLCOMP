#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <wx/event.h>
#include <wx/thread.h>

struct Entry {
  std::string user;
  std::string link;
  std::string timestamp;
};

std::string
executeYtDLPCommand(const char *cmd,
                    std::function<void(float, std::string)> onProgress);
std::string buildYtDlpCommand(size_t index, const Entry &entry,
                              const std::filesystem::path &destPath);
std::optional<std::vector<Entry>> getResponses(std::string spreadsheetId,
                                               std::string apiKey);
wxDECLARE_EVENT(EVT_DOWNLOAD_PROGRESS, wxThreadEvent);