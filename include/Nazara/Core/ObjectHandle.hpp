// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OBJECTHANDLE_HPP
#define NAZARA_OBJECTHANDLE_HPP

#include <Nazara/Core/Algorithm.hpp>
#include <ostream>

namespace Nz
{
	template<typename T> class HandledObject;

	template<typename T>
	class ObjectHandle
	{
		friend HandledObject<T>;

		public:
			ObjectHandle();
			explicit ObjectHandle(T* object);
			ObjectHandle(const ObjectHandle& handle);
			ObjectHandle(ObjectHandle&& handle) noexcept;
			~ObjectHandle();

			T* GetObject() const;

			bool IsValid() const;

			void Reset(T* object = nullptr);
			void Reset(const ObjectHandle& handle);
			void Reset(ObjectHandle&& handle) noexcept;

			ObjectHandle& Swap(ObjectHandle& handle);

			Nz::String ToString() const;

			operator bool() const;
			operator T*() const;
			T* operator->() const;

			ObjectHandle& operator=(T* object);
			ObjectHandle& operator=(const ObjectHandle& handle);
			ObjectHandle& operator=(ObjectHandle&& handle) noexcept;

			static const ObjectHandle InvalidHandle;

		protected:
			void OnObjectDestroyed();
			void OnObjectMoved(T* newObject);

			T* m_object;
	};

	template<typename T> std::ostream& operator<<(std::ostream& out, const ObjectHandle<T>& handle);

	template<typename T> bool operator==(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator==(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator==(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T> bool operator!=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator!=(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator!=(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T> bool operator<(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator<(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator<(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T> bool operator<=(const ObjectHandle<T>, const ObjectHandle<T>& rhs);
	template<typename T> bool operator<=(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator<=(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T> bool operator>(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator>(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator>(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T> bool operator>=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator>=(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator>=(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T> struct PointedType<ObjectHandle<T>> { typedef T type; };
	template<typename T> struct PointedType<const ObjectHandle<T>> { typedef T type; };
}

namespace std
{
	template<typename T>
	void swap(Nz::ObjectHandle<T>& lhs, Nz::ObjectHandle<T>& rhs);
}

#include <Nazara/Core/ObjectHandle.inl>

#endif // NAZARA_OBJECTHANDLE_HPP
