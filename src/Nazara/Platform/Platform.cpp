// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup system
	* \class Nz::Platform
	* \brief Platform class that represents the module initializer of Platform
	*/

	Platform::Platform() :
	Module("Platform", this)
	{
		if (!Window::Initialize())
			throw std::runtime_error("failed to initialize window system");

		// Must be initialized after Window
		if (!Cursor::Initialize())
			throw std::runtime_error("failed to initialize cursors");
	}

	Platform::~Platform()
	{
		Cursor::Uninitialize(); //< Must be done before Window
		Window::Uninitialize();
	}

	Platform* Platform::s_instance;
}
