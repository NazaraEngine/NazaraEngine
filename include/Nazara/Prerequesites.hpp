/*
	Nazara Engine

	Copyright (C) 2013 Jérôme "Lynix" Leclercq (Lynix680@gmail.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#ifndef NAZARA_PREREQUESITES_HPP
#define NAZARA_PREREQUESITES_HPP

#if !defined(__cplusplus) || __cplusplus < 201103L
	#error Nazara requires a C++11 compliant compiler
#endif

// Version du moteur
#define NAZARA_VERSION_MAJOR 0
#define NAZARA_VERSION_MINOR 1

#include <Nazara/Core/Config.hpp>

///TODO: Rajouter des tests d'identification de compilateurs
// NAZARA_THREADLOCAL n'existe qu'en attendant le support complet de thread_local
#if defined(__BORLANDC__)
	#define NAZARA_COMPILER_BORDLAND
	#define NAZARA_DEPRECATED(txt)
	#define NAZARA_FUNCTION __FUNC__
	#define NAZARA_THREADLOCAL __declspec(thread)
#elif defined(__clang__)
	#define NAZARA_COMPILER_CLANG
	#define NAZARA_DEPRECATED(txt) __attribute__((__deprecated__(txt)))
	#define NAZARA_FUNCTION __PRETTY_FUNCTION__
	#define NAZARA_THREADLOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__MINGW32__)
	#define NAZARA_COMPILER_GCC
	#define NAZARA_DEPRECATED(txt) __attribute__((__deprecated__(txt)))
	#define NAZARA_FUNCTION __PRETTY_FUNCTION__
	#define NAZARA_THREADLOCAL __thread

	#ifdef __MINGW32__
		#define NAZARA_COMPILER_MINGW
		#ifdef __MINGW64_VERSION_MAJOR
			#define NAZARA_COMPILER_MINGW_W64
		#endif
	#endif
#elif defined(__INTEL_COMPILER) || defined(__ICL)
	#define NAZARA_COMPILER_INTEL
	#define NAZARA_DEPRECATED(txt)
	#define NAZARA_FUNCTION __FUNCTION__
	#define NAZARA_THREADLOCAL __thread
#elif defined(_MSC_VER)
	#define NAZARA_COMPILER_MSVC
	#define NAZARA_DEPRECATED(txt) __declspec(deprecated(txt))
	#define NAZARA_FUNCTION __FUNCSIG__
	#define NAZARA_THREADLOCAL __declspec(thread)
#else
	#define NAZARA_COMPILER_UNKNOWN
	#define NAZARA_DEPRECATED(txt)
	#define NAZARA_FUNCTION __func__ // __func__ est standard depuis le C++11
	#define NAZARA_THREADLOCAL thread_local
	#error This compiler is not fully supported
#endif

#define NazaraUnused(a) (void) a

#if defined(_WIN32)
	#define NAZARA_PLATFORM_WINDOWS

	#if !defined(NAZARA_STATIC)
		#ifdef NAZARA_BUILD
			#define NAZARA_API __declspec(dllexport)
		#else
			#define NAZARA_API __declspec(dllimport)
		#endif
	#else
		#define NAZARA_API
	#endif

	// Des defines pour le header Windows
	#if defined(NAZARA_BUILD) // Pour ne pas entrer en conflit avec les defines de l'application ou d'une autre bibliothèque
		#ifndef WIN32_LEAN_AND_MEAN
			#define WIN32_LEAN_AND_MEAN
		#endif

		#ifndef NOMINMAX
			#define NOMINMAX
		#endif

		#if NAZARA_CORE_WINDOWS_VISTA
			// Version de Windows minimale : Vista
			#define NAZARA_WINNT 0x0600
		#else
			#define NAZARA_WINNT 0x0501
		#endif

		// Pour ne pas casser le define déjà en place s'il est applicable
		#if defined(_WIN32_WINNT)
			#if _WIN32_WINNT < NAZARA_WINNT
				#undef _WIN32_WINNT
				#define _WIN32_WINNT NAZARA_WINNT
			#endif
		#else
			#define _WIN32_WINNT NAZARA_WINNT
		#endif
	#endif
#elif defined(__linux__)
	#define NAZARA_PLATFORM_LINUX
	#define NAZARA_PLATFORM_POSIX

	#if !defined(NAZARA_STATIC) && defined(NAZARA_COMPILER_GCC)
		#define NAZARA_API __attribute__((visibility ("default")))
	#else
		#define NAZARA_API
	#endif
/*#elif defined(__APPLE__) && defined(__MACH__)
	#define NAZARA_API
	#define NAZARA_PLATFORM_MACOSX
	#define NAZARA_PLATFORM_POSIX*/
#else
	// À commenter pour tenter quand même une compilation
	#error This operating system is not fully supported by the Nazara Engine

	#define NAZARA_PLATFORM_UNKNOWN
	#define NAZARA_API
#endif

#if !defined(NAZARA_PLATFORM_x64) && (defined(_WIN64) ||  defined(__amd64__) || defined(__x86_64__) || defined(__ia64__) || defined(__ia64) || \
    defined(_M_IA64) || defined(__itanium__) || defined(__MINGW64__))
	#define NAZARA_PLATFORM_x64
#endif

#if !defined(NAZARA_DEBUG) && !defined(NDEBUG)
	#define NDEBUG
#endif

#include <cstdint>

static_assert(sizeof(int8_t)  == 1, "int8_t is not of the correct size" );
static_assert(sizeof(int16_t) == 2, "int16_t is not of the correct size");
static_assert(sizeof(int32_t) == 4, "int32_t is not of the correct size");
static_assert(sizeof(int64_t) == 8, "int64_t is not of the correct size");

static_assert(sizeof(uint8_t)  == 1, "uint8_t is not of the correct size" );
static_assert(sizeof(uint16_t) == 2, "uint16_t is not of the correct size");
static_assert(sizeof(uint32_t) == 4, "uint32_t is not of the correct size");
static_assert(sizeof(uint64_t) == 8, "uint64_t is not of the correct size");

typedef int8_t nzInt8;
typedef uint8_t nzUInt8;

typedef int16_t nzInt16;
typedef uint16_t nzUInt16;

typedef int32_t nzInt32;
typedef uint32_t nzUInt32;

typedef int64_t nzInt64;
typedef uint64_t nzUInt64;

#endif // NAZARA_PREREQUESITES_HPP
