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
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Renderer/ShaderImpl.hpp>
#include <Nazara/Renderer/Loaders/Texture.hpp>
#include <Nazara/Utility/BufferImpl.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <map>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	///FIXME: Solution temporaire pour plus de facilité
	enum nzMatrixCombination
	{
		nzMatrixCombination_ViewProj = nzMatrixType_Max+1,
		nzMatrixCombination_WorldView,
		nzMatrixCombination_WorldViewProj,

		nzMatrixCombination_Max = nzMatrixCombination_WorldViewProj
	};

	struct TextureUnit
	{
		NzTextureSampler sampler;
		const NzTexture* texture = nullptr;
		bool samplerUpdated = false;
		bool textureUpdated = true;
		bool updated = true;
	};

	NzBufferImpl* HardwareBufferFunction(NzBuffer* parent, nzBufferType type)
	{
		return new NzHardwareBuffer(parent, type);
	}

	constexpr unsigned int totalMatrixCount = nzMatrixCombination_Max+1;

	using VAO_Key = std::tuple<const NzContext*, const NzIndexBuffer*, const NzVertexBuffer*, const NzVertexDeclaration*, bool>;

	std::map<VAO_Key, unsigned int> s_vaos;
	std::vector<TextureUnit> s_textureUnits;
	NzBuffer* s_instancingBuffer = nullptr;
	NzMatrix4f s_matrix[totalMatrixCount];
	nzBlendFunc s_srcBlend;
	nzBlendFunc s_dstBlend;
	nzFaceCulling s_faceCulling;
	nzFaceFilling s_faceFilling;
	nzRendererComparison s_depthFunc;
	nzRendererComparison s_stencilCompare;
	nzStencilOperation s_stencilFail;
	nzStencilOperation s_stencilPass;
	nzStencilOperation s_stencilZFail;
	nzUInt8 s_maxAnisotropyLevel;
	nzUInt32 s_stencilMask;
	const NzIndexBuffer* s_indexBuffer;
	const NzRenderTarget* s_target;
	const NzShader* s_shader;
	const NzVertexBuffer* s_vertexBuffer;
	const NzVertexDeclaration* s_vertexDeclaration;
	bool s_capabilities[nzRendererCap_Max+1];
	bool s_matrixUpdated[totalMatrixCount];
	bool s_stencilFuncUpdated;
	bool s_stencilOpUpdated;
	bool s_useSamplerObjects;
	bool s_useVertexArrayObjects;
	bool s_vaoUpdated;
	int s_matrixLocation[totalMatrixCount];
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

	if (primitive > nzPrimitiveType_Max)
	{
		NazaraError("Primitive type out of enum");
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

	if (!EnsureStateUpdate(false))
	{
		NazaraError("Failed to update states");
		return;
	}

	if (s_indexBuffer->IsSequential())
		glDrawArrays(NzOpenGL::PrimitiveType[primitive], s_indexBuffer->GetStartIndex(), s_indexBuffer->GetIndexCount());
	else
	{
		GLenum type;
		const nzUInt8* ptr = reinterpret_cast<const nzUInt8*>(s_indexBuffer->GetPointer());
		if (s_indexBuffer->HasLargeIndices())
		{
			ptr += firstIndex*sizeof(nzUInt32);
			type = GL_UNSIGNED_INT;
		}
		else
		{
			ptr += firstIndex*sizeof(nzUInt16);
			type = GL_UNSIGNED_SHORT;
		}

		glDrawElements(NzOpenGL::PrimitiveType[primitive], indexCount, type, ptr);
	}
}

