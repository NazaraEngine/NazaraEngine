// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp> // Pour éviter une redéfinition de WIN32_LEAN_AND_MEAN
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
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
#include <tuple>
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

	///FIXME: Solution temporaire pour plus de facilité
	enum nzMatrixCombination
	{
		nzMatrixCombination_ViewProj = nzMatrixType_Max+1,
		nzMatrixCombination_WorldView,
		nzMatrixCombination_WorldViewProj,

		nzMatrixCombination_Max = nzMatrixCombination_WorldViewProj
	};

	NzBufferImpl* HardwareBufferFunction(NzBuffer* parent, nzBufferType type)
	{
		return new NzHardwareBuffer(parent, type);
	}

	typedef std::tuple<const NzContext*, const NzIndexBuffer*, const NzVertexBuffer*, const NzVertexDeclaration*> VAO_Key;

	constexpr unsigned int totalMatrixCount = nzMatrixCombination_Max+1;

	std::map<VAO_Key, unsigned int> s_vaos;
	NzMatrix4f s_matrix[totalMatrixCount];
	int s_matrixLocation[totalMatrixCount];
	bool s_matrixUpdated[totalMatrixCount];
	nzRendererComparison s_stencilCompare;
	nzStencilOperation s_stencilFail;
	nzStencilOperation s_stencilPass;
	nzStencilOperation s_stencilZFail;
	nzUInt32 s_stencilMask;
	const NzIndexBuffer* s_indexBuffer;
	NzRenderTarget* s_target;
	NzShader* s_shader;
	const NzVertexBuffer* s_vertexBuffer;
	const NzVertexDeclaration* s_vertexDeclaration;
	bool s_vaoUpdated;
	bool s_capabilities[nzRendererCap_Max+1];
	bool s_stencilFuncUpdated;
	bool s_stencilOpUpdated;
	unsigned int s_maxAnisotropyLevel;
	unsigned int s_maxRenderTarget;
	unsigned int s_maxTextureUnit;
	unsigned int s_stencilReference;
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
	#endif

	#if NAZARA_RENDERER_SAFE
	if (!s_indexBuffer)
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

	if (s_indexBuffer->IsSequential())
		glDrawArrays(openglPrimitive[primitive], s_indexBuffer->GetStartIndex(), s_indexBuffer->GetIndexCount());
	else
	{
		nzUInt8 indexSize = s_indexBuffer->GetIndexSize();

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

		glDrawElements(openglPrimitive[primitive], indexCount, type, reinterpret_cast<const nzUInt8*>(s_indexBuffer->GetPointer()) + firstIndex*indexSize);
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
/*
NzMatrix4f NzRenderer::GetMatrix(nzMatrixCombination combination)
{
	switch (combination)
	{
		case nzMatrixCombination_ViewProj:
			if (!s_matrixUpdated[nzMatrixCombination_ViewProj])
			{
				s_matrix[nzMatrixCombination_ViewProj] = s_matrix[nzMatrixType_View] * s_matrix[nzMatrixType_Projection];
				s_matrixUpdated[nzMatrixCombination_ViewProj] = true;
			}
			break;

		case nzMatrixCombination_WorldView:
			if (!s_matrixUpdated[nzMatrixCombination_WorldView])
			{
				s_matrix[nzMatrixCombination_WorldView] = NzMatrix4f::ConcatenateAffine(s_matrix[nzMatrixType_World], s_matrix[nzMatrixType_View]);
				s_matrixUpdated[nzMatrixCombination_WorldView] = true;
			}
			break;

		case nzMatrixCombination_WorldViewProj:
			if (!s_matrixUpdated[nzMatrixCombination_WorldViewProj])
			{
				s_matrix[nzMatrixCombination_WorldViewProj] = s_matrix[nzMatrixCombination_WorldView] * s_matrix[nzMatrixType_Projection];
				s_matrixUpdated[nzMatrixCombination_WorldViewProj] = true;
			}
			break;
	}

	return m_matrix[combination];
}
*/
NzMatrix4f NzRenderer::GetMatrix(nzMatrixType type)
{
	return s_matrix[type];
}

unsigned int NzRenderer::GetMaxAnisotropyLevel()
{
	return s_maxAnisotropyLevel;
}

unsigned int NzRenderer::GetMaxRenderTargets()
{
	return s_maxRenderTarget;
}

unsigned int NzRenderer::GetMaxTextureUnits()
{
	return s_maxTextureUnit;
}

NzShader* NzRenderer::GetShader()
{
	return s_shader;
}

NzRenderTarget* NzRenderer::GetTarget()
{
	return s_target;
}

NzRectui NzRenderer::GetViewport()
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

bool NzRenderer::HasCapability(nzRendererCap capability)
{
	return s_capabilities[capability];
}

bool NzRenderer::Initialize()
{
	if (s_moduleReferenceCouter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzUtility::Initialize())
	{
		NazaraError("Failed to initialize utility module");
		return false;
	}

	// Initialisation du module
	if (!NzOpenGL::Initialize())
	{
		NazaraError("Failed to initialize OpenGL");
		return false;
	}

	NzContext::EnsureContext();

	for (unsigned int i = 0; i < totalMatrixCount; ++i)
	{
		s_matrix[i].MakeIdentity();
		s_matrixLocation[i] = -1;
		s_matrixUpdated[i] = false;
	}

	s_indexBuffer = nullptr;
	s_shader = nullptr;
	s_stencilCompare = nzRendererComparison_Always;
	s_stencilFail = nzStencilOperation_Keep;
	s_stencilFuncUpdated = true;
	s_stencilMask = 0xFFFFFFFF;
	s_stencilOpUpdated = true;
	s_stencilPass = nzStencilOperation_Keep;
	s_stencilReference = 0;
	s_stencilZFail = nzStencilOperation_Keep;
	s_target = nullptr;
	s_vaoUpdated = false;
	s_vertexBuffer = nullptr;
	s_vertexDeclaration = nullptr;

	// Récupération des capacités
	s_capabilities[nzRendererCap_AnisotropicFilter] = NzOpenGL::IsSupported(NzOpenGL::AnisotropicFilter);
	s_capabilities[nzRendererCap_FP64] = NzOpenGL::IsSupported(NzOpenGL::FP64);
	s_capabilities[nzRendererCap_HardwareBuffer] = true; // Natif depuis OpenGL 1.5
	s_capabilities[nzRendererCap_MultipleRenderTargets] = true; // Natif depuis OpenGL 2.0
	s_capabilities[nzRendererCap_OcclusionQuery] = true; // Natif depuis OpenGL 1.5
	s_capabilities[nzRendererCap_PixelBufferObject] = NzOpenGL::IsSupported(NzOpenGL::PixelBufferObject);
	s_capabilities[nzRendererCap_Texture3D] = NzOpenGL::IsSupported(NzOpenGL::Texture3D);
	s_capabilities[nzRendererCap_TextureCubemap] = true; // Natif depuis OpenGL 1.3
	s_capabilities[nzRendererCap_TextureMulti] = true; // Natif depuis OpenGL 1.3
	s_capabilities[nzRendererCap_TextureNPOT] = true; // Natif depuis OpenGL 2.0

	if (s_capabilities[nzRendererCap_AnisotropicFilter])
	{
		GLint maxAnisotropy;
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

		s_maxAnisotropyLevel = static_cast<unsigned int>(maxAnisotropy);
	}
	else
		s_maxAnisotropyLevel = 1;

	if (s_capabilities[nzRendererCap_MultipleRenderTargets])
	{
		GLint maxDrawBuffers;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

		s_maxRenderTarget = static_cast<unsigned int>(maxDrawBuffers);
	}
	else
		s_maxRenderTarget = 1;

	if (s_capabilities[nzRendererCap_TextureMulti])
	{
		GLint maxTextureUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

		GLint maxVertexAttribs;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

		// Impossible de binder plus de texcoords que d'attributes (en sachant qu'un certain nombre est déjà pris par les autres attributs)
		s_maxTextureUnit = static_cast<unsigned int>(std::min(maxTextureUnits, maxVertexAttribs-attribIndex[nzElementUsage_TexCoord]));
	}
	else
		s_maxTextureUnit = 1;

	NzBuffer::SetBufferFunction(nzBufferStorage_Hardware, HardwareBufferFunction);

	NazaraNotice("Initialized: Renderer module");

	return true;
}

bool NzRenderer::IsInitialized()
{
	return s_moduleReferenceCouter != 0;
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

	if (s_indexBuffer != indexBuffer)
	{
		s_indexBuffer = indexBuffer;
		s_vaoUpdated = false;
	}

	return true;
}

void NzRenderer::SetMatrix(nzMatrixType type, const NzMatrix4f& matrix)
{
	s_matrix[type] = matrix;

	// Cas particulier, la matrice projection doit être inversée sur l'axe Y à cause des conventions d'OpenGL
	if (type == nzMatrixType_Projection)
		s_matrix[type] *= NzMatrix4f::Scale(NzVector3f(1.f, -1.f, 1.f));

	// Invalidation des combinaisons
	switch (type)
	{
		case nzMatrixType_View:
		case nzMatrixType_World:
			s_matrixUpdated[nzMatrixCombination_WorldView] = false;
		case nzMatrixType_Projection:
			s_matrixUpdated[nzMatrixCombination_WorldViewProj] = false;
			s_matrixUpdated[nzMatrixCombination_ViewProj] = false;
			break;
	}
}

bool NzRenderer::SetShader(NzShader* shader)
{
	if (s_shader == shader)
		return true;

	if (s_shader)
		s_shader->m_impl->Unbind();

	if (shader)
	{
		#if NAZARA_RENDERER_SAFE
		if (!shader->IsCompiled())
		{
			NazaraError("Shader is not compiled");
			shader = nullptr;

			return false;
		}
		#endif

		if (!shader->m_impl->Bind())
		{
			NazaraError("Failed to bind shader");
			shader = nullptr;

			return false;
		}

		// Récupération des indices des variables uniformes (-1 si la variable n'existe pas)
		s_matrixLocation[nzMatrixType_Projection] = shader->GetUniformLocation("ProjMatrix");
		s_matrixLocation[nzMatrixType_View] = shader->GetUniformLocation("ViewMatrix");
		s_matrixLocation[nzMatrixType_World] = shader->GetUniformLocation("WorldMatrix");

		s_matrixLocation[nzMatrixCombination_ViewProj] = shader->GetUniformLocation("ViewProjMatrix");
		s_matrixLocation[nzMatrixCombination_WorldView] = shader->GetUniformLocation("WorldViewMatrix");
		s_matrixLocation[nzMatrixCombination_WorldViewProj] = shader->GetUniformLocation("WorldViewProjMatrix");
	}

	s_shader = shader;

	return true;
}

void NzRenderer::SetStencilCompareFunction(nzRendererComparison compareFunc)
{
	if (compareFunc != s_stencilCompare)
	{
		s_stencilCompare = compareFunc;
		s_stencilFuncUpdated = false;
	}
}

void NzRenderer::SetStencilFailOperation(nzStencilOperation failOperation)
{
	if (failOperation != s_stencilFail)
	{
		s_stencilFail = failOperation;
		s_stencilOpUpdated = false;
	}
}

void NzRenderer::SetStencilMask(nzUInt32 mask)
{
	if (mask != s_stencilMask)
	{
		s_stencilMask = mask;
		s_stencilFuncUpdated = false;
	}
}

void NzRenderer::SetStencilPassOperation(nzStencilOperation passOperation)
{
	if (passOperation != s_stencilPass)
	{
		s_stencilPass = passOperation;
		s_stencilOpUpdated = false;
	}
}

void NzRenderer::SetStencilReferenceValue(unsigned int refValue)
{
	if (refValue != s_stencilReference)
	{
		s_stencilReference = refValue;
		s_stencilFuncUpdated = false;
	}
}

void NzRenderer::SetStencilZFailOperation(nzStencilOperation zfailOperation)
{
	if (zfailOperation != s_stencilZFail)
	{
		s_stencilZFail = zfailOperation;
		s_stencilOpUpdated = false;
	}
}

bool NzRenderer::SetTarget(NzRenderTarget* target)
{
	if (s_target == target)
		return true;

	if (s_target && !target->HasContext())
		s_target->Desactivate();

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
			s_target = target;
		else
		{
			NazaraError("Failed to activate target");
			s_target = nullptr;

			return false;
		}
	}
	else
		s_target = nullptr;

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

	if (s_vertexBuffer != vertexBuffer)
	{
		s_vertexBuffer = vertexBuffer;
		s_vaoUpdated = false;
	}

	return true;
}

