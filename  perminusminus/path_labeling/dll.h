/* FILE dll.h */


#ifdef WIN32

#ifdef BUILD_DLL
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
#define EXPORT __declspec(dllimport)
#endif

#else
#define EXPORT
#endif
