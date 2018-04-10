// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/SceneData.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		struct BillboardPoint
		{
			Color color;
			Vector3f position;
			Vector2f size;
			Vector2f sinCos; // must follow `size` (both will be sent as a Vector4f)
			Vector2f uv;
		};

		UInt32 s_maxQuads = std::numeric_limits<UInt16>::max() / 6;
		UInt32 s_vertexBufferSize = 4 * 1024 * 1024; // 4 MiB
	}

	/*!
	* \ingroup graphics
	* \class Nz::ForwardRenderTechnique
	* \brief Graphics class that represents the technique used in forward rendering
	*/

	/*!
	* \brief Constructs a ForwardRenderTechnique object by default
	*/

	ForwardRenderTechnique::ForwardRenderTechnique() :
	m_vertexBuffer(BufferType_Vertex),
	m_maxLightPassPerObject(3)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		std::array<UInt8, 4> whitePixel = { {255, 255, 255, 255} };
		m_whiteTexture.Create(ImageType_2D, PixelFormatType_RGBA8, 1, 1);
		m_whiteTexture.Update(whitePixel.data());

		m_vertexBuffer.Create(s_vertexBufferSize, DataStorage_Hardware, BufferUsage_Dynamic);

		m_billboardPointBuffer.Reset(&s_billboardVertexDeclaration, &m_vertexBuffer);
		m_spriteBuffer.Reset(VertexDeclaration::Get(VertexLayout_XYZ_Color_UV), &m_vertexBuffer);
	}

	/*!
	* \brief Clears the data
	*
	* \param sceneData Data of the scene
	*/

	void ForwardRenderTechnique::Clear(const SceneData& sceneData) const
	{
		Renderer::Enable(RendererParameter_DepthBuffer, true);
		Renderer::Enable(RendererParameter_DepthWrite, true);
		Renderer::Clear(RendererBuffer_Depth);

		if (sceneData.background)
			sceneData.background->Draw(sceneData.viewer);
	}

	/*!
	* \brief Draws the data of the scene
	* \return true If successful
	*
	* \param sceneData Data of the scene
	*
	* \remark Produces a NazaraAssert if viewer of the scene is invalid
	*/

	bool ForwardRenderTechnique::Draw(const SceneData& sceneData) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		m_renderQueue.Sort(sceneData.viewer);

		if (!m_renderQueue.models.empty())
			DrawModels(sceneData, m_renderQueue, m_renderQueue.models);

		if (!m_renderQueue.basicSprites.empty())
			DrawSprites(sceneData, m_renderQueue, m_renderQueue.basicSprites);

		if (!m_renderQueue.billboards.empty())
			DrawBillboards(sceneData, m_renderQueue, m_renderQueue.billboards);

		if (!m_renderQueue.depthSortedModels.empty())
			DrawModels(sceneData, m_renderQueue, m_renderQueue.depthSortedModels);

		if (!m_renderQueue.depthSortedSprites.empty())
			DrawSprites(sceneData, m_renderQueue, m_renderQueue.depthSortedSprites);

		if (!m_renderQueue.depthSortedBillboards.empty())
			DrawBillboards(sceneData, m_renderQueue, m_renderQueue.depthSortedBillboards);

		if (!m_renderQueue.customDrawables.empty())
			DrawCustomDrawables(sceneData, m_renderQueue, m_renderQueue.customDrawables);

		return true;
	}

	/*!
	* \brief Gets the maximum number of lights available per pass per object
	* \return Maximum number of light simultaneously per object
	*/

	unsigned int ForwardRenderTechnique::GetMaxLightPassPerObject() const
	{
		return m_maxLightPassPerObject;
	}

	/*!
	* \brief Gets the render queue
	* \return Pointer to the render queue
	*/

	AbstractRenderQueue* ForwardRenderTechnique::GetRenderQueue()
	{
		return &m_renderQueue;
	}

	/*!
	* \brief Gets the type of the current technique
	* \return Type of the render technique
	*/

	RenderTechniqueType ForwardRenderTechnique::GetType() const
	{
		return RenderTechniqueType_BasicForward;
	}

	/*!
	* \brief Sets the maximum number of lights available per pass per object
	*
	* \param passCount Maximum number of light simulatenously per object
	*/

	void ForwardRenderTechnique::SetMaxLightPassPerObject(unsigned int maxLightPassPerObject)
	{
		m_maxLightPassPerObject = maxLightPassPerObject;
	}

	/*!
	* \brief Initializes the forward render technique
	* \return true If successful
	*
	* \remark Produces a NazaraError if one shader creation failed
	*/

	bool ForwardRenderTechnique::Initialize()
	{
		try
		{
			ErrorFlags flags(ErrorFlag_ThrowException, true);

			s_quadIndexBuffer.Reset(false, s_maxQuads * 6, DataStorage_Hardware, 0);

			BufferMapper<IndexBuffer> mapper(s_quadIndexBuffer, BufferAccess_WriteOnly);
			UInt16* indices = static_cast<UInt16*>(mapper.GetPointer());

			for (unsigned int i = 0; i < s_maxQuads; ++i)
			{
				*indices++ = i * 4 + 0;
				*indices++ = i * 4 + 2;
				*indices++ = i * 4 + 1;

				*indices++ = i * 4 + 2;
				*indices++ = i * 4 + 3;
				*indices++ = i * 4 + 1;
			}

			mapper.Unmap(); // No point to keep the buffer open any longer

			// Quad buffer (used for instancing of billboards and sprites)
			//Note: UV are computed in the shader
			s_quadVertexBuffer.Reset(VertexDeclaration::Get(VertexLayout_XY), 4, DataStorage_Hardware, 0);

			float vertices[2 * 4] = {
			   -0.5f, -0.5f,
				0.5f, -0.5f,
			   -0.5f, 0.5f,
				0.5f, 0.5f,
			};

			s_quadVertexBuffer.FillRaw(vertices, 0, sizeof(vertices));

			// Declaration used when rendering the vertex billboards
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Color,     ComponentType_Color,  NazaraOffsetOf(BillboardPoint, color));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Position,  ComponentType_Float3, NazaraOffsetOf(BillboardPoint, position));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_TexCoord,  ComponentType_Float2, NazaraOffsetOf(BillboardPoint, uv));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Userdata0, ComponentType_Float4, NazaraOffsetOf(BillboardPoint, size)); // Includes sincos

			// Declaration used when rendering the billboards with intancing
			// The main advantage is the direct copy (std::memcpy) of data in the RenderQueue to the GPU buffer
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData0, ComponentType_Float3, NazaraOffsetOf(BasicRenderQueue::BillboardData, center));
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData1, ComponentType_Float4, NazaraOffsetOf(BasicRenderQueue::BillboardData, size)); // Englobe sincos
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData2, ComponentType_Color,  NazaraOffsetOf(BasicRenderQueue::BillboardData, color));

			s_reflectionSampler.SetFilterMode(SamplerFilter_Bilinear);
			s_reflectionSampler.SetWrapMode(SamplerWrap_Clamp);

			s_shadowSampler.SetFilterMode(SamplerFilter_Bilinear);
			s_shadowSampler.SetWrapMode(SamplerWrap_Clamp);

			std::array<UInt8, 6> whitePixels = { { 255, 255, 255, 255, 255, 255 } };
			s_dummyReflection.Create(ImageType_Cubemap, PixelFormatType_L8, 1, 1);
			s_dummyReflection.Update(whitePixels.data());
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialise: " + String(e.what()));
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the forward render technique
	*/

	void ForwardRenderTechnique::Uninitialize()
	{
		s_dummyReflection.Destroy();
		s_quadIndexBuffer.Reset();
		s_quadVertexBuffer.Reset();
	}

	/*!
	* \brief Chooses the nearest lights for one object
	*
	* \param object Sphere symbolising the object
	* \param includeDirectionalLights Should directional lights be included in the computation
	*/

	void ForwardRenderTechnique::ChooseLights(const Spheref& object, bool includeDirectionalLights) const
	{
		m_lights.clear();

		// First step: add all the lights into a common list and compute their score, exlucing those who have no chance of lighting the object
		// (Those who are too far away).

		if (includeDirectionalLights)
		{
			for (unsigned int i = 0; i < m_renderQueue.directionalLights.size(); ++i)
			{
				const auto& light = m_renderQueue.directionalLights[i];
				if (IsDirectionalLightSuitable(object, light))
					m_lights.push_back({LightType_Directional, ComputeDirectionalLightScore(object, light), i});
			}
		}

		for (unsigned int i = 0; i < m_renderQueue.pointLights.size(); ++i)
		{
			const auto& light = m_renderQueue.pointLights[i];
			if (IsPointLightSuitable(object, light))
				m_lights.push_back({LightType_Point, ComputePointLightScore(object, light), i});
		}

		for (unsigned int i = 0; i < m_renderQueue.spotLights.size(); ++i)
		{
			const auto& light = m_renderQueue.spotLights[i];
			if (IsSpotLightSuitable(object, light))
				m_lights.push_back({LightType_Spot, ComputeSpotLightScore(object, light), i});
		}

		// Then, sort the lights according to their score
		std::sort(m_lights.begin(), m_lights.end(), [](const LightIndex& light1, const LightIndex& light2)
		{
			return light1.score < light2.score;
		});
	}

	void ForwardRenderTechnique::DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::Billboard>& billboards) const
	{
		VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
		instanceBuffer->SetVertexDeclaration(&s_billboardInstanceDeclaration);

		Renderer::SetVertexBuffer(&s_quadVertexBuffer);

		Nz::BufferMapper<VertexBuffer> instanceBufferMapper;
		std::size_t billboardCount = 0;
		std::size_t maxBillboardPerDraw = instanceBuffer->GetVertexCount();

		auto Commit = [&]()
		{
			if (billboardCount > 0)
			{
				instanceBufferMapper.Unmap();

				Renderer::DrawPrimitivesInstanced(billboardCount, PrimitiveMode_TriangleStrip, 0, 4);

				billboardCount = 0;
			}
		};

		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		const Texture* lastOverlay = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		for (const BasicRenderQueue::Billboard& billboard : billboards)
		{
			const Nz::Recti& scissorRect = (billboard.scissorRect.width > 0) ? billboard.scissorRect : fullscreenScissorRect;

			if (billboard.material != lastMaterial || (billboard.material->IsScissorTestEnabled() && scissorRect != lastScissorRect))
			{
				Commit();

				const MaterialPipeline* pipeline = billboard.material->GetPipeline();
				if (lastPipeline != pipeline)
				{
					pipelineInstance = &billboard.material->GetPipeline()->Apply(ShaderFlags_Billboard | ShaderFlags_Instancing | ShaderFlags_VertexColor);

					const Shader* shader = pipelineInstance->uberInstance->GetShader();
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambient color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
						// Position of the camera
						shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

						lastShader = shader;
					}

					lastPipeline = pipeline;
				}

				if (lastMaterial != billboard.material)
				{
					billboard.material->Apply(*pipelineInstance);
					lastMaterial = billboard.material;
				}

				if (billboard.material->IsScissorTestEnabled() && scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(scissorRect);
					lastScissorRect = scissorRect;
				}
			}

			if (!instanceBufferMapper.GetBuffer())
				instanceBufferMapper.Map(instanceBuffer, BufferAccess_DiscardAndWrite);

			std::memcpy(static_cast<Nz::UInt8*>(instanceBufferMapper.GetPointer()) + sizeof(BasicRenderQueue::BillboardData) * billboardCount, &billboard.data, sizeof(BasicRenderQueue::BillboardData));
			if (++billboardCount >= maxBillboardPerDraw)
				Commit();
		}

		Commit();
	}
	
	void ForwardRenderTechnique::DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::BillboardChain>& billboards) const
	{
		VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
		instanceBuffer->SetVertexDeclaration(&s_billboardInstanceDeclaration);

		Renderer::SetVertexBuffer(&s_quadVertexBuffer);

		Nz::BufferMapper<VertexBuffer> instanceBufferMapper;
		std::size_t billboardCount = 0;
		std::size_t maxBillboardPerDraw = instanceBuffer->GetVertexCount();

		auto Commit = [&]()
		{
			if (billboardCount > 0)
			{
				instanceBufferMapper.Unmap();

				Renderer::DrawPrimitivesInstanced(billboardCount, PrimitiveMode_TriangleStrip, 0, 4);

				billboardCount = 0;
			}
		};

		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		const Texture* lastOverlay = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		for (const BasicRenderQueue::BillboardChain& billboard : billboards)
		{
			const Nz::Recti& scissorRect = (billboard.scissorRect.width > 0) ? billboard.scissorRect : fullscreenScissorRect;

			if (billboard.material != lastMaterial || (billboard.material->IsScissorTestEnabled() && scissorRect != lastScissorRect))
			{
				Commit();

				const MaterialPipeline* pipeline = billboard.material->GetPipeline();
				if (lastPipeline != pipeline)
				{
					pipelineInstance = &billboard.material->GetPipeline()->Apply(ShaderFlags_Billboard | ShaderFlags_Instancing | ShaderFlags_VertexColor);

					const Shader* shader = pipelineInstance->uberInstance->GetShader();
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambient color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
						// Position of the camera
						shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

						lastShader = shader;
					}

					lastPipeline = pipeline;
				}

				if (lastMaterial != billboard.material)
				{
					billboard.material->Apply(*pipelineInstance);
					lastMaterial = billboard.material;
				}

				if (billboard.material->IsScissorTestEnabled() && scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(scissorRect);
					lastScissorRect = scissorRect;
				}
			}

			std::size_t billboardRemaining = billboard.billboardCount;
			const BasicRenderQueue::BillboardData* billboardData = renderQueue.GetBillboardData(billboard.billboardIndex);
			do
			{
				std::size_t renderedBillboardCount = std::min(billboardRemaining, maxBillboardPerDraw - billboardCount);
				billboardRemaining -= renderedBillboardCount;

				if (!instanceBufferMapper.GetBuffer())
					instanceBufferMapper.Map(instanceBuffer, BufferAccess_DiscardAndWrite);

				std::memcpy(static_cast<Nz::UInt8*>(instanceBufferMapper.GetPointer()) + sizeof(BasicRenderQueue::BillboardData) * billboardCount, billboardData, renderedBillboardCount * sizeof(BasicRenderQueue::BillboardData));
				billboardCount += renderedBillboardCount;
				billboardData += renderedBillboardCount;

				if (billboardCount >= maxBillboardPerDraw)
					Commit();
			}
			while (billboardRemaining > 0);
		}

		Commit();
	}

	void ForwardRenderTechnique::DrawCustomDrawables(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::CustomDrawable>& customDrawables) const
	{
		for (const BasicRenderQueue::CustomDrawable& customDrawable : customDrawables)
			customDrawable.drawable->Draw();
	}
	
	void ForwardRenderTechnique::DrawModels(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const Nz::RenderQueue<Nz::BasicRenderQueue::Model>& models) const
	{
		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		///TODO: Reimplement instancing

		for (const BasicRenderQueue::Model& model : models)
		{
			const MaterialPipeline* pipeline = model.material->GetPipeline();
			if (lastPipeline != pipeline)
			{
				pipelineInstance = &model.material->GetPipeline()->Apply();

				const Shader* shader = pipelineInstance->uberInstance->GetShader();
				if (shader != lastShader)
				{
					// Index of uniforms in the shader
					shaderUniforms = GetShaderUniforms(shader);

					// Ambient color of the scene
					shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
					// Position of the camera
					shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

					lastShader = shader;
				}

				lastPipeline = pipeline;
			}

			if (lastMaterial != model.material)
			{
				model.material->Apply(*pipelineInstance);
				lastMaterial = model.material;
			}

			if (model.material->IsScissorTestEnabled())
			{
				const Nz::Recti& scissorRect = (model.scissorRect.width > 0) ? model.scissorRect : fullscreenScissorRect;
				if (scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(scissorRect);
					lastScissorRect = scissorRect;
				}
			}

			if (shaderUniforms->reflectionMap != -1)
			{
				unsigned int textureUnit = Material::GetTextureUnit(TextureMap_ReflectionCube);

				Renderer::SetTexture(textureUnit, sceneData.globalReflectionTexture);
				Renderer::SetTextureSampler(textureUnit, s_reflectionSampler);
			}

			// Handle draw call before rendering loop
			Renderer::DrawCall drawFunc;
			Renderer::DrawCallInstanced instancedDrawFunc;
			unsigned int indexCount;

			if (model.meshData.indexBuffer)
			{
				drawFunc = Renderer::DrawIndexedPrimitives;
				instancedDrawFunc = Renderer::DrawIndexedPrimitivesInstanced;
				indexCount = model.meshData.indexBuffer->GetIndexCount();
			}
			else
			{
				drawFunc = Renderer::DrawPrimitives;
				instancedDrawFunc = Renderer::DrawPrimitivesInstanced;
				indexCount = model.meshData.vertexBuffer->GetVertexCount();
			}

			Renderer::SetIndexBuffer(model.meshData.indexBuffer);
			Renderer::SetVertexBuffer(model.meshData.vertexBuffer);

			if (shaderUniforms->hasLightUniforms)
			{
				ChooseLights(model.obbSphere);

				std::size_t lightCount = m_lights.size();

				Nz::Renderer::SetMatrix(Nz::MatrixType_World, model.matrix);
				std::size_t lightIndex = 0;
				RendererComparison oldDepthFunc = Renderer::GetDepthFunc(); // In the case where we have to change it

				std::size_t passCount = (lightCount == 0) ? 1 : (lightCount - 1) / NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS + 1;
				for (std::size_t pass = 0; pass < passCount; ++pass)
				{
					lightCount -= std::min<std::size_t>(lightCount, NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS);

					if (pass == 1)
					{
						// To add the result of light computations
						// We won't interfere with materials parameters because we only render opaques objects
						// (A.K.A., without blending)
						// About the depth function, it must be applied only the first time
						Renderer::Enable(RendererParameter_Blend, true);
						Renderer::SetBlendFunc(BlendFunc_One, BlendFunc_One);
						Renderer::SetDepthFunc(RendererComparison_Equal);
					}

					// Sends the light uniforms to the shader
					for (unsigned int i = 0; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
						SendLightUniforms(lastShader, shaderUniforms->lightUniforms, i, lightIndex++, shaderUniforms->lightOffset*i);

					// And we draw
					drawFunc(model.meshData.primitiveMode, 0, indexCount);
				}

				Renderer::Enable(RendererParameter_Blend, false);
				Renderer::SetDepthFunc(oldDepthFunc);
			}
			else
			{
				Renderer::SetMatrix(MatrixType_World, model.matrix);
				drawFunc(model.meshData.primitiveMode, 0, indexCount);
			}
		}
	}

	void ForwardRenderTechnique::DrawSprites(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::SpriteChain>& spriteList) const
	{
		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		Renderer::SetIndexBuffer(&s_quadIndexBuffer);
		Renderer::SetMatrix(MatrixType_World, Matrix4f::Identity());
		Renderer::SetVertexBuffer(&m_spriteBuffer);

		const unsigned int overlayTextureUnit = Material::GetTextureUnit(TextureMap_Overlay);
		const std::size_t maxSpriteCount = std::min<std::size_t>(s_maxQuads, m_spriteBuffer.GetVertexCount() / 4);

		m_spriteChains.clear();

		auto Commit = [&]()
		{
			std::size_t spriteChainCount = m_spriteChains.size();
			if (spriteChainCount > 0)
			{
				std::size_t spriteChain = 0; // Which chain of sprites are we treating
				std::size_t spriteChainOffset = 0; // Where was the last offset where we stopped in the last chain

				do
				{
					// We open the buffer in writing mode
					BufferMapper<VertexBuffer> vertexMapper(m_spriteBuffer, BufferAccess_DiscardAndWrite);
					VertexStruct_XYZ_Color_UV* vertices = static_cast<VertexStruct_XYZ_Color_UV*>(vertexMapper.GetPointer());

					std::size_t spriteCount = 0;

					do
					{
						const VertexStruct_XYZ_Color_UV* currentChain = m_spriteChains[spriteChain].first;
						std::size_t currentChainSpriteCount = m_spriteChains[spriteChain].second;
						std::size_t count = std::min(maxSpriteCount - spriteCount, currentChainSpriteCount - spriteChainOffset);

						std::memcpy(vertices, currentChain + spriteChainOffset * 4, 4 * count * sizeof(VertexStruct_XYZ_Color_UV));
						vertices += count * 4;

						spriteCount += count;
						spriteChainOffset += count;

						// Have we treated the entire chain ?
						if (spriteChainOffset == currentChainSpriteCount)
						{
							spriteChain++;
							spriteChainOffset = 0;
						}
					}
					while (spriteCount < maxSpriteCount && spriteChain < spriteChainCount);

					vertexMapper.Unmap();

					Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, 0, spriteCount * 6);
				}
				while (spriteChain < spriteChainCount);
			}

			m_spriteChains.clear();
		};

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		const Texture* lastOverlay = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		for (const BasicRenderQueue::SpriteChain& basicSprites : spriteList)
		{
			const Nz::Recti& scissorRect = (basicSprites.scissorRect.width > 0) ? basicSprites.scissorRect : fullscreenScissorRect;

			if (basicSprites.material != lastMaterial || basicSprites.overlay != lastOverlay || (basicSprites.material->IsScissorTestEnabled() && scissorRect != lastScissorRect))
			{
				Commit();

				const MaterialPipeline* pipeline = basicSprites.material->GetPipeline();
				if (lastPipeline != pipeline)
				{
					pipelineInstance = &basicSprites.material->GetPipeline()->Apply(ShaderFlags_TextureOverlay | ShaderFlags_VertexColor);

					const Shader* shader = pipelineInstance->uberInstance->GetShader();
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambient color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
						// Position of the camera
						shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

						// Overlay texture unit
						shader->SendInteger(shaderUniforms->textureOverlay, overlayTextureUnit);

						lastShader = shader;
					}

					lastPipeline = pipeline;
				}

				if (lastMaterial != basicSprites.material)
				{
					basicSprites.material->Apply(*pipelineInstance);

					Renderer::SetTextureSampler(overlayTextureUnit, basicSprites.material->GetDiffuseSampler());

					lastMaterial = basicSprites.material;
				}

				const Nz::Texture* overlayTexture = (basicSprites.overlay) ? basicSprites.overlay : &m_whiteTexture;
				if (overlayTexture != lastOverlay)
				{
					Renderer::SetTexture(overlayTextureUnit, overlayTexture);
					lastOverlay = overlayTexture;
				}

				if (basicSprites.material->IsScissorTestEnabled() && scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(scissorRect);
					lastScissorRect = scissorRect;
				}
			}

			m_spriteChains.emplace_back(basicSprites.vertices, basicSprites.spriteCount);
		}

		Commit();
	}

	const ForwardRenderTechnique::ShaderUniforms* ForwardRenderTechnique::GetShaderUniforms(const Shader* shader) const
	{
		auto it = m_shaderUniforms.find(shader);
		if (it == m_shaderUniforms.end())
		{
			ShaderUniforms uniforms;
			uniforms.shaderReleaseSlot.Connect(shader->OnShaderRelease, this, &ForwardRenderTechnique::OnShaderInvalidated);
			uniforms.shaderUniformInvalidatedSlot.Connect(shader->OnShaderUniformInvalidated, this, &ForwardRenderTechnique::OnShaderInvalidated);

			uniforms.eyePosition = shader->GetUniformLocation("EyePosition");
			uniforms.reflectionMap = shader->GetUniformLocation("ReflectionMap");
			uniforms.sceneAmbient = shader->GetUniformLocation("SceneAmbient");
			uniforms.textureOverlay = shader->GetUniformLocation("TextureOverlay");

			int type0Location = shader->GetUniformLocation("Lights[0].type");
			int type1Location = shader->GetUniformLocation("Lights[1].type");

			if (type0Location > 0 && type1Location > 0)
			{
				uniforms.hasLightUniforms = true;
				uniforms.lightOffset = type1Location - type0Location;
				uniforms.lightUniforms.ubo = false;
				uniforms.lightUniforms.locations.type = type0Location;
				uniforms.lightUniforms.locations.color = shader->GetUniformLocation("Lights[0].color");
				uniforms.lightUniforms.locations.factors = shader->GetUniformLocation("Lights[0].factors");
				uniforms.lightUniforms.locations.lightViewProjMatrix = shader->GetUniformLocation("LightViewProjMatrix[0]");
				uniforms.lightUniforms.locations.parameters1 = shader->GetUniformLocation("Lights[0].parameters1");
				uniforms.lightUniforms.locations.parameters2 = shader->GetUniformLocation("Lights[0].parameters2");
				uniforms.lightUniforms.locations.parameters3 = shader->GetUniformLocation("Lights[0].parameters3");
				uniforms.lightUniforms.locations.shadowMapping = shader->GetUniformLocation("Lights[0].shadowMapping");
			}
			else
				uniforms.hasLightUniforms = false;

			it = m_shaderUniforms.emplace(shader, std::move(uniforms)).first;
		}

		return &it->second;
	}

	/*!
	* \brief Handle the invalidation of a shader
	*
	* \param shader Shader being invalidated
	*/

	void ForwardRenderTechnique::OnShaderInvalidated(const Shader* shader) const
	{
		m_shaderUniforms.erase(shader);
	}

	/*!
	* \brief Sends the uniforms for light
	*
	* \param shader Shader to send uniforms to
	* \param uniforms Uniforms to send
	* \param index Index of the light
	* \param uniformOffset Offset for the uniform
	* \param availableTextureUnit Unit texture available
	*/
	void ForwardRenderTechnique::SendLightUniforms(const Shader* shader, const LightUniforms& uniforms, unsigned int index, unsigned int lightIndex, unsigned int uniformOffset) const
	{
		if (lightIndex < m_lights.size())
		{
			const LightIndex& lightInfo = m_lights[lightIndex];

			shader->SendInteger(uniforms.locations.type + uniformOffset, lightInfo.type); //< Sends the light type

			switch (lightInfo.type)
			{
				case LightType_Directional:
				{
					const auto& light = m_renderQueue.directionalLights[lightInfo.index];

					shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
					shader->SendVector(uniforms.locations.factors + uniformOffset, Vector2f(light.ambientFactor, light.diffuseFactor));
					shader->SendVector(uniforms.locations.parameters1 + uniformOffset, Vector4f(light.direction));

					if (uniforms.locations.shadowMapping != -1)
						shader->SendBoolean(uniforms.locations.shadowMapping + uniformOffset, light.shadowMap != nullptr);

					if (light.shadowMap)
					{
						unsigned int textureUnit2D = Material::GetTextureUnit(static_cast<TextureMap>(TextureMap_Shadow2D_1 + index));

						Renderer::SetTexture(textureUnit2D, light.shadowMap);
						Renderer::SetTextureSampler(textureUnit2D, s_shadowSampler);

						if (uniforms.locations.lightViewProjMatrix != -1)
							shader->SendMatrix(uniforms.locations.lightViewProjMatrix + index, light.transformMatrix);
					}
					break;
				}

				case LightType_Point:
				{
					const auto& light = m_renderQueue.pointLights[lightInfo.index];

					shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
					shader->SendVector(uniforms.locations.factors + uniformOffset, Vector2f(light.ambientFactor, light.diffuseFactor));
					shader->SendVector(uniforms.locations.parameters1 + uniformOffset, Vector4f(light.position, light.attenuation));
					shader->SendVector(uniforms.locations.parameters2 + uniformOffset, Vector4f(0.f, 0.f, 0.f, light.invRadius));

					if (uniforms.locations.shadowMapping != -1)
						shader->SendBoolean(uniforms.locations.shadowMapping + uniformOffset, light.shadowMap != nullptr);

					if (light.shadowMap)
					{
						unsigned int textureUnitCube = Material::GetTextureUnit(static_cast<TextureMap>(TextureMap_ShadowCube_1 + index));

						Renderer::SetTexture(textureUnitCube, light.shadowMap);
						Renderer::SetTextureSampler(textureUnitCube, s_shadowSampler);
					}
					break;
				}

				case LightType_Spot:
				{
					const auto& light = m_renderQueue.spotLights[lightInfo.index];

					shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
					shader->SendVector(uniforms.locations.factors + uniformOffset, Vector2f(light.ambientFactor, light.diffuseFactor));
					shader->SendVector(uniforms.locations.parameters1 + uniformOffset, Vector4f(light.position, light.attenuation));
					shader->SendVector(uniforms.locations.parameters2 + uniformOffset, Vector4f(light.direction, light.invRadius));
					shader->SendVector(uniforms.locations.parameters3 + uniformOffset, Vector2f(light.innerAngleCosine, light.outerAngleCosine));

					if (uniforms.locations.shadowMapping != -1)
						shader->SendBoolean(uniforms.locations.shadowMapping + uniformOffset, light.shadowMap != nullptr);

					if (light.shadowMap)
					{
						unsigned int textureUnit2D = Material::GetTextureUnit(static_cast<TextureMap>(TextureMap_Shadow2D_1 + index));

						Renderer::SetTexture(textureUnit2D, light.shadowMap);
						Renderer::SetTextureSampler(textureUnit2D, s_shadowSampler);

						if (uniforms.locations.lightViewProjMatrix != -1)
							shader->SendMatrix(uniforms.locations.lightViewProjMatrix + index, light.transformMatrix);
					}
					break;
				}
			}
		}
		else
		{
			if (uniforms.locations.type != -1)
				shader->SendInteger(uniforms.locations.type + uniformOffset, -1); //< Disable the light in the shader
		}
	}

	IndexBuffer ForwardRenderTechnique::s_quadIndexBuffer;
	Texture ForwardRenderTechnique::s_dummyReflection;
	TextureSampler ForwardRenderTechnique::s_reflectionSampler;
	TextureSampler ForwardRenderTechnique::s_shadowSampler;
	VertexBuffer ForwardRenderTechnique::s_quadVertexBuffer;
	VertexDeclaration ForwardRenderTechnique::s_billboardInstanceDeclaration;
	VertexDeclaration ForwardRenderTechnique::s_billboardVertexDeclaration;
}
