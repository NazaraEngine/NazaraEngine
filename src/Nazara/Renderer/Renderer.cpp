// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp> // Pour éviter une redéfinition de WIN32_LEAN_AND_MEAN
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderImpl.hpp>
#include <Nazara/Utility/BufferImpl.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	const nzUInt8 attribIndex[] =
	{
		2, // nzElementUsage_Diffuse
		1, // nzElementUsage_Normal
		0, // nzElementUsage_Position
		3, // nzElementUsage_Tangent

		4  // nzElementUsage_TexCoord (Doit être le dernier de la liste car extensible)
	};


	const GLenum blendFunc[] =
	{
		GL_DST_ALPHA,           // nzBlendFunc_DestAlpha
		GL_DST_COLOR,           // nzBlendFunc_DestColor
		GL_SRC_ALPHA,           // nzBlendFunc_SrcAlpha
		GL_SRC_COLOR,           // nzBlendFunc_SrcColor
		GL_ONE_MINUS_DST_ALPHA, // nzBlendFunc_InvDestAlpha
		GL_ONE_MINUS_DST_COLOR, // nzBlendFunc_InvDestColor
		GL_ONE_MINUS_SRC_ALPHA, // nzBlendFunc_InvSrcAlpha
		GL_ONE_MINUS_SRC_COLOR, // nzBlendFunc_InvSrcColor
		GL_ONE,                 // nzBlendFunc_One
		GL_ZERO                 // nzBlendFunc_Zero
	};

	const GLenum faceCullingMode[] =
	{
		GL_BACK,          // nzFaceCulling_Back
		GL_FRONT,	      // nzFaceCulling_Front
		GL_FRONT_AND_BACK // nzFaceCulling_FrontAndBack
	};

	const GLenum faceFillingMode[] =
	{
		GL_POINT, // nzFaceFilling_Point
		GL_LINE,  // nzFaceFilling_Line
		GL_FILL   // nzFaceFilling_Fill
	};


	const GLenum openglPrimitive[] =
	{
		GL_LINES,          // nzPrimitiveType_LineList,
		GL_LINE_STRIP,     // nzPrimitiveType_LineStrip,
		GL_POINTS,         // nzPrimitiveType_PointList,
		GL_TRIANGLES,      // nzPrimitiveType_TriangleList,
		GL_TRIANGLE_STRIP, // nzPrimitiveType_TriangleStrip,
		GL_TRIANGLE_FAN    // nzPrimitiveType_TriangleFan
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
		GL_DOUBLE,        // nzElementType_Double1
		GL_DOUBLE,        // nzElementType_Double2
		GL_DOUBLE,        // nzElementType_Double3
		GL_DOUBLE,        // nzElementType_Double4
		GL_FLOAT,         // nzElementType_Float1
		GL_FLOAT,         // nzElementType_Float2
		GL_FLOAT,         // nzElementType_Float3
		GL_FLOAT          // nzElementType_Float4
	};

	const GLenum rendererComparison[] =
	{
		GL_ALWAYS,  // nzRendererComparison_Always
		GL_EQUAL,   // nzRendererComparison_Equal
		GL_GREATER, // nzRendererComparison_Greater
		GL_GEQUAL,  // nzRendererComparison_GreaterOrEqual
		GL_LESS,    // nzRendererComparison_Less
		GL_LEQUAL,  // nzRendererComparison_LessOrEqual
		GL_NEVER    // nzRendererComparison_Never
	};

	const GLenum rendererParameter[] =
	{
		GL_BLEND,       // nzRendererParameter_Blend
		GL_NONE,        // nzRendererParameter_ColorWrite
		GL_DEPTH_TEST,  // nzRendererParameter_DepthTest
		GL_NONE,        // nzRendererParameter_DepthWrite
		GL_CULL_FACE,   // nzRendererParameter_FaceCulling
		GL_STENCIL_TEST // nzRendererParameter_Stencil
	};

	const GLenum stencilOperation[] =
	{
		GL_DECR,      // nzStencilOperation_Decrement
		GL_DECR_WRAP, // nzStencilOperation_DecrementToSaturation
		GL_INCR,      // nzStencilOperation_Increment
		GL_INCR_WRAP, // nzStencilOperation_IncrementToSaturation
		GL_INVERT,    // nzStencilOperation_Invert
		GL_KEEP,      // nzStencilOperation_Keep
		GL_REPLACE,   // nzStencilOperation_Replace
		GL_ZERO       // nzStencilOperation_Zero
	};

	NzBufferImpl* HardwareBufferFunction(NzBuffer* parent, nzBufferType type)
	{
		return new NzHardwareBuffer(parent, type);
	}
}

