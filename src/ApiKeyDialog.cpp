#include "ApiKeyDialog.hpp"
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/toplevel.h>

enum { ID_TOGGLE_VISIBILITY = wxID_HIGHEST + 1, ID_TEXT_CHANGED };

ApiKeyDialog::ApiKeyDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, "Enter API Key", wxDefaultPosition,
               wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
  wxBoxSizer *outer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *inner = new wxBoxSizer(wxVERTICAL);

  wxStaticText *subtitleLabel = new wxStaticText(
      this, wxID_ANY, "Your key is stored locally and never transmitted.");
  wxFont subFont = subtitleLabel->GetFont();
  subFont.SetPointSize(subFont.GetPointSize() - 1);
  subtitleLabel->SetFont(subFont);
  subtitleLabel->SetForegroundColour(wxColour(100, 100, 100));
  inner->Add(subtitleLabel, 0, wxBOTTOM, 14);

  inner->Add(new wxStaticLine(this), 0, wxEXPAND | wxBOTTOM, 14);

  wxStaticText *fieldLabel = new wxStaticText(this, wxID_ANY, "API Key");
  wxFont fieldFont = fieldLabel->GetFont();
  fieldFont.SetWeight(wxFONTWEIGHT_SEMIBOLD);
  fieldLabel->SetFont(fieldFont);
  inner->Add(fieldLabel, 0, wxBOTTOM, 5);

  auto *inputRow = new wxBoxSizer(wxHORIZONTAL);

  keyCtrl =
      new wxTextCtrl(this, ID_TEXT_CHANGED, wxEmptyString, wxDefaultPosition,
                     wxSize(340, -1), wxTE_PASSWORD | wxTE_PROCESS_ENTER);
  keyCtrl->SetHint("xxxxxxxxxxxxxxxxxxxxxx");
  inputRow->Add(keyCtrl, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);

  toggleBtn = new wxButton(this, ID_TOGGLE_VISIBILITY, "Show",
                           wxDefaultPosition, wxSize(54, -1));
  inputRow->Add(toggleBtn, 0, wxALIGN_CENTER_VERTICAL);

  inner->Add(inputRow, 0, wxEXPAND | wxBOTTOM, 6);
  wxStaticText *hint = new wxStaticText(
      this, wxID_ANY, "Paste the key you copied from Google Cloud console.");
  wxFont hintFont = hint->GetFont();
  hintFont.SetPointSize(hintFont.GetPointSize() - 1);
  hint->SetFont(hintFont);
  hint->SetForegroundColour(wxColour(110, 110, 110));
  inner->Add(hint, 0, wxBOTTOM, 14);

  inner->Add(new wxStaticLine(this), 0, wxEXPAND | wxBOTTOM, 14);

  wxBoxSizer *btnRow = new wxBoxSizer(wxHORIZONTAL);
  btnRow->AddStretchSpacer();
  wxButton *cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");

  okBtn = new wxButton(this, wxID_OK, "Save Key");
  okBtn->SetDefault();
  okBtn->Enable(false);

  btnRow->Add(cancelButton, 0, wxRIGHT, 8);
  btnRow->Add(okBtn, 0);
  inner->Add(btnRow, 0, wxEXPAND);

  outer->Add(inner, 1, wxEXPAND | wxALL, 20);
  SetSizerAndFit(outer); // attaches sizer and sizes the dialog to fit
  Centre(wxBOTH);
  Fit();
  SetMinSize(GetSize());

  Bind(wxEVT_TEXT, &ApiKeyDialog::OnTextChanged, this, ID_TEXT_CHANGED);
  Bind(wxEVT_BUTTON, &ApiKeyDialog::OnToggleVisibility, this,
       ID_TOGGLE_VISIBILITY);
  Bind(wxEVT_BUTTON, &ApiKeyDialog::OnOk, this, wxID_OK);
}

void ApiKeyDialog::OnTextChanged(wxCommandEvent &event) {
  if (okBtn) {
    okBtn->Enable(!keyCtrl->GetValue().Trim().IsEmpty());
  }
}
void ApiKeyDialog::OnToggleVisibility(wxCommandEvent &event) {
  m_visible = !m_visible;

  wxString current = keyCtrl->GetValue();
  long style = keyCtrl->GetWindowStyle();

  if (m_visible)
    style &= ~wxTE_PASSWORD;
  else
    style |= wxTE_PASSWORD;

  wxTextCtrl *newCtrl =
      new wxTextCtrl(this, ID_TEXT_CHANGED, current, wxDefaultPosition,
                     wxSize(340, -1), style);

  newCtrl->SetHint("xxxxxxxxxxxxxxxxxxxxxx");
  newCtrl->SetFocus();
  newCtrl->SetInsertionPointEnd();

  wxSizer *sizer = keyCtrl->GetContainingSizer();
  if (sizer)
    sizer->Replace(keyCtrl, newCtrl);

  keyCtrl->Destroy();
  keyCtrl = newCtrl;
  newCtrl->Bind(wxEVT_TEXT, &ApiKeyDialog::OnTextChanged, this);

  toggleBtn->SetLabel(m_visible ? "Hide" : "Show");
  GetSizer()->Layout();
}
wxString ApiKeyDialog::GetApiKey() const {
  return keyCtrl ? keyCtrl->GetValue().Trim(true).Trim(false) : wxString{};
}
void ApiKeyDialog::OnOk(wxCommandEvent &event) {
  wxString key = GetApiKey();

  if (key.IsEmpty()) {
    wxMessageBox("Enter a valid API key before continuing.", "API Key Required",
                 wxOK | wxICON_WARNING, this);
    keyCtrl->SetFocus();
    return;
  }
  EndModal(wxID_OK);
}