#ifndef ASSERT_H
#define ASSERT_H

#include <iostream>

#ifdef DEBUG
#define ASSERT_MSG(assertion, msg)                                          \
    if (!(assertion))                                                       \
    {                                                                       \
        std::cerr << "BANG ASSERTION FAILED: '" << msg << "'" << std::endl; \
        std::abort();                                                       \
    }
#else
#define ASSERT_MSG(assertion, msg)  // No Assert in release
#endif

#define ASSERT_SOFT_MSG(assertion, msg)                            \
    if (!(assertion))                                              \
    {                                                              \
        std::cerr << "BANG SOFT ASSERTION FAILED: '" << msg << "'" \
                  << std::endl;                                    \
    }
#define ASSERT_SOFT(assertion) ASSERT_SOFT_MSG(assertion, "")

#define ASSERT(assertion) ASSERT_MSG(assertion, #assertion)
#define SASSERT(assertion) static_assert(assertion)

#endif