NzRenderer::NzRenderer()
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
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}

	if (!m_indexBuffer)
	{
		NazaraError("No index buffer");
		return;
	}
	#endif

	if (!EnsureStateUpdate())
	{
		NazaraError("Failed to update states");
		return;
	}

	if (m_indexBuffer->IsSequential())
		glDrawArrays(openglPrimitive[primitive], m_indexBuffer->GetStartIndex(), m_indexBuffer->GetIndexCount());
	else
	{
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

		glDrawElements(openglPrimitive[primitive], indexCount, type, reinterpret_cast<const nzUInt8*>(m_indexBuffer->GetPointer()) + firstIndex*indexSize);
	}
}

void NzRenderer::DrawPrimitives(nzPrimitiveType primitive, unsigned int firstVertex, unsigned int vertexCount)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	if (!EnsureStateUpdate())
	{
		NazaraError("Failed to update states");
		return;
	}

	glDrawArrays(openglPrimitive[primitive], firstVertex, vertexCount);
}

void NzRenderer::Enable(nzRendererParameter parameter, bool enable)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	switch (parameter)
	{
		case nzRendererParameter_ColorWrite:
			glColorMask(enable, enable, enable, enable);
			break;

		case nzRendererParameter_DepthWrite:
			glDepthMask(enable);
			break;

		default:
			if (enable)
				glEnable(rendererParameter[parameter]);
			else
				glDisable(rendererParameter[parameter]);

			break;
	}
}

unsigned int NzRenderer::GetMaxAnisotropyLevel() const
{
	return m_maxAnisotropyLevel;
}

unsigned int NzRenderer::GetMaxRenderTargets() const
{
	return m_maxRenderTarget;
}

unsigned int NzRenderer::GetMaxTextureUnits() const
{
	return m_maxTextureUnit;
}

NzShader* NzRenderer::GetShader() const
{
	return m_shader;
}

NzRenderTarget* NzRenderer::GetTarget() const
{
	return m_target;
}

NzRectui NzRenderer::GetViewport() const
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return NzRectui();
	}
	#endif

	GLint params[4];
	glGetIntegerv(GL_VIEWPORT, &params[0]);

	return NzRectui(params[0], params[1], params[2], params[3]);
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
	else
		m_utilityModule = nullptr;

	if (NzOpenGL::Initialize())
	{
		NzContext::EnsureContext();

		m_indexBuffer = nullptr;
		m_shader = nullptr;
		m_stencilCompare = nzRendererComparison_Always;
		m_stencilFail = nzStencilOperation_Keep;
		m_stencilFuncUpdated = true;
		m_stencilMask = 0xFFFFFFFF;
		m_stencilOpUpdated = true;
		m_stencilPass = nzStencilOperation_Keep;
		m_stencilReference = 0;
		m_stencilZFail = nzStencilOperation_Keep;
		m_target = nullptr;
		m_vaoUpdated = false;
		m_vertexBuffer = nullptr;
		m_vertexDeclaration = nullptr;

		// Récupération des capacités
		m_capabilities[nzRendererCap_AnisotropicFilter] = NzOpenGL::IsSupported(NzOpenGL::AnisotropicFilter);
		m_capabilities[nzRendererCap_FP64] = NzOpenGL::IsSupported(NzOpenGL::FP64);
		m_capabilities[nzRendererCap_HardwareBuffer] = true; // Natif depuis OpenGL 1.5
		m_capabilities[nzRendererCap_MultipleRenderTargets] = true; // Natif depuis OpenGL 2.0
		m_capabilities[nzRendererCap_OcclusionQuery] = true; // Natif depuis OpenGL 1.5
		m_capabilities[nzRendererCap_PixelBufferObject] = NzOpenGL::IsSupported(NzOpenGL::PixelBufferObject);
		m_capabilities[nzRendererCap_Texture3D] = NzOpenGL::IsSupported(NzOpenGL::Texture3D);
		m_capabilities[nzRendererCap_TextureCubemap] = true; // Natif depuis OpenGL 1.3
		m_capabilities[nzRendererCap_TextureMulti] = true; // Natif depuis OpenGL 1.3
		m_capabilities[nzRendererCap_TextureNPOT] = true; // Natif depuis OpenGL 2.0

		if (m_capabilities[nzRendererCap_AnisotropicFilter])
		{
			GLint maxAnisotropy;
			glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

			m_maxAnisotropyLevel = static_cast<unsigned int>(maxAnisotropy);
		}
		else
			m_maxAnisotropyLevel = 1;

		if (m_capabilities[nzRendererCap_MultipleRenderTargets])
		{
			GLint maxDrawBuffers;
			glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

			m_maxRenderTarget = static_cast<unsigned int>(maxDrawBuffers);
		}
		else
			m_maxRenderTarget = 1;

		if (m_capabilities[nzRendererCap_TextureMulti])
		{
			GLint maxTextureUnits;
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

			GLint maxVertexAttribs;
			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

			// Impossible de binder plus de texcoords que d'attributes (en sachant qu'un certain nombre est déjà pris par les autres attributs)
			m_maxTextureUnit = static_cast<unsigned int>(std::min(maxTextureUnits, maxVertexAttribs-attribIndex[nzElementUsage_TexCoord]));
		}
		else
			m_maxTextureUnit = 1;

		NzBuffer::SetBufferFunction(nzBufferStorage_Hardware, HardwareBufferFunction);

		s_initialized = true;

		return true;
	}
	else
		return false;
}

