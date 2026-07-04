#include "FilesPanel.hpp"
#include "interface.hpp"
#include <wx/event.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

FilesPanel::FilesPanel() : wxPanel(nullptr) {}
FilesPanel::FilesPanel(wxWindow *parent) : wxPanel(parent) { BuildUI(); }

void FilesPanel::BuildUI() {
  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
  wxStaticText *text =
      new wxStaticText(this, wxID_ANY, "Files", wxDefaultPosition,
                       wxDefaultSize, wxALIGN_CENTER);
  wxFont font = text->GetFont();
  font.SetWeight(wxFONTWEIGHT_BOLD);
  font = font.Scale(1.5f);
  text->SetFont(font);
  grid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 300));
  grid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
  grid->EnableEditing(false);
  grid->CreateGrid(0, COLUMN_COUNT);
  grid->AppendRows(3, true);
  grid->SetColLabelValue(0, "Submitter");
  grid->SetColLabelValue(1, "Video URL");
  grid->SetColLabelValue(2, "Timestamp");
  grid->SetColLabelValue(3, "File Path");

  mainSizer->Add(text, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  mainSizer->Add(grid, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
  this->SetSizer(mainSizer);

  Bind(wxEVT_SIZE, &FilesPanel::OnSize, this);
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