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
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/SceneData.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/MatrixRegistry.hpp>
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

		constexpr UInt32 s_vertexBufferSize = 4 * 1024 * 1024; // 4 MiB
		constexpr UInt32 s_maxQuadPerDraw = s_vertexBufferSize / sizeof(VertexLayout_XYZ_Color_UV);
		constexpr unsigned int InvalidTextureUnit = std::numeric_limits<unsigned int>::max();
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

		m_whiteCubemap = Nz::TextureLibrary::Get("WhiteCubemap");
		m_whiteTexture = Nz::TextureLibrary::Get("White2D");

		m_vertexBuffer.Create(s_vertexBufferSize, DataStorage_Hardware, BufferUsage_Dynamic);

		m_billboardPointBuffer.Reset(&s_billboardVertexDeclaration, &m_vertexBuffer);
		m_spriteBuffer.Reset(VertexDeclaration::Get(VertexLayout_XYZ_Color_UV), &m_vertexBuffer);

		PredefinedInstanceData instanceDataStruct = PredefinedInstanceData::GetOffset();
		m_identityInstanceData = UniformBuffer::New(instanceDataStruct.totalSize, DataStorage_Hardware, BufferUsage_Dynamic);
		{
			BufferMapper<UniformBuffer> mapper(m_identityInstanceData, BufferAccess_DiscardAndWrite);

			Nz::Matrix4f* worldMatrix = AccessByOffset<Nz::Matrix4f>(mapper.GetPointer(), instanceDataStruct.worldMatrixOffset);
			Nz::Matrix4f* invWorldMatrix = AccessByOffset<Nz::Matrix4f>(mapper.GetPointer(), instanceDataStruct.invWorldMatrixOffset);

			*worldMatrix = Matrix4f::Identity();
			*invWorldMatrix = Matrix4f::Identity();
		}

		PredefinedLightData lightDataStruct = PredefinedLightData::GetOffset();
		m_lightData = UniformBuffer::New(lightDataStruct.lightArraySize, DataStorage_Hardware, BufferUsage_Dynamic);
	}

	/*!
	* \brief Clears the data
	*
	* \param sceneData Data of the scene
	*/

	void ForwardRenderTechnique::Clear(const SceneData& sceneData) const
	{
		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		Renderer::SetScissorRect(fullscreenScissorRect);

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

	bool ForwardRenderTechnique::Draw(const SceneData& sceneData, const MatrixRegistry& matrixRegistry) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		m_renderQueue.Sort(sceneData.viewer);

		if (!m_renderQueue.models.empty())
			DrawModels(sceneData, matrixRegistry, m_renderQueue, m_renderQueue.models);

		if (!m_renderQueue.basicSprites.empty())
			DrawSprites(sceneData, m_renderQueue, m_renderQueue.basicSprites);

		if (!m_renderQueue.billboards.empty())
			DrawBillboards(sceneData, m_renderQueue, m_renderQueue.billboards);

		if (!m_renderQueue.depthSortedModels.empty())
			DrawModels(sceneData, matrixRegistry, m_renderQueue, m_renderQueue.depthSortedModels);

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

			s_quadIndexBuffer.Reset(true, s_maxQuadPerDraw * 6, DataStorage_Hardware, 0);

			BufferMapper<IndexBuffer> mapper(s_quadIndexBuffer, BufferAccess_WriteOnly);
			UInt32* indices = static_cast<UInt32*>(mapper.GetPointer());

			for (UInt32 i = 0; i < s_maxQuadPerDraw; ++i)
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
		s_quadIndexBuffer.Reset();
		s_quadVertexBuffer.Reset();
	}

	/*!
	* \brief Chooses the nearest lights for one object
	*
	* \param object Sphere symbolizing the object
	* \param includeDirectionalLights Should directional lights be included in the computation
	*/

	void ForwardRenderTechnique::ChooseLights(const Spheref& object, bool includeDirectionalLights) const
	{
		m_lights.clear();

		// First step: add all the lights into a common list and compute their score, excluding those who have no chance of lighting the object
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

				const auto& materialSettings = billboard.material->GetSettings();

				const MaterialPipeline* pipeline = billboard.material->GetPipeline();
				if (lastPipeline != pipeline)
				{
					pipelineInstance = &billboard.material->GetPipeline()->Apply(ShaderFlags_Billboard | ShaderFlags_Instancing | ShaderFlags_VertexColor);

					std::size_t viewerDataBinding = materialSettings->GetPredefinedBindingIndex(PredefinedShaderBinding_UboViewerData);
					if (viewerDataBinding != MaterialSettings::InvalidIndex)
					{
						auto it = pipelineInstance->bindings.find(materialSettings->GetTextures().size() + materialSettings->GetUniformBlocks().size() + viewerDataBinding);
						assert(it != pipelineInstance->bindings.end());

						Renderer::SetUniformBuffer(it->second, sceneData.viewer->GetViewerData());
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
	
	void ForwardRenderTechnique::DrawModels(const SceneData& sceneData, const MatrixRegistry& matrixRegistry, const BasicRenderQueue& renderQueue, const Nz::RenderQueue<Nz::BasicRenderQueue::Model>& models) const
	{
		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		///TODO: Reimplement instancing

		for (const BasicRenderQueue::Model& model : models)
		{
			const auto& materialSettings = model.material->GetSettings();

			const MaterialPipeline* pipeline = model.material->GetPipeline();
			if (lastPipeline != pipeline)
			{
				pipelineInstance = &model.material->GetPipeline()->Apply();

				std::size_t viewerDataBinding = materialSettings->GetPredefinedBindingIndex(PredefinedShaderBinding_UboViewerData);
				if (viewerDataBinding != MaterialSettings::InvalidIndex)
				{
					auto it = pipelineInstance->bindings.find(materialSettings->GetTextures().size() + materialSettings->GetUniformBlocks().size() + viewerDataBinding);
					assert(it != pipelineInstance->bindings.end());

					Renderer::SetUniformBuffer(it->second, sceneData.viewer->GetViewerData());
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

/*
			if (shaderUniforms->reflectionMap != -1)
			{
				unsigned int textureUnit = Material::GetTextureUnit(TextureMap_ReflectionCube);

				Renderer::SetTexture(textureUnit, sceneData.globalReflectionTexture);
				Renderer::SetTextureSampler(textureUnit, s_reflectionSampler);
			}
*/

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

			std::size_t instanceDataBinding = materialSettings->GetPredefinedBindingIndex(PredefinedShaderBinding_UboInstanceData);
			if (instanceDataBinding != MaterialSettings::InvalidIndex)
			{
				auto it = pipelineInstance->bindings.find(materialSettings->GetTextures().size() + materialSettings->GetUniformBlocks().size() + instanceDataBinding);
				assert(it != pipelineInstance->bindings.end());

				Renderer::SetUniformBuffer(it->second, matrixRegistry.GetUbo(model.instanceIndex));
			}

			std::size_t lightDataBinding = materialSettings->GetPredefinedBindingIndex(PredefinedShaderBinding_UboLighData);
			if (lightDataBinding != MaterialSettings::InvalidIndex)
			{
				ChooseLights(model.obbSphere);

				auto it = pipelineInstance->bindings.find(materialSettings->GetTextures().size() + materialSettings->GetUniformBlocks().size() + lightDataBinding);
				assert(it != pipelineInstance->bindings.end());

				Renderer::SetUniformBuffer(it->second, m_lightData);

				std::size_t lightIndex = 0;
				RendererComparison oldDepthFunc = Renderer::GetDepthFunc(); // In the case where we have to change it

				std::size_t passCount = (m_lights.size() == 0) ? 1 : (m_lights.size() - 1) / NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS + 1;
				for (std::size_t pass = 0; pass < passCount; ++pass)
				{
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
					UpdateLightUniforms(lightIndex, std::min<std::size_t>(m_lights.size() - lightIndex, NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS));
					lightIndex += NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS;

					// And we draw
					drawFunc(model.meshData.primitiveMode, 0, indexCount);
				}

				Renderer::Enable(RendererParameter_Blend, false);
				Renderer::SetDepthFunc(oldDepthFunc);
			}
			else
				drawFunc(model.meshData.primitiveMode, 0, indexCount);
		}
	}

	void ForwardRenderTechnique::DrawSprites(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::SpriteChain>& spriteList) const
	{
		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const std::size_t maxSpriteCount = std::min<std::size_t>(s_maxQuadPerDraw, m_spriteBuffer.GetVertexCount() / 4);

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		const Texture* lastOverlay = nullptr;
		Recti lastScissorRect = Recti(-1, -1);
		unsigned int overlayTextureUnit = InvalidTextureUnit;

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		Renderer::SetIndexBuffer(&s_quadIndexBuffer);
		Renderer::SetVertexBuffer(&m_spriteBuffer);

		auto Draw = [&]()
		{
			unsigned int firstIndex = 0;
			for (const auto& batch : m_spriteBatches)
			{
				const auto& materialSettings = batch.material->GetSettings();

				const MaterialPipeline* pipeline = batch.material->GetPipeline();
				if (pipeline != lastPipeline)
				{
					pipelineInstance = &batch.material->GetPipeline()->Apply(ShaderFlags_TextureOverlay | ShaderFlags_VertexColor);

					std::size_t instanceDataBinding = materialSettings->GetPredefinedBindingIndex(PredefinedShaderBinding_UboInstanceData);
					if (instanceDataBinding != MaterialSettings::InvalidIndex)
					{
						auto it = pipelineInstance->bindings.find(materialSettings->GetTextures().size() + materialSettings->GetUniformBlocks().size() + instanceDataBinding);
						assert(it != pipelineInstance->bindings.end());

						Renderer::SetUniformBuffer(it->second, m_identityInstanceData);
					}

					std::size_t viewerDataBinding = materialSettings->GetPredefinedBindingIndex(PredefinedShaderBinding_UboViewerData);
					if (viewerDataBinding != MaterialSettings::InvalidIndex)
					{
						auto it = pipelineInstance->bindings.find(materialSettings->GetTextures().size() + materialSettings->GetUniformBlocks().size() + viewerDataBinding);
						assert(it != pipelineInstance->bindings.end());

						Renderer::SetUniformBuffer(it->second, sceneData.viewer->GetViewerData());
					}

					lastPipeline = pipeline;
				}

				if (batch.material != lastMaterial)
				{
					batch.material->Apply(*pipelineInstance);

					const auto& matSettings = batch.material->GetSettings();
					std::size_t overlayBindingIndex = matSettings->GetPredefinedBindingIndex(PredefinedShaderBinding_TexOverlay);
					if (overlayBindingIndex != MaterialSettings::InvalidIndex)
					{
						auto it = pipelineInstance->bindings.find(overlayBindingIndex);
						assert(it != pipelineInstance->bindings.end());

						if (overlayTextureUnit != it->second)
						{
							overlayTextureUnit = it->second;
							lastOverlay = nullptr;
						}
					}
					else
						overlayTextureUnit = InvalidTextureUnit;

					lastMaterial = batch.material;
				}

				if (batch.overlayTexture != lastOverlay)
				{
					if (overlayTextureUnit != InvalidTextureUnit)
						Renderer::SetTexture(overlayTextureUnit, batch.overlayTexture);

					lastOverlay = batch.overlayTexture;
				}

				if (batch.material->IsScissorTestEnabled() && batch.scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(batch.scissorRect);
					lastScissorRect = batch.scissorRect;
				}

				unsigned int indexCount = batch.spriteCount * 6;
				Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, firstIndex, indexCount);
				firstIndex += indexCount;
			}
		};

		m_spriteBatches.clear();
		{
			BufferMapper<VertexBuffer> vertexMapper;
			VertexStruct_XYZ_Color_UV* vertices = nullptr;

			//Renderer::SetTextureSampler(overlayTextureUnit, batch.material->GetDiffuseSampler());

			std::size_t remainingSprite = maxSpriteCount;

			const Material* lastMaterial = nullptr;
			const Texture* lastOverlay = nullptr;
			Recti lastScissorRect = Recti(-1, -1);

			for (const BasicRenderQueue::SpriteChain& basicSprites : spriteList)
			{
				const Nz::Texture* overlayTexture = (basicSprites.overlay) ? basicSprites.overlay.Get() : m_whiteTexture.Get();
				const Nz::Recti& scissorRect = (basicSprites.scissorRect.width > 0) ? basicSprites.scissorRect : fullscreenScissorRect;

				const VertexStruct_XYZ_Color_UV* spriteVertices = basicSprites.vertices;
				std::size_t spriteCount = basicSprites.spriteCount;
				
				for (;;)
				{
					if (m_spriteBatches.empty() || basicSprites.material != lastMaterial || overlayTexture != lastOverlay || (basicSprites.material->IsScissorTestEnabled() && scissorRect != lastScissorRect))
					{
						m_spriteBatches.emplace_back();
						SpriteBatch& newBatch = m_spriteBatches.back();
						newBatch.material = basicSprites.material;
						newBatch.overlayTexture = overlayTexture;
						newBatch.scissorRect = scissorRect;
						newBatch.spriteCount = 0;

						lastMaterial = basicSprites.material;
						lastOverlay = overlayTexture;
						lastScissorRect = scissorRect;
					}

					SpriteBatch& currentBatch = m_spriteBatches.back();

					if (!vertices)
					{
						vertexMapper.Map(m_spriteBuffer, BufferAccess_DiscardAndWrite);
						vertices = static_cast<VertexStruct_XYZ_Color_UV*>(vertexMapper.GetPointer());
					}

					std::size_t processedSpriteCount = std::min(remainingSprite, spriteCount);
					std::size_t processedVertices = processedSpriteCount * 4;

					std::memcpy(vertices, spriteVertices, processedVertices * sizeof(VertexStruct_XYZ_Color_UV));
					vertices += processedVertices;
					spriteVertices += processedVertices;

					currentBatch.spriteCount += processedSpriteCount;
					spriteCount -= processedSpriteCount;

					remainingSprite -= processedSpriteCount;
					if (remainingSprite == 0)
					{
						vertexMapper.Unmap();
						vertices = nullptr;

						Draw();

						remainingSprite = maxSpriteCount;
						m_spriteBatches.clear();
					}

					if (spriteCount == 0)
						break;
				}
			}
		}

		Draw();
	}

	void ForwardRenderTechnique::UpdateLightUniforms(std::size_t firstLightIndex, std::size_t lightCount) const
	{
		static PredefinedLightData lightDataStruct = PredefinedLightData::GetOffset();

		if (lightCount > 0)
		{
			BufferMapper<UniformBuffer> mapper(m_lightData, BufferAccess_DiscardAndWrite);

			for (std::size_t i = 0; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
			{
				Nz::Int32* type = AccessByOffset<Nz::Int32>(mapper.GetPointer(), lightDataStruct.lightArray[i] + lightDataStruct.innerOffsets.type);

				if (i < lightCount)
				{
					const LightIndex& lightInfo = m_lights[firstLightIndex + i];

					Nz::Vector4f* color = AccessByOffset<Nz::Vector4f>(mapper.GetPointer(), lightDataStruct.lightArray[i] + lightDataStruct.innerOffsets.color);
					Nz::Vector2f* factors = AccessByOffset<Nz::Vector2f>(mapper.GetPointer(), lightDataStruct.lightArray[i] + lightDataStruct.innerOffsets.factor);
					Nz::Vector4f* parameters1 = AccessByOffset<Nz::Vector4f>(mapper.GetPointer(), lightDataStruct.lightArray[i] + lightDataStruct.innerOffsets.parameter1);
					Nz::Vector4f* parameters2 = AccessByOffset<Nz::Vector4f>(mapper.GetPointer(), lightDataStruct.lightArray[i] + lightDataStruct.innerOffsets.parameter2);
					Nz::Vector2f* parameters3 = AccessByOffset<Nz::Vector2f>(mapper.GetPointer(), lightDataStruct.lightArray[i] + lightDataStruct.innerOffsets.parameter3);
					Nz::Int32* shadowMapping = AccessByOffset<Nz::Int32>(mapper.GetPointer(), lightDataStruct.lightArray[i] + lightDataStruct.innerOffsets.shadowMappingFlag);

					*type = lightInfo.type;
					switch (lightInfo.type)
					{
						case LightType_Directional:
						{
							const auto& light = m_renderQueue.directionalLights[lightInfo.index];

							color->Set(light.color.r / 255.f, light.color.g / 255.f, light.color.b / 255.f, light.color.a / 255.f);
							factors->Set(light.ambientFactor, light.diffuseFactor);
							parameters1->Set(light.direction);

							*shadowMapping = 0;
							/*
							*shadowMapping = (light.shadowMap) ? 1 : 0;

							if (light.shadowMap)
							{
								unsigned int textureUnit2D = Material::GetTextureUnit(static_cast<TextureMap>(TextureMap_Shadow2D_1 + index));

								Renderer::SetTexture(textureUnit2D, light.shadowMap);
								Renderer::SetTextureSampler(textureUnit2D, s_shadowSampler);

								if (uniforms.locations.lightViewProjMatrix != -1)
									shader->SendMatrix(uniforms.locations.lightViewProjMatrix + index, light.transformMatrix);
							}*/
							break;
						}

						case LightType_Point:
						{
							const auto& light = m_renderQueue.pointLights[lightInfo.index];

							color->Set(light.color.r / 255.f, light.color.g / 255.f, light.color.b / 255.f, light.color.a / 255.f);
							factors->Set(light.ambientFactor, light.diffuseFactor);
							parameters1->Set(light.position, light.attenuation);
							parameters2->Set(0.f, 0.f, 0.f, light.invRadius);

							*shadowMapping = 0;
							/*
							*shadowMapping = (light.shadowMap) ? 1 : 0;

							if (light.shadowMap)
							{
								unsigned int textureUnit2D = Material::GetTextureUnit(static_cast<TextureMap>(TextureMap_Shadow2D_1 + index));

								Renderer::SetTexture(textureUnit2D, light.shadowMap);
								Renderer::SetTextureSampler(textureUnit2D, s_shadowSampler);

								if (uniforms.locations.lightViewProjMatrix != -1)
									shader->SendMatrix(uniforms.locations.lightViewProjMatrix + index, light.transformMatrix);
							}*/
							break;
						}

						case LightType_Spot:
						{
							const auto& light = m_renderQueue.spotLights[lightInfo.index];

							color->Set(light.color.r / 255.f, light.color.g / 255.f, light.color.b / 255.f, light.color.a / 255.f);
							factors->Set(light.ambientFactor, light.diffuseFactor);
							parameters1->Set(light.position, light.attenuation);
							parameters2->Set(light.direction, light.invRadius);
							parameters3->Set(light.innerAngleCosine, light.outerAngleCosine);

							*shadowMapping = 0;
							/*
							*shadowMapping = (light.shadowMap) ? 1 : 0;

							if (light.shadowMap)
							{
								unsigned int textureUnit2D = Material::GetTextureUnit(static_cast<TextureMap>(TextureMap_Shadow2D_1 + index));

								Renderer::SetTexture(textureUnit2D, light.shadowMap);
								Renderer::SetTextureSampler(textureUnit2D, s_shadowSampler);

								if (uniforms.locations.lightViewProjMatrix != -1)
									shader->SendMatrix(uniforms.locations.lightViewProjMatrix + index, light.transformMatrix);
							}*/
							break;
						}
					}
				}
				else
					*type = -1;
			}
		}
	}

	IndexBuffer ForwardRenderTechnique::s_quadIndexBuffer;
	TextureSampler ForwardRenderTechnique::s_reflectionSampler;
	TextureSampler ForwardRenderTechnique::s_shadowSampler;
	VertexBuffer ForwardRenderTechnique::s_quadVertexBuffer;
	VertexDeclaration ForwardRenderTechnique::s_billboardInstanceDeclaration;
	VertexDeclaration ForwardRenderTechnique::s_billboardVertexDeclaration;
}
