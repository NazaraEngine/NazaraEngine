// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGIN_HPP
#define NAZARA_CORE_PLUGIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/PluginInterface.hpp>
#include <memory>
#include <type_traits>

namespace Nz
{
	template<typename T>
	class Plugin
	{
		static_assert(std::is_base_of_v<PluginInterface, T>);

		public:
			Plugin(DynLib dynLib, std::unique_ptr<T> pluginInterface, bool isActive = false);
			Plugin(const Plugin&) = delete;
			Plugin(Plugin&&) = delete;
			~Plugin();

			bool Activate();

			template<typename U> Plugin<U> Cast() &&;

			void Deactivate();

			const DynLib& GetDynamicLibrary() const;

			T* operator->();
			const T* operator->() const;

			Plugin& operator=(const Plugin&) = delete;
			Plugin& operator=(Plugin&&) = delete;

		private:
			std::unique_ptr<T> m_interface;
			DynLib m_lib;
			bool m_isActive;
	};

	using GenericPlugin = Plugin<PluginInterface>;
}

#include <Nazara/Core/Plugin.inl>

#endif // NAZARA_CORE_PLUGIN_HPP
