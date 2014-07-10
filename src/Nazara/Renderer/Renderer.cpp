// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderLibrary.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/UberShaderLibrary.hpp>
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
#include <unordered_map>
#include <vector>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	enum ResourceType
	{
		ResourceType_Context,
		ResourceType_IndexBuffer,
		ResourceType_VertexBuffer,
		ResourceType_VertexDeclaration
	};

	enum UpdateFlags
	{
		Update_None = 0,

		Update_Matrices     = 0x1,
		Update_Shader       = 0x2,
		Update_Textures     = 0x4,
		Update_VAO          = 0x8
	};

	struct MatrixUnit
	{
		NzMatrix4f matrix;
		bool updated;
		int location;
	};

	struct TextureUnit
	{
		NzTextureSampler sampler;
		const NzTexture* texture = nullptr;
		bool samplerUpdated = false;
	};

	using VAO_Key = std::tuple<const NzIndexBuffer*, const NzVertexBuffer*, const NzVertexDeclaration*, const NzVertexDeclaration*>;
	using VAO_Map = std::map<VAO_Key, GLuint>;

	using Context_Map = std::unordered_map<const NzContext*, VAO_Map>;

	Context_Map s_vaos;
	std::vector<unsigned int> s_dirtyTextureUnits;
	std::vector<TextureUnit> s_textureUnits;
	GLuint s_currentVAO = 0;
	NzVertexBuffer s_instanceBuffer;
	NzVertexBuffer s_fullscreenQuadBuffer;
	MatrixUnit s_matrices[nzMatrixType_Max+1];
	NzRenderStates s_states;
	NzVector2ui s_targetSize;
	nzUInt8 s_maxAnisotropyLevel;
	nzUInt32 s_updateFlags;
	const NzIndexBuffer* s_indexBuffer;
	const NzRenderTarget* s_target;
	const NzShader* s_shader;
	const NzVertexBuffer* s_vertexBuffer;
	bool s_capabilities[nzRendererCap_Max+1];
	bool s_instancing;
	bool s_useSamplerObjects;
	bool s_useVertexArrayObjects;
	unsigned int s_maxColorAttachments;
	unsigned int s_maxRenderTarget;
	unsigned int s_maxTextureUnit;
	unsigned int s_maxVertexAttribs;

	class ResourceListener : public NzResourceListener
	{
		public:
			void OnResourceReleased(const NzResource* resource, int index) override
			{
				switch (index)
				{
					case ResourceType_Context:
					{
						const NzContext* context = static_cast<const NzContext*>(resource);
						s_vaos.erase(context);
						break;
					}

					case ResourceType_IndexBuffer:
					{
						const NzIndexBuffer* indexBuffer = static_cast<const NzIndexBuffer*>(resource);
						for (auto& pair : s_vaos)
						{
							const NzContext* context = pair.first;
							VAO_Map& vaos = pair.second;

							auto it = vaos.begin();
							while (it != vaos.end())
							{
								const VAO_Key& key = it->first;
								const NzIndexBuffer* vaoIndexBuffer = std::get<0>(key);

								if (vaoIndexBuffer == indexBuffer)
								{
									// Suppression du VAO:
									// Comme celui-ci est local à son contexte de création, sa suppression n'est possible que si
									// son contexte d'origine est actif, sinon il faudra le mettre en file d'attente
									// Ceci est géré par la méthode OpenGL::DeleteVertexArray

									NzOpenGL::DeleteVertexArray(context, it->second);
									vaos.erase(it++);
								}
								else
									++it;
							}
						}
						break;
					}

					case ResourceType_VertexBuffer:
					{
						const NzVertexBuffer* vertexBuffer = static_cast<const NzVertexBuffer*>(resource);
						for (auto& pair : s_vaos)
						{
							const NzContext* context = pair.first;
							VAO_Map& vaos = pair.second;

							auto it = vaos.begin();
							while (it != vaos.end())
							{
								const VAO_Key& key = it->first;
								const NzVertexBuffer* vaoVertexBuffer = std::get<1>(key);

								if (vaoVertexBuffer == vertexBuffer)
								{
									// Suppression du VAO:
									// Comme celui-ci est local à son contexte de création, sa suppression n'est possible que si
									// son contexte d'origine est actif, sinon il faudra le mettre en file d'attente
									// Ceci est géré par la méthode OpenGL::DeleteVertexArray

									NzOpenGL::DeleteVertexArray(context, it->second);
									vaos.erase(it++);
								}
								else
									++it;
							}
						}
						break;
					}

					case ResourceType_VertexDeclaration:
					{
						const NzVertexDeclaration* vertexDeclaration = static_cast<const NzVertexDeclaration*>(resource);
						for (auto& pair : s_vaos)
						{
							const NzContext* context = pair.first;
							VAO_Map& vaos = pair.second;

							auto it = vaos.begin();
							while (it != vaos.end())
							{
								const VAO_Key& key = it->first;
								const NzVertexDeclaration* vaoVertexDeclaration = std::get<2>(key);
								const NzVertexDeclaration* vaoInstancingDeclaration = std::get<3>(key);

								if (vaoVertexDeclaration == vertexDeclaration || vaoInstancingDeclaration == vertexDeclaration)
								{
									// Suppression du VAO:
									// Comme celui-ci est local à son contexte de création, sa suppression n'est possible que si
									// son contexte d'origine est actif, sinon il faudra le mettre en file d'attente
									// Ceci est géré par la méthode OpenGL::DeleteVertexArray

									NzOpenGL::DeleteVertexArray(context, it->second);
									vaos.erase(it++);
								}
								else
									++it;
							}
						}
						break;
					}

					default:
						NazaraInternalError("Unknown resource type");
						break;
				}
			}
	};

	ResourceListener s_listener;
}

void NzRenderer::BeginCondition(const NzGpuQuery& query, nzGpuQueryCondition condition)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (!s_capabilities[nzRendererCap_ConditionalRendering])
	{
		NazaraError("Conditional rendering is not supported");
		return;
	}
	#endif

	glBeginConditionalRender(query.GetOpenGLID(), NzOpenGL::QueryCondition[condition]);
}

void NzRenderer::Clear(nzUInt32 flags)
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
		// On n'oublie pas de mettre à jour la cible
		s_target->EnsureTargetUpdated();
		// Les états du rendu sont suceptibles d'influencer glClear
		NzOpenGL::ApplyStates(s_states);

		GLenum mask = 0;

		if (flags & nzRendererBuffer_Color)
			mask |= GL_COLOR_BUFFER_BIT;

		if (flags & nzRendererBuffer_Depth)
			mask |= GL_DEPTH_BUFFER_BIT;

		if (flags & nzRendererBuffer_Stencil)
			mask |= GL_STENCIL_BUFFER_BIT;

		glClear(mask);
	}
}

