// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Graphics
	* \brief Audio class that represents the module initializer of Graphics
	*/

	Graphics::Graphics(Config /*config*/) :
	ModuleBase("Graphics", this)
	{
	}

	Graphics::~Graphics()
	{
	}

	Graphics* Graphics::s_instance = nullptr;
}
