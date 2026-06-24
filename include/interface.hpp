#pragma once

#include <string>
#include <vector>

struct Entry {
  std::string link;
  std::string timestamp;
};
extern std::vector<Entry> entries;

std::string executeYtDLPCommand(const char *cmd);
void request_responses(std::string spreadsheetId);