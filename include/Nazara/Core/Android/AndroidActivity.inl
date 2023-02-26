// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Android/AndroidActivity.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline AConfiguration* AndroidActivity::GetConfiguration()
	{
		return m_config;
	}

	inline const AConfiguration* AndroidActivity::GetConfiguration() const
	{
		return m_config;
	}

	inline AInputQueue* AndroidActivity::GetInputQueue()
	{
		return m_inputQueue;
	}

	inline const AInputQueue* AndroidActivity::GetInputQueue() const
	{
		return m_inputQueue;
	}

	inline ANativeActivity* AndroidActivity::GetNativeActivity()
	{
		return m_activity;
	}

	inline const ANativeActivity* AndroidActivity::GetNativeActivity() const
	{
		return m_activity;
	}
	
	inline ANativeWindow* AndroidActivity::GetNativeWindow()
	{
		return m_window;
	}

	inline const ANativeWindow* AndroidActivity::GetNativeWindow() const
	{
		return m_window;
	}

	template<typename F, typename... Args>
	void AndroidActivity::Start(F&& f, Args&&... args)
	{
		m_appThread = std::thread([&]
		{
			PreMain();
			f(std::forward<Args>(args)...);
			PostMain();
		});
	}

	inline AndroidActivity* AndroidActivity::Instance()
	{
		NazaraAssert(s_instance != nullptr, "you must instantiate a AndroidActivity before initializing the engine");
		return s_instance;
	}
}

#include <Nazara/Core/DebugOff.hpp>
