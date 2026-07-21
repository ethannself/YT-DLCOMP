#include "MainFrame.hpp"
#include "ApiKeyDialog.hpp"
#include "MyApp.hpp"
#include "interface.hpp"
#include <chrono>
#include <stdexcept>
#include <thread>
#include <vector>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/gauge.h>
#include <wx/gdicmn.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/window.h>

MainFrame::MainFrame(const wxString &title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(950, 700)) {
  auto &settings = wxGetApp().settings;
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(
      ID_PATH, "&Set Destination Folder...\tCtrl-D",
      std::format("Current Destination: {}", settings.getDestPath().string()));
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

  wxNotebook *notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, wxNB_DEFAULT);
  wxPanel *panel = new wxPanel(notebook);

  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

  mainSizer->AddStretchSpacer();

  wxStaticText *staticText =
      new wxStaticText(panel, wxID_ANY, "Enter the Google Form link");

  apiTextWarning = new wxStaticText(
      panel, ID_API_UNSET, "API Key Unset!\n Please set it at: File->API Key",
      wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  apiTextWarning->SetForegroundColour(wxColour(210, 44, 44));

  spreadsheetLinkEntry =
      new wxTextCtrl(panel, wxID_FILE, "", wxDefaultPosition, wxSize(300, -1));
  linkError =
      new wxStaticText(panel, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1),
                       wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE);
  linkError->SetForegroundColour(wxColour{200, 0, 0});
  wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

  wxButton *startButton = new wxButton(panel, wxID_EXECUTE, "Download",
                                       wxDefaultPosition, wxSize(120, 35));
  wxButton *closeButton = new wxButton(panel, wxID_ANY, "Close",
                                       wxDefaultPosition, wxSize(120, 35));

  closeButton->Bind(wxEVT_BUTTON,
                    [this](wxCommandEvent &event) { Close(true); });

  buttonSizer->Add(startButton, wxSizerFlags().Border(wxRIGHT, 10));
  buttonSizer->Add(closeButton);

  wxButton *compButton = new wxButton(panel, wxID_SAVE, "Edit Comp",
                                      wxDefaultPosition, wxSize(120, 35));
  compButton->Enable(true);
  downloadLabel =
      new wxStaticText(panel, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1),
                       wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE);

  gauge = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(300, -1));
  gauge->SetValue(0);
  // if no api key show warning and block button press
  UpdateStartButton();
  apiTextWarning->Show(settings.apiKey.empty());

  mainSizer->Add(staticText, 0, wxALIGN_CENTER | wxBOTTOM, 7);
  mainSizer->Add(apiTextWarning, 0, wxALIGN_CENTER | wxBOTTOM, 8);
  mainSizer->Add(spreadsheetLinkEntry, 0, wxALIGN_CENTER | wxBOTTOM, 15);
  mainSizer->Add(linkError, 0, wxALIGN_CENTER | wxBOTTOM, 2);

  mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER, 15);
  mainSizer->Add(compButton, 0, wxALIGN_CENTER | wxTOP, 15);
  mainSizer->Add(downloadLabel, 0, wxALIGN_CENTER | wxTOP, 30);
  mainSizer->Add(gauge, 0, wxALIGN_CENTER | wxTOP, 6);

  mainSizer->AddStretchSpacer();

  panel->SetSizer(mainSizer);
  filesPanel = new FilesPanel(notebook);
  settingsPanel = new SettingsPanel(notebook);
  notebook->AddPage(panel, "Home");
  notebook->AddPage(filesPanel, "Files");
  notebook->AddPage(settingsPanel, "Settings");

  wxBoxSizer *frameSizer = new wxBoxSizer(wxVERTICAL);
  frameSizer->Add(notebook, 1, wxEXPAND);
  SetSizer(frameSizer);
  CreateStatusBar();
  SetStatusText("Welcome to YT-DLCOMP!");

  Bind(wxEVT_MENU, &MainFrame::OnSetPath, this, ID_PATH);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_BUTTON, &MainFrame::OnEnter, this, wxID_EXECUTE);
  Bind(wxEVT_BUTTON, &MainFrame::OnCreate, this, wxID_SAVE);
  Bind(wxEVT_MENU, &MainFrame::OnSetAPIKey, this, ID_SET_API);
  Bind(wxEVT_TEXT, &MainFrame::OnLinkChanged, this, wxID_FILE);
  Bind(EVT_DOWNLOAD_PROGRESS, &MainFrame::OnDownloadProgress, this);
}

