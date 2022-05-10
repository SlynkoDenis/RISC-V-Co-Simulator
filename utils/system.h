#ifndef HW_CO_SIMULATION_DIRECTORY_H
#define HW_CO_SIMULATION_DIRECTORY_H


namespace utils {
bool DirectoryExists(const char *path);
bool CreateDirectory(const char *path);
bool CreateDirectoryIfNotExists(const char *path);

bool FileExists(const char *path);
bool CreateFile(const char *path);
bool CreateFileIfNotExists(const char *path);
}   // end namespace utils

#endif // HW_CO_SIMULATION_DIRECTORY_H
