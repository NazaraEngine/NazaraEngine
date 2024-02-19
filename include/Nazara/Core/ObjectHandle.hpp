// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_OBJECTHANDLE_HPP
#define NAZARA_CORE_OBJECTHANDLE_HPP

#include <Nazara/Core/HandledObject.hpp>
#include <NazaraUtils/TypeTraits.hpp>
#include <memory>
#include <ostream>
#include <string>

namespace Nz
{
	template<typename T>
	class ObjectHandle
	{
		friend HandledObject<T>;

		public:
			ObjectHandle();
			explicit ObjectHandle(T* object);
			template<typename U> ObjectHandle(const ObjectHandle<U>& ref);
			template<typename U> ObjectHandle(ObjectHandle<U>&& ref);
			ObjectHandle(const ObjectHandle& handle) = default;
			ObjectHandle(ObjectHandle&& handle) noexcept;
			~ObjectHandle();

			T* GetObject() const;

			bool IsValid() const;

			void Reset(T* object = nullptr);
			void Reset(const ObjectHandle& handle);
			void Reset(ObjectHandle&& handle) noexcept;

			ObjectHandle& Swap(ObjectHandle& handle);

			std::string ToString() const;

			explicit operator bool() const;
			operator T*() const;
			T* operator->() const;

			ObjectHandle& operator=(T* object);
			ObjectHandle& operator=(const ObjectHandle& handle) = default;
			ObjectHandle& operator=(ObjectHandle&& handle) noexcept;

			static const ObjectHandle InvalidHandle;

		protected:
			std::shared_ptr<const Detail::HandleData> m_handleData;
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

	template<typename T> bool operator<=(const ObjectHandle<T>&, const ObjectHandle<T>& rhs);
	template<typename T> bool operator<=(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator<=(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T> bool operator>(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator>(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator>(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T> bool operator>=(const ObjectHandle<T>& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator>=(const T& lhs, const ObjectHandle<T>& rhs);
	template<typename T> bool operator>=(const ObjectHandle<T>& lhs, const T& rhs);

	template<typename T, typename U> ObjectHandle<T> ConstRefCast(const ObjectHandle<U>& ref);
	template<typename T, typename U> ObjectHandle<T> DynamicRefCast(const ObjectHandle<U>& ref);
	template<typename T, typename U> ObjectHandle<T> ReinterpretRefCast(const ObjectHandle<U>& ref);
	template<typename T, typename U> ObjectHandle<T> StaticRefCast(const ObjectHandle<U>& ref);

	template<typename T> struct PointedType<ObjectHandle<T>> { using type = T; };
	template<typename T> struct PointedType<const ObjectHandle<T>> { using type = T; };
}

namespace std
{
	template<typename T>
	void swap(Nz::ObjectHandle<T>& lhs, Nz::ObjectHandle<T>& rhs);
}

#include <Nazara/Core/ObjectHandle.inl>

#endif // NAZARA_CORE_OBJECTHANDLE_HPP