void NzRenderer::DrawFullscreenQuad()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	EnableInstancing(false);
	SetIndexBuffer(nullptr);
	SetVertexBuffer(&s_fullscreenQuadBuffer);

	if (!EnsureStateUpdate())
	{
		NazaraError("Failed to update states: " + NzError::GetLastError());
		return;
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (s_useVertexArrayObjects)
		glBindVertexArray(0);
}

void NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode mode, unsigned int firstIndex, unsigned int indexCount)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}

	if (mode > nzPrimitiveMode_Max)
	{
		NazaraError("Primitive mode out of enum");
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
		NazaraError("Failed to update states: " + NzError::GetLastError());
		return;
	}

	GLenum type;
	nzUInt8* offset = reinterpret_cast<nzUInt8*>(s_indexBuffer->GetStartOffset());

	if (s_indexBuffer->HasLargeIndices())
	{
		offset += firstIndex*sizeof(nzUInt32);
		type = GL_UNSIGNED_INT;
	}
	else
	{
		offset += firstIndex*sizeof(nzUInt16);
		type = GL_UNSIGNED_SHORT;
	}

	glDrawElements(NzOpenGL::PrimitiveMode[mode], indexCount, type, offset);

	if (s_useVertexArrayObjects)
		glBindVertexArray(0);
}

void NzRenderer::DrawIndexedPrimitivesInstanced(unsigned int instanceCount, nzPrimitiveMode mode, unsigned int firstIndex, unsigned int indexCount)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}

	if (mode > nzPrimitiveMode_Max)
	{
		NazaraError("Primitive mode out of enum");
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

	unsigned int maxInstanceCount = s_instanceBuffer.GetVertexCount();
	if (instanceCount > maxInstanceCount)
	{
		NazaraError("Instance count is over maximum instance count (" + NzString::Number(instanceCount) + " >= " NazaraStringifyMacro(NAZARA_RENDERER_MAX_INSTANCES) ")" );
		return;
	}
	#endif

	EnableInstancing(true);

	if (!EnsureStateUpdate())
	{
		NazaraError("Failed to update states: " + NzError::GetLastError());
		return;
	}

	GLenum type;
	nzUInt8* offset = reinterpret_cast<nzUInt8*>(s_indexBuffer->GetStartOffset());

	if (s_indexBuffer->HasLargeIndices())
	{
		offset += firstIndex*sizeof(nzUInt32);
		type = GL_UNSIGNED_INT;
	}
	else
	{
		offset += firstIndex*sizeof(nzUInt16);
		type = GL_UNSIGNED_SHORT;
	}

	glDrawElementsInstanced(NzOpenGL::PrimitiveMode[mode], indexCount, type, offset, instanceCount);

	if (s_useVertexArrayObjects)
		glBindVertexArray(0);
}

void NzRenderer::DrawPrimitives(nzPrimitiveMode mode, unsigned int firstVertex, unsigned int vertexCount)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}

	if (mode > nzPrimitiveMode_Max)
	{
		NazaraError("Primitive mode out of enum");
		return;
	}
	#endif

	EnableInstancing(false);

	if (!EnsureStateUpdate())
	{
		NazaraError("Failed to update states: " + NzError::GetLastError());
		return;
	}

	glDrawArrays(NzOpenGL::PrimitiveMode[mode], firstVertex, vertexCount);

	if (s_useVertexArrayObjects)
		glBindVertexArray(0);
}

void NzRenderer::DrawPrimitivesInstanced(unsigned int instanceCount, nzPrimitiveMode mode, unsigned int firstVertex, unsigned int vertexCount)
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}

	if (mode > nzPrimitiveMode_Max)
	{
		NazaraError("Primitive mode out of enum");
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

	unsigned int maxInstanceCount = s_instanceBuffer.GetVertexCount();
	if (instanceCount > maxInstanceCount)
	{
		NazaraError("Instance count is over maximum instance count (" + NzString::Number(instanceCount) + " >= " NazaraStringifyMacro(NAZARA_RENDERER_MAX_INSTANCES) ")" );
		return;
	}
	#endif

	EnableInstancing(true);

	if (!EnsureStateUpdate())
	{
		NazaraError("Failed to update states: " + NzError::GetLastError());
		return;
	}

	glDrawArraysInstanced(NzOpenGL::PrimitiveMode[mode], firstVertex, vertexCount, instanceCount);

	if (s_useVertexArrayObjects)
		glBindVertexArray(0);
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

	s_states.parameters[parameter] = enable;
}

void NzRenderer::EndCondition()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (!s_capabilities[nzRendererCap_ConditionalRendering])
	{
		NazaraError("Conditional rendering is not supported");
		return;
	}
	#endif

	glEndConditionalRender();
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

nzRendererComparison NzRenderer::GetDepthFunc()
{
	return s_states.depthFunc;
}

NzVertexBuffer* NzRenderer::GetInstanceBuffer()
{
	#if NAZARA_RENDERER_SAFE
	if (!s_capabilities[nzRendererCap_Instancing])
	{
		NazaraError("Instancing not supported");
		return nullptr;
	}
	#endif

	s_updateFlags |= Update_VAO;
	return &s_instanceBuffer;
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

	return s_states.lineWidth;
}

NzMatrix4f NzRenderer::GetMatrix(nzMatrixType type)
{
	#ifdef NAZARA_DEBUG
	if (type > nzMatrixType_Max)
	{
		NazaraError("Matrix type out of enum");
		return NzMatrix4f();
	}
	#endif

	if (!s_matrices[type].updated)
		UpdateMatrix(type);

	return s_matrices[type].matrix;
}

nzUInt8 NzRenderer::GetMaxAnisotropyLevel()
{
	return s_maxAnisotropyLevel;
}

unsigned int NzRenderer::GetMaxColorAttachments()
{
	return s_maxColorAttachments;
}

unsigned int NzRenderer::GetMaxRenderTargets()
{
	return s_maxRenderTarget;
}

unsigned int NzRenderer::GetMaxTextureUnits()
{
	return s_maxTextureUnit;
}

unsigned int NzRenderer::GetMaxVertexAttribs()
{
	return s_maxVertexAttribs;
}

float NzRenderer::GetPointSize()
{
	return s_states.pointSize;
}

const NzRenderStates& NzRenderer::GetRenderStates()
{
	return s_states;
}

NzRecti NzRenderer::GetScissorRect()
{
	return NzOpenGL::GetCurrentScissorBox();
}

const NzShader* NzRenderer::GetShader()
{
	return s_shader;
}

const NzRenderTarget* NzRenderer::GetTarget()
{
	return s_target;
}

