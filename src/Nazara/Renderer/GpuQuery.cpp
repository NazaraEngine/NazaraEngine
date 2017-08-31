// Copyright (C) 2017 Jérôme Leclercq
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

namespace Nz
{
	GpuQuery::GpuQuery() :
	m_id(0)
	{
		Context::EnsureContext();

		m_id = 0;
		glGenQueries(1, static_cast<GLuint*>(&m_id));

		#ifdef NAZARA_DEBUG
		if (!m_id)
		{
			NazaraError("Failed to create occlusion query");
			throw std::runtime_error("Constructor failed");
		}
		#endif
	}

	GpuQuery::~GpuQuery()
	{
		if (m_id)
		{
			Context::EnsureContext();

			GLuint query = static_cast<GLuint>(m_id);
			glDeleteQueries(1, &query);
		}
	}

	void GpuQuery::Begin(GpuQueryMode mode)
	{
		#ifdef NAZARA_DEBUG
		if (Context::GetCurrent() == nullptr)
		{
			NazaraError("No active context");
			return;
		}
		#endif

		#if NAZARA_RENDERER_SAFE
		if (!IsModeSupported(mode))
		{
			NazaraError("Mode (0x" + String::Number(mode, 16) + ") not supported");
			return;
		}
		#endif

		m_mode = mode;
		glBeginQuery(OpenGL::QueryMode[mode], m_id);
	}

	void GpuQuery::End()
	{
		#ifdef NAZARA_DEBUG
		if (Context::GetCurrent() == nullptr)
		{
			NazaraError("No active context");
			return;
		}
		#endif

		glEndQuery(OpenGL::QueryMode[m_mode]);
	}

	unsigned int GpuQuery::GetResult() const
	{
		Context::EnsureContext();

		GLuint result;
		glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &result);

		return result;
	}

	bool GpuQuery::IsResultAvailable() const
	{
		Context::EnsureContext();

		GLint available;
		glGetQueryObjectiv(m_id, GL_QUERY_RESULT_AVAILABLE, &available);

		return available == GL_TRUE;
	}

	unsigned int GpuQuery::GetOpenGLID() const
	{
		return m_id;
	}

	bool GpuQuery::IsModeSupported(GpuQueryMode mode)
	{
		switch (mode)
		{
			case GpuQueryMode_AnySamplesPassedConservative:
				return OpenGL::GetVersion() >= 430;

			case GpuQueryMode_AnySamplesPassed:
			case GpuQueryMode_PrimitiveGenerated:
			case GpuQueryMode_SamplesPassed:
			case GpuQueryMode_TimeElapsed:
			case GpuQueryMode_TransformFeedbackPrimitivesWritten:
				return true;
		}

		NazaraError("Gpu Query mode not handled (0x" + String::Number(mode, 16) + ')');
		return false;
	}
}
