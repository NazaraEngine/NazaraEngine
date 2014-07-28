// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Je ne suis pas fier des cinq lignes qui suivent mais difficile de faire autrement pour le moment...
#ifdef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
	#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
#else
	#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif

#include <Nazara/Core/MemoryManager.hpp>
#include <new>
#include <Nazara/Core/Debug.hpp>

inline void NzOperatorDelete(void* ptr)
{
	#if NAZARA_CORE_MANAGE_MEMORY
	NzMemoryManager::Free(ptr);
	#else
	operator delete(ptr);
	#endif
}

inline void* NzOperatorNew(std::size_t size)
{
	#if NAZARA_CORE_MANAGE_MEMORY
	return NzMemoryManager::Allocate(size);
	#else
	return operator new(size);
	#endif
}

template<typename T, typename... Args>
T* NzPlacementNew(void* ptr, Args... args)
{
	return new (ptr) T(std::forward<Args>(args)...);
}

#include <Nazara/Core/DebugOff.hpp>

// Si c'est nous qui avons défini la constante, alors il nous faut l'enlever (Pour éviter que le moteur entier n'en souffre)
#ifndef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
	#undef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif
