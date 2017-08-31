// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredFogPass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		/*!
		* \brief Builds the shader for the fog
		* \return Reference to the shader newly created
		*/

		ShaderRef BuildFogShader()
		{
			/*const UInt8 fragmentSource[] = {
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
			ShaderRef shader = Shader::New();
			if (!shader->Create())
			{
					NazaraError("Failed to load create shader");
					return nullptr;
			}

			if (!shader->AttachStageFromSource(ShaderStageType_Fragment, fragmentSource/*String(reinterpret_cast<const char*>(fragmentSource), sizeof(fragmentSource))*/))
			{
					NazaraError("Failed to load fragment shader");
					return nullptr;
			}

			if (!shader->AttachStageFromSource(ShaderStageType_Vertex, vertexSource))
			{
					NazaraError("Failed to load vertex shader");
					return nullptr;
			}

			if (!shader->Link())
			{
					NazaraError("Failed to link shader");
					return nullptr;
			}

			shader->SendInteger(shader->GetUniformLocation("ColorTexture"), 0);
			shader->SendInteger(shader->GetUniformLocation("GBuffer2"), 1);

			return shader;
		}
	}

	/*!
	* \ingroup graphics
	* \class Nz::DeferredFogPass
	* \brief Graphics class that represents the pass for fog in deferred rendering
	*/

	/*!
	* \brief Constructs a DeferredFogPass object by default
	*/

	DeferredFogPass::DeferredFogPass()
	{
		m_pointSampler.SetAnisotropyLevel(1);
		m_pointSampler.SetFilterMode(SamplerFilter_Nearest);
		m_pointSampler.SetWrapMode(SamplerWrap_Clamp);

		m_shader = BuildFogShader();
		m_shaderEyePositionLocation = m_shader->GetUniformLocation("EyePosition");

		m_states.depthBuffer = false;
	}

	DeferredFogPass::~DeferredFogPass() = default;

	/*!
	* \brief Processes the work on the data while working with textures
	* \return true
	*
	* \param sceneData Data for the scene
	* \param firstWorkTexture Index of the first texture to work with
	* \param firstWorkTexture Index of the second texture to work with
	*/

	bool DeferredFogPass::Process( const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		m_workRTT->SetColorTarget(firstWorkTexture);
		Renderer::SetTarget(m_workRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetShader(m_shader);
		m_shader->SendVector(m_shaderEyePositionLocation, sceneData.viewer->GetEyePosition());

		Renderer::SetRenderStates(m_states);
		Renderer::SetTexture(0, m_workTextures[secondWorkTexture]);
		Renderer::SetTexture(1, m_GBuffer[2]);
		Renderer::SetTextureSampler(0, m_pointSampler);
		Renderer::SetTextureSampler(1, m_pointSampler);
		Renderer::DrawFullscreenQuad();

		return true;
}
}