NzRecti NzRenderer::GetViewport()
{
	return NzOpenGL::GetCurrentViewport();
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
	if (s_moduleReferenceCounter > 0)
	{
		s_moduleReferenceCounter++;
		return true; // Déjà initialisé
	}

	// Initialisation des dépendances
	if (!NzUtility::Initialize())
	{
		NazaraError("Failed to initialize Utility module");
		return false;
	}

	s_moduleReferenceCounter++;

	// Initialisation du module
	NzCallOnExit onExit(NzRenderer::Uninitialize);

	// Initialisation d'OpenGL
	if (!NzOpenGL::Initialize())
	{
		NazaraError("Failed to initialize OpenGL");
		return false;
	}

	NzBuffer::SetBufferFunction(nzBufferStorage_Hardware, [](NzBuffer* parent, nzBufferType type) -> NzAbstractBuffer* { return new NzHardwareBuffer(parent, type); } );

	for (unsigned int i = 0; i <= nzMatrixType_Max; ++i)
	{
		MatrixUnit& unit = s_matrices[i];
		unit.location = -1;
		unit.matrix.MakeIdentity();
		unit.updated = true;
	}

	// Récupération des capacités d'OpenGL
	s_capabilities[nzRendererCap_AnisotropicFilter] = NzOpenGL::IsSupported(nzOpenGLExtension_AnisotropicFilter);
	s_capabilities[nzRendererCap_ConditionalRendering] = NzOpenGL::IsSupported(nzOpenGLExtension_ConditionalRender);
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

	NzContext::EnsureContext();

	if (s_capabilities[nzRendererCap_AnisotropicFilter])
	{
		GLfloat maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

		s_maxAnisotropyLevel = static_cast<nzUInt8>(maxAnisotropy);
	}
	else
		s_maxAnisotropyLevel = 1;

	if (s_capabilities[nzRendererCap_RenderTexture])
	{
		GLint maxColorAttachments;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

		s_maxColorAttachments = static_cast<unsigned int>(maxColorAttachments);
	}
	else
		s_maxColorAttachments = 1;

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

		s_maxTextureUnit = static_cast<unsigned int>(maxTextureUnits);
	}
	else
		s_maxTextureUnit = 1;

	GLint maxVertexAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
	s_maxVertexAttribs = static_cast<unsigned int>(maxVertexAttribs);

	s_states = NzRenderStates();

	s_indexBuffer = nullptr;
	s_shader = nullptr;
	s_target = nullptr;
	s_targetSize.Set(0U);
	s_textureUnits.resize(s_maxTextureUnit);
	s_useSamplerObjects = NzOpenGL::IsSupported(nzOpenGLExtension_SamplerObjects);
	s_useVertexArrayObjects = NzOpenGL::IsSupported(nzOpenGLExtension_VertexArrayObjects);
	s_updateFlags = Update_Matrices | Update_Shader | Update_VAO;
	s_vertexBuffer = nullptr;

	s_fullscreenQuadBuffer.Reset(NzVertexDeclaration::Get(nzVertexLayout_XY), 4, nzBufferStorage_Hardware, nzBufferUsage_Static);

	float vertices[4*2] =
	{
		-1.f, -1.f,
		1.f, -1.f,
		-1.f, 1.f,
		1.f, 1.f,
	};

	if (!s_fullscreenQuadBuffer.Fill(vertices, 0, 4))
	{
		NazaraError("Failed to fill fullscreen quad buffer");
		return false;
	}

	if (s_capabilities[nzRendererCap_Instancing])
	{
		try
		{
			NzErrorFlags errFlags(nzErrorFlag_ThrowException, true);
			s_instanceBuffer.Reset(nullptr, NAZARA_RENDERER_INSTANCE_BUFFER_SIZE, nzBufferStorage_Hardware, nzBufferUsage_Dynamic);
		}
		catch (const std::exception& e)
		{
			s_capabilities[nzRendererCap_Instancing] = false;

			NzErrorFlags flags(nzErrorFlag_ThrowExceptionDisabled);
			NazaraError("Failed to create instancing buffer: " + NzString(e.what()));
		}
	}

	if (!NzShaderLibrary::Initialize())
	{
		NazaraError("Failed to initialize shader library");
		return false;
	}

	if (!NzTextureSampler::Initialize())
	{
		NazaraError("Failed to initialize texture sampler");
		return false;
	}

	if (!NzUberShaderLibrary::Initialize())
	{
		NazaraError("Failed to initialize uber shader library");
		return false;
	}

	// Création du shader de Debug
	std::unique_ptr<NzShader> shader(new NzShader);
	shader->SetPersistent(false);

	if (!shader->Create())
	{
		NazaraError("Failed to create debug shader");
		return false;
	}

	const nzUInt8 coreFragmentShader[] = {
		#include <Nazara/Renderer/Resources/Shaders/Debug/core.frag.h>
	};

	const nzUInt8 coreVertexShader[] = {
		#include <Nazara/Renderer/Resources/Shaders/Debug/core.vert.h>
	};

	const nzUInt8 compatibilityFragmentShader[] = {
		#include <Nazara/Renderer/Resources/Shaders/Debug/compatibility.frag.h>
	};

	const nzUInt8 compatibilityVertexShader[] = {
		#include <Nazara/Renderer/Resources/Shaders/Debug/compatibility.vert.h>
	};

	const char* fragmentShader;
	const char* vertexShader;
	unsigned int fragmentShaderLength;
	unsigned int vertexShaderLength;
	if (NzOpenGL::GetGLSLVersion() >= 140)
	{
		fragmentShader = reinterpret_cast<const char*>(coreFragmentShader);
		fragmentShaderLength = sizeof(coreFragmentShader);
		vertexShader = reinterpret_cast<const char*>(coreVertexShader);
		vertexShaderLength = sizeof(coreVertexShader);
	}
	else
	{
		fragmentShader = reinterpret_cast<const char*>(compatibilityFragmentShader);
		fragmentShaderLength = sizeof(compatibilityFragmentShader);
		vertexShader = reinterpret_cast<const char*>(compatibilityVertexShader);
		vertexShaderLength = sizeof(compatibilityVertexShader);
	}

	if (!shader->AttachStageFromSource(nzShaderStage_Fragment, fragmentShader, fragmentShaderLength))
	{
		NazaraError("Failed to attach fragment stage");
		return false;
	}

	if (!shader->AttachStageFromSource(nzShaderStage_Vertex, vertexShader, vertexShaderLength))
	{
		NazaraError("Failed to attach vertex stage");
		return false;
	}

	if (!shader->Link())
	{
		NazaraError("Failed to link shader");
		return false;
	}

	NzShaderLibrary::Register("DebugSimple", shader.get());
	shader.release();

	onExit.Reset();

	NazaraNotice("Initialized: Renderer module");
	return true;
}

