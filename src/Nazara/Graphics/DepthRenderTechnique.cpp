// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Drawable.hpp>
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

		unsigned int s_maxQuads = std::numeric_limits<UInt16>::max() / 6;
		unsigned int s_vertexBufferSize = 4 * 1024 * 1024; // 4 MiB
	}

	/*!
	* \ingroup graphics
	* \class Nz::DepthRenderTechnique
	* \brief Graphics class that represents the technique used in depth rendering
	*/

	/*!
	* \brief Constructs a DepthRenderTechnique object by default
	*/

	DepthRenderTechnique::DepthRenderTechnique() :
		m_vertexBuffer(BufferType_Vertex)
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

	void DepthRenderTechnique::Clear(const SceneData& /*sceneData*/) const
	{
		Renderer::Enable(RendererParameter_DepthBuffer, true);
		Renderer::Enable(RendererParameter_DepthWrite, true);
		Renderer::Clear(RendererBuffer_Depth);

		// Just in case the background does render depth
		//if (sceneData.background)
		//	sceneData.background->Draw(sceneData.viewer);
	}

	/*!
	* \brief Draws the data of the scene
	* \return true If successful
	*
	* \param sceneData Data of the scene
	*/

	bool DepthRenderTechnique::Draw(const SceneData& sceneData) const
	{
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
	* \brief Gets the render queue
	* \return Pointer to the render queue
	*/

	AbstractRenderQueue* DepthRenderTechnique::GetRenderQueue()
	{
		return &m_renderQueue;
	}

	/*!
	* \brief Gets the type of the current technique
	* \return Type of the render technique
	*/

	RenderTechniqueType DepthRenderTechnique::GetType() const
	{
		return RenderTechniqueType_Depth;
	}

	/*!
	* \brief Initializes the depth render technique
	* \return true If successful
	*
	* \remark Produces a NazaraError if one shader creation failed
	*/

	bool DepthRenderTechnique::Initialize()
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

			mapper.Unmap(); // Inutile de garder le buffer ouvert plus longtemps

			// Quad buffer (utilisé pour l'instancing de billboard et de sprites)
			//Note: Les UV sont calculés dans le shader
			s_quadVertexBuffer.Reset(VertexDeclaration::Get(VertexLayout_XY), 4, DataStorage_Hardware, 0);

			float vertices[2 * 4] = {
			   -0.5f, -0.5f,
			    0.5f, -0.5f,
			   -0.5f, 0.5f,
			    0.5f, 0.5f,
			};

			s_quadVertexBuffer.FillRaw(vertices, 0, sizeof(vertices));

			// Déclaration lors du rendu des billboards par sommet
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Color, ComponentType_Color, NazaraOffsetOf(BillboardPoint, color));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(BillboardPoint, position));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(BillboardPoint, uv));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Userdata0, ComponentType_Float4, NazaraOffsetOf(BillboardPoint, size)); // Englobe sincos

			// Declaration utilisée lors du rendu des billboards par instancing
			// L'avantage ici est la copie directe (std::memcpy) des données de la RenderQueue vers le buffer GPU
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

	/*!
	* \brief Uninitializes the depth render technique
	*/

	void DepthRenderTechnique::Uninitialize()
	{
		s_quadIndexBuffer.Reset();
		s_quadVertexBuffer.Reset();
	}
	
	void DepthRenderTechnique::DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::Billboard>& billboards) const
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
			if (billboard.material != lastMaterial || (billboard.material->IsScissorTestEnabled() && billboard.scissorRect != lastScissorRect))
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

						lastShader = shader;
					}

					lastPipeline = pipeline;
				}

				if (lastMaterial != billboard.material)
				{
					billboard.material->Apply(*pipelineInstance);
					lastMaterial = billboard.material;
				}

				if (billboard.material->IsScissorTestEnabled() && billboard.scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect((billboard.scissorRect.width > 0) ? billboard.scissorRect : fullscreenScissorRect);
					lastScissorRect = billboard.scissorRect;
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
	
	void DepthRenderTechnique::DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::BillboardChain>& billboards) const
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
			if (billboard.material != lastMaterial || (billboard.material->IsScissorTestEnabled() && billboard.scissorRect != lastScissorRect))
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

						lastShader = shader;
					}

					lastPipeline = pipeline;
				}

				if (lastMaterial != billboard.material)
				{
					billboard.material->Apply(*pipelineInstance);
					lastMaterial = billboard.material;
				}

				if (billboard.material->IsScissorTestEnabled() && billboard.scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect((billboard.scissorRect.width > 0) ? billboard.scissorRect : fullscreenScissorRect);
					lastScissorRect = billboard.scissorRect;
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

	void DepthRenderTechnique::DrawCustomDrawables(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::CustomDrawable>& customDrawables) const
	{
		for (const BasicRenderQueue::CustomDrawable& customDrawable : customDrawables)
			customDrawable.drawable->Draw();
	}

	void DepthRenderTechnique::DrawModels(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const Nz::RenderQueue<Nz::BasicRenderQueue::Model>& models) const
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

					lastShader = shader;
				}

				lastPipeline = pipeline;
			}

			if (lastMaterial != model.material)
			{
				model.material->Apply(*pipelineInstance);
				lastMaterial = model.material;
			}

			if (model.material->IsScissorTestEnabled() && model.scissorRect != lastScissorRect)
			{
				Renderer::SetScissorRect((model.scissorRect.width > 0) ? model.scissorRect : fullscreenScissorRect);
				lastScissorRect = model.scissorRect;
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

	void DepthRenderTechnique::DrawSprites(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::SpriteChain>& spriteList) const
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
			if (basicSprites.material != lastMaterial || basicSprites.overlay != lastOverlay || (basicSprites.material->IsScissorTestEnabled() && basicSprites.scissorRect != lastScissorRect))
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

				if (basicSprites.material->IsScissorTestEnabled() && basicSprites.scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect((basicSprites.scissorRect.width > 0) ? basicSprites.scissorRect : fullscreenScissorRect);
					lastScissorRect = basicSprites.scissorRect;
				}
			}

			m_spriteChains.emplace_back(basicSprites.vertices, basicSprites.spriteCount);
		}

		Commit();
	}

	/*!
	* \brief Gets the shader uniforms
	* \return Uniforms of the shader
	*
	* \param shader Shader to get uniforms from
	*/

	const DepthRenderTechnique::ShaderUniforms* DepthRenderTechnique::GetShaderUniforms(const Shader* shader) const
	{
		auto it = m_shaderUniforms.find(shader);
		if (it == m_shaderUniforms.end())
		{
			ShaderUniforms uniforms;
			uniforms.shaderReleaseSlot.Connect(shader->OnShaderRelease, this, &DepthRenderTechnique::OnShaderInvalidated);
			uniforms.shaderUniformInvalidatedSlot.Connect(shader->OnShaderUniformInvalidated, this, &DepthRenderTechnique::OnShaderInvalidated);

			uniforms.sceneAmbient   = shader->GetUniformLocation("SceneAmbient");
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

	void DepthRenderTechnique::OnShaderInvalidated(const Shader* shader) const
	{
		m_shaderUniforms.erase(shader);
	}

	IndexBuffer DepthRenderTechnique::s_quadIndexBuffer;
	VertexBuffer DepthRenderTechnique::s_quadVertexBuffer;
	VertexDeclaration DepthRenderTechnique::s_billboardInstanceDeclaration;
	VertexDeclaration DepthRenderTechnique::s_billboardVertexDeclaration;
}
