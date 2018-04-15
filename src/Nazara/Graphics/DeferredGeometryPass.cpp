// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredGeometryPass.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/DeferredProxyRenderQueue.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/SceneData.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
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
	* \class Nz::DeferredGeometryPass
	* \brief Graphics class that represents the pass for geometries in deferred rendering
	*/

	/*!
	* \brief Constructs a DeferredGeometryPass object by default
	*/

	DeferredGeometryPass::DeferredGeometryPass() :
	m_vertexBuffer(BufferType_Vertex)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		m_whiteTexture = Nz::TextureLibrary::Get("White2D");

		m_vertexBuffer.Create(s_vertexBufferSize, DataStorage_Hardware, BufferUsage_Dynamic);

		m_billboardPointBuffer.Reset(&s_billboardVertexDeclaration, &m_vertexBuffer);
		m_spriteBuffer.Reset(VertexDeclaration::Get(VertexLayout_XYZ_Color_UV), &m_vertexBuffer);

		m_clearShader = ShaderLibrary::Get("DeferredGBufferClear");
		m_clearStates.depthBuffer = true;
		m_clearStates.faceCulling = true;
		m_clearStates.stencilTest = true;
		m_clearStates.depthFunc = RendererComparison_Always;
		m_clearStates.stencilCompare.front = RendererComparison_Always;
		m_clearStates.stencilPass.front = StencilOperation_Zero;
	}

	DeferredGeometryPass::~DeferredGeometryPass() = default;

	/*!
	* \brief Processes the work on the data while working with textures
	* \return false
	*
	* \param sceneData Data for the scene
	* \param firstWorkTexture Index of the first texture to work with
	* \param firstWorkTexture Index of the second texture to work with
	*/

	bool DeferredGeometryPass::Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");
		NazaraUnused(firstWorkTexture);
		NazaraUnused(secondWorkTexture);

		bool instancingEnabled = m_deferredTechnique->IsInstancingEnabled();

		m_GBufferRTT->SetColorTargets({0, 1, 2}); // G-Buffer
		Renderer::SetTarget(m_GBufferRTT);
		Renderer::SetScissorRect(Recti(0, 0, m_dimensions.x, m_dimensions.y));
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetRenderStates(m_clearStates);
		Renderer::SetShader(m_clearShader);
		Renderer::DrawFullscreenQuad();


		Renderer::SetMatrix(MatrixType_Projection, sceneData.viewer->GetProjectionMatrix());
		Renderer::SetMatrix(MatrixType_View, sceneData.viewer->GetViewMatrix());

		BasicRenderQueue& renderQueue = *m_renderQueue->GetDeferredRenderQueue();

		renderQueue.Sort(sceneData.viewer);

		if (!renderQueue.models.empty())
			DrawModels(sceneData, renderQueue, renderQueue.models);

		if (!renderQueue.basicSprites.empty())
			DrawSprites(sceneData, renderQueue, renderQueue.basicSprites);

		if (!renderQueue.billboards.empty())
			DrawBillboards(sceneData, renderQueue, renderQueue.billboards);

		if (!renderQueue.depthSortedModels.empty())
			DrawModels(sceneData, renderQueue, renderQueue.depthSortedModels);

		if (!renderQueue.depthSortedSprites.empty())
			DrawSprites(sceneData, renderQueue, renderQueue.depthSortedSprites);

		if (!renderQueue.depthSortedBillboards.empty())
			DrawBillboards(sceneData, renderQueue, renderQueue.depthSortedBillboards);

		return false; // We only fill the G-Buffer, the work texture are unchanged
	}

	/*!
	* \brief Resizes the texture sizes
	* \return true If successful
	*
	* \param dimensions Dimensions for the compute texture
	*/

	bool DeferredGeometryPass::Resize(const Vector2ui& dimensions)
	{
		DeferredRenderPass::Resize(dimensions);

		/*
		G-Buffer:
		Texture0: Diffuse Color + Flags
		Texture1: Encoded normal
		Texture2: Specular value + Shininess
		Texture3: N/A
		*/

		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);

			unsigned int width = dimensions.x;
			unsigned int height = dimensions.y;

			m_depthStencilTexture->Create(ImageType_2D, PixelFormatType_Depth24Stencil8, width, height);

			m_GBuffer[0]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height); // Texture 0 : Diffuse Color + Specular
			m_GBuffer[1]->Create(ImageType_2D, PixelFormatType_RG16F, width, height); // Texture 1 : Encoded normal
			m_GBuffer[2]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height); // Texture 2 : Depth (24bits) + Shininess

			m_GBufferRTT->Create(true);

			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 0, m_GBuffer[0]);
			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 1, m_GBuffer[1]);
			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 2, m_GBuffer[2]);

			// Texture 3 : Emission map ?

			m_GBufferRTT->AttachTexture(AttachmentPoint_DepthStencil, 0, m_depthStencilTexture);

			m_GBufferRTT->Unlock();

			m_workRTT->Create(true);

			for (unsigned int i = 0; i < 2; ++i)
			{
				m_workTextures[i]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height);
				m_workRTT->AttachTexture(AttachmentPoint_Color, i, m_workTextures[i]);
			}

			m_workRTT->AttachTexture(AttachmentPoint_DepthStencil, 0, m_depthStencilTexture);

			m_workRTT->Unlock();

			if (!m_workRTT->IsComplete() || !m_GBufferRTT->IsComplete())
			{
				NazaraError("Incomplete RTT");
				return false;
			}

			return true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to create G-Buffer RTT: " + String(e.what()));
			return false;
		}
	}
	
	void DeferredGeometryPass::DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::Billboard>& billboards) const
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
					pipelineInstance = &billboard.material->GetPipeline()->Apply(ShaderFlags_Billboard | ShaderFlags_Deferred | ShaderFlags_Instancing | ShaderFlags_VertexColor);

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
	
	void DeferredGeometryPass::DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::BillboardChain>& billboards) const
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
					pipelineInstance = &billboard.material->GetPipeline()->Apply(ShaderFlags_Billboard | ShaderFlags_Deferred | ShaderFlags_Instancing | ShaderFlags_VertexColor);

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

	void DeferredGeometryPass::DrawModels(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const Nz::RenderQueue<Nz::BasicRenderQueue::Model>& models) const
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
				pipelineInstance = &model.material->GetPipeline()->Apply(ShaderFlags_Deferred);

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

			Renderer::SetMatrix(MatrixType_World, model.matrix);
			drawFunc(model.meshData.primitiveMode, 0, indexCount);
		}
	}

	void DeferredGeometryPass::DrawSprites(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::SpriteChain>& spriteList) const
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
					pipelineInstance = &basicSprites.material->GetPipeline()->Apply(ShaderFlags_Deferred | ShaderFlags_TextureOverlay | ShaderFlags_VertexColor);

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

				const Nz::Texture* overlayTexture = (basicSprites.overlay) ? basicSprites.overlay.Get() : m_whiteTexture.Get();
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

	const DeferredGeometryPass::ShaderUniforms* DeferredGeometryPass::GetShaderUniforms(const Shader* shader) const
	{
		auto it = m_shaderUniforms.find(shader);
		if (it == m_shaderUniforms.end())
		{
			ShaderUniforms uniforms;
			uniforms.shaderReleaseSlot.Connect(shader->OnShaderRelease, this, &DeferredGeometryPass::OnShaderInvalidated);
			uniforms.shaderUniformInvalidatedSlot.Connect(shader->OnShaderUniformInvalidated, this, &DeferredGeometryPass::OnShaderInvalidated);

			uniforms.eyePosition = shader->GetUniformLocation("EyePosition");
			uniforms.sceneAmbient = shader->GetUniformLocation("SceneAmbient");
			uniforms.textureOverlay = shader->GetUniformLocation("TextureOverlay");

			it = m_shaderUniforms.emplace(shader, std::move(uniforms)).first;
		}

		return &it->second;
	}

	/*!
	* \brief Handle the invalidation of a shader
	*
	* \param shader Shader being invalidated
	*/

	void DeferredGeometryPass::OnShaderInvalidated(const Shader* shader) const
	{
		m_shaderUniforms.erase(shader);
	}

	bool DeferredGeometryPass::Initialize()
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
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Color, ComponentType_Color, NazaraOffsetOf(BillboardPoint, color));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(BillboardPoint, position));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(BillboardPoint, uv));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Userdata0, ComponentType_Float4, NazaraOffsetOf(BillboardPoint, size)); // Includes sincos

			// Declaration used when rendering the billboards with intancing
			// The main advantage is the direct copy (std::memcpy) of data in the RenderQueue to the GPU buffer
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData0, ComponentType_Float3, NazaraOffsetOf(BasicRenderQueue::BillboardData, center));
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData1, ComponentType_Float4, NazaraOffsetOf(BasicRenderQueue::BillboardData, size)); // Englobe sincos
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData2, ComponentType_Color, NazaraOffsetOf(BasicRenderQueue::BillboardData, color));
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialise: " + String(e.what()));
			return false;
		}

		return true;
	}

	void DeferredGeometryPass::Uninitialize()
	{
		s_quadIndexBuffer.Reset();
		s_quadVertexBuffer.Reset();
	}

	IndexBuffer DeferredGeometryPass::s_quadIndexBuffer;
	VertexBuffer DeferredGeometryPass::s_quadVertexBuffer;
	VertexDeclaration DeferredGeometryPass::s_billboardInstanceDeclaration;
	VertexDeclaration DeferredGeometryPass::s_billboardVertexDeclaration;
}
