// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <NazaraSDK/Sdk.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs an Application object without passing command-line arguments
	*
	* This calls Sdk::Initialize()
	*
	* \remark Only one Application instance can exist at a time
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
	* \brief Destructs the application object
	*
	* This destroy all worlds and windows and then calls Sdk::Uninitialize
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

		m_windows.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
		WindowInfo& info = m_windows.back();

		return static_cast<T&>(*info.window.get()); //< Warning: ugly
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
	* \brief Gets the options used to start the application
	*
	* Options are defined as "-optionName" in command-line and are always lower-case
	*
	* \return Command-line options
	*/
	inline const std::set<Nz::String>& Application::GetOptions() const
	{
		return m_options;
	}

	/*!
	* \brief Gets the parameters used to start the application
	*
	* Parameters are defined as "-key=value" in command-line, their key is lower-case but value capitals are kept.
	*
	* \return Command-line parameters
	*/
	inline const std::map<Nz::String, Nz::String>& Application::GetParameters() const
	{
		return m_parameters;
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
	* \brief Query for a command-line option
	*
	* \param option Option name
	*
	* \remark option must be lower-case
	*
	* \return True if option is present
	*
	* \see GetOptions
	*/
	inline bool Application::HasOption(const Nz::String& option) const
	{
		return m_options.count(option) != 0;
	}

	/*!
	* \brief Query for a command-line option
	*
	* \param key Parameter name
	* \param value Optional string to receive the parameter value
	*
	* \remark key must be lower-case
	*
	* \return True if parameter is present
	*
	* \see GetParameters
	*/
	inline bool Application::HasParameter(const Nz::String& key, Nz::String* value) const
	{
		auto it = m_parameters.find(key);
		if (it == m_parameters.end())
			return false;

		if (value)
			*value = it->second;

		return true;
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

	#ifndef NDK_SERVER
	inline Application::WindowInfo::WindowInfo(std::unique_ptr<Nz::Window>&& windowPtr) :
	window(std::move(windowPtr))
	{
	}
	#endif
}
