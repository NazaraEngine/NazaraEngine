// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup widgets
	* \class Nz::Widgets
	* \brief Widgets class that represents the module initializer of Widgets
	*/
	Widgets::Widgets(Config config) :
	ModuleBase("Widgets", this)
	{
		ECS::RegisterComponents();
	}

	Widgets* Widgets::s_instance = nullptr;
}
