// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// I'm not proud of those five following lines but ti's hard to do with another way now
#ifdef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
	#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
#else
	#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif

#include <Nazara/Core/MemoryHelper.hpp>
#include <Nazara/Core/MemoryManager.hpp>
#include <cassert>
#include <new>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \fn Nz::MemoryHelper
	* \brief Core functions that helps the handle of memory in the engine
	*/

	/*!
	* \brief Calls the operator delete on the pointer
	*
	* \remark Uses MemoryManager with NAZARA_CORE_MANAGE_MEMORY defined else operator delete
	*/
	inline void OperatorDelete(void* ptr)
	{
		#if NAZARA_CORE_MANAGE_MEMORY
		MemoryManager::Free(ptr);
		#else
		operator delete(ptr);
		#endif
	}

	/*!
	* \brief Calls the operator new on the pointer
	*
	* \remark Uses MemoryManager with NAZARA_CORE_MANAGE_MEMORY defined else operator new
	*/
	inline void* OperatorNew(std::size_t size)
	{
		#if NAZARA_CORE_MANAGE_MEMORY
		return MemoryManager::Allocate(size);
		#else
		return operator new(size);
		#endif
	}

	/*!
	* \brief Constructs the object inplace
	* \return Pointer to the constructed object
	*
	* \param ptr Pointer to raw memory allocated
	* \param args Arguments for the constructor
	*/
	template<typename T, typename... Args>
	T* PlacementNew(T* ptr, Args&&... args)
	{
		return new (ptr) T(std::forward<Args>(args)...);
	}

	/*!
	* \brief Calls the object destructor explicitly
	*
	* \param ptr Pointer to a previously constructed pointer on raw memory
	*
	* \remark This does not deallocate memory, and is a no-op on a null pointer
	*/
	template<typename T>
	void PlacementDestroy(T* ptr)
	{
		if (ptr)
			ptr->~T();
	}
}

#include <Nazara/Core/DebugOff.hpp>

// If we have defined the constant, then we have to undefine it (to avoid bloating in the engine)
#ifndef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
	#undef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif
