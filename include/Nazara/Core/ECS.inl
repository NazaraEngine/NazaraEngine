// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ECS.hpp>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ECS
	* \brief Core class that represents the ECS module
	*/
	inline ECS::ECS(Config /*config*/) :
	ModuleBase("ECS", this)
	{
		RegisterComponents();
	}

	inline void ECS::RegisterComponents()
	{
		if (entt::type_seq<NodeComponent>() != 0)
			throw std::runtime_error("NodeComponent has wrong index, please initialize Nazara ECS before instancing your own components");

		if (entt::type_seq<GraphicsComponent>() != 1)
			throw std::runtime_error("GraphicsComponent has wrong index, please initialize Nazara ECS before instancing your own components");

		if (entt::type_seq<RigidBody3DComponent>() != 2)
			throw std::runtime_error("GraphicsComponent has wrong index, please initialize Nazara ECS before instancing your own components");
	}
}

#include <Nazara/Core/DebugOff.hpp>
