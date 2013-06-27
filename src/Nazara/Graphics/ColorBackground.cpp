// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Graphics/ColorBackGround.hpp>
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
		"uniform vec4 Color;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = Color;\n"
		"}\n";

		const char* fragmentSource140 =
		"#version 140\n"
		"out vec4 RenderTarget0;\n"
		"uniform vec4 Color;\n"
		"void main()\n"
		"{\n"
		"	RenderTarget0 = Color;\n"
		"}\n";

		const char* vertexSource110 =
		"#version 110\n"
		"attribute vec2 VertexPosition;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(VertexPosition, 0.0, 1.0);\n"
		"}\n";

		const char* vertexSource140 =
		"#version 140\n"
		"in vec2 VertexPosition;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(VertexPosition, 0.0, 1.0);\n"
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
	static unsigned int s_colorLocation;
}

NzColorBackground::NzColorBackground(const NzColor& color) :
m_color(color)
{
	if (!s_shader)
	{
		s_shader = BuildShader();
		s_colorLocation = s_shader->GetUniformLocation("Color");
	}

	m_shader = s_shader;
}

NzColorBackground::~NzColorBackground()
{
	if (m_shader.Reset())
		s_shader = nullptr;
}

void NzColorBackground::Draw(const NzScene* scene) const
{
	NazaraUnused(scene);

	static NzRenderStates states;

	m_shader->SendColor(s_colorLocation, m_color);

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShader(m_shader);

	NzRenderer::DrawFullscreenQuad();
}

nzBackgroundType NzColorBackground::GetBackgroundType() const
{
	return nzBackgroundType_Color;
}

NzColor NzColorBackground::GetColor() const
{
	return m_color;
}

void NzColorBackground::SetColor(const NzColor& color)
{
	m_color = color;
}

