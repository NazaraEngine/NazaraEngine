// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/GpuQuery.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzGpuQuery::NzGpuQuery() :
m_id(0)
{
	if (IsSupported())
	{
		NzContext::EnsureContext();

		glGenQueries(1, reinterpret_cast<GLuint*>(&m_id));
	}
	else
	{
		NazaraError("Occlusion queries not supported");
		return;
	}

	#ifdef NAZARA_DEBUG
	if (!m_id)
	{
		NazaraError("Failed to create occlusion query");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzGpuQuery::~NzGpuQuery()
{
	if (m_id)
	{
		NzContext::EnsureContext();

		GLuint query = static_cast<GLuint>(m_id);
		glDeleteQueries(1, &query);
	}
}

void NzGpuQuery::Begin(nzGpuQueryMode mode)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (!IsModeSupported(mode))
	{
		NazaraError("Mode (0x" + NzString::Number(mode, 16) + ") not supported");
		return;
	}
	#endif

	m_mode = mode;
	glBeginQuery(NzOpenGL::QueryMode[mode], m_id);
}

void NzGpuQuery::End()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glEndQuery(NzOpenGL::QueryMode[m_mode]);
}

unsigned int NzGpuQuery::GetResult() const
{
	NzContext::EnsureContext();

	GLuint result;
	glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &result);

	return result;
}

bool NzGpuQuery::IsResultAvailable() const
{
	NzContext::EnsureContext();

	GLint available;
	glGetQueryObjectiv(m_id, GL_QUERY_RESULT_AVAILABLE, &available);

	return available == GL_TRUE;
}

unsigned int NzGpuQuery::GetOpenGLID() const
{
	return m_id;
}

bool NzGpuQuery::IsModeSupported(nzGpuQueryMode mode)
{
	switch (mode)
	{
		case nzGpuQueryMode_AnySamplesPassed:
		case nzGpuQueryMode_TimeElapsed:
			return NzOpenGL::GetVersion() >= 330;

		case nzGpuQueryMode_AnySamplesPassedConservative:
			return NzOpenGL::GetVersion() >= 430;

		case nzGpuQueryMode_PrimitiveGenerated:
		case nzGpuQueryMode_SamplesPassed:
		case nzGpuQueryMode_TransformFeedbackPrimitivesWritten:
			return true;
	}

	NazaraError("Gpu Query mode not handled (0x" + NzString::Number(mode, 16) + ')');
	return false;
}

bool NzGpuQuery::IsSupported()
{
	return NzRenderer::HasCapability(nzRendererCap_OcclusionQuery);
}
