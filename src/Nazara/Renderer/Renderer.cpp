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
#include <Nazara/Renderer/VertexDeclaration.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	const nzUInt8 attribIndex[] =
	{
		2,	// nzElementUsage_Diffuse
		1,	// nzElementUsage_Normal
		0,	// nzElementUsage_Position
		3,	// nzElementUsage_Tangent
		4	// nzElementUsage_TexCoord
	};

	const GLenum openglPrimitive[] = {
		GL_LINES,		   // nzPrimitiveType_LineList,
		GL_LINE_STRIP,	   // nzPrimitiveType_LineStrip,
		GL_POINTS,		   // nzPrimitiveType_PointList,
		GL_TRIANGLES,	   // nzPrimitiveType_TriangleList,
		GL_TRIANGLE_STRIP, // nzPrimitiveType_TriangleStrip,
		GL_TRIANGLE_FAN	   // nzPrimitiveType_TriangleFan
	};

	const nzUInt8 openglSize[] =
	{
		4, // nzElementType_Color
		1, // nzElementType_Double1
		2, // nzElementType_Double2
		3, // nzElementType_Double3
		4, // nzElementType_Double4
		1, // nzElementType_Float1
		2, // nzElementType_Float2
		3, // nzElementType_Float3
		4  // nzElementType_Float4
	};

	const GLenum openglType[] =
	{
		GL_UNSIGNED_BYTE, // nzElementType_Color
		GL_DOUBLE,		  // nzElementType_Double1
		GL_DOUBLE,		  // nzElementType_Double2
		GL_DOUBLE,		  // nzElementType_Double3
		GL_DOUBLE,		  // nzElementType_Double4
		GL_FLOAT,		  // nzElementType_Float1
		GL_FLOAT,		  // nzElementType_Float2
		GL_FLOAT,		  // nzElementType_Float3
		GL_FLOAT		  // nzElementType_Float4
	};
}

NzRenderer::NzRenderer() :
m_indexBuffer(nullptr),
m_target(nullptr),
m_shader(nullptr),
m_vertexBuffer(nullptr),
m_vertexDeclaration(nullptr),
m_statesUpdated(false)
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

	if (!m_statesUpdated)
	{
		if (!UpdateStates())
		{
			NazaraError("Failed to update states");
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
	if (!m_statesUpdated)
	{
		if (!UpdateStates())
		{
			NazaraError("Failed to update states");
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
		m_capabilities[nzRendererCap_OcclusionQuery] = true; // Natif depuis OpenGL 1.5
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
	if (indexBuffer != m_indexBuffer)
	{
		m_indexBuffer = indexBuffer;
		m_statesUpdated = false;
	}

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
	if (m_vertexBuffer != vertexBuffer)
	{
		m_vertexBuffer = vertexBuffer;
		m_statesUpdated = false;
	}

	return true;
}

bool NzRenderer::SetVertexDeclaration(const NzVertexDeclaration* vertexDeclaration)
{
	if (m_vertexDeclaration != vertexDeclaration)
	{
		m_vertexDeclaration = vertexDeclaration;
		m_statesUpdated = false;
	}

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

	s_initialized = false;

	// Libération des VAOs
	for (auto it = m_vaos.begin(); it != m_vaos.end(); ++it)
	{
		GLuint vao = it->second;
		glDeleteVertexArrays(1, &vao);
	}

	NzOpenGL::Uninitialize();

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

bool NzRenderer::UpdateStates()
{
	#if NAZARA_RENDERER_SAFE
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

	static const bool vaoSupported = NzOpenGL::IsSupported(NzOpenGL::VertexArrayObject);
	bool update;
	GLuint vao;

	// Si les VAOs sont supportés, on entoure nos appels par ceux-ci
	if (vaoSupported)
	{
		// On recherche si un VAO existe déjà avec notre configuration
		// Note: Les VAOs ne sont pas partagés entre les contextes, ces derniers font donc partie de notre configuration

		auto key = std::make_tuple(NzContext::GetCurrent(), m_indexBuffer, m_vertexBuffer, m_vertexDeclaration);
		auto it = m_vaos.find(key);
		if (it == m_vaos.end())
		{
			// On créé notre VAO
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// On l'ajoute à notre liste
			m_vaos.insert(std::make_pair(key, static_cast<unsigned int>(vao)));

			// Et on indique qu'on veut le programmer
			update = true;
		}
		else
		{
			// Notre VAO existe déjà, il est donc inutile de le reprogrammer
			vao = it->second;

			update = false;
		}
	}
	else
		update = true; // Fallback si les VAOs ne sont pas supportés

	if (update)
	{
		m_vertexBuffer->GetBuffer()->GetImpl()->Bind();

		const nzUInt8* buffer = reinterpret_cast<const nzUInt8*>(m_vertexBuffer->GetBufferPtr());

		///FIXME: Améliorer les déclarations pour permettre de faire ça plus simplement
		for (int i = 0; i < 12; ++i) // Solution temporaire, à virer
			glDisableVertexAttribArray(i); // Chaque itération tue un chaton :(

		unsigned int stride = m_vertexDeclaration->GetStride();
		unsigned int elementCount = m_vertexDeclaration->GetElementCount();
		for (unsigned int i = 0; i < elementCount; ++i)
		{
			const NzVertexDeclaration::Element* element = m_vertexDeclaration->GetElement(i);

			glEnableVertexAttribArray(attribIndex[element->usage]+element->usageIndex);
			glVertexAttribPointer(attribIndex[element->usage]+element->usageIndex,
								  openglSize[element->type],
								  openglType[element->type],
								  (element->type == nzElementType_Color) ? GL_TRUE : GL_FALSE,
								  stride,
								  &buffer[element->offset]);
		}

		if (m_indexBuffer)
			m_indexBuffer->GetBuffer()->GetImpl()->Bind();
	}

	if (vaoSupported)
	{
		// Si nous venons de définir notre VAO, nous devons le débinder pour indiquer la fin de sa construction
		if (update)
			glBindVertexArray(0);

		// Nous (re)bindons le VAO pour définir les attributs de vertice
		glBindVertexArray(vao);
	}

	m_statesUpdated = true;

	return true;
}

NzRenderer* NzRenderer::s_instance = nullptr;
bool NzRenderer::s_initialized = false;
