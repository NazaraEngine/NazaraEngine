// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp> // Pour éviter une redéfinition de WIN32_LEAN_AND_MEAN
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/AbstractShader.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Renderer/Loaders/Texture.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <map>
#include <memory>
#include <set>
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

	enum UpdateFlags
	{
		Update_None = 0,

		Update_Matrices     = 0x01,
		Update_Shader       = 0x02,
		Update_StencilFunc  = 0x04,
		Update_StencilOp    = 0x08,
		Update_Textures     = 0x10,
		Update_VAO          = 0x20,
	};

	struct TextureUnit
	{
		NzTextureSampler sampler;
		const NzTexture* texture = nullptr;
		bool samplerUpdated = false;
		bool textureUpdated = true;
	};

	NzAbstractBuffer* HardwareBufferFunction(NzBuffer* parent, nzBufferType type)
	{
		return new NzHardwareBuffer(parent, type);
	}

	constexpr unsigned int totalMatrixCount = nzMatrixCombination_Max+1;

	using VAO_Key = std::tuple<const NzContext*, const NzIndexBuffer*, const NzVertexBuffer*, const NzVertexDeclaration*, bool>;

	std::map<VAO_Key, unsigned int> s_vaos;
	std::set<unsigned int> s_dirtyTextureUnits;
	std::vector<TextureUnit> s_textureUnits;
	NzBuffer* s_instancingBuffer = nullptr;
	NzVertexBuffer* s_quadBuffer = nullptr;
	NzMatrix4f s_matrix[totalMatrixCount];
	NzVector2ui s_targetSize;
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
	nzUInt32 s_updateFlags;
	const NzIndexBuffer* s_indexBuffer;
	const NzRenderTarget* s_target;
	const NzShader* s_shader;
	const NzVertexBuffer* s_vertexBuffer;
	const NzVertexDeclaration* s_vertexDeclaration;
	bool s_capabilities[nzRendererCap_Max+1];
	bool s_instancing;
	bool s_matrixUpdated[totalMatrixCount];
	bool s_useSamplerObjects;
	bool s_useVertexArrayObjects;
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

	EnableInstancing(false);

	if (!EnsureStateUpdate())
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

	EnableInstancing(true);

	if (!EnsureStateUpdate())
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

	EnableInstancing(false);

	if (!EnsureStateUpdate())
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

	EnableInstancing(true);

	if (!EnsureStateUpdate())
	{
		NazaraError("Failed to update states");
		return;
	}

	glDrawArraysInstanced(NzOpenGL::PrimitiveType[primitive], firstVertex, vertexCount, instanceCount);
}

