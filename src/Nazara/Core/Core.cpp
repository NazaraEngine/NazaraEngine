// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/PluginManager.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Core
	* \brief Core class that represents the Core module
	*/

	Core::Core(Config /*config*/) :
	ModuleBase("Core", this, ModuleBase::NoLog{})
	{
		Log::Initialize();

		LogInit();
	}

	Core::~Core()
	{
		HardwareInfo::Uninitialize();
		PluginManager::Uninitialize();
		TaskScheduler::Uninitialize();
		LogUninit();
		Log::Uninitialize();
	}

	Core* Core::s_instance = nullptr;
}
