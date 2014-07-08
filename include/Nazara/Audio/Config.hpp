/*
	Nazara Engine - Audio module

	Copyright (C) 2014 Jérôme "Lynix" Leclercq (Lynix680@gmail.com)

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

#ifndef NAZARA_CONFIG_AUDIO_HPP
#define NAZARA_CONFIG_AUDIO_HPP

/// Modifier la configuration d'un module nécessite une recompilation quasi-intégrale de celui-ci et de ceux en héritant

// Utilise un manager de mémoire pour gérer les allocations dynamiques (détecte les leaks au prix d'allocations/libérations dynamiques plus lentes)
#define NAZARA_AUDIO_MANAGE_MEMORY 0

// Active les tests de sécurité supplémentaires (Teste notamment les arguments des fonctions, conseillé pour le développement)
#define NAZARA_AUDIO_SAFE 1

// Le nombre de buffers utilisés lors du streaming d'objets audio (Au moins deux)
#define NAZARA_AUDIO_STREAMED_BUFFER_COUNT 2

/// Vérification des valeurs et types de certaines constantes
#include <Nazara/Audio/ConfigCheck.hpp>

#endif // NAZARA_CONFIG_AUDIO_HPP
