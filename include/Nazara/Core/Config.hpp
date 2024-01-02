/*
	Nazara Engine - Core module

	Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)

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

#ifndef NAZARA_CORE_CONFIG_HPP
#define NAZARA_CORE_CONFIG_HPP

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
#define NAZARA_CORE_INCLUDE_UNICODEDATA 1

// Activate the security tests based on the code (Advised for development)
#define NAZARA_CORE_SAFE 1

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

#endif // NAZARA_CORE_CONFIG_HPP
