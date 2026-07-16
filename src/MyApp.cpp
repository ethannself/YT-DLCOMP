#include "MyApp.hpp"
#include "../include/MainFrame.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <rpcndr.h>
#include <string>
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
  std::cout << std::format("Saving settings: apiKey={}, destPath={}, "
                           "sheetsLink={},keepOriginal={}\n",
                           this->apiKey, this->destPath.string(),
                           this->sheetsLink, this->keepOriginal);
  std::ofstream file(getSettingsPath());
  file << std::format(
      "apiKey={}\ndestPath={}\nsheetsLink={}\nkeepOriginal={}\n", this->apiKey,
      this->destPath.string(), this->sheetsLink, this->keepOriginal);
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
    else if (key == "keepOriginal")
      (value == "true") ? settings.keepOriginal = true
                        : settings.keepOriginal = false;
  }
  std::cout << std::format(
      "Loaded settings: apiKey={}, destPath={}, sheetsLink={}\nkeepOriginal={}",
      settings.apiKey, settings.destPath.string(), settings.sheetsLink,
      settings.keepOriginal);
  return settings;
}
