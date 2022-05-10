#include <fcntl.h>
#include <sys/stat.h>
#include "system.h"
#include <sys/types.h>
#include <unistd.h>


namespace utils {
bool DirectoryExists(const char *path) {
    struct stat sb;
    return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

bool CreateDirectory(const char *path) {
    return (mkdir(path, 00777) == 0);
}

bool CreateDirectoryIfNotExists(const char *path) {
    bool status = true;
    if (!DirectoryExists(path)) {
        status = CreateDirectory(path);
    }
    return status;
}


bool FileExists(const char *path) {
    struct stat sb;
    return (stat(path, &sb) == 0 && S_ISREG(sb.st_mode));
}

bool CreateFile(const char *path) {
    int fd = open(path, O_WRONLY | O_CREAT, 00666);
    if (fd == -1) {
        return false;
    }
    return (close(fd) == 0);
}

bool CreateFileIfNotExists(const char *path) {
    bool status = true;
    if (!FileExists(path)) {
        status = CreateFile(path);
    }
    return status;
}
}   // end namespace utils
