// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Application.hpp>
#include <Nazara/Core/Log.hpp>
#include <regex>

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
		ParseCommandline(argc, argv);
	}

	/*!
	* \brief Runs the application by updating worlds, taking care about windows, ...
	*/
	bool Application::Run()
	{
		if (m_shouldQuit)
			return false;

		m_updateTime = m_updateClock.Restart() / 1'000'000.f;

		for (World& world : m_worlds)
			world.Update(m_updateTime);

		return true;
	}

	void Application::ClearWorlds()
	{
		m_worlds.clear();
	}

	void Application::ParseCommandline(int argc, char* argv[])
	{
		std::regex optionRegex(R"(-(\w+))");
		std::regex valueRegex(R"(-(\w+)\s*=\s*(.+))");

		std::smatch results;

		for (int i = 1; i < argc; ++i)
		{
			std::string argument(argv[i]);
			if (std::regex_match(argument, results, valueRegex))
			{
				Nz::String key(results[1].str());
				Nz::String value(results[2].str());

				m_parameters[key.ToLower()] = value;
				NazaraDebug("Registred parameter from command-line: " + key.ToLower() + "=" + value);
			}
			else if (std::regex_match(argument, results, optionRegex))
			{
				Nz::String option(results[1].str());

				m_options.insert(option);
				NazaraDebug("Registred option from command-line: " + option);
			}
			else
				NazaraWarning("Ignored command-line argument #" + Nz::String::Number(i) + " \"" + argument + '"');
		}
	}

	Application* Application::s_application = nullptr;
}
