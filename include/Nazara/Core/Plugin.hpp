// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGIN_HPP
#define NAZARA_CORE_PLUGIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Export.hpp>
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
			Plugin(DynLib dynLib, std::unique_ptr<T> pluginInterface);
			Plugin(const Plugin&) = delete;
			Plugin(Plugin&&) noexcept = default;
			~Plugin();

			bool Activate();

			template<typename U> Plugin<U> Cast() &&;

			void Deactivate();

			const DynLib& GetDynamicLibrary() const;
			T& GetInterface();
			const T& GetInterface() const;

			T* operator->();
			const T* operator->() const;

			operator Plugin<PluginInterface>() &&;

			Plugin& operator=(const Plugin&) = delete;
			Plugin& operator=(Plugin&&) noexcept = default;

		private:
			std::unique_ptr<T> m_interface;
			DynLib m_lib;
	};

	using GenericPlugin = Plugin<PluginInterface>;
}

#include <Nazara/Core/Plugin.inl>

#endif // NAZARA_CORE_PLUGIN_HPP
