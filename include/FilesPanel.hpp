#include <wx/grid.h>
#include <wx/panel.h>
#include <wx/window.h>

class FilesPanel : public wxPanel {
public:
  FilesPanel();
  FilesPanel(wxWindow *parent);

private:
  void BuildUI();
  wxGrid *grid = nullptr;
  void OnSize(wxSizeEvent &event);
  void ResizeGridColumns();
};