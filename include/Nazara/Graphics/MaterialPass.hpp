// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALPASS_HPP
#define NAZARA_GRAPHICS_MATERIALPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/UniformBuffer.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NZSL/Ast/ConstantValue.hpp>
#include <NZSL/Ast/Option.hpp>
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
		std::unordered_map<nzsl::Ast::OptionHash, nzsl::Ast::ConstantSingleValue> options;
		std::vector<std::shared_ptr<UberShader>> shaders;
	};
}

#endif // NAZARA_GRAPHICS_MATERIALPASS_HPP