bool NzRenderer::IsComponentTypeSupported(nzComponentType type)
{
	switch (type)
	{
		case nzComponentType_Color:
		case nzComponentType_Float1:
		case nzComponentType_Float2:
		case nzComponentType_Float3:
		case nzComponentType_Float4:
			return true; // Supportés nativement

		case nzComponentType_Double1:
		case nzComponentType_Double2:
		case nzComponentType_Double3:
		case nzComponentType_Double4:
			return glVertexAttribLPointer != nullptr; // Fonction requise pour envoyer des doubles

		case nzComponentType_Int1:
		case nzComponentType_Int2:
		case nzComponentType_Int3:
		case nzComponentType_Int4:
			return glVertexAttribIPointer != nullptr; // Fonction requise pour envoyer des entiers
	}

	NazaraError("Attribute type out of enum (0x" + NzString::Number(type, 16) + ')');
	return false;
}

bool NzRenderer::IsEnabled(nzRendererParameter parameter)
{
	#ifdef NAZARA_DEBUG
	if (parameter > nzRendererParameter_Max)
	{
		NazaraError("Renderer parameter out of enum");
		return false;
	}
	#endif

	return s_states.parameters[parameter];
}

bool NzRenderer::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzRenderer::SetBlendFunc(nzBlendFunc srcBlend, nzBlendFunc dstBlend)
{
	#ifdef NAZARA_DEBUG
	if (srcBlend > nzBlendFunc_Max)
	{
		NazaraError("Blend func out of enum");
		return;
	}

	if (dstBlend > nzBlendFunc_Max)
	{
		NazaraError("Blend func out of enum");
		return;
	}
	#endif

	s_states.srcBlend = srcBlend;
	s_states.dstBlend = dstBlend;
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
	if (compareFunc > nzRendererComparison_Max)
	{
		NazaraError("Renderer comparison out of enum");
		return;
	}
	#endif

	s_states.depthFunc = compareFunc;
}

void NzRenderer::SetFaceCulling(nzFaceSide faceSide)
{
	#ifdef NAZARA_DEBUG
	if (faceSide > nzFaceSide_Max)
	{
		NazaraError("Face side out of enum");
		return;
	}
	#endif

	s_states.faceCulling = faceSide;
}

void NzRenderer::SetFaceFilling(nzFaceFilling fillingMode)
{
	#ifdef NAZARA_DEBUG
	if (fillingMode > nzFaceFilling_Max)
	{
		NazaraError("Face filling out of enum");
		return;
	}
	#endif

	s_states.faceFilling = fillingMode;
}

void NzRenderer::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	#if NAZARA_RENDERER_SAFE
	if (indexBuffer && !indexBuffer->IsHardware())
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

void NzRenderer::SetLineWidth(float width)
{
	#if NAZARA_RENDERER_SAFE
	if (width <= 0.f)
	{
		NazaraError("Width must be over zero");
		return;
	}
	#endif

	s_states.lineWidth = width;
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

	s_matrices[type].matrix = matrix;
	s_matrices[type].updated = true;

	// Invalidation des combinaisons
	switch (type)
	{
		// Matrices de base
		case nzMatrixType_Projection:
			s_matrices[nzMatrixType_InvProjection].updated = false;
			s_matrices[nzMatrixType_InvViewProj].updated = false;
			s_matrices[nzMatrixType_InvWorldViewProj].updated = false;
			s_matrices[nzMatrixType_ViewProj].updated = false;
			s_matrices[nzMatrixType_WorldViewProj].updated = false;
			break;

		case nzMatrixType_View:
			s_matrices[nzMatrixType_InvView].updated = false;
			s_matrices[nzMatrixType_InvViewProj].updated = false;
			s_matrices[nzMatrixType_InvWorld].updated = false;
			s_matrices[nzMatrixType_InvWorldViewProj].updated = false;
			s_matrices[nzMatrixType_ViewProj].updated = false;
			s_matrices[nzMatrixType_World].updated = false;
			s_matrices[nzMatrixType_WorldViewProj].updated = false;
			break;

		case nzMatrixType_World:
			s_matrices[nzMatrixType_InvWorld].updated = false;
			s_matrices[nzMatrixType_InvWorldView].updated = false;
			s_matrices[nzMatrixType_InvWorldViewProj].updated = false;
			s_matrices[nzMatrixType_WorldView].updated = false;
			s_matrices[nzMatrixType_WorldViewProj].updated = false;
			break;

		// Matrices combinées
		case nzMatrixType_ViewProj:
			s_matrices[nzMatrixType_InvViewProj].updated = false;
			break;

		case nzMatrixType_WorldView:
			s_matrices[nzMatrixType_InvWorldView].updated = false;
			s_matrices[nzMatrixType_WorldViewProj].updated = false;
			break;

		case nzMatrixType_WorldViewProj:
			s_matrices[nzMatrixType_InvWorldViewProj].updated = false;
			break;

		case nzMatrixType_InvProjection:
		case nzMatrixType_InvView:
		case nzMatrixType_InvViewProj:
		case nzMatrixType_InvWorld:
		case nzMatrixType_InvWorldView:
		case nzMatrixType_InvWorldViewProj:
			break;
	}

	s_updateFlags |= Update_Matrices;
}

void NzRenderer::SetPointSize(float size)
{
	#if NAZARA_RENDERER_SAFE
	if (size <= 0.f)
	{
		NazaraError("Size must be over zero");
		return;
	}
	#endif

	s_states.pointSize = size;
}

void NzRenderer::SetRenderStates(const NzRenderStates& states)
{
	s_states = states;
}

void NzRenderer::SetScissorRect(const NzRecti& rect)
{
	NzOpenGL::BindScissorBox(rect);
}

void NzRenderer::SetShader(const NzShader* shader)
{
	#if NAZARA_RENDERER_SAFE
	if (shader)
	{
		if (!shader->IsValid() || !shader->IsLinked())
		{
			NazaraError("Invalid shader");
			return;
		}
	}
	#endif

	if (s_shader != shader)
	{
		s_shader = shader;
		s_updateFlags |= Update_Shader;
	}
}

void NzRenderer::SetStencilCompareFunction(nzRendererComparison compareFunc, nzFaceSide faceSide)
{
	#ifdef NAZARA_DEBUG
	if (compareFunc > nzRendererComparison_Max)
	{
		NazaraError("Renderer comparison out of enum");
		return;
	}

	if (faceSide > nzFaceSide_Max)
	{
		NazaraError("Face side out of enum");
		return;
	}
	#endif

	switch (faceSide)
	{
		case nzFaceSide_Back:
			s_states.backFace.stencilCompare = compareFunc;
			break;

		case nzFaceSide_Front:
			s_states.frontFace.stencilCompare = compareFunc;
			break;

		case nzFaceSide_FrontAndBack:
			s_states.backFace.stencilCompare = compareFunc;
			s_states.frontFace.stencilCompare = compareFunc;
			break;
	}
}

