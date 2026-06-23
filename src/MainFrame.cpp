#include "../include/MainFrame.hpp"
#include "../include/MyApp.hpp"
#include "MainFrame.hpp"
#include <wx/event.h>
#include <wx/msw/button.h>
#include <wx/msw/checkbox.h>
#include <wx/msw/stattext.h>
#include <wx/msw/textctrl.h>
#include <wx/sizer.h>
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition,
              wxSize(800, 600)) {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(
      ID_Path, "&Set Destination Folder...\tCtrl-D",
      std::format("Current Destination: {}", wxGetApp().destPath.string()));
  menuFile->AppendSeparator();

  menuFile->Append(wxID_EXIT);

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);
  wxPanel *panel = new wxPanel(this);

  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

  mainSizer->AddStretchSpacer();

  wxStaticText *staticText =
      new wxStaticText(panel, wxID_ANY, "Enter the Google Form link");

  wxTextCtrl *textCtrl =
      new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));

  wxButton *button = new wxButton(panel, wxID_ANY, "Button", wxDefaultPosition,
                                  wxSize(120, 35));

  mainSizer->Add(staticText, 0, wxALIGN_CENTER | wxBOTTOM, 8);
  mainSizer->Add(textCtrl, 0, wxALIGN_CENTER | wxBOTTOM, 15);
  mainSizer->Add(button, 0, wxALIGN_CENTER);

  mainSizer->AddStretchSpacer();

  panel->SetSizer(mainSizer);
  CreateStatusBar();
  SetStatusText("Welcome to YT-DLCPP!");

  Bind(wxEVT_MENU, &MainFrame::OnSetPath, this, ID_Path);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
}

void MainFrame::OnExit(wxCommandEvent &event) { Close(true); }

void MainFrame::OnAbout(wxCommandEvent &event) {
  wxMessageBox("This is a program to download videos for the song comp",
               "About YT-DLCPP", wxOK | wxICON_INFORMATION);
}

// called when player clicks file -> set destination folder or presses Ctrl+D.
void MainFrame::OnSetPath(wxCommandEvent &event) {
  wxDirDialog dlg(this, "Choose destination folder",
                  wxGetApp().destPath.string(),
                  wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
  if (dlg.ShowModal() == wxID_OK) {
    wxGetApp().destPath = dlg.GetPath().ToStdString();

    GetMenuBar()->FindItem(ID_Path)->SetHelp(
        std::format("Current Destination {}", wxGetApp().destPath.string()));

    SetStatusText("Destination: " + dlg.GetPath());
  }
}
MainFrame::MainFrame(const wxString &title) : MainFrame() { SetTitle(title); }