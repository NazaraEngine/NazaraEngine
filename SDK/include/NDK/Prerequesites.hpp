/*
	Nazara Development Kit ("NDK"), also called Nazara Engine - SDK ("Software Development Kit")

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

#ifndef NDK_PREREQUESITES_HPP
#define NDK_PREREQUESITES_HPP

/*!
* \defgroup NDK (NazaraSDK) Nazara Development Kit
*  A library grouping every modules of Nazara into multiple higher-level features suchs as scene management (handled by an ECS), application, lua binding, etc.
*/

#include <Nazara/Prerequesites.hpp>

// Importation/Exportation of the API
#if defined(NAZARA_STATIC)
  #define NDK_API
#else
  #ifdef NDK_BUILD
	  #define NDK_API NAZARA_EXPORT
  #else
	  #define NDK_API NAZARA_IMPORT
	#endif
#endif

namespace Ndk
{
	using ComponentId = Nz::UInt64;
	using ComponentIndex = Nz::UInt32;
	using EntityId = Nz::UInt32;
	using SystemIndex = Nz::UInt32;
}

#endif // NDK_PREREQUESITES_HPP
