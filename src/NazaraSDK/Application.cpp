// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Application.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <regex>

#ifndef NDK_SERVER
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>
#endif

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::Application
	* \brief NDK class that represents the application, it offers a set of tools to ease the development
	*/

	/*!
	* \brief Constructs an Application object with command-line arguments
	*
	* Pass the argc and argv arguments from the main function.
	*
	* Command-line arguments can be retrieved by application methods
	*
	* This calls Sdk::Initialize()
	*
	* \remark Only one Application instance can exist at a time
	*/
	Application::Application(int argc, char* argv[]) :
	Application()
	{
		std::regex optionRegex(R"(-(\w+))");
		std::regex valueRegex(R"(-(\w+)\s*=\s*(.+))");

		std::smatch results;

		for (int i = 1; i < argc; ++i)
		{
			std::string argument(argv[i]);
			if (std::regex_match(argument, results, valueRegex))
			{
				std::string key = Nz::ToLower(results[1].str());
				std::string value(results[2].str());

				m_parameters[key] = value;
				NazaraDebug("Registred parameter from command-line: " + key + "=" + value);
			}
			else if (std::regex_match(argument, results, optionRegex))
			{
				std::string option(results[1].str());

				m_options.insert(option);
				NazaraDebug("Registred option from command-line: " + option);
			}
			else
				NazaraWarning("Ignored command-line argument #" + Nz::NumberToString(i) + " \"" + argument + '"');
		}
	}

	/*!
	* \brief Runs the application by updating worlds, taking care about windows, ...
	*/
	bool Application::Run()
	{
		#ifndef NDK_SERVER
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
		#endif

		if (m_shouldQuit)
			return false;

		m_updateTime = m_updateClock.Restart() / 1'000'000.f;

		for (World& world : m_worlds)
			world.Update(m_updateTime);

		return true;
	}

	Application* Application::s_application = nullptr;
}
