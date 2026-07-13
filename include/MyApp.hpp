#pragma once
#include <filesystem>
#include <string>
#include <wx/app.h>

struct AppSettings {
  std::filesystem::path destPath;
  std::string apiKey;
  std::string sheetsLink;

  static std::filesystem::path getSettingsPath();
  static AppSettings LoadSettings();
  void saveSettings();
};

class MyApp : public wxApp {
public:
  bool OnInit() override;
  AppSettings settings;
};

wxDECLARE_APP(MyApp);
