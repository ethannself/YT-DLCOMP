#include <filesystem>
#include <wx/app.h>

class MyApp : public wxApp {
public:
  bool OnInit() override;
  std::filesystem::path destPath;
};

wxDECLARE_APP(MyApp);