void NzRenderer::SetBlendFunc(nzBlendFunc src, nzBlendFunc dest)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glBlendFunc(blendFunc[src], blendFunc[dest]);
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

void NzRenderer::SetFaceCulling(nzFaceCulling cullingMode)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glCullFace(faceCullingMode[cullingMode]);
}

void NzRenderer::SetFaceFilling(nzFaceFilling fillingMode)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glPolygonMode(GL_FRONT_AND_BACK, faceFillingMode[fillingMode]);
}

bool NzRenderer::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	#if NAZARA_RENDERER_SAFE
	if (indexBuffer && !indexBuffer->IsHardware() && !indexBuffer->IsSequential())
	{
		NazaraError("Buffer must be hardware");
		return false;
	}
	#endif

	if (indexBuffer != m_indexBuffer)
	{
		m_indexBuffer = indexBuffer;
		m_vaoUpdated = false;
	}

	return true;
}

bool NzRenderer::SetShader(NzShader* shader)
{
	if (shader == m_shader)
		return true;

	if (m_shader)
		m_shader->m_impl->Unbind();

	if (shader)
	{
		#if NAZARA_RENDERER_SAFE
		if (!shader->IsCompiled())
		{
			NazaraError("Shader is not compiled");
			m_shader = nullptr;

			return false;
		}
		#endif

		if (!shader->m_impl->Bind())
		{
			NazaraError("Failed to bind shader");
			m_shader = nullptr;

			return false;
		}
	}

	m_shader = shader;

	return true;
}

void NzRenderer::SetStencilCompareFunction(nzRendererComparison compareFunc)
{
	if (compareFunc != m_stencilCompare)
	{
		m_stencilCompare = compareFunc;
		m_stencilFuncUpdated = false;
	}
}

void NzRenderer::SetStencilFailOperation(nzStencilOperation failOperation)
{
	if (failOperation != m_stencilFail)
	{
		m_stencilFail = failOperation;
		m_stencilOpUpdated = false;
	}
}

void NzRenderer::SetStencilMask(nzUInt32 mask)
{
	if (mask != m_stencilMask)
	{
		m_stencilMask = mask;
		m_stencilFuncUpdated = false;
	}
}

void NzRenderer::SetStencilPassOperation(nzStencilOperation passOperation)
{
	if (passOperation != m_stencilPass)
	{
		m_stencilPass = passOperation;
		m_stencilOpUpdated = false;
	}
}

void NzRenderer::SetStencilReferenceValue(unsigned int refValue)
{
	if (refValue != m_stencilReference)
	{
		m_stencilReference = refValue;
		m_stencilFuncUpdated = false;
	}
}

void NzRenderer::SetStencilZFailOperation(nzStencilOperation zfailOperation)
{
	if (zfailOperation != m_stencilZFail)
	{
		m_stencilZFail = zfailOperation;
		m_stencilOpUpdated = false;
	}
}

