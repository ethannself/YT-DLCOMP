#include "MyApp.hpp"
#include "../include/MainFrame.hpp"
#include <wx/app.h>
#include <wx/string.h>

bool MyApp::OnInit() {
  destPath = std::filesystem::current_path();
  MainFrame *frame = new MainFrame("YT-DLCOMP");
  frame->Center();
  frame->Show(true);
  AllocConsole();
  freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
  freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
  return true;
}
