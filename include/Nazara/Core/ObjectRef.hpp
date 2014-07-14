// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCEREF_HPP
#define NAZARA_RESOURCEREF_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <type_traits>

template<typename T>
class NzObjectRef
{
	static_assert(std::is_base_of<NzRefCounted, T>::value, "ObjectRef shall only be used with RefCounted-derived type");

	public:
		NzObjectRef();
		NzObjectRef(T* resource);
		NzObjectRef(const NzObjectRef& ref);
		NzObjectRef(NzObjectRef&& ref) noexcept;
		~NzObjectRef();

		bool IsValid() const;
		T* Release();
		bool Reset(T* resource = nullptr);
		NzObjectRef& Swap(NzObjectRef& ref);

		operator bool() const;
		operator T*() const;
		T* operator->() const;

		NzObjectRef& operator=(T* resource);
		NzObjectRef& operator=(const NzObjectRef& ref);
		NzObjectRef& operator=(NzObjectRef&& ref) noexcept;

	private:
		T* m_resource;
};

#include <Nazara/Core/ObjectRef.inl>

#endif // NAZARA_RESOURCEREF_HPP
