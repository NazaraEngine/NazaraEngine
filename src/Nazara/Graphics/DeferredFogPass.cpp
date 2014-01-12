// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredFogPass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzShaderProgram* BuildFogProgram()
	{
		/*const nzUInt8 fragmentSource[] = {
				#include <Nazara/Graphics/Resources/DeferredShading/Shaders/FXAA.frag.h>
		};*/

		const char* fragmentSource =
		"#version 140\n"

		"out vec4 RenderTarget0;\n"

		"uniform sampler2D ColorTexture;\n"
		"uniform sampler2D GBuffer2;\n"
		"uniform mat4 InvViewProjMatrix;\n"
		"uniform vec2 InvTargetSize;\n"
		"uniform vec3 EyePosition;\n"

		"float n = 0.1;"
		"float f = 1000.0;"

		"float color_to_float(vec3 color)\n"
		"{\n"
			"const vec3 byte_to_float = vec3(1.0, 1.0/256, 1.0/(256*256));\n"
			"return dot(color, byte_to_float);\n"
		"}\n"

		"void main()\n"
		"{"
		"vec2 texCoord = gl_FragCoord.xy * InvTargetSize;\n"
		"\t" "vec3 color = texture(ColorTexture, texCoord).xyz;\n"
		"vec4 gVec2 = textureLod(GBuffer2, texCoord, 0.0);\n"
		"float depth = color_to_float(gVec2.xyz)*2.0 - 1.0;\n"
		"float linearDepth = (2 * n) / (f + n - depth * (f - n));"

		"vec3 viewSpace = vec3(texCoord*2.0 - 1.0, depth);\n"

		"vec4 worldPos = InvViewProjMatrix * vec4(viewSpace, 1.0);\n"
		"worldPos.xyz /= worldPos.w;\n"

		/*"float lumThreshold = 0.1;"
		"float lumMultipler = 2.0;"
		//"float lumFactor = max(dot(color, vec3(0.299, 0.587, 0.114)) - lumThreshold, 0.0) / (1.0-lumThreshold);"
		"float fogFactor = (1.0 - clamp(worldPos.y-2.0, 0.0, 1.0)) - lumFactor*lumMultipler;"
		"fogFactor += (1.0 - clamp(EyePosition.y-2.5, 0.0, 1.0));"
		"fogFactor = clamp(fogFactor, 0.0, 1.0);"*/

		"float lumThreshold = 0.8;"
		"float lumMultipler = 2.0;"
		"float luminosity = dot(color, vec3(0.299, 0.587, 0.114));"
		"float lumFactor = max(luminosity - lumThreshold, 0.0) / (1.0-lumThreshold);"

		"vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0);\n"
		"vec2 fogrange = vec2(0, 50);\n"
		"float fogeffect = clamp( 1.0 - (fogrange.y - linearDepth*0.5*f) / (fogrange.y - fogrange.x) , 0.0, 1.0 ) * fogColor.w;\n"
		"fogeffect = max(fogeffect-lumFactor, 0.0);"

		//fogeffect*=(1.0 - int(depth));
		"\t" "vec3 fragmentColor = color*(1.0-fogeffect) + fogColor.rgb * fogeffect;\n"
		"\t" "RenderTarget0 = vec4(fragmentColor, 1.0);\n"
		"}";

		const char* vertexSource =
		"#version 140\n"

		"in vec3 VertexPosition;\n"

		"void main()\n"
		"{\n"
		"\t" "gl_Position = vec4(VertexPosition, 1.0);" "\n"
		"}\n";

		///TODO: Remplacer ça par des ShaderNode
		std::unique_ptr<NzShaderProgram> program(new NzShaderProgram(nzShaderLanguage_GLSL));
		program->SetPersistent(false);

		if (!program->LoadShader(nzShaderType_Fragment, fragmentSource/*NzString(reinterpret_cast<const char*>(fragmentSource), sizeof(fragmentSource))*/))
		{
				NazaraError("Failed to load fragment shader");
				return nullptr;
		}

		if (!program->LoadShader(nzShaderType_Vertex, vertexSource))
		{
				NazaraError("Failed to load vertex shader");
				return nullptr;
		}

		if (!program->Compile())
		{
				NazaraError("Failed to compile program");
				return nullptr;
		}

		return program.release();
	}
}

NzDeferredFogPass::NzDeferredFogPass()
{
	m_program = BuildFogProgram();
	m_program->SendInteger(m_program->GetUniformLocation("ColorTexture"), 0);
	m_program->SendInteger(m_program->GetUniformLocation("GBuffer2"), 1);

	m_pointSampler.SetAnisotropyLevel(1);
	m_pointSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_pointSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_states.parameters[nzRendererParameter_DepthBuffer] = false;
}

NzDeferredFogPass::~NzDeferredFogPass() = default;

bool NzDeferredFogPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetShaderProgram(m_program);
	m_program->SendVector(m_program->GetUniformLocation(nzShaderUniform_EyePosition), scene->GetViewer()->GetEyePosition());

	NzRenderer::SetRenderStates(m_states);
	NzRenderer::SetTexture(0, m_workTextures[secondWorkTexture]);
	NzRenderer::SetTexture(1, m_GBuffer[2]);
	NzRenderer::SetTextureSampler(0, m_pointSampler);
	NzRenderer::SetTextureSampler(1, m_pointSampler);
	NzRenderer::DrawFullscreenQuad();

	return true;
}
