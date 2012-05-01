// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp> // Pour éviter une redéfinition de WIN32_LEAN_AND_MEAN
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/BufferImpl.hpp>
#include <Nazara/Renderer/ShaderImpl.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzRenderer::NzRenderer() :
m_indexBuffer(nullptr),
m_target(nullptr),
m_shader(nullptr)
{
	#if NAZARA_RENDERER_SAFE
	if (s_instance)
		throw std::runtime_error("Renderer already instanced");
	#endif

	s_instance = this;
}

NzRenderer::~NzRenderer()
{
	Uninitialize();

	s_instance = nullptr;
}

void NzRenderer::Clear(nzRendererClear flags)
{
	#if NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	if (flags)
	{
		GLenum mask = 0;

		if (flags & nzRendererClear_Color)
			mask |= GL_COLOR_BUFFER_BIT;

		if (flags & nzRendererClear_Depth)
			mask |= GL_DEPTH_BUFFER_BIT;

		if (flags & nzRendererClear_Stencil)
			mask |= GL_STENCIL_BUFFER_BIT;

		glClear(mask);
	}
}

NzShader* NzRenderer::GetShader() const
{
	return m_shader;
}

NzRenderTarget* NzRenderer::GetTarget() const
{
	return m_target;
}

bool NzRenderer::HasCapability(nzRendererCap capability) const
{
	return m_capabilities[capability];
}

bool NzRenderer::Initialize()
{
	if (NzOpenGL::Initialize())
	{
		m_capabilities[nzRendererCap_AnisotropicFilter] = NzOpenGL::IsSupported(NzOpenGL::AnisotropicFilter);
		m_capabilities[nzRendererCap_FP64] = NzOpenGL::IsSupported(NzOpenGL::FP64);
		m_capabilities[nzRendererCap_HardwareBuffer] = true; // Natif depuis OpenGL 2.0
		m_capabilities[nzRendererCap_MultipleRenderTargets] = true; // Natif depuis OpenGL 2.0
		m_capabilities[nzRendererCap_Texture3D] = NzOpenGL::IsSupported(NzOpenGL::Texture3D);
		m_capabilities[nzRendererCap_TextureCubemap] = true; // Natif depuis OpenGL 1.3
		m_capabilities[nzRendererCap_TextureMulti] = true; // Natif depuis OpenGL 1.3
		m_capabilities[nzRendererCap_TextureNPOT] = true; // Natif depuis OpenGL 2.0

		return true;
	}
	else
		return false;
}

void NzRenderer::SetClearColor(nzUInt8 r, nzUInt8 g, nzUInt8 b, nzUInt8 a)
{
	#if NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glClearColor(r/255.f, g/255.f, b/255.f, a/255.f);
}

void NzRenderer::SetClearDepth(double depth)
{
	#if NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glClearDepth(depth);
}

void NzRenderer::SetClearStencil(unsigned int value)
{
	#if NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glClearStencil(value);
}

bool NzRenderer::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	if (indexBuffer == m_indexBuffer)
		return true;

	// OpenGL ne nécessite pas de débinder un index buffer pour ne pas l'utiliser
	if (indexBuffer)
		indexBuffer->GetBuffer()->m_impl->Bind();

	return true;
}

bool NzRenderer::SetShader(NzShader* shader)
{
	if (shader == m_shader)
		return true;

	if (shader)
	{
		#if NAZARA_RENDERER_SAFE
		if (!shader->IsCompiled())
		{
			NazaraError("Shader is not compiled");
			return false;
		}
		#endif

		if (!shader->m_impl->Bind())
		{
			NazaraError("Failed to bind shader");
			return false;
		}

		m_shader = shader;
	}
	else if (m_shader)
	{
		m_shader->m_impl->Unbind();
		m_shader = nullptr;
	}

	return true;
}

bool NzRenderer::SetTarget(NzRenderTarget* target)
{
	if (target == m_target)
		return true;

	#if NAZARA_RENDERER_SAFE
	if (target && !target->CanActivate())
	{
		NazaraError("Target cannot be activated");
		return false;
	}
	#endif

	if (m_target && !m_target->HasContext())
		m_target->Desactivate();

	if (target)
	{
		if (target->Activate())
			m_target = target;
		else
		{
			NazaraError("Failed to activate target");
			m_target = nullptr;

			return false;
		}
	}
	else
		m_target = nullptr;

	return true;
}

bool NzRenderer::SetVertexBuffer(const NzVertexBuffer* vertexBuffer)
{
	if (m_vertexBuffer == vertexBuffer)
		return true;

	m_vertexBuffer = vertexBuffer;
	m_vertexBufferUpdated = false;

	return true;
}

void NzRenderer::Uninitialize()
{
	NzOpenGL::Uninitialize();
}

#if NAZARA_RENDERER_SINGLETON
void NzRenderer::Destroy()
{
	delete s_instance;
	s_instance = nullptr;
}
#endif

NzRenderer* NzRenderer::Instance()
{
	#if NAZARA_RENDERER_SINGLETON
	if (!s_instance)
		s_instance = new NzRenderer;
	#elif defined(NAZARA_DEBUG)
	if (!s_instance)
		NazaraError("Renderer not instanced");
	#endif

	return s_instance;
}

NzRenderer* NzRenderer::s_instance = nullptr;