bool NzRenderer::SetVertexDeclaration(const NzVertexDeclaration* vertexDeclaration)
{
	if (s_vertexDeclaration != vertexDeclaration)
	{
		s_vertexDeclaration = vertexDeclaration;
		s_vaoUpdated = false;
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

	unsigned int height = s_target->GetHeight();

	#if NAZARA_RENDERER_SAFE
	if (!s_target)
	{
		NazaraError("Renderer has no target");
		return;
	}

	unsigned int width = s_target->GetWidth();
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
	if (--s_moduleReferenceCouter != 0)
		return; // Encore utilisé

	// Libération du module
	NzContext::EnsureContext();

	// Libération des VAOs
	for (auto it = s_vaos.begin(); it != s_vaos.end(); ++it)
	{
		GLuint vao = static_cast<GLuint>(it->second);
		glDeleteVertexArrays(1, &vao);
	}

	NzOpenGL::Uninitialize();

	NazaraNotice("Uninitialized: Renderer module");

	// Libération des dépendances
	NzUtility::Uninitialize();
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

	#if NAZARA_RENDERER_SAFE
	if (!s_shader)
	{
		NazaraError("No shader");
		return false;
	}
	#endif

	// Il est plus rapide d'opérer sur l'implémentation du shader directement
	NzShaderImpl* shaderImpl = s_shader->m_impl;

	if (!shaderImpl->BindTextures())
		NazaraWarning("Failed to bind textures");

	for (unsigned int i = 0; i <= nzMatrixType_Max; ++i)
	{
		if (!s_matrixUpdated[i])
		{
			shaderImpl->SendMatrix(s_matrixLocation[i], s_matrix[i]);
			s_matrixUpdated[i] = true;
		}
	}

	// Cas spéciaux car il faut recalculer la matrice
	if (!s_matrixUpdated[nzMatrixCombination_ViewProj])
	{
		s_matrix[nzMatrixCombination_ViewProj] = s_matrix[nzMatrixType_View] * s_matrix[nzMatrixType_Projection];

		shaderImpl->SendMatrix(s_matrixLocation[nzMatrixCombination_ViewProj], s_matrix[nzMatrixCombination_ViewProj]);
		s_matrixUpdated[nzMatrixCombination_ViewProj] = true;
	}

	if (!s_matrixUpdated[nzMatrixCombination_WorldView])
	{
		s_matrix[nzMatrixCombination_WorldView] = NzMatrix4f::ConcatenateAffine(s_matrix[nzMatrixType_World], s_matrix[nzMatrixType_View]);

		shaderImpl->SendMatrix(s_matrixLocation[nzMatrixCombination_WorldView], s_matrix[nzMatrixCombination_WorldView]);
		s_matrixUpdated[nzMatrixCombination_WorldView] = true;
	}

	if (!s_matrixUpdated[nzMatrixCombination_WorldViewProj])
	{
		s_matrix[nzMatrixCombination_WorldViewProj] = s_matrix[nzMatrixCombination_WorldView] * s_matrix[nzMatrixType_Projection];

		shaderImpl->SendMatrix(s_matrixLocation[nzMatrixCombination_WorldViewProj], s_matrix[nzMatrixCombination_WorldViewProj]);
		s_matrixUpdated[nzMatrixCombination_WorldViewProj] = true;
	}

	if (!s_stencilFuncUpdated)
	{
		glStencilFunc(rendererComparison[s_stencilCompare], s_stencilReference, s_stencilMask);
		s_stencilFuncUpdated = true;
	}

	if (!s_stencilOpUpdated)
	{
		glStencilOp(stencilOperation[s_stencilFail], stencilOperation[s_stencilZFail], stencilOperation[s_stencilPass]);
		s_stencilOpUpdated = true;
	}

	if (!s_vaoUpdated)
	{
		#if NAZARA_RENDERER_SAFE
		if (!s_vertexBuffer)
		{
			NazaraError("No vertex buffer");
			return false;
		}

		if (!s_vertexDeclaration)
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

			auto key = std::make_tuple(NzContext::GetCurrent(), s_indexBuffer, s_vertexBuffer, s_vertexDeclaration);
			auto it = s_vaos.find(key);
			if (it == s_vaos.end())
			{
				// On créé notre VAO
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				// On l'ajoute à notre liste
				s_vaos.insert(std::make_pair(key, static_cast<unsigned int>(vao)));

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
			NzHardwareBuffer* vertexBufferImpl = static_cast<NzHardwareBuffer*>(s_vertexBuffer->GetBuffer()->GetImpl());
			vertexBufferImpl->Bind();

			const nzUInt8* buffer = reinterpret_cast<const nzUInt8*>(s_vertexBuffer->GetPointer());

			unsigned int stride = s_vertexDeclaration->GetStride(nzElementStream_VertexData);
			for (unsigned int i = 0; i <= nzElementUsage_Max; ++i)
			{
				const NzVertexElement* element = s_vertexDeclaration->GetElement(nzElementStream_VertexData, static_cast<nzElementUsage>(i));

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

			if (s_indexBuffer)
			{
				NzHardwareBuffer* indexBufferImpl = static_cast<NzHardwareBuffer*>(s_indexBuffer->GetBuffer()->GetImpl());
				indexBufferImpl->Bind();
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

		s_vaoUpdated = true;
	}

	return true;
}

unsigned int NzRenderer::s_moduleReferenceCouter = 0;
