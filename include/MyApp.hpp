#pragma once
#include <filesystem>
#include <string>
#include <wx/app.h>

class MyApp : public wxApp {
public:
  bool OnInit() override;
  std::filesystem::path destPath;
  std::string apiKey;
};

wxDECLARE_APP(MyApp);
