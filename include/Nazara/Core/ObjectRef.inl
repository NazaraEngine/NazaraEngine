// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ObjectRef
	* \brief Core class that represents a reference to an object
	*/

	/*!
	* \brief Constructs a ObjectRef object by default
	*/
	template<typename T>
	ObjectRef<T>::ObjectRef() :
	m_object(nullptr)
	{
	}

	/*!
	* \brief Constructs a ObjectRef object with a pointer to an object
	*
	* \param object Pointer to handle like a reference (can be nullptr)
	*/
	template<typename T>
	ObjectRef<T>::ObjectRef(T* object) :
	m_object(object)
	{
		if (m_object)
			m_object->AddReference();
	}

	/*!
	* \brief Constructs a ObjectRef object by assignation
	*
	* \param ref ObjectRef to assign into this
	*/
	template<typename T>
	ObjectRef<T>::ObjectRef(const ObjectRef& ref) :
	m_object(ref.m_object)
	{
		if (m_object)
			m_object->AddReference();
	}

	/*!
	* \brief Constructs a ObjectRef<U> object from another type of ObjectRef
	*
	* \param ref ObjectRef of type U to convert to type T
	*/
	template<typename T>
	template<typename U>
	ObjectRef<T>::ObjectRef(const ObjectRef<U>& ref) :
	ObjectRef(ref.Get())
	{
	}

	/*!
	* \brief Constructs a ObjectRef object by move semantic
	*
	* \param ref ObjectRef to move into this
	*/
	template<typename T>
	ObjectRef<T>::ObjectRef(ObjectRef&& ref) noexcept :
	m_object(ref.m_object)
	{
		ref.m_object = nullptr; // We steal the reference
	}

	/*!
	* \brief Destructs the object (remove a reference to the object when shared)
	*/
	template<typename T>
	ObjectRef<T>::~ObjectRef()
	{
		if (m_object)
			m_object->RemoveReference();
	}

	/*!
	* \brief Gets the underlying pointer
	* \return Underlying pointer
	*/
	template<typename T>
	T* ObjectRef<T>::Get() const
	{
		return m_object;
	}

	/*!
	* \brief Checks whether the reference is valid
	* \return true if reference is not nullptr
	*/
	template<typename T>
	bool ObjectRef<T>::IsValid() const
	{
		return m_object != nullptr;
	}

	/*!
	* \brief Releases the handle of the pointer
	* \return Underlying pointer
	*/
	template<typename T>
	T* ObjectRef<T>::Release()
	{
		if (m_object)
			m_object->RemoveReference();

		T* object = m_object;
		m_object = nullptr;

		return object;
	}

	/*!
	* \brief Resets the content of the ObjectRef with another pointer
	* \return true if old handle is destroyed
	*/
	template<typename T>
	bool ObjectRef<T>::Reset(T* object)
	{
		bool destroyed = false;
		if (m_object != object)
		{
			if (m_object)
				destroyed = m_object->RemoveReference();

			m_object = object;
			if (m_object)
				m_object->AddReference();
		}

		return destroyed;
	}

	/*!
	* \brief Swaps the content of the two ObjectRef
	* \return A reference to this
	*
	* \param ref ObjectRef to swap
	*/
	template<typename T>
	ObjectRef<T>& ObjectRef<T>::Swap(ObjectRef& ref)
	{
		std::swap(m_object, ref.m_object);

		return *this;
	}

	/*!
	* \brief Converts the ObjectRef to bool
	* \return true if reference is not nullptr
	*
	* \see IsValid
	*/
	template<typename T>
	ObjectRef<T>::operator bool() const
	{
		return IsValid();
	}

	/*!
	* \brief Dereferences the ObjectRef
	* \return Underlying pointer
	*/
	template<typename T>
	ObjectRef<T>::operator T*() const
	{
		return m_object;
	}

	/*!
	* \brief Dereferences the ObjectRef
	* \return Underlying pointer
	*/
	template<typename T>
	T* ObjectRef<T>::operator->() const
	{
		return m_object;
	}
	
	/*!
	* \brief Assigns the object into this
	* \return A reference to this
	*
	* \param object Pointer to handle like a reference (can be nullptr)
	*/
	template<typename T>
	ObjectRef<T>& ObjectRef<T>::operator=(T* object)
	{
		Reset(object);

		return *this;
	}

	/*!
	* \brief Sets the reference of the ObjectRef with the handle from another
	* \return A reference to this
	*
	* \param ref The other ObjectRef
	*/
	template<typename T>
	ObjectRef<T>& ObjectRef<T>::operator=(const ObjectRef& ref)
	{
		Reset(ref.m_object);

		return *this;
	}

	/*!
	* \brief Sets the reference of the ObjectRef from another type of ObjectRef
	* \return A reference to this
	*
	* \param ref ObjectRef of type U to convert
	*/
	template<typename T>
	template<typename U>
	ObjectRef<T>& ObjectRef<T>::operator=(const ObjectRef<U>& ref)
	{
		static_assert(std::is_convertible<U*, T*>::value, "U is not implicitly convertible to T");

		Reset(ref.Get());

		return *this;
	}

	/*!
	* \brief Moves the ObjectRef into this
	* \return A reference to this
	*
	* \param ref ObjectRef to move in this
	*/
	template<typename T>
	ObjectRef<T>& ObjectRef<T>::operator=(ObjectRef&& ref) noexcept
	{
		Reset();

		std::swap(m_object, ref.m_object);

		return *this;
	}


	/*!
	* \brief Checks whether the first object handle is equal to the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator==(const ObjectRef<T>& lhs, const ObjectRef<T>& rhs)
	{
		return lhs.Get() == rhs.Get();
	}

	/*!
	* \brief Checks whether the object is equal to the second object handle
	* \return true if it is the case
	*
	* \param first Object to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator==(const T& lhs, const ObjectRef<T>& rhs)
	{
		return &lhs == rhs.Get();
	}

	/*!
	* \brief Checks whether the object handle is equal to the second object
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second Object to compare in right hand side
	*/
	template<typename T>
	bool operator==(const ObjectRef<T>& lhs, const T& rhs)
	{
		return lhs.Get() == &rhs;
	}

	/*!
	* \brief Checks whether the first object handle is equal to the second object handle
	* \return false if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator!=(const ObjectRef<T>& lhs, const ObjectRef<T>& rhs)
	{
		return !(lhs == rhs);
	}

	/*!
	* \brief Checks whether the object is equal to the second object handle
	* \return false if it is the case
	*
	* \param first Object to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator!=(const T& lhs, const ObjectRef<T>& rhs)
	{
		return !(lhs == rhs);
	}

	/*!
	* \brief Checks whether the object handle is equal to the second object
	* \return false if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second Object to compare in right hand side
	*/
	template<typename T>
	bool operator!=(const ObjectRef<T>& lhs, const T& rhs)
	{
		return !(lhs == rhs);
	}

	/*!
	* \brief Checks whether the first object handle is less than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator<(const ObjectRef<T>& lhs, const ObjectRef<T>& rhs)
	{
		return lhs.m_object < rhs.m_object;
	}

	/*!
	* \brief Checks whether the first object handle is less than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator<(const T& lhs, const ObjectRef<T>& rhs)
	{
		return &lhs < rhs.m_object;
	}

	/*!
	* \brief Checks whether the first object handle is less than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator<(const ObjectRef<T>& lhs, const T& rhs)
	{
		return lhs.m_object < &rhs;
	}

	/*!
	* \brief Checks whether the first object handle is less or equal than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator<=(const ObjectRef<T>& lhs, const ObjectRef<T>& rhs)
	{
		return !(lhs > rhs);
	}

	/*!
	* \brief Checks whether the first object handle is less or equal than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator<=(const T& lhs, const ObjectRef<T>& rhs)
	{
		return !(lhs > rhs);
	}

	/*!
	* \brief Checks whether the first object handle is less or equal than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator<=(const ObjectRef<T>& lhs, const T& rhs)
	{
		return !(lhs > rhs);
	}

	/*!
	* \brief Checks whether the first object handle is greather than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator>(const ObjectRef<T>& lhs, const ObjectRef<T>& rhs)
	{
		return rhs < lhs;
	}

	/*!
	* \brief Checks whether the first object handle is greather than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator>(const T& lhs, const ObjectRef<T>& rhs)
	{
		return rhs < lhs;
	}

	/*!
	* \brief Checks whether the first object handle is greather than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator>(const ObjectRef<T>& lhs, const T& rhs)
	{
		return rhs < lhs;
	}

	/*!
	* \brief Checks whether the first object handle is greather or equal than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator>=(const ObjectRef<T>& lhs, const ObjectRef<T>& rhs)
	{
		return !(lhs < rhs);
	}

	/*!
	* \brief Checks whether the first object handle is greather or equal than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator>=(const T& lhs, const ObjectRef<T>& rhs)
	{
		return !(lhs < rhs);
	}

	/*!
	* \brief Checks whether the first object handle is greather or equal than the second object handle
	* \return true if it is the case
	*
	* \param first ObjectRef to compare in left hand side
	* \param second ObjectRef to compare in right hand side
	*/
	template<typename T>
	bool operator>=(const ObjectRef<T>& lhs, const T& rhs)
	{
		return !(lhs < rhs);
	}
}

namespace std
{
	/*!
	* \ingroup core
	* \brief Gives a hash representation of the object, specialisation of std
	* \return Hash of the ObjectRef
	*
	* \param object Object to hash
	*/
	template<typename T>
	struct hash<Nz::ObjectRef<T>>
	{
		size_t operator()(const Nz::ObjectRef<T>& object) const
		{
			hash<T*> h;
			return h(object.Get());
		}
	};
}

#include <Nazara/Core/DebugOff.hpp>
