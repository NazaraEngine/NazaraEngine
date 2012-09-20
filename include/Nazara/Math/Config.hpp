/*
	Nazara Engine - Mathematics module

	Copyright (C) 2012 Jérôme "Lynix" Leclercq (Lynix680@gmail.com)
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

#ifndef NAZARA_CONFIG_MATH_HPP
#define NAZARA_CONFIG_MATH_HPP

/// Chaque modification d'un paramètre du module nécessite une recompilation de celui-ci

// Définit le radian comme l'unité utilisée pour les angles
#define NAZARA_MATH_ANGLE_RADIAN 0

// Optimise les opérations entre matrices affines (Demande plusieurs comparaisons pour déterminer si une matrice est affine)
#define NAZARA_MATH_MATRIX4_CHECK_AFFINE 0

// Active les tests de sécurité basés sur le code (Conseillé pour le développement)
#define NAZARA_MATH_SAFE 1

// Protège les classes des accès concurrentiels
#define NAZARA_MATH_THREADSAFE 1

// Les classes à protéger des accès concurrentiels
#define NAZARA_THREADSAFETY_MATRIX3 1  // NzMatrix3 (COW)
#define NAZARA_THREADSAFETY_MATRIX4 1  // NzMatrix4 (COW)

#endif // NAZARA_CONFIG_MATH_HPP
