// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_PREREQUESITES_HPP
#define NAZARA_PREREQUESITES_HPP

// Version du moteur
#define NAZARA_VERSION_MAJOR 0
#define NAZARA_VERSION_MINOR 1

// (Commenté en attendant GCC 4.7)
/*#if __cplusplus < 201103L
#error Nazara requires a C++11 compliant compiler
#endif*/

///TODO: Rajouter des tests d'identification de compilateurs
#if defined(_MSC_VER)
	#define NAZARA_COMPILER_MSVC
	#define NAZARA_DEPRECATED(txt) __declspec(deprecated(txt))
	#define NAZARA_FUNCTION __FUNCSIG__
#elif defined(__GNUC__)
	#define NAZARA_COMPILER_GCC
	#define NAZARA_FUNCTION __PRETTY_FUNCTION__

	#define NAZARA_DEPRECATED(txt) __attribute__((__deprecated__(txt)))
#elif defined(__BORLANDC__)
	#define NAZARA_COMPILER_BORDLAND
	#define NAZARA_DEPRECATED(txt)
	#define NAZARA_FUNCTION __FUNC__
#else
	#define NAZARA_COMPILER_UNKNOWN
	#define NAZARA_DEPRECATED(txt)
	#define NAZARA_FUNCTION __func__ // __func__ est standard depuis le C++11
	#error This compiler is not fully supported
#endif

#define NazaraUnused(a) (void) a

#if defined(_WIN32) || defined(__WIN32__) || defined(NAZARA_PLATFORM_WINDOWSVISTA)
	#if !defined(NAZARA_STATIC)
		#ifdef NAZARA_BUILD
			#define NAZARA_API __declspec(dllexport)
		#else
			#define NAZARA_API __declspec(dllimport)
		#endif
	#else
		#define NAZARA_API
	#endif
	#define NAZARA_PLATFORM_WINDOWS

	// Des defines pour le header Windows
	#if defined(NAZARA_BUILD) // Pour ne pas entrer en conflit avec les defines de l'application ou d'une autre bibliothèque
		#ifndef WIN32_LEAN_AND_MEAN
			#define WIN32_LEAN_AND_MEAN
		#endif

		#ifndef NOMINMAX
			#define NOMINMAX
		#endif

		#ifndef _WIN32_WINNT
			#ifdef NAZARA_PLATFORM_WINDOWSVISTA
				#define _WIN32_WINNT 0x0600 // Version de Windows minimale : Vista
			#else
				#define _WIN32_WINNT 0x0501 // Version de Windows minimale : XP
			#endif
		#endif
	#endif
#elif defined(linux) || defined(__linux)
	#if !defined(NAZARA_STATIC) && defined(NAZARA_COMPILER_GCC)
		#define NAZARA_API __attribute__((visibility ("default")))
	#else
		#define NAZARA_API
	#endif
	#define NAZARA_PLATFORM_LINUX
	#define NAZARA_PLATFORM_POSIX
/*#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
	#define NAZARA_API
	#define NAZARA_PLATFORM_MACOS
	#define NAZARA_PLATFORM_POSIX
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
	#define NAZARA_API
	#define NAZARA_PLATFORM_FREEBSD
	#define NAZARA_PLATFORM_POSIX*/
#else
	// À commenter pour tenter quand même une compilation
	#error This operating system is not fully supported by the Nazara Engine

	#define NAZARA_PLATFORM_UNKNOWN
	#define NAZARA_API
#endif

#if !defined(NAZARA_DEBUG) && !defined(NDEBUG)
	#define NDEBUG
#endif

#include <cstdint>

typedef int8_t nzInt8;
typedef uint8_t nzUInt8;

typedef int16_t nzInt16;
typedef uint16_t nzUInt16;

typedef int32_t nzInt32;
typedef uint32_t nzUInt32;

typedef int64_t nzInt64;
typedef uint64_t nzUInt64;

#endif // NAZARA_PREREQUESITES_HPP
