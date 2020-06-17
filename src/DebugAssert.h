//
// Created by ArcticMarmoset on 2020-06-17.
//

#ifndef CUSTOM_DEBUG_ASSERT_H
#define CUSTOM_DEBUG_ASSERT_H

#ifndef NDEBUG
#include <iostream>
inline void assert(bool condition, const char *message = "")
{
    if (!condition)
    {
        std::cerr << "[" << __FILE__ << ":" << __LINE__ << "]: " << message << "\n";
        __debugbreak();
    }
}
#else
inline void assert(bool condition, const char *message = "") {}
#endif

#endif // !CUSTOM_DEBUG_ASSERT_H
