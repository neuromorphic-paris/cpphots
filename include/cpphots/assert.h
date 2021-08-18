/**
 * @file assert.h
 * @brief Custom assert for cpphots
 */
#ifndef CPPHOTS_ASSERT_H
#define CPPHOTS_ASSERT_H


// pretty function name
# if defined(__GNUC__)
#   define CPPHOTS_PRETTY_FUNCTION __PRETTY_FUNCTION__
# else
#   if defined __cplusplus && __cplusplus >= 201103L
#     define CPPHOTS_PRETTY_FUNCTION __func__
#   else
#     define CPPHOTS_PRETTY_FUNCTION ((const char *) 0)
#   endif
# endif

#ifndef CPPHOTS_ASSERTS
#  define cpphots_assert(expr) (static_cast<void> (0))
#else
namespace cpphots {
// Prints message and abort
extern void __cpphots_assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function);
}

// macro definition
#  define cpphots_assert(expr) \
          (static_cast<bool>(expr) \
           ? void (0) \
           : cpphots::__cpphots_assert_fail(#expr, __FILE__, __LINE__, CPPHOTS_PRETTY_FUNCTION))
#endif

#endif