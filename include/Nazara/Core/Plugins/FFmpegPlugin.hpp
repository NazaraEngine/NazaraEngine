// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGINS_FFMPEGPLUGIN_HPP
#define NAZARA_CORE_PLUGINS_FFMPEGPLUGIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/PluginInterface.hpp>

namespace Nz
{
	// Don't export class due to MinGW bug, export every method instead
	class FFmpegPlugin : public PluginInterface
	{
		public:
#ifdef NAZARA_DEBUG
			static constexpr inline std::string_view Filename = NazaraPluginPrefix "PluginFFmpeg-d";
#else
			static constexpr inline std::string_view Filename = NazaraPluginPrefix "PluginFFmpeg";
#endif

			FFmpegPlugin() = default;
			FFmpegPlugin(const FFmpegPlugin&) = delete;
			FFmpegPlugin(FFmpegPlugin&&) = delete;
			~FFmpegPlugin() = default;

			FFmpegPlugin& operator=(const FFmpegPlugin&) = delete;
			FFmpegPlugin& operator=(FFmpegPlugin&&) = delete;
	};
}

#include <Nazara/Core/Plugins/FFmpegPlugin.inl>

#endif // NAZARA_CORE_PLUGINS_FFMPEGPLUGIN_HPP
