#include "FilesPanel.hpp"
#include "interface.hpp"
#include <filesystem>
#include <iostream>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/generic/grid.h>
#include <wx/grid.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#ifdef _WIN32
#include <windows.h>
#endif
enum { COL_SUBMITTER = 0, COL_URL, COL_TIMESTAMP, COL_FILEPATH };
FilesPanel::FilesPanel() : wxPanel(nullptr) {}
FilesPanel::FilesPanel(wxWindow *parent) : wxPanel(parent) { BuildUI(); }

void FilesPanel::BuildUI() {
  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
  wxStaticText *text =
      new wxStaticText(this, wxID_ANY, "Files", wxDefaultPosition,
                       wxDefaultSize, wxALIGN_CENTER);
  wxStaticText *caution =
      new wxStaticText(this, wxID_ANY,
                       "Caution: This list will not save when closing the app, "
                       "but your files will still be there.",
                       wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
  // header font
  wxFont font = text->GetFont();
  font.SetWeight(wxFONTWEIGHT_BOLD);
  font = font.Scale(1.5f);
  text->SetFont(font);
  // caution text font
  font = caution->GetFont();
  font = font.Scale(0.9f);
  caution->SetFont(font);
  caution->SetForegroundColour(wxColour(238, 139, 0));
  grid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 300));
  grid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
  grid->EnableEditing(false);
  grid->CreateGrid(0, COLUMN_COUNT);
  // grid->AppendRows(3, true);
  grid->SetColLabelValue(0, "Submitter");
  grid->SetColLabelValue(1, "Video URL");
  grid->SetColLabelValue(2, "Timestamp");
  grid->SetColLabelValue(3, "File Path");

  mainSizer->Add(text, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  mainSizer->Add(caution, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 10);
  mainSizer->Add(grid, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
  this->SetSizer(mainSizer);

  Bind(wxEVT_SIZE, &FilesPanel::OnSize, this);
  grid->Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &FilesPanel::OnCellDoubleClick, this);
  CallAfter([this]() {
    wxSizeEvent dummy;
    this->OnSize(dummy);
  });
}
void FilesPanel::OnSize(wxSizeEvent &event) {
  event.Skip();
  CallAfter(&FilesPanel::ResizeGridColumns);
}

void FilesPanel::ResizeGridColumns() {
  if (!grid || grid->GetNumberCols() == 0)
    return;

  static const double weights[COLUMN_COUNT] = {0.15, 0.35, 0.2, 0.3};
  // small margin to avoid clipping on resize
  const int margin = 4;
  int totalWidth =
      grid->GetClientSize().GetWidth() - grid->GetRowLabelSize() - margin;
  if (totalWidth <= 0)
    return;

  int assigned = 0;
  for (int i = 0; i < COLUMN_COUNT; ++i) {
    int w = (i == COLUMN_COUNT - 1) ? totalWidth - assigned
                                    : static_cast<int>(weights[i] * totalWidth);
    grid->SetColSize(i, w);
    assigned += w;
  }
}
void FilesPanel::AddFile(const Entry &e, const std::string &path) {
  grid->AppendRows(1);
  int newRow = grid->GetNumberRows() - 1;

  grid->SetCellValue(newRow, COL_SUBMITTER, e.user);
  grid->SetCellValue(newRow, COL_URL, e.link);
  grid->SetCellValue(newRow, COL_TIMESTAMP, e.timestamp);
  grid->SetCellValue(newRow, COL_FILEPATH, path);
}

static void openFile(const std::filesystem::path &path) {
#ifdef _WIN32
  ShellExecuteW(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
#ifdef __APPLE__
  std::string cmd = "open \"" + path.string() + "\"";
#else
  std::string cmd = "xdg-open \"" + path.string() + "\"";
#endif
  system(cmd.c_str());
#endif
}
void FilesPanel::OnCellDoubleClick(wxGridEvent &e) {
  int row = e.GetRow();
  std::string link = grid->GetCellValue(row, COL_FILEPATH).ToStdString();
  std::cout << std::format("Clicked row {}\nPath: {}", row, link) << std::endl;
  openFile(link);
}
