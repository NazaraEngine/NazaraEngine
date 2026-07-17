// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline RasterPipelinePass::RasterPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	RasterPipelinePass(passData, std::move(passName), GetRenderQueues(parameters), GetRenderMask(parameters))
	{
	}
}
