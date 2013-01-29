/*
	Nazara Engine - Window module

	Copyright (C) 2012 Jérôme "Lynix" Leclercq (Lynix680@gmail.com)

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

/// Chaque modification d'un paramètre du module nécessite une recompilation de celui-ci

// Force les buffers à posséder un stride multiple de 32 bytes (Gain de performances sur certaines cartes/plus de consommation mémoire)
#define NAZARA_UTILITY_FORCE_DECLARATION_STRIDE_MULTIPLE_OF_32 0 ///FIXME: Ne peut pas être utilisé pour l'instant

// Utilise un tracker pour repérer les éventuels leaks (Ralentit l'exécution)
#define NAZARA_UTILITY_MEMORYLEAKTRACKER 0

// Le skinning doit-il prendre avantage du multi-threading ? (Boost de performances sur les processeurs multi-coeurs)
#define NAZARA_UTILITY_MULTITHREADED_SKINNING 1

// Active les tests de sécurité basés sur le code (Conseillé pour le développement)
#define NAZARA_UTILITY_SAFE 1

// Lors du parsage d'une ressource, déclenche un avertissement si une erreur non-critique est repérée dans une ressource (Plus lent)
#define NAZARA_UTILITY_STRICT_RESOURCE_PARSING 1

// Fait tourner chaque fenêtre dans un thread séparé si le système le supporte
#define NAZARA_UTILITY_THREADED_WINDOW 0

// Protège les classes des accès concurrentiels
#define NAZARA_UTILITY_THREADSAFE 1

// Les classes à protéger des accès concurrentiels
#define NAZARA_THREADSAFETY_IMAGE             1 // NzImage (COW)
#define NAZARA_THREADSAFETY_VERTEXDECLARATION 1 // NzVertexDeclaration (COW)

#endif // NAZARA_CONFIG_UTILITY_HPP
