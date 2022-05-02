#include "debug.h"


void utils::AssertionFail(const char *expr, const char *file, unsigned line, const char *function) {
    std::cerr << "ASSERTION FAILED: " << expr << std::endl;
    std::cerr << "IN " << file << ":" << std::dec << line << ":" << function << std::endl;
    std::abort();
}

void utils::PrintWarning(const char *mess, const char *file, unsigned line, const char *function) {
    std::cerr << "WARNING: " << mess << std::endl;
    std::cerr << "IN " << file << ":" << std::dec << line << ":" << function << std::endl;
}
