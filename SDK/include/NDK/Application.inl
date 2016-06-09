// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <type_traits>
#include <NDK/Sdk.hpp>

namespace Ndk
{
	inline Application::Application() :
	#ifndef NDK_SERVER
	m_exitOnClosedWindows(true),
	#endif
	m_shouldQuit(false),
	m_updateTime(0.f)
	{
		NazaraAssert(s_application == nullptr, "You can create only one application instance per program");
		s_application = this;

		Nz::ErrorFlags errFlags(Nz::ErrorFlag_ThrowException, true);

		// Initialisation du SDK
		Sdk::Initialize();
	}

	inline Application::~Application()
	{
		m_worlds.clear();
		#ifndef NDK_SERVER
		m_windows.clear();
		#endif

		// Libération du SDK
		Sdk::Uninitialize();

		// Libération automatique des modules
		s_application = nullptr;
	}

	#ifndef NDK_SERVER
	template<typename T, typename... Args> 
	T& Application::AddWindow(Args&&... args)
	{
		static_assert(std::is_base_of<Nz::Window, T>::value, "Type must inherit Window");

		m_windows.emplace_back(new T(std::forward<Args>(args)...));
		return static_cast<T&>(*m_windows.back().get());
	}
	#endif

	template<typename... Args> 
	World& Application::AddWorld(Args&&... args)
	{
		m_worlds.emplace_back(std::forward<Args>(args)...);
		return m_worlds.back();
	}

	inline float Application::GetUpdateTime() const
	{
		return m_updateTime;
	}

	#ifndef NDK_SERVER
	inline void Application::MakeExitOnLastWindowClosed(bool exitOnClosedWindows)
	{
		m_exitOnClosedWindows = exitOnClosedWindows;
	}
	#endif

	inline void Application::Quit()
	{
		m_shouldQuit = true;
	}

	inline Application* Application::Instance()
	{
		return s_application;
	}
}
