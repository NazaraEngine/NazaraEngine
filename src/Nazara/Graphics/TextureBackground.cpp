// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Graphics/TextureBackground.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzShader* BuildShader()
	{
		const char* fragmentSource110 =
		"#version 110\n"
		"varying vec2 vTexCoord;\n"
		"uniform sampler2D Texture;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = texture(Texture, vTexCoord);\n"
		"}\n";

		const char* fragmentSource140 =
		"#version 140\n"
		"in vec2 vTexCoord;\n"
		"out vec4 RenderTarget0;\n"
		"uniform sampler2D Texture;\n"
		"void main()\n"
		"{\n"
		"	RenderTarget0 = texture(Texture, vTexCoord);\n"
		"}\n";

		const char* vertexSource110 =
		"#version 110\n"
		"attribute vec2 VertexPosition;\n"
		"varying vec2 vTexCoord;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(VertexPosition, 0.0, 1.0);\n"
		"    vTexCoord = vec2((VertexPosition.x + 1.0)*0.5, (VertexPosition.y + 1.0)*0.5);\n"
		"}\n";

		const char* vertexSource140 =
		"#version 140\n"
		"in vec2 VertexPosition;\n"
		"out vec2 vTexCoord;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(VertexPosition, 0.0, 1.0);\n"
		"    vTexCoord = vec2((VertexPosition.x + 1.0)*0.5, (VertexPosition.y + 1.0)*0.5);\n"
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

	static NzShader* s_shader = nullptr;
	static unsigned int s_textureLocation;
}

NzTextureBackground::NzTextureBackground()
{
	if (!s_shader)
	{
		s_shader = BuildShader();
		s_textureLocation = s_shader->GetUniformLocation("Texture");
	}

	m_shader = s_shader;
}

NzTextureBackground::NzTextureBackground(NzTexture* texture) :
NzTextureBackground()
{
	m_texture = texture;
}

NzTextureBackground::~NzTextureBackground()
{
	if (m_shader.Reset())
		s_shader = nullptr;
}

void NzTextureBackground::Draw(const NzScene* scene) const
{
	NazaraUnused(scene);

	static NzRenderStates states;

	m_shader->SendInteger(s_textureLocation, 0);

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShader(m_shader);
	NzRenderer::SetTexture(0, m_texture);

	NzRenderer::DrawFullscreenQuad();
}

nzBackgroundType NzTextureBackground::GetBackgroundType() const
{
	return nzBackgroundType_Texture;
}

NzTexture* NzTextureBackground::GetTexture() const
{
	return m_texture;
}

void NzTextureBackground::SetTexture(NzTexture* texture)
{
	m_texture = texture;
}
