// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLUGINMANAGER_HPP
#define NAZARA_PLUGINMANAGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>

class NAZARA_API NzPluginManager
{
	public:
		NzPluginManager() = delete;
		~NzPluginManager() = delete;

		static bool AddDirectory(const NzString& directoryPath);

		static bool Initialize();

		static bool Mount(nzPlugin plugin);
		static bool Mount(const NzString& pluginPath, bool appendExtension = true);

		static void RemoveDirectory(const NzString& directoryPath);

		static void Unmount(nzPlugin plugin);
		static void Unmount(const NzString& pluginPath);

		static void Uninitialize();
};

#endif // NAZARA_PLUGINMANAGER_HPP