void NzRenderer::DrawTexture(unsigned int unit, const NzRectf& rect, const NzVector2f& uv0, const NzVector2f& uv1, float z)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (unit >= s_textureUnits.size())
	{
		NazaraError("Texture unit out of range (" + NzString::Number(unit) + " >= " + NzString::Number(s_textureUnits.size()) + ')');
		return;
	}

	if (!s_textureUnits[unit].texture)
	{
		NazaraError("No texture at unit #" + NzString::Number(unit));
		return;
	}

	if (z < 0.f || z > 1.f)
	{
		NazaraError("Z must be in range [0..1] (Got " + NzString::Number(z) + ')');
		return;
	}
	#endif

	const NzTexture* texture = s_textureUnits[unit].texture;

	if (glDrawTexture)
	{
		float xCorrect = 2.f/s_targetSize.x;
		float yCorrect = 2.f/s_targetSize.y;

		NzVector2f coords[2] =
		{
			{rect.x, rect.y},
			{rect.x+rect.width, rect.y+rect.height}
		};

		for (unsigned int i = 0; i < 2; ++i)
		{
			coords[i].x *= xCorrect;
			coords[i].x -= 1.f;

			coords[i].y *= yCorrect;
			coords[i].y -= 1.f;
		}

		const NzTextureSampler& sampler = s_textureUnits[unit].sampler;
		GLuint samplerId;
		if (s_useSamplerObjects)
			samplerId = sampler.GetOpenGLID();
		else
		{
			sampler.Apply(texture);
			samplerId = 0;
		}

		glDrawTexture(texture->GetOpenGLID(), samplerId,
		              coords[0].x, coords[0].y, coords[1].x, coords[1].y,
		              z,
		              uv0.x, 1.f-uv0.y, uv1.x, 1.f-uv1.y); // Inversion des UV sur Y
	}
	else
	{
		///FIXME: Remplacer cette immondice (Code fonctionnel mais à vomir)
		// Ce code est horrible mais la version optimisée demanderait des fonctionnalités pas encore implémentées, à venir...

		float vertices[4*(3 + 2)] =
		{
			rect.x, rect.y, z,
			uv0.x, uv0.y,

			rect.x+rect.width, rect.y, z,
			uv1.x, uv0.y,

			rect.x, rect.y+rect.height, z,
			uv0.x, uv1.y,

			rect.x+rect.width, rect.y+rect.height, z,
			uv1.x, uv1.y
		};

		if (!s_quadBuffer->Fill(vertices, 0, 4, true))
		{
			NazaraError("Failed to fill vertex buffer");
			return;
		}

		const NzShader* oldShader = s_shader;
		const NzVertexBuffer* oldBuffer = s_vertexBuffer;

		const NzShader* shader = NzShaderBuilder::Get(nzShaderFlags_DiffuseMapping | nzShaderFlags_FlipUVs);
		shader->SendTexture(shader->GetUniformLocation("MaterialDiffuseMap"), texture);

		bool faceCulling = IsEnabled(nzRendererParameter_FaceCulling);
		Enable(nzRendererParameter_FaceCulling, false);
		SetShader(shader);
		SetVertexBuffer(s_quadBuffer);

		if (!EnsureStateUpdate())
		{
			NazaraError("Failed to update states");
			return;
		}

		shader->SendMatrix(s_matrixLocation[nzMatrixCombination_WorldViewProj], NzMatrix4f::Ortho(0.f, s_targetSize.x, 0.f, s_targetSize.y, 0.f));

		glDrawArrays(NzOpenGL::PrimitiveType[nzPrimitiveType_TriangleStrip], 0, 4);

		// Restauration
		Enable(nzRendererParameter_FaceCulling, faceCulling);
		SetShader(oldShader);
		SetVertexBuffer(oldBuffer);
	}
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

void NzRenderer::Flush()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

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

NzRectui NzRenderer::GetScissorRect()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return NzRectui();
	}
	#endif

	GLint params[4];
	glGetIntegerv(GL_SCISSOR_BOX, &params[0]);

	return NzRectui(params[0], params[1], params[2], params[3]);
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

bool NzRenderer::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
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
	s_stencilMask = 0xFFFFFFFF;
	s_stencilPass = nzStencilOperation_Keep;
	s_stencilReference = 0;
	s_stencilZFail = nzStencilOperation_Keep;
	s_target = nullptr;
	s_textureUnits.resize(s_maxTextureUnit);
	s_useSamplerObjects = NzOpenGL::IsSupported(nzOpenGLExtension_SamplerObjects);
	s_useVertexArrayObjects = NzOpenGL::IsSupported(nzOpenGLExtension_VertexArrayObjects);
	s_vertexBuffer = nullptr;
	s_vertexDeclaration = nullptr;
	s_updateFlags = (Update_Matrices | Update_Shader | Update_VAO);

	NzVertexElement elements[2];
	elements[0].offset = 0;
	elements[0].type = nzElementType_Float3;
	elements[0].usage = nzElementUsage_Position;

	elements[1].offset = 3*sizeof(float);
	elements[1].type = nzElementType_Float2;
	elements[1].usage = nzElementUsage_TexCoord;

	std::unique_ptr<NzVertexDeclaration> declaration(new NzVertexDeclaration);
	if (!declaration->Create(elements, 2))
	{
		NazaraError("Failed to create quad declaration");
		Uninitialize();

		return false;
	}

	declaration->SetPersistent(false);

	s_quadBuffer = new NzVertexBuffer(declaration.get(), 4, nzBufferStorage_Hardware, nzBufferUsage_Dynamic);
	declaration.release();

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

void NzRenderer::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	#if NAZARA_RENDERER_SAFE
	if (indexBuffer && !indexBuffer->IsHardware() && !indexBuffer->IsSequential())
	{
		NazaraError("Buffer must be hardware");
		return;
	}
	#endif

	if (s_indexBuffer != indexBuffer)
	{
		s_indexBuffer = indexBuffer;
		s_updateFlags |= Update_VAO;
	}
}

void NzRenderer::SetInstancingData(const NzRenderer::InstancingData* instancingData, unsigned int instanceCount)
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

	s_updateFlags |= Update_Matrices;
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

