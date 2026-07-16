#pragma once
#include "MyApp.hpp"
#include <wx/checkbox.h>
#include <wx/filepicker.h>
#include <wx/panel.h>

class SettingsPanel : public wxPanel {
public:
  explicit SettingsPanel(wxWindow *parent);

private:
  void BuildUI();
  AppSettings &settings;
  void OnDirChanged(wxFileDirPickerEvent &e);
  void OnCheckboxChanged(wxCommandEvent &e);
  void SaveSettings(wxCommandEvent &e);
  wxDirPickerCtrl *destPicker;
  wxButton *saveButton;
  wxCheckBox *saveVideosCheckbox;
  void CheckSave();
  void LoadSettings();
};