#include "../include/MainFrame.hpp"
#include "../include/MyApp.hpp"
#include "../include/interface.hpp"
#include "ApiKeyDialog.hpp"
#include "MainFrame.hpp"
#include <iostream>
#include <vector>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>


MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition,
              wxSize(800, 600)) {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(
      ID_PATH, "&Set Destination Folder...\tCtrl-D",
      std::format("Current Destination: {}", wxGetApp().destPath.string()));
  menuFile->AppendSeparator();
  menuFile->Append(
      ID_SET_API, "&Set API Key...\tCtrl-T",
      "Set Google API Key to let the program access Google Sheets");
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

  wxStaticText *apiTextWarning = new wxStaticText(
      panel, ID_API_UNSET, "API Key Unset!\n Please set it at: File->API Key",
      wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  apiTextWarning->SetForegroundColour(wxColour(210, 44, 44));

  spreadsheetLinkEntry =
      new wxTextCtrl(panel, wxID_FILE, "", wxDefaultPosition, wxSize(300, -1));
  wxButton *button = new wxButton(panel, wxID_EXECUTE, "Start",
                                  wxDefaultPosition, wxSize(120, 35));

  // if no api key show warning and block button press
  if (wxGetApp().apiKey.empty()) {
    apiTextWarning->Show();
    button->Enable(false);
  }

  mainSizer->Add(staticText, 0, wxALIGN_CENTER | wxBOTTOM, 7);
  mainSizer->Add(apiTextWarning, 0, wxALIGN_CENTER | wxBOTTOM, 8);
  mainSizer->Add(spreadsheetLinkEntry, 0, wxALIGN_CENTER | wxBOTTOM, 15);
  mainSizer->Add(button, 0, wxALIGN_CENTER);

  mainSizer->AddStretchSpacer();

  panel->SetSizer(mainSizer);
  CreateStatusBar();
  SetStatusText("Welcome to YT-DLCPP!");

  Bind(wxEVT_MENU, &MainFrame::OnSetPath, this, ID_PATH);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_BUTTON, &MainFrame::OnEnter, this, wxID_EXECUTE);
  Bind(wxEVT_MENU, &MainFrame::OnSetAPIKey, this, ID_SET_API);
}

MainFrame::MainFrame(const wxString &title) : MainFrame() { SetTitle(title); };

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

    GetMenuBar()->FindItem(ID_PATH)->SetHelp(
        std::format("Current Destination {}", wxGetApp().destPath.string()));

    SetStatusText("Destination: " + dlg.GetPath());
  }
}
void MainFrame::OnEnter(wxCommandEvent &event) {
  std::optional<std::vector<Entry>> optResult = getResponses(
      this->spreadsheetLinkEntry->GetValue().ToStdString(), wxGetApp().apiKey);
  if (optResult.has_value()) {
    std::vector<Entry> &entries = *optResult;
  }
}
void MainFrame::OnSetAPIKey(wxCommandEvent &event) {
  ApiKeyDialog dlg(this);
  dlg.ShowModal();
}