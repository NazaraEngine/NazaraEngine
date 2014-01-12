// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCEREF_HPP
#define NAZARA_RESOURCEREF_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <type_traits>

template<typename T>
class NzResourceRef
{
	static_assert(std::is_base_of<NzResource, T>::value, "ResourceRef should only be used with resource type");

	public:
		NzResourceRef();
		NzResourceRef(T* resource);
		NzResourceRef(const NzResourceRef& ref);
		NzResourceRef(NzResourceRef&& ref) noexcept;
		~NzResourceRef();

		bool IsValid() const;
		T* Release();
		bool Reset(T* resource = nullptr);
		NzResourceRef& Swap(NzResourceRef& ref);

		operator bool() const;
		operator T*() const;
		T* operator->() const;

		NzResourceRef& operator=(T* resource);
		NzResourceRef& operator=(const NzResourceRef& ref);
		NzResourceRef& operator=(NzResourceRef&& ref) noexcept;

	private:
		T* m_resource;
};

#include <Nazara/Core/ResourceRef.inl>

#endif // NAZARA_RESOURCEREF_HPP
