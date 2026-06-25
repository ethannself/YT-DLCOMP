#pragma once
#include <wx/app.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/wx.h>

enum { ID_PATH = 1, ID_API_UNSET, ID_SET_API };

class MainFrame : public wxFrame {
public:
  MainFrame();
  MainFrame(const wxString &title);

private:
  void OnSetPath(wxCommandEvent &event);
  void OnExit(wxCommandEvent &event);
  void OnAbout(wxCommandEvent &event);
  void OnEnter(wxCommandEvent &event);
  void OnSetAPIKey(wxCommandEvent &event);
  void OnLinkChanged(wxCommandEvent &event);
  wxTextCtrl *spreadsheetLinkEntry;

  void UpdateStartButton();
};
