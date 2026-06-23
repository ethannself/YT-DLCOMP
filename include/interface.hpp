#pragma once
#include <array>
#include <cpr/api.h>
#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>

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
void request_responses(std::string spreadsheetId) {
  char *apiKey = nullptr;
  size_t size = 0;
  std::string range = "Form%20Responses%201!A:Z";
  if (_dupenv_s(&apiKey, &size, "apiKey") == 0 && apiKey != nullptr) {

    auto response = cpr::Get(cpr::Url{std::format(
        "https://sheets.googleapis.com/v4/spreadsheets/{}/values/{}?key={}",
        spreadsheetId, range, apiKey)});
  }
  // process json here
}