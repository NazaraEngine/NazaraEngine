// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ECS_HPP
#define NAZARA_ECS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class CameraComponent;
	class GraphicsComponent;
	class NodeComponent;
	class RigidBody3DComponent;

	class ECS : public ModuleBase<ECS>
	{
		friend ModuleBase;
		friend class Audio;
		friend class Graphics;
		friend class Physics2D;
		friend class Physics3D;
		friend class Utility;
		friend class Widgets;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			inline ECS(Config /*config*/);
			~ECS() = default;

		private:
			static inline void RegisterComponents();

			NAZARA_CORE_API static ECS* s_instance;
	};
}

#include <Nazara/Core/ECS.inl>

#endif
