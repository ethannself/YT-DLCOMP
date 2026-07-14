#include "MyApp.hpp"
#include <wx/panel.h>
class SettingsPanel : public wxPanel {
public:
  explicit SettingsPanel(wxWindow *parent);

private:
  void BuildUI();
  AppSettings &settings;
};