#include <iostream>
#include <filesystem>
#include "ResourceFile.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace fs = std::filesystem;

#ifdef __APPLE__
// Function to get the executable path on macOS
static std::string getExecutablePath() {
  char path[1024];
  uint32_t size = sizeof(path);
  if (_NSGetExecutablePath(path, &size) != 0) {
    std::cerr << "Buffer too small; need size " << size << std::endl;
    return "";
  }
  return std::string(path);
}

// Function to get the directory of the executable on macOS
static std::string getExecutableDir() {
  fs::path exePath = getExecutablePath();
  return exePath.parent_path().string();
}
#endif

// Function to get the file path
std::string ResourceFile::getFilePath(const std::string& filename) {
#ifdef __APPLE__
  // Check if the file exists in the same directory as the executable
  fs::path exeDir = getExecutableDir();
  fs::path filePath = exeDir / filename;
  if (fs::exists(filePath)) {
    return filePath.string();
  }

  // If not found, check in the resource path of the app bundle
  fs::path resourcePath = exeDir / "../Resources" / filename;
  if (fs::exists(resourcePath)) {
    return resourcePath.string();
  }

#else
  // On other platforms, just check if the file exists in the current directory
  fs::path filePath = filename;
  if (fs::exists(filePath)) {
    return filePath.string();
  }
#endif

  // File not found
  return "";
}
