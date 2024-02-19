// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGINS_ASSIMPPLUGIN_HPP
#define NAZARA_CORE_PLUGINS_ASSIMPPLUGIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/PluginInterface.hpp>

namespace Nz
{
	// Don't export class due to MinGW bug, export every method instead
	class AssimpPlugin : public PluginInterface
	{
		public:
#ifdef NAZARA_DEBUG
			static constexpr inline std::string_view Filename = NazaraPluginPrefix "PluginAssimp-d";
#else
			static constexpr inline std::string_view Filename = NazaraPluginPrefix "PluginAssimp";
#endif

			AssimpPlugin() = default;
			AssimpPlugin(const AssimpPlugin&) = delete;
			AssimpPlugin(AssimpPlugin&&) = delete;
			~AssimpPlugin() = default;

			AssimpPlugin& operator=(const AssimpPlugin&) = delete;
			AssimpPlugin& operator=(AssimpPlugin&&) = delete;
	};

#ifdef NAZARA_PLUGINS_STATIC
	template<>
	struct PluginProvider<AssimpPlugin>
	{
		static std::unique_ptr<AssimpPlugin> Instantiate();
	};
#endif
}

#include <Nazara/Core/Plugins/AssimpPlugin.inl>

#endif // NAZARA_CORE_PLUGINS_ASSIMPPLUGIN_HPP
