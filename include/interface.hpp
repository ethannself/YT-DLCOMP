#pragma once

#include <optional>
#include <string>
#include <vector>

struct Entry {
  std::string user;
  std::string link;
  std::string timestamp;
};

std::string executeYtDLPCommand(const char *cmd);
std::optional<std::vector<Entry>> getResponses(std::string spreadsheetId);