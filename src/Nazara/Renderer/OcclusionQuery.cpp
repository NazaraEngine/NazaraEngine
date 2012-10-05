// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/OcclusionQuery.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzOcclusionQuery::NzOcclusionQuery() :
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

NzOcclusionQuery::~NzOcclusionQuery()
{
	if (m_id)
	{
		NzContext::EnsureContext();

		GLuint query = static_cast<GLuint>(m_id);
		glDeleteQueries(1, &query);
	}
}

void NzOcclusionQuery::Begin()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glBeginQuery(GL_SAMPLES_PASSED, m_id);
}

void NzOcclusionQuery::End()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glEndQuery(GL_SAMPLES_PASSED);
}

unsigned int NzOcclusionQuery::GetResult() const
{
	NzContext::EnsureContext();

	GLuint result;
	glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &result);

	return result;
}

bool NzOcclusionQuery::IsResultAvailable() const
{
	NzContext::EnsureContext();

	GLint available;
	glGetQueryObjectiv(m_id, GL_QUERY_RESULT_AVAILABLE, &available);

	return available == GL_TRUE;
}

bool NzOcclusionQuery::IsSupported()
{
	return NzRenderer::HasCapability(nzRendererCap_OcclusionQuery);
}
