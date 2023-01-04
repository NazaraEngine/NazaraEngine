// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_APPWINDOWINGCOMPONENT_HPP
#define NAZARA_PLATFORM_APPWINDOWINGCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Platform/Config.hpp>

namespace Nz
{
	class NAZARA_PLATFORM_API AppWindowingComponent : public ApplicationComponent
	{
		public:
			AppWindowingComponent() = default;
			AppWindowingComponent(const AppWindowingComponent&) = delete;
			AppWindowingComponent(AppWindowingComponent&&) = delete;
			~AppWindowingComponent() = default;

			void Update(Time elapsedTime) override;

			AppWindowingComponent& operator=(const AppWindowingComponent&) = delete;
			AppWindowingComponent& operator=(AppWindowingComponent&&) = delete;
	};
}

#include <Nazara/Platform/AppWindowingComponent.inl>

#endif // NAZARA_PLATFORM_APPWINDOWINGCOMPONENT_HPP
