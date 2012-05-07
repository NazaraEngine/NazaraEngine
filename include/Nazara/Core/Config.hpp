/*
	Nazara Engine

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

#ifndef NAZARA_CONFIG_CORE_HPP
#define NAZARA_CONFIG_CORE_HPP

/// Chaque modification d'un paramètre du module nécessite une recompilation de celui-ci

// Appelle exit dès qu'une assertion est invalide
#define NAZARA_CORE_EXIT_ON_ASSERT_FAILURE 1

// Teste les assertions
#define NAZARA_CORE_ENABLE_ASSERTS 0

// Taille du buffer lors d'une lecture complète d'un fichier (ex: Hash)
#define NAZARA_CORE_FILE_BUFFERSIZE 4096

// Le moteur doit-il incorporer les Unicode Character Data (Nécessaires pour faire fonctionner le flag NzString::HandleUTF8)
#define NAZARA_CORE_INCLUDE_UNICODEDATA 0

// Utilise un tracker pour repérer les éventuels leaks (Ralentit l'exécution)
#define NAZARA_CORE_MEMORYLEAKTRACKER 0

// Standardise les séparateurs des dossiers selon le système d'exploitation courant
#define NAZARA_CORE_NORMALIZE_DIRECTORY_SEPARATORS 1

// Précision des réels lors de la transformation en texte (Max. chiffres après la virgule)
#define NAZARA_CORE_REAL_PRECISION 6

// Redirige la sortie du log sur le flux d'erreur standard (cerr) en cas d'erreur d'écriture (ou d'ouverture du fichier)
#define NAZARA_CORE_REDIRECT_TO_CERR_ON_LOG_FAILURE 1

// Active les tests de sécurité basés sur le code (Conseillé pour le développement)
#define NAZARA_CORE_SAFE 1

// Protége le module des accès concurrentiels
#define NAZARA_CORE_THREADSAFE 1

#if NAZARA_CORE_THREADSAFE
	#define NAZARA_THREADSAFETY_BYTEARRAY 1    // NzByteArray (COW)
	#define NAZARA_THREADSAFETY_CLOCK 0        // NzClock
	#define NAZARA_THREADSAFETY_DIRECTORY 1    // NzDirectory
	#define NAZARA_THREADSAFETY_DYNLIB 1       // NzDynLib
	#define NAZARA_THREADSAFETY_FILE 1         // NzFile
	#define NAZARA_THREADSAFETY_HASHDIGEST 0   // NzHashDigest
	#define NAZARA_THREADSAFETY_LOG 1          // NzLog
	#define NAZARA_THREADSAFETY_STRING 1       // NzString (COW)
	#define NAZARA_THREADSAFETY_STRINGSTREAM 0 // NzStringStream
#endif

/*
// Règle le temps entre le réveil du thread des timers et l'activation d'un timer (En millisecondes)
#define NAZARA_CORE_TIMER_WAKEUPTIME 10
*/

#endif // NAZARA_CONFIG_CORE_HPP