void NzRenderer::SetStencilFailOperation(nzStencilOperation failOperation, nzFaceSide faceSide)
{
	#ifdef NAZARA_DEBUG
	if (failOperation > nzStencilOperation_Max)
	{
		NazaraError("Stencil fail operation out of enum");
		return;
	}

	if (faceSide > nzFaceSide_Max)
	{
		NazaraError("Face side out of enum");
		return;
	}
	#endif

	switch (faceSide)
	{
		case nzFaceSide_Back:
			s_states.backFace.stencilFail = failOperation;
			break;

		case nzFaceSide_Front:
			s_states.frontFace.stencilFail = failOperation;
			break;

		case nzFaceSide_FrontAndBack:
			s_states.backFace.stencilFail = failOperation;
			s_states.frontFace.stencilFail = failOperation;
			break;
	}
}

void NzRenderer::SetStencilMask(nzUInt32 mask, nzFaceSide faceSide)
{
	#ifdef NAZARA_DEBUG
	if (faceSide > nzFaceSide_Max)
	{
		NazaraError("Face side out of enum");
		return;
	}
	#endif

	switch (faceSide)
	{
		case nzFaceSide_Back:
			s_states.backFace.stencilMask = mask;
			break;

		case nzFaceSide_Front:
			s_states.frontFace.stencilMask = mask;
			break;

		case nzFaceSide_FrontAndBack:
			s_states.backFace.stencilMask = mask;
			s_states.frontFace.stencilMask = mask;
			break;
	}
}

void NzRenderer::SetStencilPassOperation(nzStencilOperation passOperation, nzFaceSide faceSide)
{
	#ifdef NAZARA_DEBUG
	if (passOperation > nzStencilOperation_Max)
	{
		NazaraError("Stencil pass operation out of enum");
		return;
	}

	if (faceSide > nzFaceSide_Max)
	{
		NazaraError("Face side out of enum");
		return;
	}
	#endif

	switch (faceSide)
	{
		case nzFaceSide_Back:
			s_states.backFace.stencilPass = passOperation;
			break;

		case nzFaceSide_Front:
			s_states.frontFace.stencilPass = passOperation;
			break;

		case nzFaceSide_FrontAndBack:
			s_states.backFace.stencilPass = passOperation;
			s_states.frontFace.stencilPass = passOperation;
			break;
	}
}

void NzRenderer::SetStencilReferenceValue(unsigned int refValue, nzFaceSide faceSide)
{
	#ifdef NAZARA_DEBUG
	if (faceSide > nzFaceSide_Max)
	{
		NazaraError("Face side out of enum");
		return;
	}
	#endif

	switch (faceSide)
	{
		case nzFaceSide_Back:
			s_states.backFace.stencilReference = refValue;
			break;

		case nzFaceSide_Front:
			s_states.frontFace.stencilReference = refValue;
			break;

		case nzFaceSide_FrontAndBack:
			s_states.backFace.stencilReference = refValue;
			s_states.frontFace.stencilReference = refValue;
			break;
	}
}

void NzRenderer::SetStencilZFailOperation(nzStencilOperation zfailOperation, nzFaceSide faceSide)
{
	#ifdef NAZARA_DEBUG
	if (zfailOperation > nzStencilOperation_Max)
	{
		NazaraError("Stencil pass operation out of enum");
		return;
	}

	if (faceSide > nzFaceSide_Max)
	{
		NazaraError("Face side out of enum");
		return;
	}
	#endif

	switch (faceSide)
	{
		case nzFaceSide_Back:
			s_states.backFace.stencilZFail = zfailOperation;
			break;

		case nzFaceSide_Front:
			s_states.frontFace.stencilZFail = zfailOperation;
			break;

		case nzFaceSide_FrontAndBack:
			s_states.backFace.stencilZFail = zfailOperation;
			s_states.frontFace.stencilZFail = zfailOperation;
			break;
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

	NzOpenGL::SetTarget(s_target);

	return true;
}

void NzRenderer::SetTexture(nzUInt8 unit, const NzTexture* texture)
{
	#if NAZARA_RENDERER_SAFE
	if (unit >= s_maxTextureUnit)
	{
		NazaraError("Texture unit out of range (" + NzString::Number(unit) + " >= " + NzString::Number(s_maxTextureUnit) + ')');
		return;
	}
	#endif

	if (s_textureUnits[unit].texture != texture)
	{
		s_textureUnits[unit].texture = texture;

		if (texture)
		{
			if (s_textureUnits[unit].sampler.UseMipmaps(texture->HasMipmaps()))
				s_textureUnits[unit].samplerUpdated = false;
		}

		s_dirtyTextureUnits.push_back(unit);
		s_updateFlags |= Update_Textures;
	}
}

void NzRenderer::SetTextureSampler(nzUInt8 unit, const NzTextureSampler& sampler)
{
	#if NAZARA_RENDERER_SAFE
	if (unit >= s_maxTextureUnit)
	{
		NazaraError("Texture unit out of range (" + NzString::Number(unit) + " >= " + NzString::Number(s_maxTextureUnit) + ')');
		return;
	}
	#endif

	s_textureUnits[unit].sampler = sampler;
	s_textureUnits[unit].samplerUpdated = false;

	if (s_textureUnits[unit].texture)
		s_textureUnits[unit].sampler.UseMipmaps(s_textureUnits[unit].texture->HasMipmaps());

	s_dirtyTextureUnits.push_back(unit);
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
		s_updateFlags |= Update_VAO;
	}
}

