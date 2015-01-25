// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OBJECTLISTENERWRAPPER_HPP
#define NAZARA_OBJECTLISTENERWRAPPER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectListener.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <type_traits>

template<typename T>
class NzObjectListenerWrapper
{
	static_assert(std::is_base_of<NzRefCounted, T>::value, "ObjListenerWrapper should only be used with RefCounted-derived type");

	public:
		NzObjectListenerWrapper(NzObjectListener* listener, int index = 0, T* object = nullptr);
		NzObjectListenerWrapper(const NzObjectListenerWrapper& listener);
		NzObjectListenerWrapper(NzObjectListenerWrapper&& listener);
		~NzObjectListenerWrapper();

		bool IsValid() const;
		void Reset(T* object = nullptr);

		operator bool() const;
		operator T*() const;
		T* operator->() const;

		NzObjectListenerWrapper& operator=(T* object);
		NzObjectListenerWrapper& operator=(const NzObjectListenerWrapper& listener);
		NzObjectListenerWrapper& operator=(NzObjectListenerWrapper&& listener);

	private:
		T* m_object;
		NzObjectListener* m_listener;
		int m_index;
};

#include <Nazara/Core/ObjectListenerWrapper.inl>

#endif // NAZARA_OBJECTLISTENERWRAPPER_HPP
