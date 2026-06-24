#include "interface.hpp"

#include <array>
#include <cpr/api.h>
#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

std::string executeYtDLPCommand(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;

#if defined(_WIN32)
  std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
#else
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
#endif

  if (!pipe)
    throw std::runtime_error("popen() failed!");

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}
std::optional<std::vector<Entry>> getResponses(std::string spreadsheetId) {
  // TODO: entering api key in client.
  char *apiKey = nullptr;
  size_t size = 0;
  std::vector<Entry> entries;
  std::string range = "Form%20Responses%201!A:Z";
  if (_dupenv_s(&apiKey, &size, "apiKey") == 0 && apiKey != nullptr) {

    auto response = cpr::Get(cpr::Url{std::format(
        "https://sheets.googleapis.com/v4/spreadsheets/{}/values/{}?key={}",
        spreadsheetId, range, apiKey)});

    free(apiKey);
    if (response.status_code == 200) {
      // process json here
      auto json = nlohmann::json::parse(response.text);

      auto rows = json["values"];
      // first row is headers so start at index 1
      for (size_t i = 1; i < rows.size(); ++i) {
        auto row = rows[i];
        Entry entry = Entry{row[1], row[2], row[3]};
        std::cout << "Username: " << entry.user << " Link: " << entry.link
                  << " Timestamp: " << entry.timestamp << std::endl;
        entries.push_back(entry);
      }

      return entries;
    } else if (response.status_code == 0) {
      // status 0 means the request never got through (no internet, bad URL,
      // etc.)
      std::cerr << "Request failed: " << response.error.message << std::endl;
    } else {
      std::cerr << "HTTP error " << response.status_code << ": "
                << response.text << std::endl;
    }
  } else {
    std::cerr << "apiKey environment variable not set!" << std::endl;
  }
  return std::nullopt;
}