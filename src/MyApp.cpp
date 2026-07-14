#include "MyApp.hpp"
#include "../include/MainFrame.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <wx/app.h>
#include <wx/stdpaths.h>
#include <wx/string.h>

bool MyApp::OnInit() {
  AllocConsole();
  freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
  freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
  settings = AppSettings::LoadSettings();
  MainFrame *frame = new MainFrame("YT-DLCOMP");
  frame->Center();
  frame->Show(true);
  return true;
}
std::filesystem::path AppSettings::getSettingsPath() {
  return std::filesystem::path{
             wxStandardPaths::Get().GetExecutablePath().ToStdString()}
             .parent_path() /
         "settings.ini";
}
void AppSettings::saveSettings() {
  std::cout << std::format(
      "Saving settings: apiKey={}, destPath={}, sheetsLink={}\n", this->apiKey,
      this->destPath.string(), this->sheetsLink);
  std::ofstream file(getSettingsPath());
  file << std::format("apiKey={}\ndestPath={}\nsheetsLink={}\n", this->apiKey,
                      this->destPath.string(), this->sheetsLink);
}
AppSettings AppSettings::LoadSettings() {
  AppSettings settings{};
  std::ifstream file(getSettingsPath());
  if (!file.is_open())
    return settings;
  std::string line;

  while (std::getline(file, line)) {
    auto delim = line.find('=');
    if (delim == std::string::npos)
      continue;
    std::string key = line.substr(0, delim);
    std::string value = line.substr(delim + 1);

    if (key == "apiKey")
      settings.apiKey = value;
    else if (key == "destPath")
      settings.destPath = std::filesystem::path{value};
    else if (key == "sheetsLink")
      settings.sheetsLink = value;
  }
  std::cout << std::format(
      "Loaded settings: apiKey={}, destPath={}, sheetsLink={}\n",
      settings.apiKey, settings.destPath.string(), settings.sheetsLink);
  return settings;
}
std::variant<std::filesystem::path, std::string>
AppSettings::operator[](size_t index) {
  switch (index) {
  case 0:
    return this->destPath;
  case 1:
    return this->apiKey;
  case 2:
    return this->sheetsLink;
  default:
    throw std::out_of_range("AppSettings::operator[]: invalid index");
  }
}