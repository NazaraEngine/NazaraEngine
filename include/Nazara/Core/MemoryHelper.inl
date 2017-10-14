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
#include <algorithm>
#include <cassert>
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
	* \brief Calls the object destructor explicitly
	*
	* \param ptr Pointer to a previously constructed pointer on raw memory
	*/
	template<typename T>
	void PlacementDestroy(T* ptr)
	{
		ptr->~T();
	}

	/*!
	* \ingroup core
	* \class Nz::StackArray
	* \brief Core class that represents a stack-allocated (if alloca is present) array
	*/

	template<typename T>
	StackArray<T>::StackArray(T* stackMemory, std::size_t size) :
	m_size(size),
	m_ptr(stackMemory)
	{
		for (std::size_t i = 0; i < m_size; ++i)
			PlacementNew(&m_ptr[i]);
	}

	template<typename T>
	StackArray<T>::StackArray(T* stackMemory, std::size_t size, NoInitTag) :
	m_size(size),
	m_ptr(stackMemory)
	{
	}

	template<typename T>
	StackArray<T>::~StackArray()
	{
		for (std::size_t i = 0; i < m_size; ++i)
			m_ptr[i].~T();

		#ifndef NAZARA_ALLOCA_SUPPORT
		OperatorDelete(m_ptr);
		#endif
	}

	template<typename T>
	typename StackArray<T>::reference StackArray<T>::back()
	{
		assert(m_size != 0);
		return m_ptr[m_size - 1];
	}

	template<typename T>
	typename StackArray<T>::const_reference StackArray<T>::back() const
	{
		assert(m_size != 0);
		return m_ptr[m_size - 1];
	}

	template<typename T>
	typename StackArray<T>::iterator StackArray<T>::begin() noexcept
	{
		return iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackArray<T>::const_iterator StackArray<T>::begin() const noexcept
	{
		return const_iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackArray<T>::const_iterator StackArray<T>::cbegin() const noexcept
	{
		return const_iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackArray<T>::const_iterator StackArray<T>::cend() const noexcept
	{
		return const_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackArray<T>::const_reverse_iterator StackArray<T>::crbegin() const noexcept
	{
		return const_reverse_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackArray<T>::const_reverse_iterator StackArray<T>::crend() const noexcept
	{
		return const_reverse_iterator(&m_ptr[0]);
	}

	template<typename T>
	T* StackArray<T>::data() noexcept
	{
		return m_ptr;
	}

	template<typename T>
	const T* StackArray<T>::data() const noexcept
	{
		return m_ptr;
	}

	template<typename T>
	bool StackArray<T>::empty() const noexcept
	{
		return m_size == 0;
	}

	template<typename T>
	typename StackArray<T>::iterator StackArray<T>::end() noexcept
	{
		return iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackArray<T>::const_iterator StackArray<T>::end() const noexcept
	{
		return const_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	void StackArray<T>::fill(const T& value)
	{
		std::fill(begin(), end(), value);
	}

	template<typename T>
	typename StackArray<T>::reference StackArray<T>::front() noexcept
	{
		return m_ptr[0];
	}

	template<typename T>
	typename StackArray<T>::const_reference StackArray<T>::front() const noexcept
	{
		return m_ptr[0];
	}

	template<typename T>
	typename StackArray<T>::size_type StackArray<T>::max_size() const noexcept
	{
		return size();
	}

	template<typename T>
	typename StackArray<T>::reverse_iterator StackArray<T>::rbegin() noexcept
	{
		return reverse_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackArray<T>::const_reverse_iterator StackArray<T>::rbegin() const noexcept
	{
		return reverse_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackArray<T>::reverse_iterator StackArray<T>::rend() noexcept
	{
		return reverse_iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackArray<T>::const_reverse_iterator StackArray<T>::rend() const noexcept
	{
		return reverse_iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackArray<T>::size_type StackArray<T>::size() const noexcept
	{
		return m_size;
	}

	template<typename T>
	typename StackArray<T>::reference StackArray<T>::operator[](size_type pos)
	{
		assert(pos < m_size);
		return m_ptr[pos];
	}

	template<typename T>
	typename StackArray<T>::const_reference StackArray<T>::operator[](size_type pos) const
	{
		assert(pos < m_size);
		return m_ptr[pos];
	}
}

#include <Nazara/Core/DebugOff.hpp>

// If we have defined the constant, then we have to undefine it (to avoid bloating in the engine)
#ifndef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
	#undef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif
