// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderPipeline::RenderPipeline() :
	m_valid(false)
	{
	}

	inline RenderPipeline::~RenderPipeline()
	{
	}

	inline bool RenderPipeline::Create(const RenderPipelineInfo& pipelineInfo)
	{
		NazaraAssert(pipelineInfo.shader, "Invalid shader");

		m_pipelineInfo = pipelineInfo;
		m_valid = true;

		return true;
	}

	inline void RenderPipeline::Destroy()
	{
		m_valid = false;
	}

	inline const RenderPipelineInfo& RenderPipeline::GetInfo() const
	{
		NazaraAssert(m_valid, "Invalid pipeline info");

		return m_pipelineInfo;
	}

	inline bool RenderPipeline::IsValid() const
	{
		return m_valid;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
