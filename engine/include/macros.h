#ifndef MACROS_H
#define MACROS_H

#define STATIC_ASSERT(EXPRESSION) typedef uint8_t static_assert_t[(EXPRESSION) ? (1) : (-1)]

#define ARRAY_COUNT(ARRAY) ((uint64_t)(sizeof(ARRAY) / sizeof((ARRAY)[0ULL])))
#define OFFSET_OF(TYPE, MEMBER) ((uint64_t)&(((TYPE *)0)->MEMBER))

#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#define CLAMP(A, LO, HI) ((A) < (LO) ? (LO) : ((A) > (HI) ? (HI) : (A)))

#define ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((uint64_t)(VALUE)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))
#define ALIGN_UP_BY(VALUE, ALIGNMENT) ((((uint64_t)(VALUE)) + (((uint64_t)(ALIGNMENT)) - 1ULL)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))

#ifdef BUILD_DEBUG
#  define VK_CHECK(EXPRESSION)                              \
    {                                                       \
      VkResult result = (EXPRESSION);                       \
      if (result != VK_SUCCESS) {                           \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define VK_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#ifdef COMPILER_MSVC
#  define ALIGNAS(N) __declspec(align(N))
#else
#  define ALIGNAS(N) _Alignas(N)
#endif // COMPILER_SELECTION

#ifdef COMPILER_MSVC
#  define ALIGNOF(T) __alignof(T)
#else
#  define ALIGNOF(T) _Alignof(T)
#endif // COMPILER_SELECTION

#endif // MACROS_H
