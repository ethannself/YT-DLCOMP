#include "SettingsPanel.hpp"
#include "MyApp.hpp"
#include <iostream>
#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/filepicker.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>

SettingsPanel::SettingsPanel(wxWindow *parent)
    : wxPanel(parent), settings(wxGetApp().settings) {
  BuildUI();
}

void SettingsPanel::BuildUI() {
  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

  // destPath, apiKey, sheetsLink;
  wxStaticText *destLabel =
      new wxStaticText(this, wxID_ANY, "Video Destination Path");
  wxDirPickerCtrl *destPicker = new wxDirPickerCtrl(
      this, wxID_ANY,
      (!settings.destPath.string().empty()) ? settings.destPath.string()
                                            : wxString(),
      "Choose Destination Folder", wxDefaultPosition, wxDefaultSize,
      wxDIRP_DEFAULT_STYLE);

  destPicker->Bind(wxEVT_DIRPICKER_CHANGED, &SettingsPanel::OnDirChanged, this);
  wxCheckBox *saveVideosCheckbox =
      new wxCheckBox(this, wxID_ANY, "Keep original videos after editting");

  mainSizer->Add(destLabel, 0, wxEXPAND | wxBOTTOM, 5);
  mainSizer->Add(destPicker, 0, wxEXPAND | wxBOTTOM, 15);
  // mainSizer->AddSpacer(5);
  mainSizer->Add(saveVideosCheckbox, 0, wxEXPAND, 0);

  wxBoxSizer *outerSizer = new wxBoxSizer(wxVERTICAL);
  outerSizer->Add(mainSizer, 1, wxEXPAND | wxALL, 15);

  this->SetSizer(outerSizer);
}
void SettingsPanel::OnDirChanged(wxFileDirPickerEvent &e) {
  settings.destPath = e.GetPath().ToStdString();
}