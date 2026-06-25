#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

struct Entry {
  std::string user;
  std::string link;
  std::string timestamp;
};

std::string executeYtDLPCommand(const char *cmd);
std::string buildYtDlpCommand(const Entry &entry,
                              const std::filesystem::path &destPath);
std::optional<std::vector<Entry>> getResponses(std::string spreadsheetId,
                                               std::string apiKey);