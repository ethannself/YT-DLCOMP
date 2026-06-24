#pragma once
#include <wx/app.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/wx.h>

enum { ID_Path = 1 };

class MainFrame : public wxFrame {
public:
  MainFrame();
  MainFrame(const wxString &title);

private:
  void OnSetPath(wxCommandEvent &event);
  void OnExit(wxCommandEvent &event);
  void OnAbout(wxCommandEvent &event);
  void OnEnter(wxCommandEvent &event);

  wxTextCtrl *spreadsheetLinkEntry;
};
