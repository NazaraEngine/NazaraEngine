// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// I'm not proud of those five following lines but ti's hard to do with another way now
#ifdef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
	#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
#else
	#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif

#include <Nazara/Core/StackVector.hpp>
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
	* \class Nz::StackVector
	* \brief Core class that represents a stack-allocated (if alloca is present) vector, that is with a capacity different from its size
	*/
	template<typename T>
	StackVector<T>::StackVector(T* stackMemory, std::size_t capacity) :
	m_capacity(capacity),
	m_size(0),
	m_ptr(stackMemory)
	{
	}

	template<typename T>
	StackVector<T>::~StackVector()
	{
		clear();

		#ifndef NAZARA_ALLOCA_SUPPORT
		OperatorDelete(m_ptr);
		#endif
	}

	template<typename T>
	typename StackVector<T>::reference StackVector<T>::back()
	{
		assert(m_size != 0);
		return m_ptr[m_size - 1];
	}

	template<typename T>
	typename StackVector<T>::const_reference StackVector<T>::back() const
	{
		assert(m_size != 0);
		return m_ptr[m_size - 1];
	}

	template<typename T>
	typename StackVector<T>::iterator StackVector<T>::begin() noexcept
	{
		return iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackVector<T>::const_iterator StackVector<T>::begin() const noexcept
	{
		return const_iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackVector<T>::size_type StackVector<T>::capacity() const noexcept
	{
		return m_capacity;
	}

	template<typename T>
	void StackVector<T>::clear() noexcept
	{
		resize(0);
	}

	template<typename T>
	typename StackVector<T>::const_iterator StackVector<T>::cbegin() const noexcept
	{
		return const_iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackVector<T>::const_iterator StackVector<T>::cend() const noexcept
	{
		return const_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackVector<T>::const_reverse_iterator StackVector<T>::crbegin() const noexcept
	{
		return const_reverse_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackVector<T>::const_reverse_iterator StackVector<T>::crend() const noexcept
	{
		return const_reverse_iterator(&m_ptr[0]);
	}

	template<typename T>
	T* StackVector<T>::data() noexcept
	{
		return m_ptr;
	}

	template<typename T>
	const T* StackVector<T>::data() const noexcept
	{
		return m_ptr;
	}

	template<typename T>
	template<typename... Args>
	typename StackVector<T>::iterator StackVector<T>::emplace(const_iterator pos, Args&& ...args)
	{
		assert(m_size < m_capacity);
		assert(pos >= begin() && pos <= end());

		std::size_t index = std::distance<const_iterator>(begin(), pos);
		if (pos < end())
		{
			iterator lastElement = end() - 1;
			PlacementNew(&m_ptr[m_size], std::move(*lastElement));

			if (&m_ptr[index] < lastElement)
				std::move(&m_ptr[index], lastElement, &m_ptr[index + 1]);
		}
		m_size++;

		return PlacementNew(&m_ptr[index], std::forward<Args>(args)...);
	}

	template<typename T>
	template<typename... Args>
	typename StackVector<T>::reference Nz::StackVector<T>::emplace_back(Args&&... args)
	{
		assert(m_size < m_capacity);
		return *PlacementNew(&m_ptr[m_size++], std::forward<Args>(args)...);
	}

	template<typename T>
	bool StackVector<T>::empty() const noexcept
	{
		return m_size == 0;
	}

	template<typename T>
	typename StackVector<T>::iterator StackVector<T>::end() noexcept
	{
		return iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackVector<T>::const_iterator StackVector<T>::end() const noexcept
	{
		return const_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackVector<T>::iterator StackVector<T>::erase(const_iterator pos)
	{
		assert(pos < end());
		std::size_t index = std::distance(begin(), pos);
		std::move(pos + 1, end(), pos);
		pop_back();

		return iterator(&m_ptr[index]);
	}

	template<typename T>
	typename StackVector<T>::iterator StackVector<T>::erase(const_iterator first, const_iterator last)
	{
		if (first == last)
			return first;

		assert(first < last);
		assert(first >= begin() && last <= end());

		std::size_t index = std::distance(begin(), first);
		std::move(last, end(), first);
		resize(size() - (last - first));

		return iterator(&m_ptr[index]);
	}

	template<typename T>
	typename StackVector<T>::reference StackVector<T>::front() noexcept
	{
		return m_ptr[0];
	}

	template<typename T>
	typename StackVector<T>::const_reference StackVector<T>::front() const noexcept
	{
		return m_ptr[0];
	}

	template<typename T>
	typename StackVector<T>::iterator StackVector<T>::insert(const_iterator pos, const T& value)
	{
		return emplace(pos, value);
	}

	template<typename T>
	typename StackVector<T>::iterator StackVector<T>::insert(const_iterator pos, T&& value)
	{
		return emplace(pos, std::move(value));
	}

	template<typename T>
	typename StackVector<T>::size_type StackVector<T>::max_size() const noexcept
	{
		return capacity();
	}

	template<typename T>
	typename StackVector<T>::reference StackVector<T>::push_back(const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value)
	{
		assert(m_size < m_capacity);
		return *PlacementNew(&m_ptr[m_size++], value);
	}

	template<typename T>
	typename StackVector<T>::reference StackVector<T>::push_back(T&& value) noexcept(std::is_nothrow_move_constructible<T>::value)
	{
		assert(m_size < m_capacity);
		return *PlacementNew(&m_ptr[m_size++], std::move(value));
	}

	template<typename T>
	void StackVector<T>::pop_back()
	{
		assert(!empty());
		PlacementDestroy(&m_ptr[--m_size]);
	}

	template<typename T>
	void StackVector<T>::resize(size_type count)
	{
		assert(count < m_capacity);
		if (count > m_size)
		{
			for (std::size_t i = m_size; i < count; ++i)
				PlacementNew(&m_ptr[i]);

			m_size = count;
		}
		else if (count < m_size)
		{
			for (std::size_t i = count; i < m_size; ++i)
				PlacementDestroy(&m_ptr[i]);

			m_size = count;
		}
	}

	template<typename T>
	void StackVector<T>::resize(size_type count, const value_type& value)
	{
		assert(count < m_capacity);
		if (count > m_size)
		{
			for (std::size_t i = m_size; i < count; ++i)
				PlacementNew(&m_ptr[i], value);

			m_size = count;
		}
		else if (count < m_size)
		{
			for (std::size_t i = count; i < m_size; ++i)
				PlacementDestroy(&m_ptr[i]);

			m_size = count;
		}
	}

	template<typename T>
	typename StackVector<T>::reverse_iterator StackVector<T>::rbegin() noexcept
	{
		return reverse_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackVector<T>::const_reverse_iterator StackVector<T>::rbegin() const noexcept
	{
		return reverse_iterator(&m_ptr[m_size]);
	}

	template<typename T>
	typename StackVector<T>::reverse_iterator StackVector<T>::rend() noexcept
	{
		return reverse_iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackVector<T>::const_reverse_iterator StackVector<T>::rend() const noexcept
	{
		return reverse_iterator(&m_ptr[0]);
	}

	template<typename T>
	typename StackVector<T>::size_type StackVector<T>::size() const noexcept
	{
		return m_size;
	}

	template<typename T>
	typename StackVector<T>::reference StackVector<T>::operator[](size_type pos)
	{
		assert(pos < m_size);
		return m_ptr[pos];
	}

	template<typename T>
	typename StackVector<T>::const_reference StackVector<T>::operator[](size_type pos) const
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
