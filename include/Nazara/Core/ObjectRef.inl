// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename T>
	ObjectRef<T>::ObjectRef() :
	m_object(nullptr)
	{
	}

	template<typename T>
	ObjectRef<T>::ObjectRef(T* object) :
	m_object(object)
	{
		if (m_object)
			m_object->AddReference();
	}

	template<typename T>
	ObjectRef<T>::ObjectRef(const ObjectRef& ref) :
	m_object(ref.m_object)
	{
		if (m_object)
			m_object->AddReference();
	}

	template<typename T>
	template<typename U>
	ObjectRef<T>::ObjectRef(const ObjectRef<U>& ref) :
	ObjectRef(ref.Get())
	{
	}

	template<typename T>
	ObjectRef<T>::ObjectRef(ObjectRef&& ref) noexcept :
	m_object(ref.m_object)
	{
		ref.m_object = nullptr; // On vole la référence
	}

	template<typename T>
	ObjectRef<T>::~ObjectRef()
	{
		if (m_object)
			m_object->RemoveReference();
	}

	template<typename T>
	T* ObjectRef<T>::Get() const
	{
		return m_object;
	}

	template<typename T>
	bool ObjectRef<T>::IsValid() const
	{
		return m_object != nullptr;
	}

	template<typename T>
	T* ObjectRef<T>::Release()
	{
		T* object = m_object;
		m_object = nullptr;

		return object;
	}

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

	template<typename T>
	ObjectRef<T>& ObjectRef<T>::Swap(ObjectRef& ref)
	{
		std::swap(m_object, ref.m_object);

		return *this;
	}

	template<typename T>
	ObjectRef<T>::operator bool() const
	{
		return IsValid();
	}

	template<typename T>
	ObjectRef<T>::operator T*() const
	{
		return m_object;
	}

	template<typename T>
	T* ObjectRef<T>::operator->() const
	{
		return m_object;
	}

	template<typename T>
	ObjectRef<T>& ObjectRef<T>::operator=(T* object)
	{
		Reset(object);

		return *this;
	}

	template<typename T>
	ObjectRef<T>& ObjectRef<T>::operator=(const ObjectRef& ref)
	{
		Reset(ref.m_object);

		return *this;
	}

	template<typename T>
	template<typename U>
	ObjectRef<T>& ObjectRef<T>::operator=(const ObjectRef<U>& ref)
	{
		static_assert(std::is_convertible<U*, T*>::value, "U is not implicitly convertible to T");

		Reset(ref.Get());

		return *this;
	}

	template<typename T>
	ObjectRef<T>& ObjectRef<T>::operator=(ObjectRef&& ref) noexcept
	{
		Reset();

		std::swap(m_object, ref.m_object);

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>
