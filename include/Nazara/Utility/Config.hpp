/*
	Nazara Engine - Utility module

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

#ifndef NAZARA_CONFIG_UTILITY_HPP
#define NAZARA_CONFIG_UTILITY_HPP

/// Each modification of a parameter needs a recompilation of the module

// Use the MemoryManager to manage dynamic allocations (can detect memory leak but allocations/frees are slower)
#define NAZARA_UTILITY_MANAGE_MEMORY 0

// Activate the security tests based on the code (Advised for development)
#define NAZARA_UTILITY_SAFE 1

// When a resource is being parsed, it triggers a warning if a non-critical error is found in the resource (Slower)
#define NAZARA_UTILITY_STRICT_RESOURCE_PARSING 1

// Protect the classes against data race
//#define NAZARA_UTILITY_THREADSAFE 1

// Force the buffers to have a stride which is a multiple of 32 bytes (Gain of performances on certain cards/more memory consumption)
#define NAZARA_UTILITY_VERTEX_DECLARATION_FORCE_STRIDE_MULTIPLE_OF_32 0 ///FIXME: Can not be used for the moment

/// Each modification of a parameter following implies a modification (often minor) of the code

// The maximal number of weights acting on a vertex (In case of overflow, the surnumerous weights would be ignored and the others renormalized)
#define NAZARA_UTILITY_SKINNING_MAX_WEIGHTS 4

/// Checking the values and types of certain constants
#include <Nazara/Utility/ConfigCheck.hpp>

#if defined(NAZARA_STATIC)
	#define NAZARA_UTILITY_API
#else
	#ifdef NAZARA_UTILITY_BUILD
		#define NAZARA_UTILITY_API NAZARA_EXPORT
	#else
		#define NAZARA_UTILITY_API NAZARA_IMPORT
	#endif
#endif

#endif // NAZARA_CONFIG_UTILITY_HPP
