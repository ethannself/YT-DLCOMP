#include "SettingsPanel.hpp"
#include "MyApp.hpp"
#include <wx/sizer.h>
#include <wx/window.h>

SettingsPanel::SettingsPanel(wxWindow *parent)
    : wxPanel(parent), settings(wxGetApp().settings) {
  BuildUI();
}

void SettingsPanel::BuildUI() {
  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

  this->SetSizer(mainSizer);

  for (size_t i = 0; i < NUM_SETTINGS; ++i) {
  }
}