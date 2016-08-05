// Copyright (C) 2015 Jérôme Leclercq
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
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <memory>
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

		std::array<UInt8, 4> whitePixel = {255, 255, 255, 255};
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

		for (auto& pair : m_renderQueue.layers)
		{
			ForwardRenderQueue::Layer& layer = pair.second;

			if (!layer.opaqueModels.empty())
				DrawOpaqueModels(sceneData, layer);

			if (!layer.transparentModels.empty())
				DrawTransparentModels(sceneData, layer);

			if (!layer.basicSprites.empty())
				DrawBasicSprites(sceneData, layer);

			if (!layer.billboards.empty())
				DrawBillboards(sceneData, layer);

			for (const Drawable* drawable : layer.otherDrawables)
				drawable->Draw();
		}

		return true;
	}

	/*!
	* \brief Gets the maximum number of lights available per pass per object
	* \return Maximum number of light simulatenously per object
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

			s_quadIndexBuffer.Reset(false, s_maxQuads * 6, DataStorage_Hardware, BufferUsage_Static);

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
			s_quadVertexBuffer.Reset(VertexDeclaration::Get(VertexLayout_XY), 4, DataStorage_Hardware, BufferUsage_Static);

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
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData0, ComponentType_Float3, NazaraOffsetOf(ForwardRenderQueue::BillboardData, center));
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData1, ComponentType_Float4, NazaraOffsetOf(ForwardRenderQueue::BillboardData, size)); // Englobe sincos
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData2, ComponentType_Color,  NazaraOffsetOf(ForwardRenderQueue::BillboardData, color));

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

	/*!
	* \brief Draws basic sprites
	*
	* \param sceneData Data of the scene
	* \param layer Layer of the rendering
	*
	* \remark Produces a NazaraAssert is viewer is invalid
	*/

	void ForwardRenderTechnique::DrawBasicSprites(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;

		Renderer::SetIndexBuffer(&s_quadIndexBuffer);
		Renderer::SetMatrix(MatrixType_World, Matrix4f::Identity());
		Renderer::SetVertexBuffer(&m_spriteBuffer);

		for (auto& pipelinePair : layer.basicSprites)
		{
			const MaterialPipeline* pipeline = pipelinePair.first;
			auto& pipelineEntry = pipelinePair.second;

			if (pipelineEntry.enabled)
			{
				const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply(ShaderFlags_TextureOverlay | ShaderFlags_VertexColor);

				const Shader* shader = pipelineInstance.uberInstance->GetShader();

				// Uniforms are conserved in our program, there's no point to send them back until they change
				if (shader != lastShader)
				{
					// Index of uniforms in the shader
					shaderUniforms = GetShaderUniforms(shader);

					// Ambiant color of the scene
					shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
					// Position of the camera
					shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

					lastShader = shader;
				}

				for (auto& materialPair : pipelineEntry.materialMap)
				{
					const Material* material = materialPair.first;
					auto& matEntry = materialPair.second;

					if (matEntry.enabled)
					{
						UInt8 overlayUnit;
						material->Apply(pipelineInstance, 0, &overlayUnit);
						overlayUnit++;

						shader->SendInteger(shaderUniforms->textureOverlay, overlayUnit);

						Renderer::SetTextureSampler(overlayUnit, material->GetDiffuseSampler());

						auto& overlayMap = matEntry.overlayMap;
						for (auto& overlayIt : overlayMap)
						{
							const Texture* overlay = overlayIt.first;
							auto& spriteChainVector = overlayIt.second.spriteChains;

							unsigned int spriteChainCount = spriteChainVector.size();
							if (spriteChainCount > 0)
							{
								Renderer::SetTexture(overlayUnit, (overlay) ? overlay : &m_whiteTexture);

								unsigned int spriteChain = 0; // Which chain of sprites are we treating
								unsigned int spriteChainOffset = 0; // Where was the last offset where we stopped in the last chain

								do
								{
									// We open the buffer in writing mode
									BufferMapper<VertexBuffer> vertexMapper(m_spriteBuffer, BufferAccess_DiscardAndWrite);
									VertexStruct_XYZ_Color_UV* vertices = static_cast<VertexStruct_XYZ_Color_UV*>(vertexMapper.GetPointer());

									unsigned int spriteCount = 0;
									unsigned int maxSpriteCount = std::min(s_maxQuads, m_spriteBuffer.GetVertexCount() / 4);

									do
									{
										ForwardRenderQueue::SpriteChain_XYZ_Color_UV& currentChain = spriteChainVector[spriteChain];
										unsigned int count = std::min(maxSpriteCount - spriteCount, currentChain.spriteCount - spriteChainOffset);

										std::memcpy(vertices, currentChain.vertices + spriteChainOffset * 4, 4 * count * sizeof(VertexStruct_XYZ_Color_UV));
										vertices += count * 4;

										spriteCount += count;
										spriteChainOffset += count;

										// Have we treated the entire chain ?
										if (spriteChainOffset == currentChain.spriteCount)
										{
											spriteChain++;
											spriteChainOffset = 0;
										}
									} while (spriteCount < maxSpriteCount && spriteChain < spriteChainCount);

									vertexMapper.Unmap();

									Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, 0, spriteCount * 6);
								} while (spriteChain < spriteChainCount);

								spriteChainVector.clear();
							}
						}

						// We set it back to zero
						matEntry.enabled = false;
					}
				}
				pipelineEntry.enabled = false;
			}
		}
	}

	/*!
	* \brief Draws billboards
	*
	* \param sceneData Data of the scene
	* \param layer Layer of the rendering
	*
	* \remark Produces a NazaraAssert is viewer is invalid
	*/

	void ForwardRenderTechnique::DrawBillboards(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;

		if (Renderer::HasCapability(RendererCap_Instancing))
		{
			VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
			instanceBuffer->SetVertexDeclaration(&s_billboardInstanceDeclaration);

			Renderer::SetVertexBuffer(&s_quadVertexBuffer);

			for (auto& pipelinePair : layer.billboards)
			{
				const MaterialPipeline* pipeline = pipelinePair.first;
				auto& pipelineEntry = pipelinePair.second;

				if (pipelineEntry.enabled)
				{
					const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply(ShaderFlags_Billboard | ShaderFlags_Instancing | ShaderFlags_VertexColor);

					const Shader* shader = pipelineInstance.uberInstance->GetShader();

					// Uniforms are conserved in our program, there's no point to send them back until they change
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambiant color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
						// Position of the camera
						shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

						lastShader = shader;
					}

					for (auto& matIt : pipelinePair.second.materialMap)
					{
						const Material* material = matIt.first;
						auto& entry = matIt.second;
						auto& billboardVector = entry.billboards;

						unsigned int billboardCount = billboardVector.size();
						if (billboardCount > 0)
						{
							// We begin to apply the material (and get the shader activated doing so)
							material->Apply(pipelineInstance);

							const ForwardRenderQueue::BillboardData* data = &billboardVector[0];
							unsigned int maxBillboardPerDraw = instanceBuffer->GetVertexCount();
							do
							{
								unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboardPerDraw);
								billboardCount -= renderedBillboardCount;

								instanceBuffer->Fill(data, 0, renderedBillboardCount, true);
								data += renderedBillboardCount;

								Renderer::DrawPrimitivesInstanced(renderedBillboardCount, PrimitiveMode_TriangleStrip, 0, 4);
							}
							while (billboardCount > 0);

							billboardVector.clear();
						}
					}
				}
			}
		}
		else
		{
			Renderer::SetIndexBuffer(&s_quadIndexBuffer);
			Renderer::SetVertexBuffer(&m_billboardPointBuffer);

			for (auto& pipelinePair : layer.billboards)
			{
				const MaterialPipeline* pipeline = pipelinePair.first;
				auto& pipelineEntry = pipelinePair.second;

				if (pipelineEntry.enabled)
				{
					const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply(ShaderFlags_Billboard | ShaderFlags_VertexColor);

					const Shader* shader = pipelineInstance.uberInstance->GetShader();

					// Uniforms are conserved in our program, there's no point to send them back until they change
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambiant color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
						// Position of the camera
						shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

						lastShader = shader;
					}

					for (auto& matIt : pipelinePair.second.materialMap)
					{
						const Material* material = matIt.first;
						auto& entry = matIt.second;
						auto& billboardVector = entry.billboards;

						const ForwardRenderQueue::BillboardData* data = &billboardVector[0];
						unsigned int maxBillboardPerDraw = std::min(s_maxQuads, m_billboardPointBuffer.GetVertexCount() / 4);

						unsigned int billboardCount = billboardVector.size();
						do
						{
							unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboardPerDraw);
							billboardCount -= renderedBillboardCount;

							BufferMapper<VertexBuffer> vertexMapper(m_billboardPointBuffer, BufferAccess_DiscardAndWrite, 0, renderedBillboardCount * 4);
							BillboardPoint* vertices = static_cast<BillboardPoint*>(vertexMapper.GetPointer());

							for (unsigned int i = 0; i < renderedBillboardCount; ++i)
							{
								const ForwardRenderQueue::BillboardData& billboard = *data++;

								vertices->color = billboard.color;
								vertices->position = billboard.center;
								vertices->sinCos = billboard.sinCos;
								vertices->size = billboard.size;
								vertices->uv.Set(0.f, 1.f);
								vertices++;

								vertices->color = billboard.color;
								vertices->position = billboard.center;
								vertices->sinCos = billboard.sinCos;
								vertices->size = billboard.size;
								vertices->uv.Set(1.f, 1.f);
								vertices++;

								vertices->color = billboard.color;
								vertices->position = billboard.center;
								vertices->sinCos = billboard.sinCos;
								vertices->size = billboard.size;
								vertices->uv.Set(0.f, 0.f);
								vertices++;

								vertices->color = billboard.color;
								vertices->position = billboard.center;
								vertices->sinCos = billboard.sinCos;
								vertices->size = billboard.size;
								vertices->uv.Set(1.f, 0.f);
								vertices++;
							}

							vertexMapper.Unmap();

							Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, 0, renderedBillboardCount * 6);
						}
						while (billboardCount > 0);

						billboardVector.clear();
					}
				}
			}
		}
	}

	/*!
	* \brief Draws opaques models
	*
	* \param sceneData Data of the scene
	* \param layer Layer of the rendering
	*
	* \remark Produces a NazaraAssert is viewer is invalid
	*/

	void ForwardRenderTechnique::DrawOpaqueModels(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;

		for (auto& pipelinePair : layer.opaqueModels)
		{
			const MaterialPipeline* pipeline = pipelinePair.first;
			auto& pipelineEntry = pipelinePair.second;

			if (pipelineEntry.maxInstanceCount > 0)
			{
				bool instancing = (pipelineEntry.maxInstanceCount > NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT);
				const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply((instancing) ? ShaderFlags_Instancing : 0);

				const Shader* shader = pipelineInstance.uberInstance->GetShader();

				// Uniforms are conserved in our program, there's no point to send them back until they change
				if (shader != lastShader)
				{
					// Index of uniforms in the shader
					shaderUniforms = GetShaderUniforms(shader);

					// Ambiant color of the scene
					shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
					// Position of the camera
					shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

					lastShader = shader;
				}

				for (auto& materialPair : pipelineEntry.materialMap)
				{
					const Material* material = materialPair.first;
					auto& matEntry = materialPair.second;

					if (matEntry.enabled)
					{
						UInt8 freeTextureUnit;
						material->Apply(pipelineInstance, 0, &freeTextureUnit);

						ForwardRenderQueue::MeshInstanceContainer& meshInstances = matEntry.meshMap;

						// Meshes
						for (auto& meshIt : meshInstances)
						{
							const MeshData& meshData = meshIt.first;
							auto& meshEntry = meshIt.second;

							const Spheref& squaredBoundingSphere = meshEntry.squaredBoundingSphere;
							std::vector<Matrix4f>& instances = meshEntry.instances;

							if (!instances.empty())
							{
								const IndexBuffer* indexBuffer = meshData.indexBuffer;
								const VertexBuffer* vertexBuffer = meshData.vertexBuffer;

								// Handle draw call before rendering loop
								Renderer::DrawCall drawFunc;
								Renderer::DrawCallInstanced instancedDrawFunc;
								unsigned int indexCount;

								if (indexBuffer)
								{
									drawFunc = Renderer::DrawIndexedPrimitives;
									instancedDrawFunc = Renderer::DrawIndexedPrimitivesInstanced;
									indexCount = indexBuffer->GetIndexCount();
								}
								else
								{
									drawFunc = Renderer::DrawPrimitives;
									instancedDrawFunc = Renderer::DrawPrimitivesInstanced;
									indexCount = vertexBuffer->GetVertexCount();
								}

								Renderer::SetIndexBuffer(indexBuffer);
								Renderer::SetVertexBuffer(vertexBuffer);

								if (instancing)
								{
									// We compute the number of instances that we will be able to draw this time (depending on the instancing buffer size)
									VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
									instanceBuffer->SetVertexDeclaration(VertexDeclaration::Get(VertexLayout_Matrix4));

									// With instancing, impossible to select the lights for each object
									// So, it's only activated for directional lights
									unsigned int lightCount = m_renderQueue.directionalLights.size();
									unsigned int lightIndex = 0;
									RendererComparison oldDepthFunc = Renderer::GetDepthFunc();

									unsigned int passCount = (lightCount == 0) ? 1 : (lightCount - 1) / NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS + 1;
									for (unsigned int pass = 0; pass < passCount; ++pass)
									{
										if (shaderUniforms->hasLightUniforms)
										{
											unsigned int renderedLightCount = std::min(lightCount, NazaraSuffixMacro(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, U));
											lightCount -= renderedLightCount;

											if (pass == 1)
											{
												// To add the result of light computations
												// We won't interfeer with materials parameters because we only render opaques objects
												// (A.K.A., without blending)
												// About the depth function, it must be applied only the first time
												Renderer::Enable(RendererParameter_Blend, true);
												Renderer::SetBlendFunc(BlendFunc_One, BlendFunc_One);
												Renderer::SetDepthFunc(RendererComparison_Equal);
											}

											// Sends the uniforms
											for (unsigned int i = 0; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
												SendLightUniforms(shader, shaderUniforms->lightUniforms, lightIndex++, shaderUniforms->lightOffset * i, freeTextureUnit + i);
										}

										const Matrix4f* instanceMatrices = &instances[0];
										unsigned int instanceCount = instances.size();
										unsigned int maxInstanceCount = instanceBuffer->GetVertexCount(); // Maximum number of instance in one batch

										while (instanceCount > 0)
										{
											// We compute the number of instances that we will be able to draw this time (depending on the instancing buffer size)
											unsigned int renderedInstanceCount = std::min(instanceCount, maxInstanceCount);
											instanceCount -= renderedInstanceCount;

											// We fill the instancing buffer with our world matrices
											instanceBuffer->Fill(instanceMatrices, 0, renderedInstanceCount, true);
											instanceMatrices += renderedInstanceCount;

											// And we draw
											instancedDrawFunc(renderedInstanceCount, meshData.primitiveMode, 0, indexCount);
										}
									}

									// We don't forget to disable the blending to avoid to interferering with the rest of the rendering
									Renderer::Enable(RendererParameter_Blend, false);
									Renderer::SetDepthFunc(oldDepthFunc);
								}
								else
								{
									if (shaderUniforms->hasLightUniforms)
									{
										for (const Matrix4f& matrix : instances)
										{
											// Choose the lights depending on an object position and apparent radius
											ChooseLights(Spheref(matrix.GetTranslation() + squaredBoundingSphere.GetPosition(), squaredBoundingSphere.radius));

											unsigned int lightCount = m_lights.size();

											Renderer::SetMatrix(MatrixType_World, matrix);
											unsigned int lightIndex = 0;
											RendererComparison oldDepthFunc = Renderer::GetDepthFunc(); // In the case where we have to change it

											unsigned int passCount = (lightCount == 0) ? 1 : (lightCount - 1) / NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS + 1;
											for (unsigned int pass = 0; pass < passCount; ++pass)
											{
												lightCount -= std::min(lightCount, NazaraSuffixMacro(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, U));

												if (pass == 1)
												{
													// To add the result of light computations
													// We won't interfeer with materials parameters because we only render opaques objects
													// (A.K.A., without blending)
													// About the depth function, it must be applied only the first time
													Renderer::Enable(RendererParameter_Blend, true);
													Renderer::SetBlendFunc(BlendFunc_One, BlendFunc_One);
													Renderer::SetDepthFunc(RendererComparison_Equal);
												}

												// Sends the light uniforms to the shader
												for (unsigned int i = 0; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
													SendLightUniforms(shader, shaderUniforms->lightUniforms, lightIndex++, shaderUniforms->lightOffset*i, freeTextureUnit + i);

												// And we draw
												drawFunc(meshData.primitiveMode, 0, indexCount);
											}

											Renderer::Enable(RendererParameter_Blend, false);
											Renderer::SetDepthFunc(oldDepthFunc);
										}
									}
									else
									{
										// Without instancing, we must do a draw call for each instance
										// This may be faster than instancing under a certain number
										// Due to the time to modify the instancing buffer
										for (const Matrix4f& matrix : instances)
										{
											Renderer::SetMatrix(MatrixType_World, matrix);
											drawFunc(meshData.primitiveMode, 0, indexCount);
										}
									}
								}
								instances.clear();
							}
						}

						matEntry.enabled = false;
					}
				}

				pipelineEntry.maxInstanceCount = 0;
			}
		}
	}

	/*!
	* \brief Draws transparent models
	*
	* \param sceneData Data of the scene
	* \param layer Layer of the rendering
	*
	* \remark Produces a NazaraAssert is viewer is invalid
	*/

	void ForwardRenderTechnique::DrawTransparentModels(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		const MaterialPipeline* lastPipeline = nullptr;
		const MaterialPipeline::Instance* pipelineInstance = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		unsigned int lightCount = 0;

		for (unsigned int index : layer.transparentModels)
		{
			const ForwardRenderQueue::TransparentModelData& modelData = layer.transparentModelData[index];

			// Material
			const Material* material = modelData.material;

			const MaterialPipeline* pipeline = material->GetPipeline();
			if (pipeline != lastPipeline)
			{
				pipelineInstance = &pipeline->Apply();
				lastPipeline = pipeline;
			}

			// We begin to apply the material (and get the shader activated doing so)
			UInt8 freeTextureUnit;
			const Shader* shader = material->Apply(*pipelineInstance, 0, &freeTextureUnit);

			// Uniforms are conserved in our program, there's no point to send them back until they change
			if (shader != lastShader)
			{
				// Index of uniforms in the shader
				shaderUniforms = GetShaderUniforms(shader);

				// Ambiant color of the scene
				shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
				// Position of the camera
				shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

				// We send the directional lights if there is one (same for all)
				if (shaderUniforms->hasLightUniforms)
				{
					lightCount = std::min(m_renderQueue.directionalLights.size(), static_cast<decltype(m_renderQueue.directionalLights.size())>(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS));

					for (unsigned int i = 0; i < lightCount; ++i)
						SendLightUniforms(shader, shaderUniforms->lightUniforms, i, shaderUniforms->lightOffset * i, freeTextureUnit++);
				}

				lastShader = shader;
			}

			// Mesh
			const Matrix4f& matrix = modelData.transformMatrix;
			const MeshData& meshData = modelData.meshData;

			const IndexBuffer* indexBuffer = meshData.indexBuffer;
			const VertexBuffer* vertexBuffer = meshData.vertexBuffer;

			// Handle draw call before the rendering loop
			Renderer::DrawCall drawFunc;
			unsigned int indexCount;

			if (indexBuffer)
			{
				drawFunc = Renderer::DrawIndexedPrimitives;
				indexCount = indexBuffer->GetIndexCount();
			}
			else
			{
				drawFunc = Renderer::DrawPrimitives;
				indexCount = vertexBuffer->GetVertexCount();
			}

			Renderer::SetIndexBuffer(indexBuffer);
			Renderer::SetVertexBuffer(vertexBuffer);

			if (shaderUniforms->hasLightUniforms && lightCount < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS)
			{
				// Compute the closest lights
				Vector3f position = matrix.GetTranslation() + modelData.squaredBoundingSphere.GetPosition();
				float radius = modelData.squaredBoundingSphere.radius;
				ChooseLights(Spheref(position, radius), false);

				for (unsigned int i = lightCount; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
					SendLightUniforms(shader, shaderUniforms->lightUniforms, i, shaderUniforms->lightOffset*i, freeTextureUnit++);
			}

			Renderer::SetMatrix(MatrixType_World, matrix);
			drawFunc(meshData.primitiveMode, 0, indexCount);
		}
	}

	/*!
	* \brief Gets the shader uniforms
	* \return Uniforms of the shader
	*
	* \param shader Shader to get uniforms from
	*/

	const ForwardRenderTechnique::ShaderUniforms* ForwardRenderTechnique::GetShaderUniforms(const Shader* shader) const
	{
		auto it = m_shaderUniforms.find(shader);
		if (it == m_shaderUniforms.end())
		{
			ShaderUniforms uniforms;
			uniforms.shaderReleaseSlot.Connect(shader->OnShaderRelease, this, &ForwardRenderTechnique::OnShaderInvalidated);
			uniforms.shaderUniformInvalidatedSlot.Connect(shader->OnShaderUniformInvalidated, this, &ForwardRenderTechnique::OnShaderInvalidated);

			uniforms.eyePosition = shader->GetUniformLocation("EyePosition");
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
				uniforms.lightUniforms.locations.pointLightShadowMap = shader->GetUniformLocation("PointLightShadowMap[0]");
				uniforms.lightUniforms.locations.shadowMapping = shader->GetUniformLocation("Lights[0].shadowMapping");
				uniforms.lightUniforms.locations.directionalSpotLightShadowMap = shader->GetUniformLocation("DirectionalSpotLightShadowMap[0]");
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

	IndexBuffer ForwardRenderTechnique::s_quadIndexBuffer;
	TextureSampler ForwardRenderTechnique::s_shadowSampler;
	VertexBuffer ForwardRenderTechnique::s_quadVertexBuffer;
	VertexDeclaration ForwardRenderTechnique::s_billboardInstanceDeclaration;
	VertexDeclaration ForwardRenderTechnique::s_billboardVertexDeclaration;
}