void MainFrame::OnExit(wxCommandEvent &event) { Close(true); }

void MainFrame::OnAbout(wxCommandEvent &event) {
  wxMessageBox("This is a program to download videos for the song comp",
               "About YT-DLCOMP", wxOK | wxICON_INFORMATION);
}

// called when player clicks file -> set destination folder or presses Ctrl+D.
void MainFrame::OnSetPath(wxCommandEvent &event) {
  auto &settings = wxGetApp().settings;
  wxDirDialog dlg(this, "Choose destination folder",
                  settings.getDestPath().string(),
                  wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
  if (dlg.ShowModal() == wxID_OK) {
    settings.destPath = dlg.GetPath().ToStdString();
    settings.saveSettings();
    GetMenuBar()->FindItem(ID_PATH)->SetHelp(
        std::format("Current Destination {}", settings.getDestPath().string()));

    SetStatusText("Destination: " + dlg.GetPath());
  }
}
// on pressing start
void MainFrame::OnEnter(wxCommandEvent &event) {
  auto &settings = wxGetApp().settings;
  std::optional<std::vector<Entry>> optResult;
  try {
    optResult = getResponses(
        this->spreadsheetLinkEntry->GetValue().ToStdString(), settings.apiKey);

  } catch (std::runtime_error &err) {
    DisplayError(err.what(), 3);
    return;
  }
  if (!optResult.has_value())
    return;

  settings.saveSettings();
  filesPanel->SetEntries(optResult.value());

  DownloadEntries(filesPanel, this,
                  [this](const std::string &text) { SetStatusText(text); });
}
void MainFrame::OnCreate(wxCommandEvent &event) {
  const std::vector<Entry> &entries = filesPanel->GetEntries();
  // todo introvideo handling

  // add text
  this->downloadLabel->SetLabelText("Adding labels to videos...");
  gauge->SetValue(0);
  AddEntryLabels(entries);
  // stitch videos together
  this->downloadLabel->SetLabelText("Combining videos...");
  gauge->SetValue(50);
  CombineEntries(entries);
  this->downloadLabel->SetLabelText("Done!");
  gauge->SetValue(100);
  cleanup();
}
// upon clicking Save Key at file -> set api key
void MainFrame::OnSetAPIKey(wxCommandEvent &event) {
  auto &app = wxGetApp();
  ApiKeyDialog dlg(this);
  if (dlg.ShowModal() == wxID_OK) {
    app.settings.apiKey = dlg.GetApiKey().ToStdString();
    app.settings.saveSettings();
    apiTextWarning->Show(app.settings.apiKey.empty());
    UpdateStartButton();
  }
}
// upon updating the Google Sheets link
void MainFrame::OnLinkChanged(wxCommandEvent &event) { UpdateStartButton(); }

// enables the start button if these conditions are met: Api key set, sheets
// link set.
void MainFrame::UpdateStartButton() {
  auto *button = FindWindow(wxID_EXECUTE);
  bool hasKey = !wxGetApp().settings.apiKey.empty();
  bool hasLink = !spreadsheetLinkEntry->GetValue().IsEmpty();
  button->Enable(hasKey && hasLink);
}
// updates download gauge progress
void MainFrame::OnDownloadProgress(wxThreadEvent &event) {
  // annoying hacky workaround to disable the gauge animation
  // (only works up to 99% as values over 100 will hit an assert..)
  if (event.GetInt() <= 99) {
    gauge->SetValue(event.GetInt() + 1);
  }
  //
  gauge->SetValue(event.GetInt());
  if (!event.GetString().empty()) {
    // std::cout << event.GetString() << std::endl;
    this->downloadLabel->SetLabelText(event.GetString());
  }
}
// display an error under the sheets text control for durationSeconds (async)
void MainFrame::DisplayError(std::string message, size_t durationSeconds) {
  linkError->SetLabelText(std::format("Error: {}", message));
  wxStaticText *errLabel = linkError; // capture by value, not `this`

  std::thread([errLabel, durationSeconds]() {
    std::this_thread::sleep_for(std::chrono::seconds(durationSeconds));
    if (errLabel) {
      errLabel->CallAfter([errLabel]() { errLabel->SetLabelText(""); });
    }
  }).detach();
}