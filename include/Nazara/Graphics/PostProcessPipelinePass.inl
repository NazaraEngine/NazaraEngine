// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline PostProcessPipelinePass::PostProcessPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	PostProcessPipelinePass(passData, std::move(passName), GetShaderName(parameters))
	{
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
