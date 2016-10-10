/*
	Nazara Engine - Core module

	Copyright (C) 2015 Jérôme "Lynix" Leclercq (Lynix680@gmail.com)

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

#pragma once

#ifndef NAZARA_CONFIG_CORE_HPP
#define NAZARA_CONFIG_CORE_HPP

/*!
* \defgroup core (NazaraCore) Core module
*  Core/System module including classes to handle threading, time, hardware info, memory management, etc...
*/

/// Each modification of a parameter needs a recompilation of the module

// Precision of reals when transformed into string (Max. numbers after the coma)
#define NAZARA_CORE_DECIMAL_DIGITS 6

// Duplicate the log output on the standard output flux (cout)
#define NAZARA_CORE_DUPLICATE_LOG_TO_COUT 0

// Checks the assertions
#define NAZARA_CORE_ENABLE_ASSERTS 0

// Call exit when an assertion is invalid
#define NAZARA_CORE_EXIT_ON_ASSERT_FAILURE 1

// Size of buffer when reading entirely a file (ex: Hash)
#define NAZARA_CORE_FILE_BUFFERSIZE 4096

// Incorporate the Unicode Character Data table (Necessary to make it work with the flag String::HandleUTF8)
#define NAZARA_CORE_INCLUDE_UNICODEDATA 0

// Use the MemoryManager to manage dynamic allocations (can detect memory leak but allocations/frees are slower)
#define NAZARA_CORE_MANAGE_MEMORY 0

// Activate the security tests based on the code (Advised for development)
#define NAZARA_CORE_SAFE 1

// Protect the classes against data race
#define NAZARA_CORE_THREADSAFE 1

// Classes to protect against data race
#define NAZARA_THREADSAFETY_CLOCK 0        // Clock
#define NAZARA_THREADSAFETY_DIRECTORY 1    // Directory
#define NAZARA_THREADSAFETY_DYNLIB 1       // DynLib
#define NAZARA_THREADSAFETY_FILE 1         // File
#define NAZARA_THREADSAFETY_LOG 1          // Log
#define NAZARA_THREADSAFETY_REFCOUNTED 1   // RefCounted

// Number of spinlocks to use with the Windows critical sections (0 to disable)
#define NAZARA_CORE_WINDOWS_CS_SPINLOCKS 4096

// Optimize the Windows implementation with technologies of Windows NT 6.0 (and greater) (Break the compatibility with Windows XP)
#define NAZARA_CORE_WINDOWS_NT6 0


/*
// Sets the time between waking thread timers and activating a timer (in milliseconds)
#define NAZARA_CORE_TIMER_WAKEUPTIME 10
*/

/// Checking the values and types of certain constants
#include <Nazara/Core/ConfigCheck.hpp>

#if defined(NAZARA_STATIC)
	#define NAZARA_CORE_API
#else
	#ifdef NAZARA_CORE_BUILD
		#define NAZARA_CORE_API NAZARA_EXPORT
	#else
		#define NAZARA_CORE_API NAZARA_IMPORT
	#endif
#endif

#endif // NAZARA_CONFIG_CORE_HPP