void NzRenderer::SetViewport(const NzRecti& viewport)
{
	NzOpenGL::BindViewport(viewport);
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

	NzShaderLibrary::Unregister("DebugSimple");

	NzUberShaderLibrary::Uninitialize();
	NzTextureSampler::Uninitialize();
	NzShaderLibrary::Uninitialize();
	NzDebugDrawer::Uninitialize();

	s_textureUnits.clear();

	// Libération des buffers
	s_fullscreenQuadBuffer.Reset();
	s_instanceBuffer.Reset();

	// Libération des VAOs
	for (auto& pair : s_vaos)
	{
		const NzContext* context = pair.first;

		for (auto& pair2 : pair.second)
		{
			const VAO_Key& key = pair2.first;
			const NzIndexBuffer* indexBuffer = std::get<0>(key);
			const NzVertexBuffer* vertexBuffer = std::get<1>(key);
			const NzVertexDeclaration* vertexDeclaration = std::get<2>(key);
			const NzVertexDeclaration* instancingDeclaration = std::get<3>(key);

			if (indexBuffer)
				indexBuffer->RemoveResourceListener(&s_listener);

			vertexBuffer->RemoveResourceListener(&s_listener);
			vertexDeclaration->RemoveResourceListener(&s_listener);

			if (instancingDeclaration)
				instancingDeclaration->RemoveResourceListener(&s_listener);

			NzOpenGL::DeleteVertexArray(context, pair2.second);
		}
	}

	s_vaos.clear();

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
	// Toutes les erreurs sont silencieuses car l'erreur est gérée par la fonction appelante
	NzErrorFlags flags(nzErrorFlag_Silent | nzErrorFlag_ThrowExceptionDisabled);

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

	if (!s_target)
	{
		NazaraError("No target");
		return false;
	}
	#endif

	s_target->EnsureTargetUpdated();

	s_shader->Bind(); // Active le programme si ce n'est pas déjà le cas

	// Si le programme a été changé depuis la dernière fois
	if (s_updateFlags & Update_Shader)
	{
		// Récupération des indices des variables uniformes (-1 si la variable n'existe pas)
		s_matrices[nzMatrixType_Projection].location = s_shader->GetUniformLocation(nzShaderUniform_ProjMatrix);
		s_matrices[nzMatrixType_View].location = s_shader->GetUniformLocation(nzShaderUniform_ViewMatrix);
		s_matrices[nzMatrixType_World].location = s_shader->GetUniformLocation(nzShaderUniform_WorldMatrix);

		s_matrices[nzMatrixType_ViewProj].location = s_shader->GetUniformLocation(nzShaderUniform_ViewProjMatrix);
		s_matrices[nzMatrixType_WorldView].location = s_shader->GetUniformLocation(nzShaderUniform_WorldViewMatrix);
		s_matrices[nzMatrixType_WorldViewProj].location = s_shader->GetUniformLocation(nzShaderUniform_WorldViewProjMatrix);

		s_matrices[nzMatrixType_InvProjection].location = s_shader->GetUniformLocation(nzShaderUniform_InvProjMatrix);
		s_matrices[nzMatrixType_InvView].location = s_shader->GetUniformLocation(nzShaderUniform_InvViewMatrix);
		s_matrices[nzMatrixType_InvViewProj].location = s_shader->GetUniformLocation(nzShaderUniform_InvViewProjMatrix);
		s_matrices[nzMatrixType_InvWorld].location = s_shader->GetUniformLocation(nzShaderUniform_InvWorldMatrix);
		s_matrices[nzMatrixType_InvWorldView].location = s_shader->GetUniformLocation(nzShaderUniform_InvWorldViewMatrix);
		s_matrices[nzMatrixType_InvWorldViewProj].location = s_shader->GetUniformLocation(nzShaderUniform_InvWorldViewProjMatrix);

		s_targetSize.Set(0U); // On force l'envoi des uniformes
		s_updateFlags |= Update_Matrices; // Changement de programme, on renvoie toutes les matrices demandées

		s_updateFlags &= ~Update_Shader;
	}

	// Envoi des uniformes liées au Renderer
	NzVector2ui targetSize(s_target->GetWidth(), s_target->GetHeight());
	if (s_targetSize != targetSize)
	{
		int location;

		location = s_shader->GetUniformLocation(nzShaderUniform_InvTargetSize);
		if (location != -1)
			s_shader->SendVector(location, 1.f/NzVector2f(targetSize));

		location = s_shader->GetUniformLocation(nzShaderUniform_TargetSize);
		if (location != -1)
			s_shader->SendVector(location, NzVector2f(targetSize));

		s_targetSize.Set(targetSize);
	}

	if (s_updateFlags != Update_None)
	{
		if (s_updateFlags & Update_Textures)
		{
			if (s_useSamplerObjects)
			{
				for (unsigned int i : s_dirtyTextureUnits)
				{
					TextureUnit& unit = s_textureUnits[i];

					if (unit.texture && !unit.samplerUpdated)
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

					if (unit.texture && !unit.samplerUpdated)
					{
						NzOpenGL::BindTextureUnit(i);
						unit.sampler.Apply(unit.texture);
						unit.samplerUpdated = true;
					}
				}
			}

			s_dirtyTextureUnits.clear(); // Ne change pas la capacité
			s_updateFlags &= ~Update_Textures;
		}

		if (s_updateFlags & Update_Matrices)
		{
			for (unsigned int i = 0; i <= nzMatrixType_Max; ++i)
			{
				MatrixUnit& unit = s_matrices[i];
				if (unit.location != -1) // On ne traite que les matrices existant dans le programme
				{
					if (!unit.updated)
						UpdateMatrix(static_cast<nzMatrixType>(i));

					s_shader->SendMatrix(unit.location, unit.matrix);
				}
			}

			s_updateFlags &= ~Update_Matrices;
		}

		if (s_updateFlags & Update_VAO)
		{
			#if NAZARA_RENDERER_SAFE
			if (!s_vertexBuffer)
			{
				NazaraError("No vertex buffer");
				return false;
			}
			#endif

			bool update;
			VAO_Map::iterator vaoIt;

			// Si les VAOs sont supportés, on entoure nos appels par ceux-ci
			if (s_useVertexArrayObjects)
			{
				// Note: Les VAOs ne sont pas partagés entre les contextes, nous avons donc un tableau de VAOs par contexte
				const NzContext* context = NzContext::GetCurrent();

				VAO_Map* vaos;
				auto it = s_vaos.find(context);
				if (it == s_vaos.end())
				{
					context->AddResourceListener(&s_listener, ResourceType_Context);
					auto pair = s_vaos.insert(std::make_pair(context, Context_Map::mapped_type()));
					vaos = &pair.first->second;
				}
				else
					vaos = &it->second;

				// Notre clé est composée de ce qui définit un VAO
				const NzVertexDeclaration* vertexDeclaration = s_vertexBuffer->GetVertexDeclaration();
				const NzVertexDeclaration* instancingDeclaration = (s_instancing) ? s_instanceBuffer.GetVertexDeclaration() : nullptr;
				VAO_Key key(s_indexBuffer, s_vertexBuffer, vertexDeclaration, instancingDeclaration);

				// On recherche un VAO existant avec notre configuration
				vaoIt = vaos->find(key);
				if (vaoIt == vaos->end())
				{
					// On créé notre VAO
					glGenVertexArrays(1, &s_currentVAO);
					glBindVertexArray(s_currentVAO);

					// On l'ajoute à notre liste
					vaoIt = vaos->insert(std::make_pair(key, s_currentVAO)).first;
					if (s_indexBuffer)
						s_indexBuffer->AddResourceListener(&s_listener, ResourceType_IndexBuffer);

					s_vertexBuffer->AddResourceListener(&s_listener, ResourceType_VertexBuffer);
					vertexDeclaration->AddResourceListener(&s_listener, ResourceType_VertexDeclaration);

					if (instancingDeclaration)
						instancingDeclaration->AddResourceListener(&s_listener, ResourceType_VertexDeclaration);

					// Et on indique qu'on veut le programmer
					update = true;
				}
				else
				{
					// Notre VAO existe déjà, il est donc inutile de le reprogrammer
					s_currentVAO = vaoIt->second;

					update = false;
				}
			}
			else
				update = true; // Fallback si les VAOs ne sont pas supportés

			bool updateFailed = false;

			if (update)
			{
				const NzVertexDeclaration* vertexDeclaration;
				unsigned int bufferOffset;
				unsigned int stride;

				NzHardwareBuffer* vertexBufferImpl = static_cast<NzHardwareBuffer*>(s_vertexBuffer->GetBuffer()->GetImpl());
				glBindBuffer(NzOpenGL::BufferTarget[nzBufferType_Vertex], vertexBufferImpl->GetOpenGLID());

				bufferOffset = s_vertexBuffer->GetStartOffset();
				vertexDeclaration = s_vertexBuffer->GetVertexDeclaration();
				stride = vertexDeclaration->GetStride();
				for (unsigned int i = nzVertexComponent_FirstVertexData; i <= nzVertexComponent_LastVertexData; ++i)
				{
					nzComponentType type;
					bool enabled;
					unsigned int offset;
					vertexDeclaration->GetComponent(static_cast<nzVertexComponent>(i), &enabled, &type, &offset);

					if (!IsComponentTypeSupported(type))
					{
						NazaraError("Invalid declaration: Vertex component 0x" + NzString::Number(i, 16) + " (type: 0x" + NzString::Number(type, 16) + ") is not supported");
						updateFailed = true;
						break;
					}

					if (enabled)
					{
						glEnableVertexAttribArray(NzOpenGL::VertexComponentIndex[i]);
						if (type <= nzComponentType_Double1 && type >= nzComponentType_Double4)
						{
							glVertexAttribLPointer(NzOpenGL::VertexComponentIndex[i],
							                       NzUtility::ComponentCount[type],
							                       NzOpenGL::ComponentType[type],
							                       stride,
							                       reinterpret_cast<void*>(bufferOffset + offset));
						}
						else if (type <= nzComponentType_Int1 && type >= nzComponentType_Int4)
						{
							glVertexAttribIPointer(NzOpenGL::VertexComponentIndex[i],
							                       NzUtility::ComponentCount[type],
							                       NzOpenGL::ComponentType[type],
							                       stride,
							                       reinterpret_cast<void*>(bufferOffset + offset));
						}
						else
						{
							glVertexAttribPointer(NzOpenGL::VertexComponentIndex[i],
							                      NzUtility::ComponentCount[type],
							                      NzOpenGL::ComponentType[type],
							                      (type == nzComponentType_Color) ? GL_TRUE : GL_FALSE,
							                      stride,
							                      reinterpret_cast<void*>(bufferOffset + offset));
						}
					}
					else
						glDisableVertexAttribArray(NzOpenGL::VertexComponentIndex[i]);
				}

				if (!updateFailed)
				{
					if (s_instancing)
					{
						NzHardwareBuffer* instanceBufferImpl = static_cast<NzHardwareBuffer*>(s_instanceBuffer.GetBuffer()->GetImpl());
						glBindBuffer(NzOpenGL::BufferTarget[nzBufferType_Vertex], instanceBufferImpl->GetOpenGLID());

						bufferOffset = s_instanceBuffer.GetStartOffset();
						vertexDeclaration = s_instanceBuffer.GetVertexDeclaration();
						stride = vertexDeclaration->GetStride();
						for (unsigned int i = nzVertexComponent_FirstInstanceData; i <= nzVertexComponent_LastInstanceData; ++i)
						{
							nzComponentType type;
							bool enabled;
							unsigned int offset;
							vertexDeclaration->GetComponent(static_cast<nzVertexComponent>(i), &enabled, &type, &offset);

							if (!IsComponentTypeSupported(type))
							{
								NazaraError("Invalid declaration: Vertex component 0x" + NzString::Number(i, 16) + " (type: 0x" + NzString::Number(type, 16) + ") is not supported");
								updateFailed = true;
								break;
							}

							if (enabled)
							{
								glEnableVertexAttribArray(NzOpenGL::VertexComponentIndex[i]);
								if (type <= nzComponentType_Double1 && type >= nzComponentType_Double4)
								{
									glVertexAttribLPointer(NzOpenGL::VertexComponentIndex[i],
														   NzUtility::ComponentCount[type],
														   NzOpenGL::ComponentType[type],
														   stride,
														   reinterpret_cast<void*>(bufferOffset + offset));
								}
								else if (type <= nzComponentType_Int1 && type >= nzComponentType_Int4)
								{
									glVertexAttribIPointer(NzOpenGL::VertexComponentIndex[i],
									                       NzUtility::ComponentCount[type],
									                       NzOpenGL::ComponentType[type],
									                       stride,
									                       reinterpret_cast<void*>(bufferOffset + offset));
								}
								else
								{
									glVertexAttribPointer(NzOpenGL::VertexComponentIndex[i],
									                      NzUtility::ComponentCount[type],
									                      NzOpenGL::ComponentType[type],
									                      (type == nzComponentType_Color) ? GL_TRUE : GL_FALSE,
									                      stride,
									                      reinterpret_cast<void*>(bufferOffset + offset));
								}

								glVertexAttribDivisor(NzOpenGL::VertexComponentIndex[i], 1);
							}
							else
								glDisableVertexAttribArray(NzOpenGL::VertexComponentIndex[i]);
						}
					}
					else
					{
						for (unsigned int i = nzVertexComponent_FirstInstanceData; i <= nzVertexComponent_LastInstanceData; ++i)
							glDisableVertexAttribArray(NzOpenGL::VertexComponentIndex[i]);
					}

					// Et on active l'index buffer (Un seul index buffer par VAO)
					if (s_indexBuffer)
					{
						NzHardwareBuffer* indexBufferImpl = static_cast<NzHardwareBuffer*>(s_indexBuffer->GetBuffer()->GetImpl());
						glBindBuffer(NzOpenGL::BufferTarget[nzBufferType_Index], indexBufferImpl->GetOpenGLID());
					}
					else
						glBindBuffer(NzOpenGL::BufferTarget[nzBufferType_Index], 0);
				}

				// On invalide les bindings des buffers (car nous les avons défini manuellement)
				NzOpenGL::SetBuffer(nzBufferType_Index, 0);
				NzOpenGL::SetBuffer(nzBufferType_Vertex, 0);
			}

			if (s_useVertexArrayObjects)
			{
				if (update)
				{
					if (updateFailed)
					{
						// La création de notre VAO a échoué, libérons-le et marquons-le comme problématique
						glDeleteVertexArrays(1, &vaoIt->second);
						vaoIt->second = 0;
						s_currentVAO = 0;
					}
					else
						glBindVertexArray(0); // On marque la fin de la construction du VAO en le débindant
				}

				// En cas de non-support des VAOs, les attributs doivent être respécifiés à chaque frame
				s_updateFlags &= ~Update_VAO;
			}
		}

		#ifdef NAZARA_DEBUG
		if (s_updateFlags != Update_None && !s_useVertexArrayObjects && s_updateFlags != Update_VAO)
			NazaraWarning("Update flags not fully cleared");
		#endif
	}

	// On bind notre VAO
	if (s_useVertexArrayObjects)
	{
		if (!s_currentVAO)
		{
			NazaraError("Failed to create VAO");
			return false;
		}

		glBindVertexArray(s_currentVAO);
	}

	// On vérifie que les textures actuellement bindées sont bien nos textures
	// Ceci à cause du fait qu'il est possible que des opérations sur les textures aient eu lieu
	// entre le dernier rendu et maintenant
	for (unsigned int i = 0; i < s_maxTextureUnit; ++i)
	{
		const NzTexture* texture = s_textureUnits[i].texture;
		if (texture)
		{
			NzOpenGL::BindTexture(i, texture->GetType(), texture->GetOpenGLID());
			texture->EnsureMipmapsUpdate();
		}
	}

	// Et on termine par envoyer nos états au driver
	NzOpenGL::ApplyStates(s_states);

	return true;
}

