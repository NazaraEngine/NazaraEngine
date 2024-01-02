/*
	Nazara Engine - Math module

	Copyright (C) 2024 Jérôme "SirLynix" Leclercq (Lynix680@gmail.com)
	                   Rémi "overdrivr" Bèges (remi.beges@laposte.net)

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

#ifndef NAZARA_MATH_CONFIG_HPP
#define NAZARA_MATH_CONFIG_HPP

/*!
* \defgroup math (NazaraMath) Mathematics module
*  2D/3D mathematics module including matrix, vector, box, sphere, quaternion, ...
*/

/// Each modification of a parameter of the module needs a recompilation of the unit

// Checks for transform matrix on common operations (Concatenate, Inverse, ...) and warns if the operation could be optimized
// also checks if transform calls are called on transform matrices
#define NAZARA_MATH_MATRIX4_CHECK_TRANSFORM 0

// Enable tests of security based on the code (Advised for the development)
#define NAZARA_MATH_SAFE 1

#endif // NAZARA_MATH_CONFIG_HPP
