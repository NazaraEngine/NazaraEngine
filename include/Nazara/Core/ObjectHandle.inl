// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <functional>
#include <limits>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ObjectHandle
	* \brief Core class that represents a object handle
	*/

	/*!
	* \brief Constructs a ObjectHandle object with a pointer to an object
	*
	* \param object Pointer to handle like an object (can be nullptr)
	*/
	template<typename T>
	ObjectHandle<T>::ObjectHandle() :
	m_handleData(Detail::HandleData::GetEmptyObject())
	{
	}

	/*!
	* \brief Constructs a ObjectHandle object by move semantic
	*
	* \param handle ObjectHandle to move into this
	*/
	template<typename T>
	ObjectHandle<T>::ObjectHandle(ObjectHandle&& handle) noexcept
	{
		Reset(std::move(handle));
	}

	/*!
	* \brief Constructs a ObjectHandle object with a pointer to an object
	*
	* \param object Pointer to handle like an object (can be nullptr)
	*/
	template<typename T>
	ObjectHandle<T>::ObjectHandle(T* object)
	{
		Reset(object);
	}

	/*!
	* \brief Destructs the object and calls reset with nullptr
	*
	* \see Reset
	*/
	template<typename T>
	ObjectHandle<T>::~ObjectHandle()
	{
		Reset(nullptr);
	}

	/*!
	* \brief Gets the underlying object
	* \return Underlying object
	*/
	template<typename T>
	T* ObjectHandle<T>::GetObject() const
	{
		return static_cast<T*>(m_handleData->object);
	}

	/*!
	* \brief Checks whether the object is valid
	* \return true if object is not nullptr
	*/
	template<typename T>
	bool ObjectHandle<T>::IsValid() const
	{
		return m_handleData->object != nullptr;
	}

	/*!
	* \brief Resets the content of the ObjectHandle with another object
	*
	* \param object Object to handle
	*/
	template<typename T>
	void ObjectHandle<T>::Reset(T* object)
	{
		if (object)
			m_handleData = object->GetHandleData();
		else
			m_handleData = Detail::HandleData::GetEmptyObject();
	}

	/*!
	* \brief Resets the content of this with another object
	*
	* \param handle New object to handle
	*/
	template<typename T>
	void ObjectHandle<T>::Reset(const ObjectHandle& handle)
	{
		m_handleData = handle.m_handleData;
	}

	/*!
	* \brief Resets the content of this with another object by move semantic
	*
	* \param handle New object to handle to move into this
	*/
	template<typename T>
	void ObjectHandle<T>::Reset(ObjectHandle&& handle) noexcept
	{
		m_handleData = std::move(handle.m_handleData);
		handle.m_handleData = Detail::HandleData::GetEmptyObject();
	}

	/*!
	* \brief Swaps the content of the two ObjectHandle
	* \return A reference to this
	*
	* \param handle ObjectHandle to swap
	*/
	template<typename T>
	ObjectHandle<T>& ObjectHandle<T>::Swap(ObjectHandle& handle)
	{
		// We do the swap
		std::swap(m_handleData, handle.m_handleData);
		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object "ObjectHandle(object representation) or Null"
	*/
	template<typename T>
	Nz::String ObjectHandle<T>::ToString() const
	{
		Nz::StringStream ss;
		ss << "ObjectHandle(";
		if (IsValid())
			ss << GetObject()->ToString();
		else
			ss << "Null";

		ss << ')';

		return ss;
	}

	/*!
	* \brief Converts the ObjectHandle to bool
	* \return true if reference is not nullptr
	*
	* \see IsValid
	*/
	template<typename T>
	ObjectHandle<T>::operator bool() const
	{
		return IsValid();
	}

	/*!
	* \brief Dereferences the ObjectHandle
	* \return Underlying pointer
	*/
	template<typename T>
	ObjectHandle<T>::operator T*() const
	{
		return GetObject();
	}

	/*!
	* \brief Dereferences the ObjectHandle
	* \return Underlying pointer
	*/
	template<typename T>
	T* ObjectHandle<T>::operator->() const
	{
		return GetObject();
	}

	/*!
	* \brief Assigns the entity into this
	* \return A reference to this
	*
	* \param entity Pointer to handle like an object (can be nullptr)
	*/
	template<typename T>
	ObjectHandle<T>& ObjectHandle<T>::operator=(T* object)
	{
		Reset(object);

		return *this;
	}

	/*!
	* \brief Moves the ObjectHandle into this
	* \return A reference to this
	*
	* \param handle ObjectHandle to move in this
	*/
	template<typename T>
	ObjectHandle<T>& ObjectHandle<T>::operator=(ObjectHandle&& handle) noexcept
	{
		Reset(std::move(handle));

		return *this;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param handle The ObjectHandle to output
	*/
	template<typename T>
	std::ostream& operator<<(std::ostream& out, const ObjectHandle<T>& handle)
	{
		out << handle.ToString();
		return out;
	}

	/*!
	* \brief Checks whether the first object handle is equal to the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator==(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return lhs.GetObject() == rhs.GetObject();
	}

	/*!
	* \brief Checks whether the object is equal to the second object handle
	* \return true if it is the case
	*
	* \param lhs Object to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator==(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return &lhs == rhs.GetObject();
	}

	/*!
	* \brief Checks whether the object handle is equal to the second object
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs Object to compare in right hand side
	*/
	template<typename T>
	bool operator==(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return lhs.GetObject() == &rhs;
	}

	/*!
	* \brief Checks whether the first object handle is equal to the second object handle
	* \return false if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator!=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs == rhs);
	}

	/*!
	* \brief Checks whether the object is equal to the second object handle
	* \return false if it is the case
	*
	* \param lhs Object to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator!=(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs == rhs);
	}

	/*!
	* \brief Checks whether the object handle is equal to the second object
	* \return false if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs Object to compare in right hand side
	*/
	template<typename T>
	bool operator!=(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return !(lhs == rhs);
	}

	/*!
	* \brief Checks whether the first object handle is less than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator<(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return lhs.GetObject() < rhs.GetObject();
	}

	/*!
	* \brief Checks whether the first object handle is less than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator<(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return &lhs < rhs.GetObject();
	}

	/*!
	* \brief Checks whether the first object handle is less than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator<(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return lhs.GetObject() < &rhs;
	}

	/*!
	* \brief Checks whether the first object handle is less or equal than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator<=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs > rhs);
	}

	/*!
	* \brief Checks whether the first object handle is less or equal than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator<=(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs > rhs);
	}

	/*!
	* \brief Checks whether the first object handle is less or equal than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator<=(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return !(lhs > rhs);
	}

	/*!
	* \brief Checks whether the first object handle is greather than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator>(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return rhs < lhs;
	}

	/*!
	* \brief Checks whether the first object handle is greather than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator>(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return rhs < lhs;
	}

	/*!
	* \brief Checks whether the first object handle is greather than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator>(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return rhs < lhs;
	}

	/*!
	* \brief Checks whether the first object handle is greather or equal than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator>=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs < rhs);
	}

	/*!
	* \brief Checks whether the first object handle is greather or equal than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator>=(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs < rhs);
	}

	/*!
	* \brief Checks whether the first object handle is greather or equal than the second object handle
	* \return true if it is the case
	*
	* \param lhs ObjectHandle to compare in left hand side
	* \param rhs ObjectHandle to compare in right hand side
	*/
	template<typename T>
	bool operator>=(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return !(lhs < rhs);
	}

	template<typename T>
	const ObjectHandle<T> ObjectHandle<T>::InvalidHandle;
}

namespace std
{
	/*!
	* \brief Swaps two ObjectHandle, specialisation of std
	*
	* \param lhs First object handle
	* \param rhs Second object handle
	*/
	template<typename T>
	void swap(Nz::ObjectHandle<T>& lhs, Nz::ObjectHandle<T>& rhs)
	{
		lhs.Swap(rhs);
	}
}
