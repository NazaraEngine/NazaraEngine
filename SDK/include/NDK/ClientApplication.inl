// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/ClientApplication.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <NDK/ClientSdk.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs an Application object without passing command-line arguments
	*
	* This calls Sdk::Initialize()
	*
	* \remark Only one Application instance can exist at a time
	*/
	inline ClientApplication::ClientApplication() :
	m_overlayFlags(0U),
	m_exitOnClosedWindows(true)
	{
		NazaraAssert(s_clientApplication == nullptr, "You can create only one application instance per program");
		s_clientApplication = this;

		Nz::ErrorFlags errFlags(Nz::ErrorFlag_ThrowException, true);

		// Initialisation du SDK
		ClientSdk::Initialize();
	}

	/*!
	* \brief Destructs the application object
	*
	* This destroy all worlds and windows and then calls Sdk::Uninitialize
	*/
	inline ClientApplication::~ClientApplication()
	{
		ClearWorlds();
		m_windows.clear();

		ClientSdk::Uninitialize();
		s_clientApplication = nullptr;
	}

	/*!
	* \brief Adds a window to the application
	* \return A reference to the newly created windows
	*
	* \param args Arguments used to create the window
	*/
	template<typename T, typename... Args>
	T& ClientApplication::AddWindow(Args&&... args)
	{
		static_assert(std::is_base_of<Nz::Window, T>::value, "Type must inherit Window");

		m_windows.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
		WindowInfo& info = m_windows.back();

		T& window = static_cast<T&>(*info.window.get()); //< Warning: ugly

		SetupWindow(info, &window, std::is_base_of<Nz::RenderTarget, T>());

		return window;
	}

	/*!
	* \brief Enable/disable debug console
	*
	* \param enable Should the console overlay be enabled
	*/
	inline void ClientApplication::EnableConsole(bool enable)
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

	/*!
	* \brief Enable/disable debug FPS counter
	*
	* \param enable Should the FPS counter be displayed
	*/
	inline void ClientApplication::EnableFPSCounter(bool enable)
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
	inline ClientApplication::ConsoleOverlay& ClientApplication::GetConsoleOverlay(std::size_t windowIndex)
	{
		NazaraAssert(m_overlayFlags & OverlayFlags_Console, "Console overlay is not enabled");
		NazaraAssert(windowIndex <= m_windows.size(), "Window index is out of range");

		return *m_windows[windowIndex].console;
	}

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
	inline ClientApplication::FPSCounterOverlay& ClientApplication::GetFPSCounterOverlay(std::size_t windowIndex)
	{
		NazaraAssert(m_overlayFlags & OverlayFlags_FPSCounter, "FPS counter overlay is not enabled");
		NazaraAssert(windowIndex <= m_windows.size(), "Window index is out of range");

		return *m_windows[windowIndex].fpsCounter;
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
	inline bool ClientApplication::IsConsoleEnabled() const
	{
		return (m_overlayFlags & OverlayFlags_Console) != 0;
	}

	/*!
	* \brief Checks if the FPS counter overlay is enabled
	* \return True if the FPS counter overlay is enabled
	*
	* \see GetFPSCounterOverlay
	*/
	inline bool ClientApplication::IsFPSCounterEnabled() const
	{
		return (m_overlayFlags & OverlayFlags_FPSCounter) != 0;
	}

	/*!
	* \brief Makes the application exit when there's no more open window
	*
	* \param exitOnClosedWindows Should exit be called when no more window is open
	*/
	inline void ClientApplication::MakeExitOnLastWindowClosed(bool exitOnClosedWindows)
	{
		m_exitOnClosedWindows = exitOnClosedWindows;
	}

	/*!
	* \brief Gets the singleton instance of the application
	* \return Singleton application
	*/

	inline ClientApplication* ClientApplication::Instance()
	{
		return s_clientApplication;
	}

	template<typename T>
	inline void ClientApplication::SetupWindow(WindowInfo& info, T* renderTarget, std::true_type)
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
	inline void ClientApplication::SetupWindow(WindowInfo&, T*, std::false_type)
	{
	}

	inline ClientApplication::WindowInfo::WindowInfo(std::unique_ptr<Nz::Window>&& windowPtr) :
	renderTarget(nullptr),
	window(std::move(windowPtr))
	{
	}
}
