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
}