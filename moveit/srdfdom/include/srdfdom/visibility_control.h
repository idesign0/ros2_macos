#ifndef SRDFDOM__VISIBILITY_CONTROL_H_
#define SRDFDOM__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define SRDFDOM_EXPORT __attribute__((dllexport))
#define SRDFDOM_IMPORT __attribute__((dllimport))
#else
#define SRDFDOM_EXPORT __declspec(dllexport)
#define SRDFDOM_IMPORT __declspec(dllimport)
#endif
#ifdef SRDFDOM_BUILDING_DLL
#define SRDFDOM_PUBLIC SRDFDOM_EXPORT
#else
#define SRDFDOM_PUBLIC SRDFDOM_IMPORT
#endif
#define SRDFDOM_PUBLIC_TYPE SRDFDOM_PUBLIC
#define SRDFDOM_LOCAL
#else
#define SRDFDOM_EXPORT __attribute__((visibility("default")))
#define SRDFDOM_IMPORT
#if __GNUC__ >= 4
#define SRDFDOM_PUBLIC __attribute__((visibility("default")))
#define SRDFDOM_LOCAL __attribute__((visibility("hidden")))
#else
#define SRDFDOM_PUBLIC
#define SRDFDOM_LOCAL
#endif
#define SRDFDOM_PUBLIC_TYPE
#endif

#endif  // SRDFDOM__VISIBILITY_CONTROL_H_
