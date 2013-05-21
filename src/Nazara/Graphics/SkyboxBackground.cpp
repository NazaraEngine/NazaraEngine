// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#define NAZARA_RENDERER_OPENGL // Nécessaire pour accéder à OpenGL

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzIndexBuffer* BuildIndexBuffer()
	{
		std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(36, false, nzBufferStorage_Hardware, nzBufferUsage_Static));
		indexBuffer->SetPersistent(false);

		nzUInt16 indices[6*6] =
		{
			0, 1, 2, 0, 2, 3,
			3, 2, 6, 3, 6, 7,
			7, 6, 5, 7, 5, 4,
			4, 5, 1, 4, 1, 0,
			0, 3, 7, 0, 7, 4,
			1, 6, 2, 1, 5, 6
		};

		if (!indexBuffer->Fill(indices, 0, 36))
		{
			NazaraError("Failed to create vertex buffer");
			return nullptr;
		}

		return indexBuffer.release();
	}

	NzShader* BuildShader()
	{
		const char* fragmentSource110 =
		"#version 110\n"
		"varying vec3 vTexCoord;\n"
		"uniform samplerCube Skybox;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = textureCube(Skybox, vTexCoord);\n"
		"}\n";

		const char* fragmentSource140 =
		"#version 140\n"
		"in vec3 vTexCoord;\n"
		"out vec4 RenderTarget0;\n"
		"uniform samplerCube Skybox;\n"
		"void main()\n"
		"{\n"
		"	RenderTarget0 = texture(Skybox, vTexCoord);\n"
		"}\n";

		const char* vertexSource110 =
		"#version 110\n"
		"attribute vec3 VertexPosition;\n"
		"varying vec3 vTexCoord;\n"
		"uniform mat4 WorldViewProjMatrix;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n"
		"    vTexCoord = vec3(VertexPosition.x, -VertexPosition.y, -VertexPosition.z);\n"
		"}\n";

		const char* vertexSource140 =
		"#version 140\n"
		"in vec3 VertexPosition;\n"
		"out vec3 vTexCoord;\n"
		"uniform mat4 WorldViewProjMatrix;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n"
		"    vTexCoord = vec3(VertexPosition.x, -VertexPosition.y, -VertexPosition.z);\n"
		"}\n";

		///TODO: Remplacer ça par des ShaderNode
		std::unique_ptr<NzShader> shader(new NzShader(nzShaderLanguage_GLSL));
		shader->SetPersistent(false);

		bool useGLSL140 = (NzOpenGL::GetVersion() >= 310);

		if (!shader->Load(nzShaderType_Fragment, (useGLSL140) ? fragmentSource140 : fragmentSource110))
		{
			NazaraError("Failed to load fragment shader");
			return nullptr;
		}

		if (!shader->Load(nzShaderType_Vertex, (useGLSL140) ? vertexSource140 : vertexSource110))
		{
			NazaraError("Failed to load vertex shader");
			return nullptr;
		}

		if (!shader->Compile())
		{
			NazaraError("Failed to compile shader");
			return nullptr;
		}

		return shader.release();
	}

	NzVertexBuffer* BuildVertexBuffer()
	{
		std::unique_ptr<NzVertexDeclaration> declaration(new NzVertexDeclaration);
		declaration->SetPersistent(false);

		NzVertexElement elements;
		elements.offset = 0;
		elements.type = nzElementType_Float3;
		elements.usage = nzElementUsage_Position;

		if (!declaration->Create(&elements, 1))
		{
			NazaraError("Failed to create declaration");
			return nullptr;
		}

		std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(declaration.get(), 8, nzBufferStorage_Hardware, nzBufferUsage_Static));
		declaration.release();
		vertexBuffer->SetPersistent(false);

		float vertices[8*(sizeof(float)*3)] =
		{
			-1.0,  1.0,  1.0,
			-1.0, -1.0,  1.0,
			 1.0, -1.0,  1.0,
			 1.0,  1.0,  1.0,
			-1.0,  1.0, -1.0,
			-1.0, -1.0, -1.0,
			 1.0, -1.0, -1.0,
			 1.0,  1.0, -1.0,
		};

		if (!vertexBuffer->Fill(vertices, 0, 8))
		{
			NazaraError("Failed to create vertex buffer");
			return nullptr;
		}

		return vertexBuffer.release();
	}

	static NzIndexBuffer* s_indexBuffer = nullptr;
	static NzShader* s_shader = nullptr;
	static NzVertexBuffer* s_vertexBuffer = nullptr;
}

