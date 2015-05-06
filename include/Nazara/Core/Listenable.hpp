// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LISTENABLE_HPP
#define NAZARA_LISTENABLE_HPP

#include <Nazara/Prerequesites.hpp>
#include <unordered_map>

template<typename Base>
class NzListenable
{
	public:
		NzListenable();
		~NzListenable() = default;

		template<typename L> void AddListener(L* listener, void* userdata = nullptr) const;
		template<typename L> void RemoveListener(L* listener) const;

		template<typename F, typename... Args> void Notify(F callback, Args&&... args);
		template<typename F> void NotifyRelease(F callback);

	private:
		mutable std::unordered_map<void*, void*> m_listeners;
		bool m_listenersLocked;
};

#include <Nazara/Core/Listenable.inl>

#endif // NAZARA_LISTENABLE_HPP
