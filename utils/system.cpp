#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include "file.h"
#include <fts.h>
#include "macros.h"
#include <sys/stat.h>
#include <string.h>
#include "system.h"
#include <sys/types.h>
#include <unistd.h>


namespace {
inline void ReportError(std::string what, std::string path) {
    WARNING(what + path + "\nerrno: " + strerror(errno));
}

[[maybe_unused]] int IsDirectoryEmpty(const char *path) {
    DIR *dir = opendir(path);
    if (dir == nullptr) {
        ReportError("failed to open directory ", path);
        return -1;
    }

    int n = 0;
    struct dirent *d = nullptr;
    while ((d = readdir(dir)) != nullptr) {
        if (++n > 2) {
          break;
        }
    }

    if (closedir(dir)) {
        ReportError("failed to close directory ", path);
    }
    if (n <= 2) {
        return 1;
    } else {
        return 0;
    }
}

[[maybe_unused]] bool RemoveSingleDirectory(const char *path) {
    if (rmdir(path)) {
        ReportError("failed to remove directory ", path);
        return false;
    }
    return true;
}
}   // end anonymous namespace


namespace utils {
bool DirectoryExists(const char *path) {
    struct stat sb;
    return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

bool CreateDirectory(const char *path) {
    if (mkdir(path, 00777)) {
        ReportError("failed to create directory ", path);
        return false;
    }
    return true;
}

bool CreateDirectoryIfNotExists(const char *path) {
    bool status = true;
    if (!DirectoryExists(path)) {
        status = CreateDirectory(path);
    }
    return status;
}

bool RemoveDirectory(const char *path) {
    bool success = true;
    FTS *ftsp = nullptr;
    FTSENT *curr = nullptr;

    char *files[] = {const_cast<char*>(path), nullptr};

    // FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
    //                in multithreaded programs
    // FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
    //                of the specified directory
    // FTS_XDEV     - Don't cross filesystem boundaries
    ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, nullptr);
    if (!ftsp) {
        ReportError("fts_open failed for ", path);
        return false;
    }

    while ((curr = fts_read(ftsp))) {
        switch (curr->fts_info) {
        default:
            WARNING("unsupported status: " + std::to_string(curr->fts_info));
            break;
        case FTS_NS:
        case FTS_DNR:
        case FTS_ERR:
            ReportError("fts_read failed for ", curr->fts_accpath);
            break;
        case FTS_DC:
        case FTS_DOT:
        case FTS_NSOK:
            // Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were
            // passed to fts_open()
            break;
        case FTS_D:
            // Do nothing. Need depth-first search, so directories are deleted
            // in FTS_DP
            break;
        case FTS_DP:
        case FTS_F:
        case FTS_SL:
        case FTS_SLNONE:
        case FTS_DEFAULT:
            if (remove(curr->fts_accpath)) {
                ReportError("failed to remove ", curr->fts_path);
                success = false;
            }
            break;
        }
    }

    if (ftsp) {
        if (fts_close(ftsp)) {
            ReportError("fts_close failed for ", path);
        }
    }

    return success;
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

bool RemoveFile(const char *path) {
    if (remove(path)) {
        ReportError("failed to remove ", path);
        return false;
    }
    return true;
}

bool CreateNewFile(const char *path) {
    return (!FileExists(path) || RemoveFile(path)) && CreateFile(path);
}

// reading large files in memory may occupy much space,
// but this way is faster than implemented by getc;
// this implementation also works for binary files
int CompareTwoFiles(const char *path1, const char *path2) {
    File file(open(path1, O_RDONLY));
    if (!file.IsValid()) {
        throw FileOpenException("failed to open " + std::string(path1));
    }
    auto sz = file.GetFileSize();
    std::vector<char> pipe_mem(sz + 1, '\0');
    if (read(file.GetFd(), pipe_mem.data(), sz) < 0) {
        ReportError("failed to read file ", path1);

    }
    file.Close();

    file = File(open(path2, O_RDONLY));
    if (!file.IsValid()) {
        throw FileOpenException("failed to open " + std::string(path2));
    }
    auto tmp_sz = file.GetFileSize();

    if (sz > tmp_sz) {
        return 1;
    }
    if (sz < tmp_sz) {
        return -1;
    }

    std::vector<char> funct_mem(sz + 1, '\0');
    if (read(file.GetFd(), funct_mem.data(), sz) < 0) {
        ReportError("failed to read file ", path2);
    }
    file.Close();

    return std::strcmp(pipe_mem.data(), funct_mem.data());
}
}   // end namespace utils
