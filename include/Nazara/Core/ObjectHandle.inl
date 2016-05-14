// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <functional>
#include <limits>

namespace Nz
{
	template<typename T>
	ObjectHandle<T>::ObjectHandle() :
	m_object(nullptr)
	{
	}

	template<typename T>
	ObjectHandle<T>::ObjectHandle(T* object) :
	ObjectHandle()
	{
		Reset(object);
	}

	template<typename T>
	ObjectHandle<T>::ObjectHandle(const ObjectHandle<T>& handle) :
	ObjectHandle()
	{
		Reset(handle);
	}

	template<typename T>
	ObjectHandle<T>::ObjectHandle(ObjectHandle<T>&& handle) noexcept :
	ObjectHandle()
	{
		Reset(std::move(handle));
	}

	template<typename T>
	ObjectHandle<T>::~ObjectHandle()
	{
		Reset(nullptr);
	}

	template<typename T>
	T* ObjectHandle<T>::GetObject() const
	{
		return m_object;
	}

	template<typename T>
	bool ObjectHandle<T>::IsValid() const
	{
		return m_object != nullptr;
	}

	template<typename T>
	void ObjectHandle<T>::Reset(T* object)
	{
		// Si nous avions déjà une entité, nous devons l'informer que nous ne pointons plus sur elle
		if (m_object)
			m_object->UnregisterHandle(this);

		m_object = object;
		if (m_object)
			// On informe la nouvelle entité que nous pointons sur elle
			m_object->RegisterHandle(this);
	}

	template<typename T>
	void ObjectHandle<T>::Reset(const ObjectHandle<T>& handle)
	{
		Reset(handle.GetObject());
	}

	template<typename T>
	void ObjectHandle<T>::Reset(ObjectHandle<T>&& handle) noexcept
	{
		if (m_object)
			m_object->UnregisterHandle(this);

		if (T* object = handle.GetObject())
		{
			m_object = handle.m_object;
			handle.m_object = nullptr;
			object->UpdateHandle(&handle, this);
		}
	}

	template<typename T>
	ObjectHandle<T>& ObjectHandle<T>::Swap(ObjectHandle<T>& handle)
	{
		// Comme nous inversons les handles, nous devons prévenir les entités
		// La version par défaut de swap (à base de move) aurait fonctionné,
		// mais en enregistrant les handles une fois de plus que nécessaire (à cause de la copie temporaire).
		if (m_object)
		{
			m_object->UnregisterHandle(this);
			m_object->RegisterHandle(&handle);
		}

		if (handle.m_object)
		{
			handle.m_object->UnregisterHandle(&handle);
			handle.m_object->RegisterHandle(this);
		}

		// On effectue l'échange
		std::swap(m_object, handle.m_object);
		return *this;
	}

	template<typename T>
	Nz::String ObjectHandle<T>::ToString() const
	{
		Nz::StringStream ss;
		ss << "ObjectHandle(";
		if (IsValid())
			ss << m_object->ToString();
		else
			ss << "Null";

		ss << ')';

		return ss;
	}

	template<typename T>
	ObjectHandle<T>::operator bool() const
	{
		return IsValid();
	}

	template<typename T>
	ObjectHandle<T>::operator T*() const
	{
		return m_object;
	}

	template<typename T>
	T* ObjectHandle<T>::operator->() const
	{
		return m_object;
	}

	template<typename T>
	ObjectHandle<T>& ObjectHandle<T>::operator=(T* entity)
	{
		Reset(entity);

		return *this;
	}

	template<typename T>
	ObjectHandle<T>& ObjectHandle<T>::operator=(const ObjectHandle<T>& handle)
	{
		Reset(handle);

		return *this;
	}

	template<typename T>
	ObjectHandle<T>& ObjectHandle<T>::operator=(ObjectHandle<T>&& handle) noexcept
	{
		Reset(std::move(handle));

		return *this;
	}

	template<typename T>
	void ObjectHandle<T>::OnObjectDestroyed()
	{
		// Shortcut
		m_object = nullptr;
	}

	template<typename T>
	void ObjectHandle<T>::OnObjectMoved(T* newObject)
	{
		// The object has been moved, update our pointer
		m_object = newObject;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& out, const ObjectHandle<T>& handle)
	{
		return handle.ToString();
	}

	template<typename T>
	bool operator==(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return lhs.GetObject() == rhs.GetObject();
	}

	template<typename T>
	bool operator==(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return &lhs == rhs.GetObject();
	}

	template<typename T>
	bool operator==(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return lhs.GetObject() == &rhs;
	}

	template<typename T>
	bool operator!=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	bool operator!=(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	bool operator!=(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	bool operator<(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return lhs.m_object < rhs.m_object;
	}

	template<typename T>
	bool operator<(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return &lhs < rhs.m_object;
	}

	template<typename T>
	bool operator<(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return lhs.m_object < &rhs;
	}

	template<typename T>
	bool operator<=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs > rhs);
	}

	template<typename T>
	bool operator<=(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs > rhs);
	}

	template<typename T>
	bool operator<=(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return !(lhs > rhs);
	}

	template<typename T>
	bool operator>(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return rhs < lhs;
	}

	template<typename T>
	bool operator>(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return rhs < lhs;
	}

	template<typename T>
	bool operator>(const ObjectHandle<T>& lhs, const T& rhs)
	{
		return rhs < lhs;
	}

	template<typename T>
	bool operator>=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs < rhs);
	}

	template<typename T>
	bool operator>=(const T& lhs, const ObjectHandle<T>& rhs)
	{
		return !(lhs < rhs);
	}

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
	template<typename T>
	void swap(Nz::ObjectHandle<T>& lhs, Nz::ObjectHandle<T>& rhs)
	{
		lhs.Swap(rhs);
	}
}
