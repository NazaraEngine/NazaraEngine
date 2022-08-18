// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_PLUGIN_ASSIMPPLUGIN_HPP
#define NAZARA_UTILITY_PLUGIN_ASSIMPPLUGIN_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/PluginInterface.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API AssimpPlugin : public PluginInterface
	{
		public:
#ifdef NAZARA_DEBUG
			static constexpr std::string_view Filename = "PluginAssimp-d";
#else
			static constexpr std::string_view Filename = "PluginAssimp";
#endif

			AssimpPlugin() = default;
			AssimpPlugin(const AssimpPlugin&) = delete;
			AssimpPlugin(AssimpPlugin&&) = delete;
			~AssimpPlugin() = default;

			AssimpPlugin& operator=(const AssimpPlugin&) = delete;
			AssimpPlugin& operator=(AssimpPlugin&&) = delete;
	};
}

#include <Nazara/Utility/Plugins/AssimpPlugin.inl>

#endif // NAZARA_UTILITY_PLUGIN_ASSIMPPLUGIN_HPP
