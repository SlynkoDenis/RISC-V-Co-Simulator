#ifndef HW_CO_SIMULATION_DEBUG_H
#define HW_CO_SIMULATION_DEBUG_H

#include <iostream>


namespace utils {
void AssertionFail(const char *expr, const char *file, unsigned line, const char *function);
void PrintWarning(const char *mess, const char *file, unsigned line, const char *function);
}   // end namespace utils

#endif // HW_CO_SIMULATION_DEBUG_H
