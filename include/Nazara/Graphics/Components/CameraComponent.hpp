// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CAMERACOMPONENT_HPP
#define NAZARA_CAMERACOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Camera.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API CameraComponent : public Camera
	{
		public:
			using Camera::Camera;
			CameraComponent(const CameraComponent& camera) = default;
			CameraComponent(CameraComponent&& camera) noexcept = default;
			~CameraComponent() = default;

			CameraComponent& operator=(const CameraComponent& camera) = default;
			CameraComponent& operator=(CameraComponent&& camera) noexcept = default;
	};
}

#include <Nazara/Graphics/Components/CameraComponent.inl>

#endif