bool NzRenderer::SetTarget(NzRenderTarget* target)
{
	if (target == m_target)
		return true;

	if (m_target && !m_target->HasContext())
		m_target->Desactivate();

	if (target)
	{
		#if NAZARA_RENDERER_SAFE
		if (!target->IsValid())
		{
			NazaraError("Target not valid");
			return false;
		}
		#endif

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
	#if NAZARA_RENDERER_SAFE
	if (vertexBuffer && !vertexBuffer->IsHardware())
	{
		NazaraError("Buffer must be hardware");
		return false;
	}
	#endif

	if (m_vertexBuffer != vertexBuffer)
	{
		m_vertexBuffer = vertexBuffer;
		m_vaoUpdated = false;
	}

	return true;
}

bool NzRenderer::SetVertexDeclaration(const NzVertexDeclaration* vertexDeclaration)
{
	if (m_vertexDeclaration != vertexDeclaration)
	{
		m_vertexDeclaration = vertexDeclaration;
		m_vaoUpdated = false;
	}

	return true;
}

void NzRenderer::SetViewport(const NzRectui& viewport)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	unsigned int height = m_target->GetHeight();

	#if NAZARA_RENDERER_SAFE
	if (!m_target)
	{
		NazaraError("Renderer has no target");
		return;
	}

	unsigned int width = m_target->GetWidth();
	if (viewport.x+viewport.width > width || viewport.y+viewport.height > height)
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return;
	}
	#endif

	glViewport(viewport.x, height-viewport.height-viewport.y, viewport.width, viewport.height);
	glScissor(viewport.x, height-viewport.height-viewport.y, viewport.width, viewport.height);
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

	NzContext::EnsureContext();

	s_initialized = false;

	// Libération des VAOs
	for (auto it = m_vaos.begin(); it != m_vaos.end(); ++it)
	{
		GLuint vao = static_cast<GLuint>(it->second);
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

bool NzRenderer::EnsureStateUpdate()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return false;
	}
	#endif

	if (!m_stencilFuncUpdated)
	{
		glStencilFunc(rendererComparison[m_stencilCompare], m_stencilReference, m_stencilMask);
		m_stencilFuncUpdated = true;
	}

	if (!m_stencilOpUpdated)
	{
		glStencilOp(stencilOperation[m_stencilFail], stencilOperation[m_stencilZFail], stencilOperation[m_stencilPass]);
		m_stencilOpUpdated = true;
	}

	if (!m_vaoUpdated)
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
			NzHardwareBuffer* vertexBuffer = static_cast<NzHardwareBuffer*>(m_vertexBuffer->GetBuffer()->GetImpl());
			vertexBuffer->Bind();

			const nzUInt8* buffer = reinterpret_cast<const nzUInt8*>(m_vertexBuffer->GetPointer());

			unsigned int stride = m_vertexDeclaration->GetStride(nzElementStream_VertexData);
			for (unsigned int i = 0; i <= nzElementUsage_Max; ++i)
			{
				const NzVertexElement* element = m_vertexDeclaration->GetElement(nzElementStream_VertexData, static_cast<nzElementUsage>(i));

				if (element)
				{
					glEnableVertexAttribArray(attribIndex[i]);
					glVertexAttribPointer(attribIndex[i],
										  openglSize[element->type],
										  openglType[element->type],
										  (element->type == nzElementType_Color) ? GL_TRUE : GL_FALSE,
										  stride,
										  &buffer[element->offset]);
				}
				else
					glDisableVertexAttribArray(attribIndex[i]);
			}

			if (m_indexBuffer)
			{
				NzHardwareBuffer* indexBuffer = static_cast<NzHardwareBuffer*>(m_indexBuffer->GetBuffer()->GetImpl());
				indexBuffer->Bind();
			}
		}

		if (vaoSupported)
		{
			// Si nous venons de définir notre VAO, nous devons le débinder pour indiquer la fin de sa construction
			if (update)
				glBindVertexArray(0);

			// Nous (re)bindons le VAO pour définir les attributs de vertice
			glBindVertexArray(vao);
		}

		m_vaoUpdated = true;
	}

	return true;
}

NzRenderer* NzRenderer::s_instance = nullptr;
bool NzRenderer::s_initialized = false;
