// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_PREDEFINEDSHADERSTRUCTBUILDER_HPP
#define NAZARA_RENDERER_PREDEFINEDSHADERSTRUCTBUILDER_HPP

#include <NZSL/Math/FieldOffsets.hpp>

namespace Nz
{
	struct DebugDrawerData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t viewProjOffset;

		std::size_t totalSize;

		static constexpr DebugDrawerData Build();
	};
}

#include <Nazara/Renderer/PredefinedShaderStructBuilder.inl>

#endif // NAZARA_RENDERER_PREDEFINEDSHADERSTRUCTBUILDER_HPP