void NzRenderer::DrawIndexedPrimitivesInstanced(unsigned int instanceCount, nzPrimitiveType primitive, unsigned int firstIndex, unsigned int indexCount)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}

	if (primitive > nzPrimitiveType_Max)
	{
		NazaraError("Primitive type out of enum");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (!s_capabilities[nzRendererCap_Instancing])
	{
		NazaraError("Instancing not supported");
		return;
	}

	if (!s_indexBuffer)
	{
		NazaraError("No index buffer");
		return;
	}

	if (instanceCount == 0)
	{
		NazaraError("Instance count must be over 0");
		return;
	}

	if (instanceCount > NAZARA_RENDERER_INSTANCING_MAX)
	{
		NazaraError("Instance count is over maximum instance count (" + NzString::Number(instanceCount) + " >= " + NzString::Number(NAZARA_RENDERER_INSTANCING_MAX) + ')');
		return;
	}
	#endif

	if (!EnsureStateUpdate(true))
	{
		NazaraError("Failed to update states");
		return;
	}

	if (s_indexBuffer->IsSequential())
		glDrawArraysInstanced(NzOpenGL::PrimitiveType[primitive], s_indexBuffer->GetStartIndex(), s_indexBuffer->GetIndexCount(), instanceCount);
	else
	{
		GLenum type;
		const nzUInt8* ptr = reinterpret_cast<const nzUInt8*>(s_indexBuffer->GetPointer());
		if (s_indexBuffer->HasLargeIndices())
		{
			ptr += firstIndex*sizeof(nzUInt32);
			type = GL_UNSIGNED_INT;
		}
		else
		{
			ptr += firstIndex*sizeof(nzUInt16);
			type = GL_UNSIGNED_SHORT;
		}

		glDrawElementsInstanced(NzOpenGL::PrimitiveType[primitive], indexCount, type, ptr, instanceCount);
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

	if (primitive > nzPrimitiveType_Max)
	{
		NazaraError("Primitive type out of enum");
		return;
	}
	#endif

	if (!EnsureStateUpdate(false))
	{
		NazaraError("Failed to update states");
		return;
	}

	glDrawArrays(NzOpenGL::PrimitiveType[primitive], firstVertex, vertexCount);
}

void NzRenderer::DrawPrimitivesInstanced(unsigned int instanceCount, nzPrimitiveType primitive, unsigned int firstVertex, unsigned int vertexCount)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}

	if (primitive > nzPrimitiveType_Max)
	{
		NazaraError("Primitive type out of enum");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (!s_capabilities[nzRendererCap_Instancing])
	{
		NazaraError("Instancing not supported");
		return;
	}

	if (instanceCount == 0)
	{
		NazaraError("Instance count must be over 0");
		return;
	}

	if (instanceCount > NAZARA_RENDERER_INSTANCING_MAX)
	{
		NazaraError("Instance count is over maximum instance count (" + NzString::Number(instanceCount) + " >= " + NzString::Number(NAZARA_RENDERER_INSTANCING_MAX) + ')');
		return;
	}
	#endif

	if (!EnsureStateUpdate(true))
	{
		NazaraError("Failed to update states");
		return;
	}

	glDrawArraysInstanced(NzOpenGL::PrimitiveType[primitive], firstVertex, vertexCount, instanceCount);
}

void NzRenderer::Enable(nzRendererParameter parameter, bool enable)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}

	if (parameter > nzRendererParameter_Max)
	{
		NazaraError("Renderer parameter out of enum");
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
				glEnable(NzOpenGL::RendererParameter[parameter]);
			else
				glDisable(NzOpenGL::RendererParameter[parameter]);

			break;
	}
}

void NzRenderer::FillInstancingBuffer(const NzRenderer::InstancingData* instancingData, unsigned int instanceCount)
{
	#if NAZARA_RENDERER_SAFE
	if (!s_capabilities[nzRendererCap_Instancing])
	{
		NazaraError("Instancing not supported");
		return;
	}

	if (!instancingData)
	{
		NazaraError("Instancing data must be valid");
		return;
	}

	if (instanceCount == 0)
	{
		NazaraError("Instance count must be over 0");
		return;
	}

	if (instanceCount > NAZARA_RENDERER_INSTANCING_MAX)
	{
		NazaraError("Instance count is over maximum instance count (" + NzString::Number(instanceCount) + " >= " + NzString::Number(NAZARA_RENDERER_INSTANCING_MAX) + ')');
		return;
	}
	#endif

	if (!s_instancingBuffer->Fill(instancingData, 0, instanceCount, true))
		NazaraError("Failed to fill instancing buffer");
}

void NzRenderer::Flush()
{
	glFlush();
}

