#pragma once
#include "MyApp.hpp"
#include <wx/filepicker.h>
#include <wx/panel.h>
class SettingsPanel : public wxPanel {
public:
  explicit SettingsPanel(wxWindow *parent);

private:
  void BuildUI();
  AppSettings &settings;
  void OnDirChanged(wxFileDirPickerEvent &e);
};