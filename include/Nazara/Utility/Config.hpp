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

/// Chaque modification d'un paramètre du module nécessite une recompilation de celui-ci

// Utilise un manager de mémoire pour gérer les allocations dynamiques (détecte les leaks au prix d'allocations/libérations dynamiques plus lentes)
#define NAZARA_UTILITY_MANAGE_MEMORY 0

// Active les tests de sécurité basés sur le code (Conseillé pour le développement)
#define NAZARA_UTILITY_SAFE 1

// Lors du parsage d'une ressource, déclenche un avertissement si une erreur non-critique est repérée dans une ressource (Plus lent)
#define NAZARA_UTILITY_STRICT_RESOURCE_PARSING 1

// Protège les classes des accès concurrentiels
//#define NAZARA_UTILITY_THREADSAFE 1

// Force les buffers à posséder un stride multiple de 32 bytes (Gain de performances sur certaines cartes/plus de consommation mémoire)
#define NAZARA_UTILITY_VERTEX_DECLARATION_FORCE_STRIDE_MULTIPLE_OF_32 0 ///FIXME: Ne peut pas être utilisé pour l'instant

// Sous Windows, fait en sorte que les touches ALT et F10 n'activent pas le menu de la fenêtre
#define NAZARA_UTILITY_WINDOWS_DISABLE_MENU_KEYS 1

/// Chaque modification d'un paramètre ci-dessous implique une modification (souvent mineure) du code

// Le nombre maximum de poids affectant un sommet (En cas de dépassement, les poids supplémentaires seront ignorés et les autres renormalisés)
#define NAZARA_UTILITY_SKINNING_MAX_WEIGHTS 4

/// Vérification des valeurs et types de certaines constantes
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