float NzRenderer::GetLineWidth()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return 0.f;
	}
	#endif

	float lineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	return lineWidth;
}
/*
NzMatrix4f NzRenderer::GetMatrix(nzMatrixCombination combination)
{
	///FIXME: Duplication
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
	#ifdef NAZARA_DEBUG
	if (type > nzMatrixType_Max)
	{
		NazaraError("Matrix type out of enum");
		return NzMatrix4f();
	}
	#endif

	return s_matrix[type];
}

nzUInt8 NzRenderer::GetMaxAnisotropyLevel()
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

float NzRenderer::GetPointSize()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return 0.f;
	}
	#endif

	float pointSize;
	glGetFloatv(GL_POINT_SIZE, &pointSize);

	return pointSize;
}

const NzShader* NzRenderer::GetShader()
{
	return s_shader;
}

const NzRenderTarget* NzRenderer::GetTarget()
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
	#ifdef NAZARA_DEBUG
	if (capability > nzRendererCap_Max)
	{
		NazaraError("Renderer capability out of enum");
		return false;
	}
	#endif

	return s_capabilities[capability];
}

bool NzRenderer::Initialize(bool initializeDebugDrawer)
{
	if (s_moduleReferenceCounter++ != 0)
	{
		if (initializeDebugDrawer && !NzDebugDrawer::Initialize())
			NazaraWarning("Failed to initialize debug drawer"); // Non-critique

		return true; // Déjà initialisé
	}

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

	NzBuffer::SetBufferFunction(nzBufferStorage_Hardware, HardwareBufferFunction);

	for (unsigned int i = 0; i < totalMatrixCount; ++i)
	{
		s_matrix[i].MakeIdentity();
		s_matrixLocation[i] = -1;
		s_matrixUpdated[i] = false;
	}

	// Récupération des capacités d'OpenGL
	s_capabilities[nzRendererCap_AnisotropicFilter] = NzOpenGL::IsSupported(nzOpenGLExtension_AnisotropicFilter);
	s_capabilities[nzRendererCap_FP64] = NzOpenGL::IsSupported(nzOpenGLExtension_FP64);
	s_capabilities[nzRendererCap_HardwareBuffer] = true; // Natif depuis OpenGL 1.5
	s_capabilities[nzRendererCap_Instancing] = NzOpenGL::IsSupported(nzOpenGLExtension_DrawInstanced) && NzOpenGL::IsSupported(nzOpenGLExtension_InstancedArray);
	s_capabilities[nzRendererCap_MultipleRenderTargets] = (glBindFragDataLocation != nullptr); // Natif depuis OpenGL 2.0 mais inutile sans glBindFragDataLocation
	s_capabilities[nzRendererCap_OcclusionQuery] = true; // Natif depuis OpenGL 1.5
	s_capabilities[nzRendererCap_PixelBufferObject] = NzOpenGL::IsSupported(nzOpenGLExtension_PixelBufferObject);
	s_capabilities[nzRendererCap_RenderTexture] = NzOpenGL::IsSupported(nzOpenGLExtension_FrameBufferObject);
	s_capabilities[nzRendererCap_Texture3D] = true; // Natif depuis OpenGL 1.2
	s_capabilities[nzRendererCap_TextureCubemap] = true; // Natif depuis OpenGL 1.3
	s_capabilities[nzRendererCap_TextureMulti] = true; // Natif depuis OpenGL 1.3
	s_capabilities[nzRendererCap_TextureNPOT] = true; // Natif depuis OpenGL 2.0

	if (s_capabilities[nzRendererCap_AnisotropicFilter])
	{
		GLfloat maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

		s_maxAnisotropyLevel = static_cast<nzUInt8>(maxAnisotropy);
	}
	else
		s_maxAnisotropyLevel = 1;

	if (s_capabilities[nzRendererCap_Instancing])
	{
		s_instancingBuffer = new NzBuffer(nzBufferType_Vertex);
		if (!s_instancingBuffer->Create(NAZARA_RENDERER_INSTANCING_MAX, sizeof(InstancingData), nzBufferStorage_Hardware, nzBufferUsage_Dynamic))
		{
			s_capabilities[nzRendererCap_Instancing] = false;

			delete s_instancingBuffer;
			s_instancingBuffer = nullptr;

			NazaraWarning("Failed to create instancing buffer, disabled instancing.");
		}
	}

	if (s_capabilities[nzRendererCap_MultipleRenderTargets])
	{
		GLint maxDrawBuffers;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

		GLint maxColorAttachments;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

		s_maxRenderTarget = static_cast<unsigned int>(std::min(maxColorAttachments, maxDrawBuffers));
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
		s_maxTextureUnit = static_cast<unsigned int>(std::min(maxTextureUnits, maxVertexAttribs-NzOpenGL::AttributeIndex[nzElementUsage_TexCoord]));
	}
	else
		s_maxTextureUnit = 1;

	s_dstBlend = nzBlendFunc_Zero;
	s_faceCulling = nzFaceCulling_Back;
	s_faceFilling = nzFaceFilling_Fill;
	s_indexBuffer = nullptr;
	s_shader = nullptr;
	s_srcBlend = nzBlendFunc_One;
	s_stencilCompare = nzRendererComparison_Always;
	s_stencilFail = nzStencilOperation_Keep;
	s_stencilFuncUpdated = true;
	s_stencilMask = 0xFFFFFFFF;
	s_stencilOpUpdated = true;
	s_stencilPass = nzStencilOperation_Keep;
	s_stencilReference = 0;
	s_stencilZFail = nzStencilOperation_Keep;
	s_target = nullptr;
	s_textureUnits.resize(s_maxTextureUnit);
	s_useSamplerObjects = NzOpenGL::IsSupported(nzOpenGLExtension_SamplerObjects);
	s_useVertexArrayObjects = NzOpenGL::IsSupported(nzOpenGLExtension_VertexArrayObjects);
	s_vaoUpdated = false;
	s_vertexBuffer = nullptr;
	s_vertexDeclaration = nullptr;

	if (initializeDebugDrawer && !NzDebugDrawer::Initialize())
		NazaraWarning("Failed to initialize debug drawer"); // Non-critique

	if (!NzShaderBuilder::Initialize())
	{
		NazaraError("Failed to initialize shader builder");
		Uninitialize();

		return false;
	}

	if (!NzTextureSampler::Initialize())
	{
		NazaraError("Failed to initialize texture sampler");
		Uninitialize();

		return false;
	}

	// Loaders
	NzLoaders_Texture_Register();

	NazaraNotice("Initialized: Renderer module");

	return true;
}

bool NzRenderer::IsEnabled(nzRendererParameter parameter)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return false;
	}

	if (parameter > nzRendererParameter_Max)
	{
		NazaraError("Renderer parameter out of enum");
		return false;
	}
	#endif

	switch (parameter)
	{
		case nzRendererParameter_ColorWrite:
		{
			GLboolean enabled;
			glGetBooleanv(GL_COLOR_WRITEMASK, &enabled);

			return enabled;
		}

		case nzRendererParameter_DepthWrite:
		{
			GLboolean enabled;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &enabled);

			return enabled;
		}

		default:
			return glIsEnabled(NzOpenGL::RendererParameter[parameter]);
	}
}

bool NzRenderer::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzRenderer::SetBlendFunc(nzBlendFunc srcBlend, nzBlendFunc destBlend)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	if (s_srcBlend != srcBlend || s_dstBlend != destBlend)
	{
		glBlendFunc(NzOpenGL::BlendFunc[srcBlend], NzOpenGL::BlendFunc[destBlend]);
		s_srcBlend = srcBlend;
		s_dstBlend = destBlend;
	}
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

void NzRenderer::SetDepthFunc(nzRendererComparison compareFunc)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	if (s_depthFunc != compareFunc)
	{
		glDepthFunc(NzOpenGL::RendererComparison[compareFunc]);
		s_depthFunc = compareFunc;
	}
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

	if (s_faceCulling != cullingMode)
	{
		glCullFace(NzOpenGL::FaceCulling[cullingMode]);
		s_faceCulling = cullingMode;
	}
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

	if (s_faceFilling != fillingMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, NzOpenGL::FaceFilling[fillingMode]);
		s_faceFilling = fillingMode;
	}
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

void NzRenderer::SetLineWidth(float width)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (width <= 0.f)
	{
		NazaraError("Width must be over zero");
		return;
	}
	#endif

	glLineWidth(width);
}

void NzRenderer::SetMatrix(nzMatrixType type, const NzMatrix4f& matrix)
{
	#ifdef NAZARA_DEBUG
	if (type > nzMatrixType_Max)
	{
		NazaraError("Matrix type out of enum");
		return;
	}
	#endif

	s_matrix[type] = matrix;
	s_matrixUpdated[type] = false;

	// Invalidation des combinaisons
	if (type == nzMatrixType_View)
	{
		s_matrixUpdated[nzMatrixCombination_ViewProj] = false;
		s_matrixUpdated[nzMatrixCombination_WorldView] = false;
	}
	else if (type == nzMatrixType_Projection)
		s_matrixUpdated[nzMatrixCombination_ViewProj] = false;
	else if (type == nzMatrixType_World)
		s_matrixUpdated[nzMatrixCombination_WorldView] = false;

	s_matrixUpdated[nzMatrixCombination_WorldViewProj] = false; // Toujours invalidée
}

void NzRenderer::SetPointSize(float size)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (size <= 0.f)
	{
		NazaraError("Size must be over zero");
		return;
	}
	#endif

	glPointSize(size);
}

bool NzRenderer::SetShader(const NzShader* shader)
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

		///FIXME: Peut être optimisé
		for (unsigned int i = 0; i < totalMatrixCount; ++i)
			s_matrixUpdated[i] = false;
	}

	s_shader = shader;

	return true;
}

void NzRenderer::SetStencilCompareFunction(nzRendererComparison compareFunc)
{
	#ifdef NAZARA_DEBUG
	if (compareFunc > nzRendererComparison_Max)
	{
		NazaraError("Renderer comparison out of enum");
		return;
	}
	#endif

	if (compareFunc != s_stencilCompare)
	{
		s_stencilCompare = compareFunc;
		s_stencilFuncUpdated = false;
	}
}

void NzRenderer::SetStencilFailOperation(nzStencilOperation failOperation)
{
	#ifdef NAZARA_DEBUG
	if (failOperation > nzStencilOperation_Max)
	{
		NazaraError("Stencil fail operation out of enum");
		return;
	}
	#endif

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
	#ifdef NAZARA_DEBUG
	if (passOperation > nzStencilOperation_Max)
	{
		NazaraError("Stencil pass operation out of enum");
		return;
	}
	#endif

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
	#ifdef NAZARA_DEBUG
	if (zfailOperation > nzStencilOperation_Max)
	{
		NazaraError("Stencil zfail operation out of enum");
		return;
	}
	#endif

	if (zfailOperation != s_stencilZFail)
	{
		s_stencilZFail = zfailOperation;
		s_stencilOpUpdated = false;
	}
}

bool NzRenderer::SetTarget(const NzRenderTarget* target)
{
	if (s_target == target)
		return true;

	if (s_target)
	{
		if (!s_target->HasContext())
			s_target->Desactivate();

		s_target = nullptr;
	}

	if (target)
	{
		#if NAZARA_RENDERER_SAFE
		if (!target->IsRenderable())
		{
			NazaraError("Target not renderable");
			return false;
		}
		#endif

		if (!target->Activate())
		{
			NazaraError("Failed to activate target");
			return false;
		}

		s_target = target;
	}

	return true;
}

void NzRenderer::SetTexture(nzUInt8 unit, const NzTexture* texture)
{
	#if NAZARA_RENDERER_SAFE
	if (unit >= s_textureUnits.size())
	{
		NazaraError("Texture unit out of range (" + NzString::Number(unit) + " >= " + NzString::Number(s_textureUnits.size()) + ')');
		return;
	}
	#endif

	if (!texture) // Pas besoin de mettre à jour s'il n'y a pas de texture
		return;

	if (s_textureUnits[unit].texture != texture)
	{
		s_textureUnits[unit].texture = texture;
		s_textureUnits[unit].textureUpdated = false;

		if (s_textureUnits[unit].sampler.UseMipmaps(texture->HasMipmaps()))
			s_textureUnits[unit].samplerUpdated = false;

		s_textureUnits[unit].updated = false;
	}
}

void NzRenderer::SetTextureSampler(nzUInt8 unit, const NzTextureSampler& sampler)
{
	#if NAZARA_RENDERER_SAFE
	if (unit >= s_textureUnits.size())
	{
		NazaraError("Texture unit out of range (" + NzString::Number(unit) + " >= " + NzString::Number(s_textureUnits.size()) + ')');
		return;
	}
	#endif

	s_textureUnits[unit].sampler = sampler;
	s_textureUnits[unit].samplerUpdated = false;
	s_textureUnits[unit].updated = false;

	if (s_textureUnits[unit].texture)
		s_textureUnits[unit].sampler.UseMipmaps(s_textureUnits[unit].texture->HasMipmaps());
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

		const NzVertexDeclaration* vertexDeclaration = s_vertexBuffer->GetVertexDeclaration();
		if (s_vertexDeclaration != vertexDeclaration)
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
	if (s_moduleReferenceCounter != 1)
	{
		// Le module est soit encore utilisé, soit pas initialisé
		if (s_moduleReferenceCounter > 1)
			s_moduleReferenceCounter--;

		return;
	}

	// Libération du module
	s_moduleReferenceCounter = 0;

	s_textureUnits.clear();

	// Loaders
	NzLoaders_Texture_Unregister();

	NzDebugDrawer::Uninitialize();
	NzShaderBuilder::Uninitialize();
	NzTextureSampler::Uninitialize();

	NzContext::EnsureContext();

	// Libération du buffer d'instancing
	if (s_instancingBuffer)
	{
		delete s_instancingBuffer;
		s_instancingBuffer = nullptr;
	}

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

bool NzRenderer::EnsureStateUpdate(bool instancing)
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
	shaderImpl->BindTextures();

	if (s_useSamplerObjects)
	{
		///FIXME: Itère sur toutes les unités (Dont beaucoup inutilisées)
		for (unsigned int i = 0; i < s_textureUnits.size(); ++i)
		{
			TextureUnit& unit = s_textureUnits[i];

			if (!unit.updated)
			{
				if (!unit.textureUpdated)
				{
					glActiveTexture(GL_TEXTURE0 + i);
					unit.texture->Bind();

					unit.textureUpdated = true;
				}

				if (!unit.samplerUpdated)
				{
					unit.sampler.Bind(i);
					unit.samplerUpdated = true;
				}

				unit.updated = true;
			}
		}
	}
	else
	{
		///FIXME: Itère sur toutes les unités (Dont beaucoup inutilisées)
		for (unsigned int i = 0; i < s_textureUnits.size(); ++i)
		{
			TextureUnit& unit = s_textureUnits[i];

			if (!unit.updated)
			{
				glActiveTexture(GL_TEXTURE0 + i);

				unit.texture->Bind();
				unit.textureUpdated = true;

				unit.sampler.Apply(unit.texture);
				unit.samplerUpdated = true;

				unit.updated = true;
			}
		}
	}

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
		s_matrix[nzMatrixCombination_ViewProj] = s_matrix[nzMatrixType_View];
		s_matrix[nzMatrixCombination_ViewProj].Concatenate(s_matrix[nzMatrixType_Projection]);

		shaderImpl->SendMatrix(s_matrixLocation[nzMatrixCombination_ViewProj], s_matrix[nzMatrixCombination_ViewProj]);
		s_matrixUpdated[nzMatrixCombination_ViewProj] = true;
	}

	if (!s_matrixUpdated[nzMatrixCombination_WorldView])
	{
		s_matrix[nzMatrixCombination_WorldView] = s_matrix[nzMatrixType_World];
		s_matrix[nzMatrixCombination_WorldView].ConcatenateAffine(s_matrix[nzMatrixType_View]);

		shaderImpl->SendMatrix(s_matrixLocation[nzMatrixCombination_WorldView], s_matrix[nzMatrixCombination_WorldView]);
		s_matrixUpdated[nzMatrixCombination_WorldView] = true;
	}

	if (!s_matrixUpdated[nzMatrixCombination_WorldViewProj])
	{
		s_matrix[nzMatrixCombination_WorldViewProj] = s_matrix[nzMatrixCombination_WorldView];
		s_matrix[nzMatrixCombination_WorldViewProj].Concatenate(s_matrix[nzMatrixType_Projection]);

		shaderImpl->SendMatrix(s_matrixLocation[nzMatrixCombination_WorldViewProj], s_matrix[nzMatrixCombination_WorldViewProj]);
		s_matrixUpdated[nzMatrixCombination_WorldViewProj] = true;
	}

	if (!s_stencilFuncUpdated)
	{
		glStencilFunc(NzOpenGL::RendererComparison[s_stencilCompare], s_stencilReference, s_stencilMask);
		s_stencilFuncUpdated = true;
	}

	if (!s_stencilOpUpdated)
	{
		glStencilOp(NzOpenGL::StencilOperation[s_stencilFail], NzOpenGL::StencilOperation[s_stencilZFail], NzOpenGL::StencilOperation[s_stencilPass]);
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

		bool update;
		GLuint vao;

		// Si les VAOs sont supportés, on entoure nos appels par ceux-ci
		if (s_useVertexArrayObjects)
		{
			// On recherche si un VAO existe déjà avec notre configuration
			// Note: Les VAOs ne sont pas partagés entre les contextes, ces derniers font donc partie de notre configuration

			auto key = std::make_tuple(NzContext::GetCurrent(), s_indexBuffer, s_vertexBuffer, s_vertexDeclaration, instancing);
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

			const nzUInt8* buffer = static_cast<const nzUInt8*>(s_vertexBuffer->GetPointer());
			unsigned int stride = s_vertexDeclaration->GetStride(nzElementStream_VertexData);
			for (unsigned int i = 0; i <= nzElementUsage_Max; ++i)
			{
				nzElementUsage usage = static_cast<nzElementUsage>(i);
				if (s_vertexDeclaration->HasElement(nzElementStream_VertexData, usage))
				{
					const NzVertexElement* element = s_vertexDeclaration->GetElement(nzElementStream_VertexData, usage);

					glEnableVertexAttribArray(NzOpenGL::AttributeIndex[i]);
					glVertexAttribPointer(NzOpenGL::AttributeIndex[i],
					                      NzVertexDeclaration::GetElementCount(element->type),
					                      NzOpenGL::ElementType[element->type],
					                      (element->type == nzElementType_Color) ? GL_TRUE : GL_FALSE,
					                      stride,
					                      &buffer[element->offset]);
				}
				else
					glDisableVertexAttribArray(NzOpenGL::AttributeIndex[i]);
			}

			if (instancing)
			{
				static_cast<NzHardwareBuffer*>(s_instancingBuffer->GetImpl())->Bind();

				unsigned int instanceMatrixIndex = NzOpenGL::AttributeIndex[nzElementUsage_TexCoord] + 8;
				for (unsigned int i = 0; i < 4; ++i)
				{
					glEnableVertexAttribArray(instanceMatrixIndex);
					glVertexAttribPointer(instanceMatrixIndex, 4, GL_FLOAT, GL_FALSE, sizeof(InstancingData), reinterpret_cast<GLvoid*>(offsetof(InstancingData, worldMatrix) + i*sizeof(float)*4));
					glVertexAttribDivisor(instanceMatrixIndex, 1);

					instanceMatrixIndex++;
				}
			}
			else
				glDisableVertexAttribArray(NzOpenGL::AttributeIndex[nzElementUsage_TexCoord]+8);

			if (s_indexBuffer)
			{
				NzHardwareBuffer* indexBufferImpl = static_cast<NzHardwareBuffer*>(s_indexBuffer->GetBuffer()->GetImpl());
				indexBufferImpl->Bind();
			}
		}

		if (s_useVertexArrayObjects)
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

unsigned int NzRenderer::s_moduleReferenceCounter = 0;
