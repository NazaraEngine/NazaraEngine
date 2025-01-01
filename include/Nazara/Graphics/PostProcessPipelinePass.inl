// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline PostProcessPipelinePass::PostProcessPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	PostProcessPipelinePass(passData, std::move(passName), GetShaderName(parameters))
	{
	}
}
