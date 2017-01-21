// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::AbstractBackground
	* \brief Graphics class that represents the background for our scene
	*
	* \remark This class is abstract
	*/

	AbstractBackground::~AbstractBackground() = default;

	BackgroundLibrary::LibraryMap AbstractBackground::s_library;
}
