#pragma once
#include "Entry.hpp"
#include "FilesPanel.hpp"
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <wx/event.h>
#include <wx/thread.h>

std::string
executeYtDLPCommand(const char *cmd, std::string_view filename,
                    std::function<void(float, std::string)> onProgress);
std::string buildYtDlpCommand(size_t index, const Entry &entry,
                              const std::filesystem::path &destPath);
std::optional<std::vector<Entry>> getResponses(std::string spreadsheetId,
                                               std::string apiKey);
// std::string buildEntryLabelCommand(const Entry &entry);
void AddEntryLabels(const std::vector<Entry> &entries);
void CombineEntries(const std::vector<Entry> &entries);
int executeffmpegCommand(const std::string &command);
void cleanup();

wxDECLARE_EVENT(EVT_DOWNLOAD_PROGRESS, wxThreadEvent);