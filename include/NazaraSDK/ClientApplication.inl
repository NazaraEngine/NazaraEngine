// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/ClientApplication.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <NazaraSDK/ClientSdk.hpp>

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

		return static_cast<T&>(*info.window.get()); //< Warning: ugly
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

	inline ClientApplication::WindowInfo::WindowInfo(std::unique_ptr<Nz::Window>&& windowPtr) :
	window(std::move(windowPtr))
	{
	}
}
