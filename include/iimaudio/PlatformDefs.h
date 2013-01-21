/**
 * @file 	PlatformDefs.h
 *
 * @date 	20.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Platform specific definitions.
 */
#ifndef PLATFORMDEFS_H_
#define PLATFORMDEFS_H_

#ifdef __linux__
#define EXPORT
#else
#ifdef _WIN32
#ifdef iimaudio_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#else
#define EXPORT
#endif
#endif


#ifdef PACKED
#undef PACKED
#endif
#ifdef PACKED_POST
#undef PACKED_POST
#endif

#ifdef __linux__
#define PACKED_PRE
#define PACKED __attribute__((packed))
#else
#ifdef _WIN32
#define PACKED_PRE __pragma (pack (push,1))
#define PACKED __pragma (pack (pop))
#else
#define PACKED_PRE
#define PACKED
#endif
#endif

#endif
