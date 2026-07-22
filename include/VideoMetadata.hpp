#include <iostream>
#include <string>

struct VideoMetadata {
  std::string track;  // if provided by yt
  std::string artist; // if provided by yt

  long long views;
};

inline std::ostream &operator<<(std::ostream &os, const VideoMetadata &md) {
  os << "Track: " << md.track << ", Artist: " << md.artist
     << " Views: " << md.views << std::endl;

  return os;
}