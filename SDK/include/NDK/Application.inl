// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <NDK/Sdk.hpp>

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
	m_overlayFlags(0U),
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

		T& window = static_cast<T&>(*info.window.get()); //< Warning: ugly

		SetupWindow(info, &window, std::is_base_of<Nz::RenderTarget, T>());

		return window;
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
	* \brief Enable/disable debug console
	*
	* \param enable Should the console overlay be enabled
	*/
	#ifndef NDK_SERVER
	inline void Application::EnableConsole(bool enable)
	{
		if (enable != ((m_overlayFlags & OverlayFlags_Console) != 0))
		{
			if (enable)
			{
				if (m_overlayFlags == 0)
				{
					for (WindowInfo& info : m_windows)
						SetupOverlay(info);
				}

				for (WindowInfo& info : m_windows)
				{
					if (info.renderTarget)
						SetupConsole(info);
				}

				m_overlayFlags |= OverlayFlags_Console;

			}
			else
			{
				for (WindowInfo& info : m_windows)
					info.console.reset();

				m_overlayFlags &= ~OverlayFlags_Console;
				if (m_overlayFlags == 0)
				{
					for (WindowInfo& info : m_windows)
						info.overlayWorld.reset();
				}
			}
		}
	}
	#endif

	/*!
	* \brief Enable/disable debug FPS counter
	*
	* \param enable Should the FPS counter be displayed
	*/
	#ifndef NDK_SERVER
	inline void Application::EnableFPSCounter(bool enable)
	{
		if (enable != ((m_overlayFlags & OverlayFlags_FPSCounter) != 0))
		{
			if (enable)
			{
				if (m_overlayFlags == 0)
				{
					for (WindowInfo& info : m_windows)
						SetupOverlay(info);
				}

				for (WindowInfo& info : m_windows)
				{
					if (info.renderTarget)
						SetupFPSCounter(info);
				}

				m_overlayFlags |= OverlayFlags_FPSCounter;

			}
			else
			{
				for (WindowInfo& info : m_windows)
					info.fpsCounter.reset();

				m_overlayFlags &= ~OverlayFlags_FPSCounter;
				if (m_overlayFlags == 0)
				{
					for (WindowInfo& info : m_windows)
						info.overlayWorld.reset();
				}
			}
		}
	}
	#endif

	/*!
	* \brief Gets the console overlay for a specific window
	*
	* \param windowIndex Index of the window to get
	*
	* \remark The console overlay must be enabled
	*
	* \return A reference to the console overlay of the window
	*
	* \see IsConsoleOverlayEnabled
	*/
	#ifndef NDK_SERVER
	inline Application::ConsoleOverlay& Application::GetConsoleOverlay(std::size_t windowIndex)
	{
		NazaraAssert(m_overlayFlags & OverlayFlags_Console, "Console overlay is not enabled");
		NazaraAssert(windowIndex <= m_windows.size(), "Window index is out of range");

		return *m_windows[windowIndex].console;
	}
	#endif

	/*!
	* \brief Gets the console overlay for a specific window
	*
	* \param windowIndex Index of the window to get
	*
	* \remark The console overlay must be enabled
	*
	* \return A reference to the console overlay of the window
	*
	* \see IsFPSCounterEnabled
	*/
	#ifndef NDK_SERVER
	inline Application::FPSCounterOverlay& Application::GetFPSCounterOverlay(std::size_t windowIndex)
	{
		NazaraAssert(m_overlayFlags & OverlayFlags_FPSCounter, "FPS counter overlay is not enabled");
		NazaraAssert(windowIndex <= m_windows.size(), "Window index is out of range");

		return *m_windows[windowIndex].fpsCounter;
	}
	#endif

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
	* \brief Checks if the console overlay is enabled
	*
	* \remark This has nothing to do with the visibility state of the console
	*
	* \return True if the console overlay is enabled
	*
	* \see GetConsoleOverlay
	*/
	#ifndef NDK_SERVER
	inline bool Application::IsConsoleEnabled() const
	{
		return (m_overlayFlags & OverlayFlags_Console) != 0;
	}
	#endif

	/*!
	* \brief Checks if the FPS counter overlay is enabled
	* \return True if the FPS counter overlay is enabled
	*
	* \see GetFPSCounterOverlay
	*/
	#ifndef NDK_SERVER
	inline bool Application::IsFPSCounterEnabled() const
	{
		return (m_overlayFlags & OverlayFlags_FPSCounter) != 0;
	}
	#endif

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
	template<typename T>
	inline void Application::SetupWindow(WindowInfo& info, T* renderTarget, std::true_type)
	{
		info.renderTarget = renderTarget;

		if (m_overlayFlags)
		{
			SetupOverlay(info);

			if (m_overlayFlags & OverlayFlags_Console)
				SetupConsole(info);

			if (m_overlayFlags & OverlayFlags_FPSCounter)
				SetupFPSCounter(info);
		}
	}

	template<typename T>
	inline void Application::SetupWindow(WindowInfo&, T*, std::false_type)
	{
	}

	inline Application::WindowInfo::WindowInfo(std::unique_ptr<Nz::Window>&& windowPtr) :
	renderTarget(nullptr),
	window(std::move(windowPtr))
	{
	}
	#endif
}
