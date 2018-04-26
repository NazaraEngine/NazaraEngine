// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredLightScatteringPass.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/BasicRenderQueue.hpp>
#include <Nazara/Graphics/SceneData.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	constexpr unsigned int RTTScale = 2;

	namespace
	{
		ShaderRef BuildOccluderExtractionShader()
		{
			/*const UInt8 fragmentSource[] = {
					#include <Nazara/Graphics/Resources/DeferredShading/Shaders/FXAA.frag.h>
			};*/

			const char* fragmentSource = R"(
#version 140

out vec4 RenderTarget0;

uniform sampler2D GBuffer0;
uniform sampler2D GBufferDepth;
uniform vec2 InvTargetSize;

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
	float depth = texture(GBufferDepth, texCoord).r;

	//vec3 color = (depth > 0.99) ? vec3(0.5) : vec3(0.0);

	RenderTarget0 = vec4(vec3(0.0), 1.0);
	gl_FragDepth = depth;
}
)";

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

			if (!shader->AttachStageFromSource(ShaderStageType_Fragment, fragmentSource))
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
			shader->SendInteger(shader->GetUniformLocation("GBufferDepth"), 1);

			return shader;
		}

		ShaderRef BuildLightScatteringShader()
		{
			// From http://fabiensanglard.net/lightScattering/

			/*const UInt8 fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/FXAA.frag.h>
			};*/

			const char* fragmentSource = R"(
#version 140

out vec4 RenderTarget0;

uniform float Decay;
uniform float Density;
uniform float Exposure;
uniform float Weight;
uniform vec2 LightPositionOnScreen;
uniform sampler2D OccluderTexture;
uniform vec2 InvTargetSize;

const int NUM_SAMPLES = 100;

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
	vec2 deltaTexCoord = texCoord - LightPositionOnScreen.xy;
	deltaTexCoord *= 1.0 /  float(NUM_SAMPLES) * Density;

	float illuminationDecay = 1.0;
	vec3 color = vec3(0.0);
	
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
			texCoord -= deltaTexCoord;
			vec3 sample = texture2D(OccluderTexture, texCoord).rgb;
			
			sample *= illuminationDecay * Weight;
			
			color += sample;
			
			illuminationDecay *= Decay;
	}
	
	RenderTarget0 = vec4(color * Exposure, 1.0);
}
)";

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

			if (!shader->AttachStageFromSource(ShaderStageType_Fragment, fragmentSource))
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

			shader->SendInteger(shader->GetUniformLocation("OccluderTexture"), 0);
			shader->SendFloat(shader->GetUniformLocation("Decay"), 1.0f);
			shader->SendFloat(shader->GetUniformLocation("Density"), 0.84f);
			shader->SendFloat(shader->GetUniformLocation("Exposure"), 0.0034f);
			shader->SendFloat(shader->GetUniformLocation("Weight"), 5.65f);

			return shader;
		}
	}

	/*!
	* \ingroup graphics
	* \class Nz::DeferredLightScatteringPass
	* \brief Graphics class that represents the pass for fog in deferred rendering
	*/

	/*!
	* \brief Constructs a DeferredLightScatteringPass object by default
	*/
	DeferredLightScatteringPass::DeferredLightScatteringPass() :
	m_billboardInstanceData(Nz::Matrix4f::Identity()),
	m_billboardInstanceData2(Nz::Matrix4f::Identity()),
	m_blurPassCount(0)
	{
		m_pointSampler.SetAnisotropyLevel(1);
		m_pointSampler.SetFilterMode(SamplerFilter_Nearest);
		m_pointSampler.SetWrapMode(SamplerWrap_Clamp);

		m_bilinearSampler.SetAnisotropyLevel(1);
		m_bilinearSampler.SetFilterMode(SamplerFilter_Bilinear);
		m_bilinearSampler.SetWrapMode(SamplerWrap_Clamp);

		m_blendShader = ShaderLibrary::Get("DeferredBloomFinal");
		m_occluderExtractionShader = BuildOccluderExtractionShader();
		m_lightScatteringShader = BuildLightScatteringShader();
		m_lightScatteringLightPositionLocation = m_lightScatteringShader->GetUniformLocation("LightPositionOnScreen");

		m_gaussianBlurShader = ShaderLibrary::Get("DeferredGaussianBlur");
		m_gaussianBlurShaderFilterLocation = m_gaussianBlurShader->GetUniformLocation("Filter");

		m_blendingStates.depthBuffer = false;
		m_blendingStates.blending = true;
		m_blendingStates.dstBlend = BlendFunc_One;
		m_blendingStates.srcBlend = BlendFunc_One;

		m_states.depthBuffer = true;
		m_states.depthFunc = RendererComparison_Always;

		Nz::MaterialRef testMat = Nz::Material::New("Translucent3D");
		testMat->EnableDepthBuffer(true);
		testMat->SetDiffuseMap("resources/flare1.png");
		testMat->SetShader("Basic");

		m_test.SetMaterial(testMat);
		m_test.SetSize(3.f, 3.f);

		m_billboardInstanceData.renderOrder = 0;
		m_billboardInstanceData.transformMatrix = Nz::Matrix4f::Translate(Nz::Vector3f::Zero());
		m_test.UpdateData(&m_billboardInstanceData);

		m_billboardInstanceData2.renderOrder = 0;
		m_billboardInstanceData2.transformMatrix = Nz::Matrix4f::Translate(Nz::Vector3f::Left() * 10.f);
		m_test.UpdateData(&m_billboardInstanceData2);

	}

	DeferredLightScatteringPass::~DeferredLightScatteringPass() = default;

	/*!
	* \brief Processes the work on the data while working with textures
	* \return true
	*
	* \param sceneData Data for the scene
	* \param firstWorkTexture Index of the first texture to work with
	* \param firstWorkTexture Index of the second texture to work with
	*/
	bool DeferredLightScatteringPass::Process(const SceneData& sceneData, unsigned int workTexture, unsigned int sceneTexture) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		Renderer::SetTarget(&m_occluderRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x / RTTScale, m_dimensions.y / RTTScale));

		// Occluders + light rendering
		m_occluderRTT.SetColorTarget(0);

		Renderer::SetRenderStates(m_states);
		Renderer::SetTextureSampler(0, m_pointSampler);
		Renderer::SetTextureSampler(1, m_pointSampler);

		Renderer::SetShader(m_occluderExtractionShader);

		Renderer::SetTexture(0, m_GBuffer[0]);
		Renderer::SetTexture(1, m_depthStencilTexture);

		Renderer::DrawFullscreenQuad();

		AbstractRenderQueue* renderQueue = m_renderTechnique.GetRenderQueue();
		renderQueue->Clear();

		m_test.AddToRenderQueue(renderQueue, m_billboardInstanceData, Nz::Recti(-1, -1));

		Renderer::SetMatrix(MatrixType_Projection, sceneData.viewer->GetProjectionMatrix());
		Renderer::SetMatrix(MatrixType_View, sceneData.viewer->GetViewMatrix());

		m_renderTechnique.Draw(sceneData);

		// Light scattering rendering
		m_occluderRTT.SetColorTarget(1);

		Renderer::SetRenderStates(m_states);

		auto Project = [&](Nz::Vector3f pos) -> Nz::Vector3f
		{
			Nz::Vector4f worldPosition(pos, 1.f);
			worldPosition = sceneData.viewer->GetViewMatrix() * worldPosition;
			worldPosition = sceneData.viewer->GetProjectionMatrix() * worldPosition;
			worldPosition /= worldPosition.w;

			Nz::Vector3f screenPosition(worldPosition.x * 0.5f + 0.5f, worldPosition.y * 0.5f + 0.5f, worldPosition.z * 0.5f + 0.5f);
			return screenPosition;
		};

		Renderer::SetShader(m_lightScatteringShader);

		Renderer::SetTexture(0, m_occluderColorTextures[0]);

		m_lightScatteringShader->SendVector(m_lightScatteringLightPositionLocation, Nz::Vector2f(Project(Nz::Vector3f::Zero())));

		Renderer::DrawFullscreenQuad();

		m_lightScatteringShader->SendVector(m_lightScatteringLightPositionLocation, Nz::Vector2f(Project(Nz::Vector3f::Left() * 10.f)));

		Renderer::DrawFullscreenQuad();

		// Apply some blur
		Renderer::SetTextureSampler(0, m_bilinearSampler);

		for (unsigned int i = 0; i < m_blurPassCount; ++i)
		{
			m_occluderRTT.SetColorTarget(0); // occluderTextureA

			m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, Vector2f(1.f, 0.f));

			Renderer::SetTexture(0, m_occluderColorTextures[1]);
			Renderer::DrawFullscreenQuad();

			m_occluderRTT.SetColorTarget(1); // occluderTextureB

			m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, Vector2f(0.f, 1.f));

			Renderer::SetTexture(0, m_occluderColorTextures[0]);
			Renderer::DrawFullscreenQuad();
		}

		// Blend it with the scene

		Renderer::SetTextureSampler(0, m_bilinearSampler);
		Renderer::SetTextureSampler(1, m_bilinearSampler);

		m_workRTT->SetColorTarget(workTexture);
		Renderer::SetTarget(m_workRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetShader(m_blendShader);
		Renderer::SetTexture(0, m_occluderColorTextures[1]);
		Renderer::SetTexture(1, m_workTextures[sceneTexture]);
		Renderer::DrawFullscreenQuad();

		return true;
		/*const unsigned int dofBlurPass = 2;
		for (unsigned int i = 0; i < dofBlurPass; ++i)
		{
			m_dofRTT.SetColorTarget(0); // dofTextureA

			m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, Vector2f(1.f, 0.f));

			Renderer::SetTexture(0, (i == 0) ? m_workTextures[secondWorkTexture] : static_cast<const Texture*>(m_dofTextures[1]));
			Renderer::DrawFullscreenQuad();

			m_dofRTT.SetColorTarget(1); // dofTextureB

			m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, Vector2f(0.f, 1.f));

			Renderer::SetTexture(0, m_dofTextures[0]);
			Renderer::DrawFullscreenQuad();
		}*/


		/*m_workRTT->SetColorTarget(firstWorkTexture);
		Renderer::SetTarget(m_workRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetShader(m_shader);
		m_shader->SendVector(m_shaderEyePositionLocation, sceneData.viewer->GetEyePosition());

		Renderer::SetRenderStates(m_states);
		Renderer::SetTexture(0, m_workTextures[secondWorkTexture]);
		Renderer::SetTexture(1, m_GBuffer[2]);
		Renderer::SetTextureSampler(0, m_pointSampler);
		Renderer::SetTextureSampler(1, m_pointSampler);
		Renderer::DrawFullscreenQuad();*/

		return true;
	}

	/*!
	* \brief Resizes the texture sizes
	* \return true If successful
	*
	* \param dimensions Dimensions for the compute texture
	*/
	bool DeferredLightScatteringPass::Resize(const Vector2ui& GBufferSize)
	{
		DeferredRenderPass::Resize(GBufferSize);

		if (!m_occluderRTT.Create(true))
			return false;

		Nz::UInt8 colorIndex = 0;
		for (Nz::TextureRef& occluderColor : m_occluderColorTextures)
		{
			occluderColor = Nz::Texture::New();
			if (!occluderColor->Create(Nz::ImageType_2D, Nz::PixelFormatType_RGB8, GBufferSize.x / RTTScale, GBufferSize.y / RTTScale))
				return false;

			if (!m_occluderRTT.AttachTexture(AttachmentPoint_Color, colorIndex++, occluderColor))
				return false;
		}

		m_occluderDepthTexture = Nz::Texture::New();
		if (!m_occluderDepthTexture->Create(Nz::ImageType_2D, Nz::PixelFormatType_Depth16, GBufferSize.x / RTTScale, GBufferSize.y / RTTScale))
			return false;

		if (!m_occluderRTT.AttachTexture(AttachmentPoint_Depth, 0, m_occluderDepthTexture))
			return false;

		m_occluderRTT.Unlock();

		if (!m_occluderRTT.IsComplete())
		{
			NazaraError("Incomplete RTT");
			return false;
		}

		return true;
	}
}
