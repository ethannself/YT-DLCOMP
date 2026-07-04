#include "FilesPanel.hpp"
#include <wx/event.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/grid.h>
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

  wxGrid *grid = new wxGrid();

  mainSizer->Add(text, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 10);

  this->SetSizer(mainSizer);
}