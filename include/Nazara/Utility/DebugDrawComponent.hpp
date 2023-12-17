// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_DEBUGDRAWCOMPONENT_HPP
#define NAZARA_UTILITY_DEBUGDRAWCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API DebugDrawComponent
	{
		public:
			DebugDrawComponent() = default;
			DebugDrawComponent(const DebugDrawComponent&) = delete;
			DebugDrawComponent(DebugDrawComponent&&) = delete;
			~DebugDrawComponent() = default;

			DebugDrawComponent& operator=(const DebugDrawComponent&) = delete;
			DebugDrawComponent& operator=(DebugDrawComponent&&) = delete;

		private:
	};
}

#include <Nazara/Utility/DebugDrawComponent.inl>

#endif // NAZARA_UTILITY_DEBUGDRAWCOMPONENT_HPP
