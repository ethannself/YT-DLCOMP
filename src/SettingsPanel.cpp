#include "SettingsPanel.hpp"
#include "MyApp.hpp"
#include <wx/button.h>
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
  destPicker = new wxDirPickerCtrl(
      this, wxID_ANY,
      (!settings.destPath.string().empty()) ? settings.destPath.string()
                                            : wxString(),
      "Choose Destination Folder", wxDefaultPosition, wxDefaultSize,
      wxDIRP_DEFAULT_STYLE);

  destPicker->Bind(wxEVT_DIRPICKER_CHANGED, &SettingsPanel::OnDirChanged, this);
  wxCheckBox *saveVideosCheckbox =
      new wxCheckBox(this, wxID_ANY, "Keep original videos after editting");

  saveButton =
      new wxButton(this, wxID_ANY, "Save", wxDefaultPosition, wxDefaultSize);

  saveButton->Bind(wxEVT_BUTTON, &SettingsPanel::SaveSettings, this);

  mainSizer->Add(destLabel, 0, wxEXPAND | wxBOTTOM, 5);
  mainSizer->Add(destPicker, 0, wxEXPAND | wxBOTTOM, 15);
  // mainSizer->AddSpacer(5);
  mainSizer->Add(saveVideosCheckbox, 0, wxEXPAND, 0);
  mainSizer->Add(saveButton, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 150);
  wxBoxSizer *outerSizer = new wxBoxSizer(wxVERTICAL);
  outerSizer->Add(mainSizer, 1, wxEXPAND | wxALL, 15);

  this->SetSizer(outerSizer);
}
void SettingsPanel::OnDirChanged(wxFileDirPickerEvent &e) {

  if (e.GetPath().ToStdString() != settings.destPath) {
    saveButton->Enable(true);
  }
}
void SettingsPanel::SaveSettings(wxCommandEvent &e) {
  settings.destPath = destPicker->GetPath().ToStdString();
  settings.saveSettings();
}