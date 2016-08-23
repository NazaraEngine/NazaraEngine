/*
	Nazara Engine - Vulkan

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

#ifndef NAZARA_CONFIG_VULKANRENDERER_HPP
#define NAZARA_CONFIG_VULKANRENDERER_HPP

/// Chaque modification d'un paramètre du module nécessite une recompilation de celui-ci

// Utilise le MemoryManager pour gérer les allocations dynamiques (détecte les leaks au prix d'allocations/libérations dynamiques plus lentes)
#define NAZARA_VULKANRENDERER_MANAGE_MEMORY 0

// Active les tests de sécurité basés sur le code (Conseillé pour le développement)
#define NAZARA_VULKANRENDERER_SAFE 1

/// Chaque modification d'un paramètre ci-dessous implique une modification (souvent mineure) du code

/// Vérification des valeurs et types de certaines constantes
#include <Nazara/VulkanRenderer/ConfigCheck.hpp>

#if !defined(NAZARA_STATIC)
	#ifdef NAZARA_VULKANRENDERER_BUILD
		#define NAZARA_VULKANRENDERER_API NAZARA_EXPORT
	#else
		#define NAZARA_VULKANRENDERER_API NAZARA_IMPORT
	#endif
#else
	#define NAZARA_VULKANRENDERER_API
#endif

#endif // NAZARA_CONFIG_MODULENAME_HPP
