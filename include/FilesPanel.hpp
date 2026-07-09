#include "interface.hpp"
#include <string>
#include <vector>
#include <wx/grid.h>
#include <wx/panel.h>
#include <wx/window.h>

class FilesPanel : public wxPanel {
public:
  FilesPanel();
  FilesPanel(wxWindow *parent);
  void AddFile(const Entry &e, const std::string &path);
  void SetEntries(std::vector<Entry> entries) {
    this->entries = std::move(entries);
  }
  const std::vector<Entry> &GetEntries() { return this->entries; }

private:
  std::vector<Entry> entries;
  void BuildUI();
  wxGrid *grid = nullptr;
  void OnSize(wxSizeEvent &event);
  void ResizeGridColumns();
  void OnCellDoubleClick(wxGridEvent &e);
};