// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ECS.hpp>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename T>
		struct RegisterComponent
		{
			void operator()(entt::id_type& expectedId)
			{
				if (entt::type_seq<T>() != expectedId++)
					throw std::runtime_error(std::string(entt::type_name<T>::value()) + " has wrong index, please initialize Nazara ECS before instancing your own components");
			}
		};
	}

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
		entt::id_type expectedId = 0;
		TypeListApply<TypeList<NodeComponent, CameraComponent, GraphicsComponent, RigidBody3DComponent>, Detail::RegisterComponent>(expectedId);
	}
}

#include <Nazara/Core/DebugOff.hpp>
