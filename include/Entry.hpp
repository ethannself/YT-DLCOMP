#pragma once
#include <filesystem>
#include <string>
const int COLUMN_COUNT = 4; // user, link, timestamp, file location
struct Entry {
  std::string user;
  std::string link;
  std::string timestamp;
  std::filesystem::path path;
};