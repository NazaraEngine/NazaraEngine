// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALPASS_HPP
#define NAZARA_GRAPHICS_MATERIALPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <NazaraUtils/Constants.hpp>
#include <NZSL/Ast/ConstantValue.hpp>
#include <NZSL/Ast/Option.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	struct MaterialPass
	{
		MaterialPassFlags flags;
		RenderStates states;
		UInt32 renderQueue = MaxValue(); //< Should be set
		std::unordered_map<nzsl::Ast::OptionHash, nzsl::Ast::ConstantSingleValue> options;
		std::vector<std::shared_ptr<UberShader>> shaders;
	};
}

#endif // NAZARA_GRAPHICS_MATERIALPASS_HPP
