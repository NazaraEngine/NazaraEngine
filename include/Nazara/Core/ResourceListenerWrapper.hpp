// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCELISTENERWRAPPER_HPP
#define NAZARA_RESOURCELISTENERWRAPPER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <type_traits>

template<typename T>
class NzResourceListenerWrapper
{
	static_assert(std::is_base_of<NzResource, T>::value, "ResourceRef should only be used with resource type");

	public:
		NzResourceListenerWrapper(NzResourceListener* listener, int index = 0, T* resource = nullptr);
		NzResourceListenerWrapper(const NzResourceListenerWrapper& listener);
		NzResourceListenerWrapper(NzResourceListenerWrapper&& listener);
		~NzResourceListenerWrapper();

		bool IsValid() const;
		void Reset(T* resource = nullptr);

		operator bool() const;
		operator T*() const;
		T* operator->() const;

		NzResourceListenerWrapper& operator=(T* resource);
		NzResourceListenerWrapper& operator=(const NzResourceListenerWrapper& listener);
		NzResourceListenerWrapper& operator=(NzResourceListenerWrapper&& listener);

	private:
		T* m_resource;
		NzResourceListener* m_listener;
		int m_index;
};

#include <Nazara/Core/ResourceListenerWrapper.inl>

#endif // NAZARA_RESOURCELISTENERWRAPPER_HPP
