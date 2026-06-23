#include "MyApp.hpp"
#include "../include/Mainframe.hpp"
#include <wx/app.h>
#include <wx/string.h>

bool MyApp::OnInit() {
  destPath = std::filesystem::current_path();
  MainFrame *frame = new MainFrame("YT-DLCPP");
  frame->Center();
  frame->Show(true);
  return true;
}