void NzRenderer::OnShaderReleased(const NzShader* shader)
{
	if (s_shader == shader)
	{
		s_shader = nullptr;
		s_updateFlags |= Update_Shader;
	}
}

void NzRenderer::OnTextureReleased(const NzTexture* texture)
{
	for (TextureUnit& unit : s_textureUnits)
	{
		if (unit.texture == texture)
			unit.texture = nullptr;
			// Inutile de changer le flag pour une texture désactivée
	}
}

void NzRenderer::UpdateMatrix(nzMatrixType type)
{
	#ifdef NAZARA_DEBUG
	if (type > nzMatrixType_Max)
	{
		NazaraError("Matrix type out of enum");
		return;
	}
	#endif

	switch (type)
	{
		// Matrices de base
		case nzMatrixType_Projection:
		case nzMatrixType_View:
		case nzMatrixType_World:
			s_matrices[type].updated = true;
			break;

		// Matrices combinées
		case nzMatrixType_ViewProj:
			s_matrices[nzMatrixType_ViewProj].matrix = s_matrices[nzMatrixType_View].matrix * s_matrices[nzMatrixType_Projection].matrix;
			s_matrices[nzMatrixType_ViewProj].updated = true;
			break;

		case nzMatrixType_WorldView:
			s_matrices[nzMatrixType_WorldView].matrix = s_matrices[nzMatrixType_World].matrix;
			s_matrices[nzMatrixType_WorldView].matrix.ConcatenateAffine(s_matrices[nzMatrixType_View].matrix);
			s_matrices[nzMatrixType_WorldView].updated = true;
			break;

		case nzMatrixType_WorldViewProj:
			if (!s_matrices[nzMatrixType_WorldView].updated)
				UpdateMatrix(nzMatrixType_WorldView);

			s_matrices[nzMatrixType_WorldViewProj].matrix = s_matrices[nzMatrixType_WorldView].matrix;
			s_matrices[nzMatrixType_WorldViewProj].matrix.Concatenate(s_matrices[nzMatrixType_Projection].matrix);
			s_matrices[nzMatrixType_WorldViewProj].updated = true;
			break;

		// Matrices inversées
		case nzMatrixType_InvProjection:
			if (!s_matrices[nzMatrixType_Projection].updated)
				UpdateMatrix(nzMatrixType_Projection);

			if (!s_matrices[nzMatrixType_Projection].matrix.GetInverse(&s_matrices[nzMatrixType_InvProjection].matrix))
				NazaraWarning("Failed to inverse Proj matrix");

			s_matrices[nzMatrixType_InvProjection].updated = true;
			break;

		case nzMatrixType_InvView:
			if (!s_matrices[nzMatrixType_View].updated)
				UpdateMatrix(nzMatrixType_View);

			if (!s_matrices[nzMatrixType_View].matrix.GetInverse(&s_matrices[nzMatrixType_InvView].matrix))
				NazaraWarning("Failed to inverse View matrix");

			s_matrices[nzMatrixType_InvView].updated = true;
			break;

		case nzMatrixType_InvViewProj:
			if (!s_matrices[nzMatrixType_ViewProj].updated)
				UpdateMatrix(nzMatrixType_ViewProj);

			if (!s_matrices[nzMatrixType_ViewProj].matrix.GetInverse(&s_matrices[nzMatrixType_InvViewProj].matrix))
				NazaraWarning("Failed to inverse ViewProj matrix");

			s_matrices[nzMatrixType_InvViewProj].updated = true;
			break;

		case nzMatrixType_InvWorld:
			if (!s_matrices[nzMatrixType_World].updated)
				UpdateMatrix(nzMatrixType_World);

			if (!s_matrices[nzMatrixType_World].matrix.GetInverse(&s_matrices[nzMatrixType_InvWorld].matrix))
				NazaraWarning("Failed to inverse World matrix");

			s_matrices[nzMatrixType_InvWorld].updated = true;
			break;

		case nzMatrixType_InvWorldView:
			if (!s_matrices[nzMatrixType_WorldView].updated)
				UpdateMatrix(nzMatrixType_WorldView);

			if (!s_matrices[nzMatrixType_WorldView].matrix.GetInverse(&s_matrices[nzMatrixType_InvWorldView].matrix))
				NazaraWarning("Failed to inverse WorldView matrix");

			s_matrices[nzMatrixType_InvWorldView].updated = true;
			break;

		case nzMatrixType_InvWorldViewProj:
			if (!s_matrices[nzMatrixType_WorldViewProj].updated)
				UpdateMatrix(nzMatrixType_WorldViewProj);

			if (!s_matrices[nzMatrixType_WorldViewProj].matrix.GetInverse(&s_matrices[nzMatrixType_InvWorldViewProj].matrix))
				NazaraWarning("Failed to inverse WorldViewProj matrix");

			s_matrices[nzMatrixType_InvWorldViewProj].updated = true;
			break;
	}
}

unsigned int NzRenderer::s_moduleReferenceCounter = 0;
