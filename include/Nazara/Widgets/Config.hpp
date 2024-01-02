/*
	Nazara Engine - Widgets module

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

#ifndef NAZARA_WIDGETS_CONFIG_HPP
#define NAZARA_WIDGETS_CONFIG_HPP

/*!
* \defgroup widgets (NazaraWidgets) Widgets module
*  Widgets module including classes to handle widgets...
*/

/// Each modification of a parameter needs a recompilation of the module

/// Checking the values and types of certain constants
#include <Nazara/Widgets/ConfigCheck.hpp>

#if !defined(NAZARA_STATIC)
	#ifdef NAZARA_WIDGETS_BUILD
		#define NAZARA_WIDGETS_API NAZARA_EXPORT
	#else
		#define NAZARA_WIDGETS_API NAZARA_IMPORT
	#endif
#else
	#define NAZARA_WIDGETS_API
#endif

#endif // NAZARA_WIDGETS_CONFIG_HPP
