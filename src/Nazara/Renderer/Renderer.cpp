// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp> // Pour éviter une redéfinition de WIN32_LEAN_AND_MEAN
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/BufferImpl.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/IndexBuffer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderImpl.hpp>
#include <Nazara/Renderer/VertexBuffer.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	GLenum openglPrimitive[] = {
		GL_LINES,		   // nzPrimitiveType_LineList,
		GL_LINE_STRIP,	   // nzPrimitiveType_LineStrip,
		GL_POINTS,		   // nzPrimitiveType_PointList,
		GL_TRIANGLES,	   // nzPrimitiveType_TriangleList,
		GL_TRIANGLE_STRIP, // nzPrimitiveType_TriangleStrip,
		GL_TRIANGLE_FAN	   // nzPrimitiveType_TriangleFan
	};
}

NzRenderer::NzRenderer() :
m_indexBuffer(nullptr),
m_target(nullptr),
m_shader(nullptr),
m_vertexBuffer(nullptr),
m_vertexDeclaration(nullptr),
m_vertexBufferUpdated(false)
{
	#if NAZARA_RENDERER_SAFE
	if (s_instance)
		throw std::runtime_error("Renderer already instanced");
	#endif

	s_instance = this;
}

NzRenderer::~NzRenderer()
{
	if (s_initialized)
		Uninitialize();

	s_instance = nullptr;
}

void NzRenderer::Clear(unsigned long flags)
{
	#ifdef NAZARA_DEBUG
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

void NzRenderer::DrawIndexedPrimitives(nzPrimitiveType primitive, unsigned int firstIndex, unsigned int indexCount)
{
	#ifdef NAZARA_DEBUG
	if (!m_indexBuffer)
	{
		NazaraError("No index buffer");
		return;
	}
	#endif

	if (!m_vertexBufferUpdated)
	{
		if (!UpdateVertexBuffer())
		{
			NazaraError("Failed to update vertex buffer");
			return;
		}
	}

	nzUInt8 indexSize = m_indexBuffer->GetIndexSize();

	GLenum type;
	switch (indexSize)
	{
		case 1:
			type = GL_UNSIGNED_BYTE;
			break;

		case 2:
			type = GL_UNSIGNED_SHORT;
			break;

		case 4:
			type = GL_UNSIGNED_INT;
			break;

		default:
			NazaraError("Invalid index size (" + NzString::Number(indexSize) + ')');
			return;
	}

	glDrawElements(openglPrimitive[primitive], indexCount, type, reinterpret_cast<const nzUInt8*>(m_indexBuffer->GetBufferPtr()) + firstIndex*indexSize);
}

void NzRenderer::DrawPrimitives(nzPrimitiveType primitive, unsigned int firstVertex, unsigned int vertexCount)
{
	if (!m_vertexBufferUpdated)
	{
		if (!UpdateVertexBuffer())
		{
			NazaraError("Failed to update vertex buffer");
			return;
		}
	}

	glDrawArrays(openglPrimitive[primitive], firstVertex, vertexCount);
}

unsigned int NzRenderer::GetMaxTextureUnits() const
{
	static int maxTextureUnits = -1;
	if (maxTextureUnits == -1)
	{
		if (m_capabilities[nzRendererCap_TextureMulti])
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		else
			maxTextureUnits = 1;
	}

	return maxTextureUnits;
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
	#if NAZARA_RENDERER_SAFE
	if (s_initialized)
	{
		NazaraError("Renderer already initialized");
		return true;
	}
	#endif

	// Initialisation du module Utility
	if (!NzUtility::IsInitialized())
	{
		m_utilityModule = new NzUtility;
		m_utilityModule->Initialize();
	}

	if (NzOpenGL::Initialize())
	{
		m_capabilities[nzRendererCap_AnisotropicFilter] = NzOpenGL::IsSupported(NzOpenGL::AnisotropicFilter);
		m_capabilities[nzRendererCap_FP64] = NzOpenGL::IsSupported(NzOpenGL::FP64);
		m_capabilities[nzRendererCap_HardwareBuffer] = true; // Natif depuis OpenGL 1.5
		m_capabilities[nzRendererCap_MultipleRenderTargets] = true; // Natif depuis OpenGL 2.0
		m_capabilities[nzRendererCap_OcclusionQuery] = // Natif depuis OpenGL 1.5
		m_capabilities[nzRendererCap_SoftwareBuffer] = NzOpenGL::GetVersion() <= 300; // Déprécié en OpenGL 3
		m_capabilities[nzRendererCap_Texture3D] = NzOpenGL::IsSupported(NzOpenGL::Texture3D);
		m_capabilities[nzRendererCap_TextureCubemap] = true; // Natif depuis OpenGL 1.3
		m_capabilities[nzRendererCap_TextureMulti] = true; // Natif depuis OpenGL 1.3
		m_capabilities[nzRendererCap_TextureNPOT] = true; // Natif depuis OpenGL 2.0

		s_initialized = true;

		return true;
	}
	else
		return false;
}

void NzRenderer::SetClearColor(const NzColor& color)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glClearColor(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);
}

void NzRenderer::SetClearColor(nzUInt8 r, nzUInt8 g, nzUInt8 b, nzUInt8 a)
{
	#ifdef NAZARA_DEBUG
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
	#ifdef NAZARA_DEBUG
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
	#ifdef NAZARA_DEBUG
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
		m_vertexBufferUpdated = false;
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

	if (m_vertexBuffer && vertexBuffer)
	{
		// Si l'ancien buffer et le nouveau sont hardware, pas besoin de mettre à jour la déclaration
		if (!m_vertexBuffer->IsHardware() || !vertexBuffer->IsHardware())
			m_vertexBufferUpdated = false;
	}
	m_vertexBuffer = vertexBuffer;

	return true;
}

bool NzRenderer::SetVertexDeclaration(const NzVertexDeclaration* vertexDeclaration)
{
	m_vertexDeclaration = vertexDeclaration;
	m_vertexBufferUpdated = false;

	return true;
}

void NzRenderer::Uninitialize()
{
	#if NAZARA_RENDERER_SAFE
	if (!s_initialized)
	{
		NazaraError("Renderer not initialized");
		return;
	}
	#endif

	NzOpenGL::Uninitialize();

	s_initialized = false;

	if (m_utilityModule)
	{
		delete m_utilityModule;
		m_utilityModule = nullptr;
	}
}

NzRenderer* NzRenderer::Instance()
{
	#if defined(NAZARA_DEBUG)
	if (!s_instance)
		NazaraError("Renderer not instanced");
	#endif

	return s_instance;
}

bool NzRenderer::IsInitialized()
{
	return s_initialized;
}

bool NzRenderer::UpdateVertexBuffer()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_shader)
	{
		NazaraError("No shader");
		return false;
	}

	if (!m_vertexBuffer)
	{
		NazaraError("No vertex buffer");
		return false;
	}

	if (!m_vertexDeclaration)
	{
		NazaraError("No vertex declaration");
		return false;
	}
	#endif

	if (!m_shader->m_impl->UpdateVertexBuffer(m_vertexBuffer, m_vertexDeclaration))
		return false;

	m_vertexBufferUpdated = true;

	return true;
}

NzRenderer* NzRenderer::s_instance = nullptr;
bool NzRenderer::s_initialized = false;
