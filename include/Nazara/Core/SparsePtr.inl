// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/SparsePtr.hpp>
#include <cassert>
#include <iterator>
#include <limits>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::SparsePtr
	* \brief Core class that represents a pointer and the step between two elements
	*/

	/*!
	* \brief Constructs a SparsePtr object by default
	*/

	template<typename T>
	SparsePtr<T>::SparsePtr()
	{
		Reset();
	}

	/*!
	* \brief Constructs a SparsePtr object with a pointer
	*
	* \param ptr Pointer to data
	*/

	template<typename T>
	SparsePtr<T>::SparsePtr(T* ptr)
	{
		Reset(ptr);
	}

	/*!
	* \brief Constructs a SparsePtr object with a pointer and a step
	*
	* \param ptr Pointer to data
	* \param stride Step between two elements
	*/

	template<typename T>
	SparsePtr<T>::SparsePtr(VoidPtr ptr, int stride)
	{
		Reset(ptr, stride);
	}

	/*!
	* \brief Constructs a SparsePtr object with a pointer and a step
	*
	* \param ptr Pointer to data
	* \param stride Step between two elements
	*
	* \remark This constructor only exists because std::size_t is a frequent type for constructing this object, but stride may not be higher than int max
	*/

	template<typename T>
	SparsePtr<T>::SparsePtr(VoidPtr ptr, std::size_t stride)
	{
		assert(stride <= static_cast<unsigned int>(std::numeric_limits<int>::max()));
		Reset(ptr, static_cast<int>(stride));
	}

	/*!
	* \brief Constructs a SparsePtr object from another type of SparsePtr
	*
	* \param ptr Pointer to data of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	SparsePtr<T>::SparsePtr(const SparsePtr<U>& ptr)
	{
		Reset(ptr);
	}

	/*!
	* \brief Gets the original pointer
	* \return Pointer to the first data
	*/

	template<typename T>
	typename SparsePtr<T>::VoidPtr SparsePtr<T>::GetPtr() const
	{
		return m_ptr;
	}

	/*!
	* \brief Gets the stride
	* \return Step between two elements
	*/

	template<typename T>
	int SparsePtr<T>::GetStride() const
	{
		return m_stride;
	}

	/*!
	* \brief Resets the SparsePtr
	*/

	template<typename T>
	void SparsePtr<T>::Reset()
	{
		SetPtr(nullptr);
		SetStride(0);
	}

	/*!
	* \brief Resets the SparsePtr with a pointer
	*
	* \param ptr Pointer to data
	*
	* \remark stride is set to sizeof(T)
	*/

	template<typename T>
	void SparsePtr<T>::Reset(T* ptr)
	{
		SetPtr(ptr);
		SetStride(sizeof(T));
	}

	/*!
	* \brief Resets the SparsePtr with a pointer and its stride
	*
	* \param ptr Pointer to data
	* \param stride Step between two elements
	*/

	template<typename T>
	void SparsePtr<T>::Reset(VoidPtr ptr, int stride)
	{
		SetPtr(ptr);
		SetStride(stride);
	}

	/*!
	* \brief Resets the SparsePtr with another SparsePtr
	*
	* \param ptr Another sparsePtr
	*/

	template<typename T>
	void SparsePtr<T>::Reset(const SparsePtr& ptr)
	{
		SetPtr(ptr.GetPtr());
		SetStride(ptr.GetStride());
	}

	/*!
	* \brief Resets the SparsePtr with another type of SparsePtr
	*
	* \param ptr Another sparsePtr
	*/

	template<typename T>
	template<typename U>
	void SparsePtr<T>::Reset(const SparsePtr<U>& ptr)
	{
		static_assert(std::is_convertible<U*, T*>::value, "Source type pointer cannot be implicitely converted to target type pointer");

		SetPtr(static_cast<T*>(ptr.GetPtr()));
		SetStride(ptr.GetStride());
	}

	/*!
	* \brief Sets the pointer
	*
	* \param ptr Pointer to data
	*/

	template<typename T>
	void SparsePtr<T>::SetPtr(VoidPtr ptr)
	{
		m_ptr = static_cast<BytePtr>(ptr);
	}

	/*!
	* \brief Sets the stride
	*
	* \param stride Step between two elements
	*/

	template<typename T>
	void SparsePtr<T>::SetStride(int stride)
	{
		m_stride = stride;
	}

	/*!
	* \brief Converts the pointer to bool
	* \return true if pointer is not nullptr
	*/

	template<typename T>
	SparsePtr<T>::operator bool() const
	{
		return m_ptr != nullptr;
	}

	/*!
	* \brief Converts the pointer to a pointer to the value
	* \return The value of the pointer
	*/

	template<typename T>
	SparsePtr<T>::operator T*() const
	{
		return reinterpret_cast<T*>(m_ptr);
	}

	/*!
	* \brief Dereferences the pointer
	* \return The dereferencing of the pointer
	*/

	template<typename T>
	T& SparsePtr<T>::operator*() const
	{
		return *reinterpret_cast<T*>(m_ptr);
	}

	/*!
	* \brief Dereferences the pointer
	* \return The dereferencing of the pointer
	*/

	template<typename T>
	T* SparsePtr<T>::operator->() const
	{
		return reinterpret_cast<T*>(m_ptr);
	}

	/*!
	* \brief Gets the ith element of the stride pointer
	* \return A reference to the ith value
	*
	* \param index Number of stride to do
	*/

	template<typename T>
	T& SparsePtr<T>::operator[](int index) const
	{
		return *reinterpret_cast<T*>(m_ptr + index * m_stride);
	}

	/*!
	* \brief Gets the SparsePtr with an offset
	* \return A SparsePtr with the new stride
	*
	* \param count Number of stride to do
	*/

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator+(int count) const
	{
		return SparsePtr(m_ptr + count * m_stride, m_stride);
	}

	/*!
	* \brief Gets the SparsePtr with an offset
	* \return A SparsePtr with the new stride
	*
	* \param count Number of stride to do
	*/

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator+(unsigned int count) const
	{
		return SparsePtr(m_ptr + count * m_stride, m_stride);
	}

	/*!
	* \brief Gets the SparsePtr with an offset
	* \return A SparsePtr with the new stride
	*
	* \param count Number of stride to do
	*/

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator-(int count) const
	{
		return SparsePtr(m_ptr - count * m_stride, m_stride);
	}

	/*!
	* \brief Gets the SparsePtr with an offset
	* \return A SparsePtr with the new stride
	*
	* \param count Number of stride to do
	*/

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator-(unsigned int count) const
	{
		return SparsePtr(m_ptr - count * m_stride, m_stride);
	}

	/*!
	* \brief Gets the difference between the two SparsePtr
	* \return The difference of elements: ptr - this->ptr
	*
	* \param ptr Other ptr
	*/

	template<typename T>
	std::ptrdiff_t SparsePtr<T>::operator-(const SparsePtr& ptr) const
	{
		return (m_ptr - ptr.m_ptr) / m_stride;
	}

	/*!
	* \brief Gets the SparsePtr with an offset
	* \return A reference to this pointer with the new stride
	*
	* \param count Number of stride to do
	*/

	template<typename T>
	SparsePtr<T>& SparsePtr<T>::operator+=(int count)
	{
		m_ptr += count * m_stride;

		return *this;
	}

	/*!
	* \brief Gets the SparsePtr with an offset
	* \return A reference to this pointer with the new stride
	*
	* \param count Number of stride to do
	*/

	template<typename T>
	SparsePtr<T>& SparsePtr<T>::operator-=(int count)
	{
		m_ptr -= count * m_stride;

		return *this;
	}

	/*!
	* \brief Gets the SparsePtr with the next element
	* \return A reference to this pointer updated
	*/

	template<typename T>
	SparsePtr<T>& SparsePtr<T>::operator++()
	{
		m_ptr += m_stride;

		return *this;
	}

	/*!
	* \brief Gets the SparsePtr with the next element
	* \return A SparsePtr not updated
	*/

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator++(int)
	{
		// We copy the object
		SparsePtr tmp(*this);

		// We modify it
		operator++();

		// We return the copy
		return tmp;
	}

	/*!
	* \brief Gets the SparsePtr with the previous element
	* \return A reference to this pointer updated
	*/

	template<typename T>
	SparsePtr<T>& SparsePtr<T>::operator--()
	{
		m_ptr -= m_stride;
		return *this;
	}

	/*!
	* \brief Gets the SparsePtr with the previous element
	* \return A SparsePtr not updated
	*/

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator--(int)
	{
		// We copy the object
		SparsePtr tmp(*this);

		// We modify it
		operator--();

		// We return the copy
		return tmp;
	}

	/*!
	* \brief Compares the SparsePtr to another one
	* \return true if the two SparsePtr are pointing to the same memory
	*
	* \param ptr Other SparsePtr to compare with
	*/

	template<typename T>
	bool SparsePtr<T>::operator==(const SparsePtr& ptr) const
	{
		return m_ptr == ptr.m_ptr;
	}

	/*!
	* \brief Compares the SparsePtr to another one
	* \return false if the two SparsePtr are pointing to the same memory
	*
	* \param ptr Other SparsePtr to compare with
	*/

	template<typename T>
	bool SparsePtr<T>::operator!=(const SparsePtr& ptr) const
	{
		return m_ptr != ptr.m_ptr;
	}

	/*!
	* \brief Compares the SparsePtr to another one
	* \return true if the first SparsePtr is pointing to memory inferior to the second one
	*
	* \param ptr Other SparsePtr to compare with
	*/

	template<typename T>
	bool SparsePtr<T>::operator<(const SparsePtr& ptr) const
	{
		return m_ptr < ptr.m_ptr;
	}

	/*!
	* \brief Compares the SparsePtr to another one
	* \return true if the first SparsePtr is pointing to memory superior to the second one
	*
	* \param ptr Other SparsePtr to compare with
	*/

	template<typename T>
	bool SparsePtr<T>::operator>(const SparsePtr& ptr) const
	{
		return m_ptr > ptr.m_ptr;
	}

	/*!
	* \brief Compares the SparsePtr to another one
	* \return true if the first SparsePtr is pointing to memory inferior or equal to the second one
	*
	* \param ptr Other SparsePtr to compare with
	*/

	template<typename T>
	bool SparsePtr<T>::operator<=(const SparsePtr& ptr) const
	{
		return m_ptr <= ptr.m_ptr;
	}

	/*!
	* \brief Compares the SparsePtr to another one
	* \return true if the first SparsePtr is pointing to memory superior or equal to the second one
	*
	* \param ptr Other SparsePtr to compare with
	*/

	template<typename T>
	bool SparsePtr<T>::operator>=(const SparsePtr& ptr) const
	{
		return m_ptr >= ptr.m_ptr;
	}
}

namespace std
{
	template<typename T>
	struct iterator_traits<Nz::SparsePtr<T>>
	{
		using difference_type = ptrdiff_t;
		using iterator_category = random_access_iterator_tag;
		using reference = const T&;
		using pointer = const T*;
		using value_type = T;
	};
}

#include <Nazara/Core/DebugOff.hpp>