void NzRenderer::SetScissorRect(const NzRectui& rect)
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
	if (rect.x+rect.width > width || rect.y+rect.height > height)
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return;
	}
	#endif

	glScissor(rect.x, height-rect.height-rect.y, rect.width, rect.height);
}

void NzRenderer::SetShader(const NzShader* shader)
{
	#if NAZARA_RENDERER_SAFE
	if (shader && !shader->IsCompiled())
	{
		NazaraError("Shader is not compiled");
		return;
	}
	#endif

	if (s_shader != shader)
	{
		s_shader = shader;
		s_updateFlags |= Update_Shader;
	}
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
		s_updateFlags |= Update_StencilFunc;
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
		s_updateFlags |= Update_StencilOp;
	}
}

void NzRenderer::SetStencilMask(nzUInt32 mask)
{
	if (mask != s_stencilMask)
	{
		s_stencilMask = mask;
		s_updateFlags |= Update_StencilFunc;
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
		s_updateFlags |= Update_StencilOp;
	}
}

void NzRenderer::SetStencilReferenceValue(unsigned int refValue)
{
	if (refValue != s_stencilReference)
	{
		s_stencilReference = refValue;
		s_updateFlags |= Update_StencilFunc;
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
		s_updateFlags |= Update_StencilOp;
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
		s_targetSize.Set(target->GetWidth(), target->GetHeight());
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

	if (s_textureUnits[unit].texture != texture)
	{
		s_textureUnits[unit].texture = texture;
		s_textureUnits[unit].textureUpdated = false;

		if (texture)
		{
			if (s_textureUnits[unit].sampler.UseMipmaps(texture->HasMipmaps()))
				s_textureUnits[unit].samplerUpdated = false;
		}

		s_dirtyTextureUnits.insert(unit);
		s_updateFlags |= Update_Textures;
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

	if (s_textureUnits[unit].texture)
		s_textureUnits[unit].sampler.UseMipmaps(s_textureUnits[unit].texture->HasMipmaps());

	s_dirtyTextureUnits.insert(unit);
	s_updateFlags |= Update_Textures;
}

void NzRenderer::SetVertexBuffer(const NzVertexBuffer* vertexBuffer)
{
	#if NAZARA_RENDERER_SAFE
	if (vertexBuffer && !vertexBuffer->IsHardware())
	{
		NazaraError("Buffer must be hardware");
		return;
	}
	#endif

	if (vertexBuffer && s_vertexBuffer != vertexBuffer)
	{
		s_vertexBuffer = vertexBuffer;

		const NzVertexDeclaration* vertexDeclaration = s_vertexBuffer->GetVertexDeclaration();
		if (s_vertexDeclaration != vertexDeclaration)
			s_vertexDeclaration = vertexDeclaration;

		s_updateFlags |= Update_VAO;
	}
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

	NzContext::EnsureContext();

	// Libération du module
	s_moduleReferenceCounter = 0;

	s_textureUnits.clear();

	// Loaders
	NzLoaders_Texture_Unregister();

	NzDebugDrawer::Uninitialize();
	NzShaderBuilder::Uninitialize();
	NzTextureSampler::Uninitialize();

	// Libération des buffers
	delete s_quadBuffer;

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

void NzRenderer::EnableInstancing(bool instancing)
{
	if (s_instancing != instancing)
	{
		s_updateFlags |= Update_VAO;
		s_instancing = instancing;
	}
}

bool NzRenderer::EnsureStateUpdate()
{
	if (s_updateFlags != Update_None)
	{
		#ifdef NAZARA_DEBUG
		if (NzContext::GetCurrent() == nullptr)
		{
			NazaraError("No active context");
			return false;
		}
		#endif

		NzAbstractShader* shaderImpl;

		if (s_updateFlags & Update_Shader)
		{
			#if NAZARA_RENDERER_SAFE
			if (!s_shader)
			{
				NazaraError("No shader");
				return false;
			}
			#endif

			// Il est plus rapide d'opérer sur l'implémentation du shader directement
			shaderImpl = s_shader->m_impl;
			shaderImpl->Bind();
			shaderImpl->BindTextures();

			// Récupération des indices des variables uniformes (-1 si la variable n'existe pas)
			s_matrixLocation[nzMatrixType_Projection] = shaderImpl->GetUniformLocation(nzShaderUniform_ProjMatrix);
			s_matrixLocation[nzMatrixType_View] = shaderImpl->GetUniformLocation(nzShaderUniform_ViewMatrix);
			s_matrixLocation[nzMatrixType_World] = shaderImpl->GetUniformLocation(nzShaderUniform_WorldMatrix);

			s_matrixLocation[nzMatrixCombination_ViewProj] = shaderImpl->GetUniformLocation(nzShaderUniform_ViewProjMatrix);
			s_matrixLocation[nzMatrixCombination_WorldView] = shaderImpl->GetUniformLocation(nzShaderUniform_WorldViewMatrix);
			s_matrixLocation[nzMatrixCombination_WorldViewProj] = shaderImpl->GetUniformLocation(nzShaderUniform_WorldViewProjMatrix);

			s_updateFlags |= Update_Matrices;
			for (unsigned int i = 0; i < totalMatrixCount; ++i)
			{
				///TODO: Voir le FIXME au niveau de l'envoi des matrices
				/*if (s_matrixLocation[i] != -1)
					s_matrixUpdated[i] = false;
				else*/
					s_matrixUpdated[i] = false;
			}

			s_updateFlags &= ~Update_Shader;
		}
		else
			shaderImpl = s_shader->m_impl;

		if (s_updateFlags & Update_Textures)
		{
			if (s_useSamplerObjects)
			{
				for (unsigned int i : s_dirtyTextureUnits)
				{
					TextureUnit& unit = s_textureUnits[i];

					if (!unit.textureUpdated)
					{
						NzOpenGL::SetTextureUnit(i);
						unit.texture->Bind();

						unit.textureUpdated = true;
					}

					if (!unit.samplerUpdated)
					{
						unit.sampler.Bind(i);
						unit.samplerUpdated = true;
					}
				}
			}
			else
			{
				for (unsigned int i : s_dirtyTextureUnits)
				{
					TextureUnit& unit = s_textureUnits[i];

					NzOpenGL::SetTextureUnit(i);

					unit.texture->Bind();
					unit.textureUpdated = true;

					unit.sampler.Apply(unit.texture);
					unit.samplerUpdated = true;
				}
			}

			s_dirtyTextureUnits.clear(); // Ne change pas la capacité
			s_updateFlags &= ~Update_Textures;
		}

		if (s_updateFlags & Update_Matrices)
		{
			for (unsigned int i = 0; i <= nzMatrixType_Max; ++i)
			{
				if (!s_matrixUpdated[i])
				{
					shaderImpl->SendMatrix(s_matrixLocation[i], s_matrix[i]);
					s_matrixUpdated[i] = true;
				}
			}

			///FIXME: Optimiser les matrices
			///TODO: Prendre en compte les FIXME...
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

			s_updateFlags &= ~Update_Matrices;
		}

		if (s_updateFlags & Update_StencilFunc)
		{
			glStencilFunc(NzOpenGL::RendererComparison[s_stencilCompare], s_stencilReference, s_stencilMask);
			s_updateFlags &= ~Update_StencilFunc;
		}

		if (s_updateFlags & Update_StencilOp)
		{
			glStencilOp(NzOpenGL::StencilOperation[s_stencilFail], NzOpenGL::StencilOperation[s_stencilZFail], NzOpenGL::StencilOperation[s_stencilPass]);
			s_updateFlags &= ~Update_StencilOp;
		}

		if (s_updateFlags & Update_VAO)
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

				auto key = std::make_tuple(NzContext::GetCurrent(), s_indexBuffer, s_vertexBuffer, s_vertexDeclaration, s_instancing);
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

				if (s_instancing)
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
                {
					for (unsigned int i = 8; i < 8+4; ++i)
						glDisableVertexAttribArray(NzOpenGL::AttributeIndex[nzElementUsage_TexCoord]+i);
                }

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

			s_updateFlags &= ~Update_VAO;
		}

		#ifdef NAZARA_DEBUG
		if (s_updateFlags != Update_None)
			NazaraWarning("Update flags not fully cleared");
		#endif
	}

	///FIXME: Rebinder le shader, les textures et le VAO via l'API NzOpenGL ?
	// Le problème étant que si une modification est faite à une ressource, celle-ci ne sera pas rebindée alors qu'elle le devrait

	return true;
}

unsigned int NzRenderer::s_moduleReferenceCounter = 0;
