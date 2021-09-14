#ifndef ASMLGEN_APPLICATION_ASSERTION_VERSION_H
#define ASMLGEN_APPLICATION_ASSERTION_VERSION_H

#ifdef NDEBUG
#define ASSERT(expression, message) ((void)0)
#else
#include <cassert>
#define ASSERT(expression, message) assert(((void)(message), (expression)))
#endif

#endif