NzSkyboxBackground::NzSkyboxBackground()
{
	if (!s_indexBuffer)
		s_indexBuffer = BuildIndexBuffer();

	if (!s_shader)
		s_shader = BuildShader();

	if (!s_vertexBuffer)
		s_vertexBuffer = BuildVertexBuffer();

	m_indexBuffer = s_indexBuffer;
	m_indexBuffer->AddResourceReference();

	m_shader = s_shader;
	m_shader->AddResourceReference();

	m_sampler.SetWrapMode(nzSamplerWrap_Clamp); // Nécessaire pour ne pas voir les côtés

	m_vertexBuffer = s_vertexBuffer;
	m_vertexBuffer->AddResourceReference();
}

NzSkyboxBackground::NzSkyboxBackground(NzTexture* cubemapTexture) :
NzSkyboxBackground()
{
	SetTexture(cubemapTexture);
}

NzSkyboxBackground::~NzSkyboxBackground()
{
	if (m_indexBuffer->RemoveResourceReference())
		s_indexBuffer = nullptr;

	if (m_shader->RemoveResourceReference())
		s_shader = nullptr;

	if (m_vertexBuffer->RemoveResourceReference())
		s_vertexBuffer = nullptr;
}

void NzSkyboxBackground::Draw(const NzScene* scene) const
{
	nzUInt8 textureUnit;
	m_shader->SendTexture(m_shader->GetUniformLocation("Skybox"), m_texture, &textureUnit);

	const NzCamera* camera = scene->GetActiveCamera();

	const NzMatrix4f& viewMatrix = camera->GetViewMatrix();
	NzMatrix4f skyboxMatrix(viewMatrix);
	skyboxMatrix.SetTranslation(NzVector3f::Zero());

	NzRenderer::Enable(nzRendererParameter_Blend, false);
	NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
	NzRenderer::Enable(nzRendererParameter_FaceCulling, true);

	NzRenderer::SetDepthFunc(nzRendererComparison_Less);
	NzRenderer::SetFaceCulling(nzFaceCulling_Front);
	NzRenderer::SetFaceFilling(nzFaceFilling_Fill);
	NzRenderer::SetIndexBuffer(m_indexBuffer);
	NzRenderer::SetMatrix(nzMatrixType_View, skyboxMatrix);
	NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Scale(NzVector3f(camera->GetZNear())));
	NzRenderer::SetShader(m_shader);
	NzRenderer::SetTextureSampler(textureUnit, m_sampler);
	NzRenderer::SetVertexBuffer(m_vertexBuffer);

	NzRenderer::DrawIndexedPrimitives(nzPrimitiveType_TriangleList, 0, 36);

	NzRenderer::SetMatrix(nzMatrixType_View, viewMatrix);
}

nzBackgroundType NzSkyboxBackground::GetBackgroundType() const
{
	return nzBackgroundType_Skybox;
}

NzTexture* NzSkyboxBackground::GetTexture() const
{
	return m_texture;
}

void NzSkyboxBackground::SetTexture(NzTexture* cubemapTexture)
{
	#if NAZARA_GRAPHICS_SAFE
	if (cubemapTexture)
	{
		if (!cubemapTexture->IsValid())
		{
			NazaraError("Texture must be valid");
			return;
		}

		if (!cubemapTexture->IsCubemap())
		{
			NazaraError("Texture must be a cubemap");
			return;
		}
	}
	#endif

	m_texture = cubemapTexture;
}
