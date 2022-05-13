#ifndef HW_CO_SIMULATION_DIRECTORY_H
#define HW_CO_SIMULATION_DIRECTORY_H


namespace utils {
// TODO: implement using filesystem std library
// hint: must add compilation options: -std=c++17 -std=gnu++17 -lstdc++fs

bool DirectoryExists(const char *path);

bool CreateDirectory(const char *path);

bool CreateDirectoryIfNotExists(const char *path);

bool RemoveDirectory(const char *path);

bool FileExists(const char *path);

bool CreateFile(const char *path);

bool CreateFileIfNotExists(const char *path);

bool RemoveFile(const char *path);

bool CreateNewFile(const char *path);

int CompareTwoFiles(const char *path1, const char *path2);
}   // end namespace utils

#endif // HW_CO_SIMULATION_DIRECTORY_H
