#include "interface.hpp"
#include <string>
#include <wx/grid.h>
#include <wx/panel.h>
#include <wx/window.h>

class FilesPanel : public wxPanel {
public:
  FilesPanel();
  FilesPanel(wxWindow *parent);
  void AddFile(const Entry &e, const std::string &path);

private:
  void BuildUI();
  wxGrid *grid = nullptr;
  void OnSize(wxSizeEvent &event);
  void ResizeGridColumns();
};