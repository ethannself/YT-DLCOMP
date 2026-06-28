#include "../include/MainFrame.hpp"
#include "../include/MyApp.hpp"
#include "../include/interface.hpp"
#include "ApiKeyDialog.hpp"
#include "MainFrame.hpp"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/gauge.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/window.h>

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition,
              wxSize(800, 600)) {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(ID_PATH, "&Set Destination Folder...\tCtrl-D",
                   std::format("Current Destination: {}",
                               wxGetApp().settings.destPath.string()));
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

  gauge = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(300, -1));
  gauge->SetValue(0);
  // if no api key show warning and block button press
  UpdateStartButton();
  apiTextWarning->Show(wxGetApp().settings.apiKey.empty());

  mainSizer->Add(staticText, 0, wxALIGN_CENTER | wxBOTTOM, 7);
  mainSizer->Add(apiTextWarning, 0, wxALIGN_CENTER | wxBOTTOM, 8);
  mainSizer->Add(spreadsheetLinkEntry, 0, wxALIGN_CENTER | wxBOTTOM, 15);
  mainSizer->Add(button, 0, wxALIGN_CENTER | wxBOTTOM, 15);
  mainSizer->Add(gauge, 0, wxALIGN_CENTER);

  mainSizer->AddStretchSpacer();

  panel->SetSizer(mainSizer);
  CreateStatusBar();
  SetStatusText("Welcome to YT-DLCOMP!");

  Bind(wxEVT_MENU, &MainFrame::OnSetPath, this, ID_PATH);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_BUTTON, &MainFrame::OnEnter, this, wxID_EXECUTE);
  Bind(wxEVT_MENU, &MainFrame::OnSetAPIKey, this, ID_SET_API);
  Bind(wxEVT_TEXT, &MainFrame::OnLinkChanged, this, wxID_FILE);
  Bind(EVT_DOWNLOAD_PROGRESS, &MainFrame::OnDownloadProgress, this);
}

MainFrame::MainFrame(const wxString &title) : MainFrame() { SetTitle(title); };

void MainFrame::OnExit(wxCommandEvent &event) { Close(true); }

void MainFrame::OnAbout(wxCommandEvent &event) {
  wxMessageBox("This is a program to download videos for the song comp",
               "About YT-DLCOMP", wxOK | wxICON_INFORMATION);
}

// called when player clicks file -> set destination folder or presses Ctrl+D.
void MainFrame::OnSetPath(wxCommandEvent &event) {
  wxDirDialog dlg(this, "Choose destination folder",
                  wxGetApp().settings.destPath.string(),
                  wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
  if (dlg.ShowModal() == wxID_OK) {
    wxGetApp().settings.destPath = dlg.GetPath().ToStdString();
    wxGetApp().settings.saveSettings();

    GetMenuBar()->FindItem(ID_PATH)->SetHelp(std::format(
        "Current Destination {}", wxGetApp().settings.destPath.string()));

    SetStatusText("Destination: " + dlg.GetPath());
  }
}
void MainFrame::OnEnter(wxCommandEvent &event) {
  try {
    std::optional<std::vector<Entry>> optResult =
        getResponses(this->spreadsheetLinkEntry->GetValue().ToStdString(),
                     wxGetApp().settings.apiKey);
    if (optResult.has_value()) {
      wxGetApp().settings.saveSettings();
      std::vector<Entry> &entries = *optResult;
      auto destPath = wxGetApp().settings.destPath;
      size_t total = entries.size();

      std::thread([entries, destPath, total, this]() {
        for (size_t i = 0; i < entries.size(); ++i) {
          try {
            std::string command =
                buildYtDlpCommand(i + 1, entries[i], destPath);
            std::string res = executeYtDLPCommand(
                command.c_str(), [&](float pct, std::string ext) {
                  auto *evt = new wxThreadEvent(EVT_DOWNLOAD_PROGRESS);
                  if (pct >= 0.0)
                    evt->SetInt(static_cast<int>(pct));
                  if (!ext.empty())
                    evt->SetString(wxString{
                        std::format("Downloading file {}/{} {}.{} {}%/100%",
                                    i + 1, entries.size(), i + 1, ext, pct)});

                  wxQueueEvent(this, evt);
                });
          } catch (std::runtime_error &err) {
            std::cout << "[Error] buildYtDlpCommand: " << err.what()
                      << std::endl;
          }
        }

        auto *evt = new wxThreadEvent(EVT_DOWNLOAD_PROGRESS);
        evt->SetInt(100);
        wxQueueEvent(this, evt);
      }).detach();
    }
  } catch (std::runtime_error &err) {
    std::cout << "[Error] getResponses: " << err.what() << std::endl;
  }
}
void MainFrame::OnSetAPIKey(wxCommandEvent &event) {
  auto *apiTextWarning = FindWindow(ID_API_UNSET);
  auto *button = FindWindow(wxID_EXECUTE);
  ApiKeyDialog dlg(this);
  if (dlg.ShowModal() == wxID_OK) {
    wxGetApp().settings.apiKey = dlg.GetApiKey().ToStdString();
    wxGetApp().settings.saveSettings();
    apiTextWarning->Show(wxGetApp().settings.apiKey.empty());
    UpdateStartButton();
  }
}
void MainFrame::OnLinkChanged(wxCommandEvent &event) { UpdateStartButton(); }

void MainFrame::UpdateStartButton() {
  auto *button = FindWindow(wxID_EXECUTE);
  bool hasKey = !wxGetApp().settings.apiKey.empty();
  bool hasLink = !spreadsheetLinkEntry->GetValue().IsEmpty();
  button->Enable(hasKey && hasLink);
}

void MainFrame::OnDownloadProgress(wxThreadEvent &event) {
  gauge->SetValue(event.GetInt());
  if (!event.GetString().empty()) {
    std::cout << event.GetString() << std::endl;
  }
}