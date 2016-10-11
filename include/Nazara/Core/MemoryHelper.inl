// Copyright (C) 2015 Jérôme Leclercq
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
#include <new>
#include <utility>
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
	* \ingroup core
	* \class Nz::StackAllocation
	* \brief Core class that represents a stack allocation
	*/


	/*!
	* \brief Constructs a StackAllocation object with a pointer to a memory allocated with NAZARA_ALLOCA or OperatorNew is alloca is not supported
	*
	* \param ptr Pointer to raw memory
	*/
	inline StackAllocation::StackAllocation(void* stackMemory) :
	m_ptr(stackMemory)
	{
	}

	/*!
	* \brief Destructs the object and release memory if necessary
	*/
	inline StackAllocation::~StackAllocation()
	{
		#ifndef NAZARA_ALLOCA_SUPPORT
		OperatorDelete(m_ptr);
		#endif
	}

	/*!
	* \brief Access the internal pointer
	* \return internal memory pointer
	*/
	inline void* StackAllocation::GetPtr()
	{
		return m_ptr;
	}

	/*!
	* \brief Access the internal pointer
	* \return internal memory pointer
	*/
	inline StackAllocation::operator void*()
	{
		return m_ptr;
	}
}

#include <Nazara/Core/DebugOff.hpp>

// If we have defined the constant, then we have to undefine it (to avoid bloating in the engine)
#ifndef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
	#undef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif
