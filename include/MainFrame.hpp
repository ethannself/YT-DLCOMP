#pragma once
#include <wx/app.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/stattext.h>
#include <wx/wx.h>

enum { ID_PATH = wxID_HIGHEST + 1, ID_API_UNSET, ID_SET_API };

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
  void OnDownloadProgress(wxThreadEvent &event);
  wxTextCtrl *spreadsheetLinkEntry;
  wxStaticText *linkError;
  wxGauge *gauge;
  wxStaticText *downloadLabel;
  wxStaticText *apiTextWarning;
  void UpdateStartButton();
  void DisplayError(std::string message, size_t durationSeconds);
};
