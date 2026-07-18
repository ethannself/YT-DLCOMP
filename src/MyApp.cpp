#include "MyApp.hpp"
#include "../include/MainFrame.hpp"
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <rpcndr.h>
#include <wx/app.h>
#include <wx/stdpaths.h>
#include <wx/string.h>

static const auto DEFAULT_DESTINATION_PATH =
    std::filesystem::path{
        wxStandardPaths::Get().GetExecutablePath().ToStdString()}
        .parent_path() /
    "data";

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
         "settings.json";
}
std::filesystem::path AppSettings::getDestPath() {
  auto path =
      (!this->destPath.empty() ? this->destPath : DEFAULT_DESTINATION_PATH);
  return path;
}

void AppSettings::saveSettings() {
  std::cout << std::format("Saving settings: apiKey={}, destPath={}, "
                           "sheetsLink={},keepOriginal={}\n",
                           this->apiKey, this->destPath.string(),
                           this->sheetsLink, this->keepOriginal);

  auto path = getSettingsPath();
  std::filesystem::create_directories(path.parent_path());

  nlohmann::json j = *this;
  std::ofstream file(path);
  file << j.dump(4);
}
AppSettings AppSettings::LoadSettings() {
  AppSettings settings{};
  auto path = getSettingsPath();

  if (!std::filesystem::exists(path))
    return settings;
  try {
    std::ifstream file(getSettingsPath());
    nlohmann::json j;
    file >> j;
    settings = j.get<AppSettings>();
  } catch (const std::exception &e) {
    std::cerr << "[LoadSettings] Error: " << e.what() << std::endl;
  }
  return settings;
}
