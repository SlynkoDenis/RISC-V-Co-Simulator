#ifndef HW_CO_SIMULATION_MACROS_H
#define HW_CO_SIMULATION_MACROS_H

#include "debug.h"

#ifdef DEBUG

#define ASSERT_FAIL(expr) utils::AssertionFail(expr, __FILE__, __LINE__, __FUNCTION__)

// TODO: resolve
// #define LIKELY(expr) (__builtin_expect((expr) != 0, true))
#define UNLIKELY(expr) (__builtin_expect((expr) != 0, false))

#define ASSERT(expr)            \
    if (UNLIKELY(!(expr))) {    \
        ASSERT_FAIL(#expr);     \
    }

#else

#define ASSERT(expr) static_cast<void>(0)

#endif

#define WARNING(mess) utils::PrintWarning(#mess, __FILE__, __LINE__, __FUNCTION__)


#define DEFAULT_DTOR(TypeName) \
    ~TypeName() = default

#define DEFAULT_COPY_CTOR(TypeName) \
    TypeName(const TypeName &) = default;

#define DEFAULT_COPY_OPERATOR(TypeName) \
    TypeName &operator=(const TypeName &) = default

#define DEFAULT_COPY_SEMANTIC(TypeName) \
    DEFAULT_COPY_CTOR(TypeName)         \
    DEFAULT_COPY_OPERATOR(TypeName)

#define DEFAULT_MOVE_CTOR(TypeName) \
    TypeName(TypeName &&) = default;

#define DEFAULT_MOVE_OPERATOR(TypeName) \
    TypeName &operator=(TypeName &&) = default

#define DEFAULT_MOVE_SEMANTIC(TypeName) \
    DEFAULT_MOVE_CTOR(TypeName)         \
    DEFAULT_MOVE_OPERATOR(TypeName)

#define NO_COPY_CTOR(TypeName) \
    TypeName(const TypeName &) = delete;

#define NO_COPY_OPERATOR(TypeName) \
    TypeName &operator=(const TypeName &) = delete

#define NO_COPY_SEMANTIC(TypeName) \
    NO_COPY_CTOR(TypeName)         \
    NO_COPY_OPERATOR(TypeName)

#define NO_MOVE_CTOR(TypeName) \
    TypeName(TypeName &&) = delete;

#define NO_MOVE_OPERATOR(TypeName) \
    TypeName &operator=(TypeName &&) = delete

#define NO_MOVE_SEMANTIC(TypeName) \
    NO_MOVE_CTOR(TypeName)         \
    NO_MOVE_OPERATOR(TypeName)

#endif // HW_CO_SIMULATION_MACROS_H
