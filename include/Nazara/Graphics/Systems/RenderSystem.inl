// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Systems/RenderSystem.hpp>
#include <type_traits>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	template<typename T, typename ...Args>
	T& RenderSystem::CreateWindow(Args&& ...args)
	{
		static_assert(std::is_base_of_v<RenderWindow, T>, "T must inherit RenderWindow");

		auto windowPtr = std::make_unique<T>(std::forward<Args>(args)...);
		T& windowRef = *windowPtr;

		m_renderWindows.emplace_back(std::move(windowPtr));

		return windowRef;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
