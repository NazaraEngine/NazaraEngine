// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/ClientApplication.hpp>
#include <Nazara/Core/Log.hpp>
#include <regex>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::ClientApplication
	* \brief NDK class that represents a client-side application, it offers a set of tools to ease the development
	*/

	/*!
	* \brief Constructs an ClientApplication object with command-line arguments
	*
	* Pass the argc and argv arguments from the main function.
	*
	* Command-line arguments can be retrieved by application methods
	*
	* This calls Sdk::Initialize()
	*
	* \remark Only one Application instance can exist at a time
	*/
	ClientApplication::ClientApplication(int argc, char* argv[]) :
	ClientApplication()
	{
		ParseCommandline(argc, argv);
	}

	/*!
	* \brief Runs the application by updating worlds, taking care about windows, ...
	*/
	bool ClientApplication::Run()
	{
		if (!Application::Run())
			return false;

		bool hasAtLeastOneActiveWindow = false;

		auto it = m_windows.begin();
		while (it != m_windows.end())
		{
			Nz::Window& window = *it->window;

			window.ProcessEvents();

			if (!window.IsOpen(true))
			{
				it = m_windows.erase(it);
				continue;
			}

			hasAtLeastOneActiveWindow = true;

			++it;
		}

		if (m_exitOnClosedWindows && !hasAtLeastOneActiveWindow)
			return false;

		return true;
	}

	ClientApplication* ClientApplication::s_clientApplication = nullptr;
}
