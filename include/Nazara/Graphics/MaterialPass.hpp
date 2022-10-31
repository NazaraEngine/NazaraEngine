// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALPASS_HPP
#define NAZARA_GRAPHICS_MATERIALPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/UniformBuffer.hpp>
#include <Nazara/Utils/Signal.hpp>
#include <NZSL/Ast/ConstantValue.hpp>
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace Nz
{
	struct MaterialPass
	{
		MaterialPassFlags flags;
		RenderStates states;
		std::unordered_map<UInt32, nzsl::Ast::ConstantSingleValue> options;
		std::vector<std::shared_ptr<UberShader>> shaders;
	};
}

#endif // NAZARA_GRAPHICS_MATERIALPASS_HPP
