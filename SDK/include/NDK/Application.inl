// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <type_traits>
#include <NDK/Sdk.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs an Application object by default
	*
	* \remark Produces a NazaraAssert if there's more than one application instance currently running
	*/

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

	/*!
	* \brief Destructs the object
	*/

	inline Application::~Application()
	{
		m_worlds.clear();
		#ifndef NDK_SERVER
		m_windows.clear();
		#endif

		// Free of SDK
		Sdk::Uninitialize();

		// Automatic free of modules
		s_application = nullptr;
	}

	/*!
	* \brief Adds a window to the application
	* \return A reference to the newly created windows
	*
	* \param args Arguments used to create the window
	*/

	#ifndef NDK_SERVER
	template<typename T, typename... Args> 
	T& Application::AddWindow(Args&&... args)
	{
		static_assert(std::is_base_of<Nz::Window, T>::value, "Type must inherit Window");

		m_windows.emplace_back(new T(std::forward<Args>(args)...));
		return static_cast<T&>(*m_windows.back().get());
	}
	#endif

	/*!
	* \brief Adds a world to the application
	* \return A reference to the newly created world
	*
	* \param args Arguments used to create the world
	*/

	template<typename... Args> 
	World& Application::AddWorld(Args&&... args)
	{
		m_worlds.emplace_back(std::forward<Args>(args)...);
		return m_worlds.back();
	}

	/*!
	* \brief Gets the update time of the application
	* \return Update rate
	*/

	inline float Application::GetUpdateTime() const
	{
		return m_updateTime;
	}

	/*!
	* \brief Makes the application exit when there's no more open window
	*
	* \param exitOnClosedWindows Should exit be called when no more window is open
	*/

	#ifndef NDK_SERVER
	inline void Application::MakeExitOnLastWindowClosed(bool exitOnClosedWindows)
	{
		m_exitOnClosedWindows = exitOnClosedWindows;
	}
	#endif

	/*!
	* \brief Quits the application
	*/

	inline void Application::Quit()
	{
		m_shouldQuit = true;
	}

	/*!
	* \brief Gets the singleton instance of the application
	* \return Singleton application
	*/

	inline Application* Application::Instance()
	{
		return s_application;
	}
}
