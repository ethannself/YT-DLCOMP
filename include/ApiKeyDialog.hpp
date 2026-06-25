#pragma once

#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/textctrl.h>
class ApiKeyDialog : public wxDialog {
public:
  ApiKeyDialog(wxWindow *parent);

  wxString GetApiKey() const;

private:
  wxTextCtrl *keyCtrl = nullptr;
  wxButton *toggleBtn = nullptr;
  wxButton *okBtn = nullptr;
  bool m_visible = false;
  // events
  void OnTextChanged(wxCommandEvent &event);
  void OnToggleVisibility(wxCommandEvent &event);
  void OnOk(wxCommandEvent &event);
};