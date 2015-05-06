// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Surveiller les shaders et supprimer les données uniformes en cas de changement (recompilation/destruction)

namespace
{
	struct BillboardPoint
	{
		NzColor color;
		NzVector3f position;
		NzVector2f size;
		NzVector2f sinCos; // doit suivre size
		NzVector2f uv;
	};

	unsigned int s_maxQuads = std::numeric_limits<nzUInt16>::max()/6;
	unsigned int s_vertexBufferSize = 4*1024*1024; // 4 MiB
}

NzForwardRenderTechnique::NzForwardRenderTechnique() :
m_vertexBuffer(nzBufferType_Vertex),
m_maxLightPassPerObject(3)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	m_vertexBuffer.Create(s_vertexBufferSize, nzDataStorage_Hardware, nzBufferUsage_Dynamic);

	m_billboardPointBuffer.Reset(&s_billboardVertexDeclaration, &m_vertexBuffer);
	m_spriteBuffer.Reset(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Color_UV), &m_vertexBuffer);
}

void NzForwardRenderTechnique::Clear(const NzScene* scene) const
{
	NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
	NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
	NzRenderer::Clear(nzRendererBuffer_Depth);

	NzAbstractBackground* background = (scene->IsBackgroundEnabled()) ? scene->GetBackground() : nullptr;
	if (background)
		background->Draw(scene);
}

bool NzForwardRenderTechnique::Draw(const NzScene* scene) const
{
	m_directionalLights.SetLights(&m_renderQueue.directionalLights[0], m_renderQueue.directionalLights.size());
	m_lights.SetLights(&m_renderQueue.lights[0], m_renderQueue.lights.size());
	m_renderQueue.Sort(scene->GetViewer());

	if (!m_renderQueue.opaqueModels.empty())
		DrawOpaqueModels(scene);

	if (!m_renderQueue.transparentModels.empty())
		DrawTransparentModels(scene);

	if (!m_renderQueue.basicSprites.empty())
		DrawBasicSprites(scene);

	if (!m_renderQueue.billboards.empty())
		DrawBillboards(scene);

	// Les autres drawables (Exemple: Terrain)
	for (const NzDrawable* drawable : m_renderQueue.otherDrawables)
		drawable->Draw();

	return true;
}

unsigned int NzForwardRenderTechnique::GetMaxLightPassPerObject() const
{
	return m_maxLightPassPerObject;
}

NzAbstractRenderQueue* NzForwardRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

nzRenderTechniqueType NzForwardRenderTechnique::GetType() const
{
	return nzRenderTechniqueType_BasicForward;
}

void NzForwardRenderTechnique::SetMaxLightPassPerObject(unsigned int passCount)
{
	m_maxLightPassPerObject = passCount;
}

void NzForwardRenderTechnique::DrawBasicSprites(const NzScene* scene) const
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShader* lastShader = nullptr;
	const ShaderUniforms* shaderUniforms = nullptr;

	NzRenderer::SetIndexBuffer(&s_quadIndexBuffer);
	NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
	NzRenderer::SetVertexBuffer(&m_spriteBuffer);

	for (auto& matIt : m_renderQueue.basicSprites)
	{
		const NzMaterial* material = matIt.first;
		auto& matEntry = matIt.second;

		if (matEntry.enabled)
		{
			auto& overlayMap = matEntry.overlayMap;
			for (auto& overlayIt : overlayMap)
			{
				const NzTexture* overlay = overlayIt.first;
				auto& spriteChainVector = overlayIt.second.spriteChains;

				unsigned int spriteChainCount = spriteChainVector.size();
				if (spriteChainCount > 0)
				{
					// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
					nzUInt32 flags = nzShaderFlags_VertexColor;
					if (overlay)
						flags |= nzShaderFlags_TextureOverlay;

					nzUInt8 overlayUnit;
					const NzShader* shader = material->Apply(flags, 0, &overlayUnit);

					if (overlay)
					{
						overlayUnit++;
						NzRenderer::SetTexture(overlayUnit, overlay);
						NzRenderer::SetTextureSampler(overlayUnit, material->GetDiffuseSampler());
					}

					// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
					if (shader != lastShader)
					{
						// Index des uniformes dans le shader
						shaderUniforms = GetShaderUniforms(shader);

						// Couleur ambiante de la scène
						shader->SendColor(shaderUniforms->sceneAmbient, scene->GetAmbientColor());
						// Overlay
						shader->SendInteger(shaderUniforms->textureOverlay, overlayUnit);
						// Position de la caméra
						shader->SendVector(shaderUniforms->eyePosition, viewer->GetEyePosition());

						lastShader = shader;
					}

					unsigned int spriteChain = 0; // Quelle chaîne de sprite traitons-nous
					unsigned int spriteChainOffset = 0; // À quel offset dans la dernière chaîne nous sommes-nous arrêtés

					do
					{
						// On ouvre le buffer en écriture
						NzBufferMapper<NzVertexBuffer> vertexMapper(m_spriteBuffer, nzBufferAccess_DiscardAndWrite);
						NzVertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<NzVertexStruct_XYZ_Color_UV*>(vertexMapper.GetPointer());

						unsigned int spriteCount = 0;
						unsigned int maxSpriteCount = std::min(s_maxQuads, m_spriteBuffer.GetVertexCount()/4);

						do
						{
							NzForwardRenderQueue::SpriteChain_XYZ_Color_UV& currentChain = spriteChainVector[spriteChain];
							unsigned int count = std::min(maxSpriteCount - spriteCount, currentChain.spriteCount - spriteChainOffset);

							std::memcpy(vertices, currentChain.vertices + spriteChainOffset*4, 4*count*sizeof(NzVertexStruct_XYZ_Color_UV));
							vertices += count*4;

							spriteCount += count;
							spriteChainOffset += count;

							// Avons-nous traité la chaîne entière ?
							if (spriteChainOffset == currentChain.spriteCount)
							{
								spriteChain++;
								spriteChainOffset = 0;
							}
						}
						while (spriteCount < maxSpriteCount && spriteChain < spriteChainCount);

						vertexMapper.Unmap();

						NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, spriteCount*6);
					}
					while (spriteChain < spriteChainCount);

					spriteChainVector.clear();
				}
			}

			// On remet à zéro
			matEntry.enabled = false;
		}
	}
}

bool NzForwardRenderTechnique::Initialize()
{
	try
	{
		NzErrorFlags flags(nzErrorFlag_ThrowException, true);

		s_quadIndexBuffer.Reset(false, s_maxQuads*6, nzDataStorage_Hardware, nzBufferUsage_Static);

		NzBufferMapper<NzIndexBuffer> mapper(s_quadIndexBuffer, nzBufferAccess_WriteOnly);
		nzUInt16* indices = static_cast<nzUInt16*>(mapper.GetPointer());

		for (unsigned int i = 0; i < s_maxQuads; ++i)
		{
			*indices++ = i*4 + 0;
			*indices++ = i*4 + 2;
			*indices++ = i*4 + 1;

			*indices++ = i*4 + 2;
			*indices++ = i*4 + 3;
			*indices++ = i*4 + 1;
		}

		mapper.Unmap(); // Inutile de garder le buffer ouvert plus longtemps

		// Quad buffer (utilisé pour l'instancing de billboard et de sprites)
		//Note: Les UV sont calculés dans le shader
		s_quadVertexBuffer.Reset(NzVertexDeclaration::Get(nzVertexLayout_XY), 4, nzDataStorage_Hardware, nzBufferUsage_Static);

		float vertices[2*4] = {
		   -0.5f, -0.5f,
			0.5f, -0.5f,
		   -0.5f, 0.5f,
			0.5f, 0.5f,
		};

		s_quadVertexBuffer.FillRaw(vertices, 0, sizeof(vertices));

		// Déclaration lors du rendu des billboards par sommet
		s_billboardVertexDeclaration.EnableComponent(nzVertexComponent_Color,     nzComponentType_Color,  NzOffsetOf(BillboardPoint, color));
		s_billboardVertexDeclaration.EnableComponent(nzVertexComponent_Position,  nzComponentType_Float3, NzOffsetOf(BillboardPoint, position));
		s_billboardVertexDeclaration.EnableComponent(nzVertexComponent_TexCoord,  nzComponentType_Float2, NzOffsetOf(BillboardPoint, uv));
		s_billboardVertexDeclaration.EnableComponent(nzVertexComponent_Userdata0, nzComponentType_Float4, NzOffsetOf(BillboardPoint, size)); // Englobe sincos

		// Declaration utilisée lors du rendu des billboards par instancing
		// L'avantage ici est la copie directe (std::memcpy) des données de la RenderQueue vers le buffer GPU
		s_billboardInstanceDeclaration.EnableComponent(nzVertexComponent_InstanceData0, nzComponentType_Float3, NzOffsetOf(NzForwardRenderQueue::BillboardData, center));
		s_billboardInstanceDeclaration.EnableComponent(nzVertexComponent_InstanceData1, nzComponentType_Float4, NzOffsetOf(NzForwardRenderQueue::BillboardData, size)); // Englobe sincos
		s_billboardInstanceDeclaration.EnableComponent(nzVertexComponent_InstanceData2, nzComponentType_Color,  NzOffsetOf(NzForwardRenderQueue::BillboardData, color));
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to initialise: " + NzString(e.what()));
		return false;
	}

	return true;
}

void NzForwardRenderTechnique::Uninitialize()
{
	s_quadIndexBuffer.Reset();
	s_quadVertexBuffer.Reset();
}

void NzForwardRenderTechnique::DrawBillboards(const NzScene* scene) const
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShader* lastShader = nullptr;
	const ShaderUniforms* shaderUniforms = nullptr;

	if (NzRenderer::HasCapability(nzRendererCap_Instancing))
	{
		NzVertexBuffer* instanceBuffer = NzRenderer::GetInstanceBuffer();
		instanceBuffer->SetVertexDeclaration(&s_billboardInstanceDeclaration);

		NzRenderer::SetVertexBuffer(&s_quadVertexBuffer);

		for (auto& matIt : m_renderQueue.billboards)
		{
			const NzMaterial* material = matIt.first;
			auto& entry = matIt.second;
			auto& billboardVector = entry.billboards;

			unsigned int billboardCount = billboardVector.size();
			if (billboardCount > 0)
			{
				// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
				const NzShader* shader = material->Apply(nzShaderFlags_Billboard | nzShaderFlags_Instancing | nzShaderFlags_VertexColor);

				// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
				if (shader != lastShader)
				{
					// Index des uniformes dans le shader
					shaderUniforms = GetShaderUniforms(shader);

					// Couleur ambiante de la scène
					shader->SendColor(shaderUniforms->sceneAmbient, scene->GetAmbientColor());
					// Position de la caméra
					shader->SendVector(shaderUniforms->eyePosition, viewer->GetEyePosition());

					lastShader = shader;
				}

				const NzForwardRenderQueue::BillboardData* data = &billboardVector[0];
				unsigned int maxBillboardPerDraw = instanceBuffer->GetVertexCount();
				do
				{
					unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboardPerDraw);
					billboardCount -= renderedBillboardCount;

					instanceBuffer->Fill(data, 0, renderedBillboardCount, true);
					data += renderedBillboardCount;

					NzRenderer::DrawPrimitivesInstanced(renderedBillboardCount, nzPrimitiveMode_TriangleStrip, 0, 4);
				}
				while (billboardCount > 0);

				billboardVector.clear();
			}
		}
	}
	else
	{
		NzRenderer::SetIndexBuffer(&s_quadIndexBuffer);
		NzRenderer::SetVertexBuffer(&m_billboardPointBuffer);

		for (auto& matIt : m_renderQueue.billboards)
		{
			const NzMaterial* material = matIt.first;
			auto& entry = matIt.second;
			auto& billboardVector = entry.billboards;

			unsigned int billboardCount = billboardVector.size();
			if (billboardCount > 0)
			{
				// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
				const NzShader* shader = material->Apply(nzShaderFlags_Billboard | nzShaderFlags_VertexColor);

				// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
				if (shader != lastShader)
				{
					// Couleur ambiante de la scène
					shader->SendColor(shaderUniforms->sceneAmbient, scene->GetAmbientColor());
					// Position de la caméra
					shader->SendVector(shaderUniforms->eyePosition, viewer->GetEyePosition());

					lastShader = shader;
				}

				const NzForwardRenderQueue::BillboardData* data = &billboardVector[0];
				unsigned int maxBillboardPerDraw = std::min(s_maxQuads, m_billboardPointBuffer.GetVertexCount()/4);

				do
				{
					unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboardPerDraw);
					billboardCount -= renderedBillboardCount;

					NzBufferMapper<NzVertexBuffer> vertexMapper(m_billboardPointBuffer, nzBufferAccess_DiscardAndWrite, 0, renderedBillboardCount*4);
					BillboardPoint* vertices = reinterpret_cast<BillboardPoint*>(vertexMapper.GetPointer());

					for (unsigned int i = 0; i < renderedBillboardCount; ++i)
					{
						const NzForwardRenderQueue::BillboardData& billboard = *data++;

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

					NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, renderedBillboardCount*6);
				}
				while (billboardCount > 0);

				billboardVector.clear();
			}
		}
	}
}

void NzForwardRenderTechnique::DrawOpaqueModels(const NzScene* scene) const
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShader* lastShader = nullptr;
	const ShaderUniforms* shaderUniforms = nullptr;

	for (auto& matIt : m_renderQueue.opaqueModels)
	{
		auto& matEntry = matIt.second;

		if (matEntry.enabled)
		{
			NzForwardRenderQueue::MeshInstanceContainer& meshInstances = matEntry.meshMap;

			if (!meshInstances.empty())
			{
				const NzMaterial* material = matIt.first;

				// Nous utilisons de l'instancing que lorsqu'aucune lumière (autre que directionnelle) n'est active
				// Ceci car l'instancing n'est pas compatible avec la recherche des lumières les plus proches
				// (Le deferred shading n'a pas ce problème)
				bool instancing = m_instancingEnabled && (!material->IsLightingEnabled() || m_lights.IsEmpty()) && matEntry.instancingEnabled;

				// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
				const NzShader* shader = material->Apply((instancing) ? nzShaderFlags_Instancing : 0);

				// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
				if (shader != lastShader)
				{
					// Index des uniformes dans le shader
					shaderUniforms = GetShaderUniforms(shader);

					// Couleur ambiante de la scène
					shader->SendColor(shaderUniforms->sceneAmbient, scene->GetAmbientColor());
					// Position de la caméra
					shader->SendVector(shaderUniforms->eyePosition, viewer->GetEyePosition());

					lastShader = shader;
				}

				// Meshes
				for (auto& meshIt : meshInstances)
				{
					const NzMeshData& meshData = meshIt.first;
					auto& meshEntry = meshIt.second;

					const NzSpheref& squaredBoundingSphere = meshEntry.squaredBoundingSphere;
					std::vector<NzMatrix4f>& instances = meshEntry.instances;

					if (!instances.empty())
					{
						const NzIndexBuffer* indexBuffer = meshData.indexBuffer;
						const NzVertexBuffer* vertexBuffer = meshData.vertexBuffer;

						// Gestion du draw call avant la boucle de rendu
						NzRenderer::DrawCall drawFunc;
						NzRenderer::DrawCallInstanced instancedDrawFunc;
						unsigned int indexCount;

						if (indexBuffer)
						{
							drawFunc = NzRenderer::DrawIndexedPrimitives;
							instancedDrawFunc = NzRenderer::DrawIndexedPrimitivesInstanced;
							indexCount = indexBuffer->GetIndexCount();
						}
						else
						{
							drawFunc = NzRenderer::DrawPrimitives;
							instancedDrawFunc = NzRenderer::DrawPrimitivesInstanced;
							indexCount = vertexBuffer->GetVertexCount();
						}

						NzRenderer::SetIndexBuffer(indexBuffer);
						NzRenderer::SetVertexBuffer(vertexBuffer);

						if (instancing)
						{
							// On calcule le nombre d'instances que l'on pourra afficher cette fois-ci (Selon la taille du buffer d'instancing)
							NzVertexBuffer* instanceBuffer = NzRenderer::GetInstanceBuffer();
							instanceBuffer->SetVertexDeclaration(NzVertexDeclaration::Get(nzVertexLayout_Matrix4));

							// Avec l'instancing, impossible de sélectionner les lumières pour chaque objet
							// Du coup, il n'est activé que pour les lumières directionnelles
							unsigned int lightCount = m_directionalLights.GetLightCount();
							unsigned int lightIndex = 0;
							nzRendererComparison oldDepthFunc = NzRenderer::GetDepthFunc();

							unsigned int passCount = (lightCount == 0) ? 1 : (lightCount-1)/NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS + 1;
							for (unsigned int pass = 0; pass < passCount; ++pass)
							{
								if (shaderUniforms->hasLightUniforms)
								{
									unsigned int renderedLightCount = std::min(lightCount, NazaraSuffixMacro(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, U));
									lightCount -= renderedLightCount;

									if (pass == 1)
									{
										// Pour additionner le résultat des calculs de lumière
										// Aucune chance d'interférer avec les paramètres du matériau car nous ne rendons que les objets opaques
										// (Autrement dit, sans blending)
										// Quant à la fonction de profondeur, elle ne doit être appliquée que la première fois
										NzRenderer::Enable(nzRendererParameter_Blend, true);
										NzRenderer::SetBlendFunc(nzBlendFunc_One, nzBlendFunc_One);
										NzRenderer::SetDepthFunc(nzRendererComparison_Equal);
									}

									for (unsigned int i = 0; i < renderedLightCount; ++i)
										m_directionalLights.GetLight(lightIndex++)->Enable(shader, shaderUniforms->lightUniforms, shaderUniforms->lightOffset*i);

									for (unsigned int i = renderedLightCount; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
										NzLight::Disable(shader, shaderUniforms->lightUniforms, shaderUniforms->lightOffset*i);
								}

								const NzMatrix4f* instanceMatrices = &instances[0];
								unsigned int instanceCount = instances.size();
								unsigned int maxInstanceCount = instanceBuffer->GetVertexCount(); // Le nombre maximum d'instances en une fois

								while (instanceCount > 0)
								{
									// On calcule le nombre d'instances que l'on pourra afficher cette fois-ci (Selon la taille du buffer d'instancing)
									unsigned int renderedInstanceCount = std::min(instanceCount, maxInstanceCount);
									instanceCount -= renderedInstanceCount;

									// On remplit l'instancing buffer avec nos matrices world
									instanceBuffer->Fill(instanceMatrices, 0, renderedInstanceCount, true);
									instanceMatrices += renderedInstanceCount;

									// Et on affiche
									instancedDrawFunc(renderedInstanceCount, meshData.primitiveMode, 0, indexCount);
								}
							}

							// On n'oublie pas de désactiver le blending pour ne pas interférer sur le reste du rendu
							NzRenderer::Enable(nzRendererParameter_Blend, false);
							NzRenderer::SetDepthFunc(oldDepthFunc);
						}
						else
						{
							if (shaderUniforms->hasLightUniforms)
							{
								for (const NzMatrix4f& matrix : instances)
								{
									unsigned int directionalLightCount = m_directionalLights.GetLightCount();
									unsigned int otherLightCount = m_lights.ComputeClosestLights(matrix.GetTranslation() + squaredBoundingSphere.GetPosition(), squaredBoundingSphere.radius, m_maxLightPassPerObject*NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS - directionalLightCount);
									unsigned int lightCount = directionalLightCount + otherLightCount;

									NzRenderer::SetMatrix(nzMatrixType_World, matrix);
									unsigned int directionalLightIndex = 0;
									unsigned int otherLightIndex = 0;
									nzRendererComparison oldDepthFunc = NzRenderer::GetDepthFunc(); // Dans le cas où nous aurions à le changer

									unsigned int passCount = (lightCount == 0) ? 1 : (lightCount-1)/NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS + 1;
									for (unsigned int pass = 0; pass < passCount; ++pass)
									{
										unsigned int renderedLightCount = std::min(lightCount, NazaraSuffixMacro(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, U));
										lightCount -= renderedLightCount;

										if (pass == 1)
										{
											// Pour additionner le résultat des calculs de lumière
											// Aucune chance d'interférer avec les paramètres du matériau car nous ne rendons que les objets opaques
											// (Autrement dit, sans blending)
											// Quant à la fonction de profondeur, elle ne doit être appliquée que la première fois
											NzRenderer::Enable(nzRendererParameter_Blend, true);
											NzRenderer::SetBlendFunc(nzBlendFunc_One, nzBlendFunc_One);
											NzRenderer::SetDepthFunc(nzRendererComparison_Equal);
										}

										// On active les lumières de cette passe-ci
										for (unsigned int i = 0; i < renderedLightCount; ++i)
										{
											if (directionalLightIndex >= directionalLightCount)
												m_lights.GetResult(otherLightIndex++)->Enable(shader, shaderUniforms->lightUniforms, shaderUniforms->lightOffset*i);
											else
												m_directionalLights.GetLight(directionalLightIndex++)->Enable(shader, shaderUniforms->lightUniforms, shaderUniforms->lightOffset*i);
										}

										// On désactive l'éventuel surplus
										for (unsigned int i = renderedLightCount; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
											NzLight::Disable(shader, shaderUniforms->lightUniforms, shaderUniforms->lightOffset*i);

										// Et on passe à l'affichage
										drawFunc(meshData.primitiveMode, 0, indexCount);
									}

									NzRenderer::Enable(nzRendererParameter_Blend, false);
									NzRenderer::SetDepthFunc(oldDepthFunc);
								}
							}
							else
							{
								// Sans instancing, on doit effectuer un draw call pour chaque instance
								// Cela reste néanmoins plus rapide que l'instancing en dessous d'un certain nombre d'instances
								// À cause du temps de modification du buffer d'instancing
								for (const NzMatrix4f& matrix : instances)
								{
									NzRenderer::SetMatrix(nzMatrixType_World, matrix);
									drawFunc(meshData.primitiveMode, 0, indexCount);
								}
							}
						}
						instances.clear();
					}
				}
			}

			// Et on remet à zéro les données
			matEntry.enabled = false;
			matEntry.instancingEnabled = false;
		}
	}
}

void NzForwardRenderTechnique::DrawTransparentModels(const NzScene* scene) const
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShader* lastShader = nullptr;
	const ShaderUniforms* shaderUniforms = nullptr;
	unsigned int lightCount = 0;

	for (unsigned int index : m_renderQueue.transparentModels)
	{
		const NzForwardRenderQueue::TransparentModelData& modelData = m_renderQueue.transparentModelData[index];

		// Matériau
		const NzMaterial* material = modelData.material;

		// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
		const NzShader* shader = material->Apply();

		// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
		if (shader != lastShader)
		{
			// Index des uniformes dans le shader
			shaderUniforms = GetShaderUniforms(shader);

			// Couleur ambiante de la scène
			shader->SendColor(shaderUniforms->sceneAmbient, scene->GetAmbientColor());
			// Position de la caméra
			shader->SendVector(shaderUniforms->eyePosition, viewer->GetEyePosition());

			// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
			if (shaderUniforms->hasLightUniforms)
			{
				lightCount = std::min(m_directionalLights.GetLightCount(), NazaraSuffixMacro(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, U));
				for (unsigned int i = 0; i < lightCount; ++i)
					m_directionalLights.GetLight(i)->Enable(shader, shaderUniforms->lightUniforms, shaderUniforms->lightOffset*i);
			}

			lastShader = shader;
		}

		// Mesh
		const NzMatrix4f& matrix = modelData.transformMatrix;
		const NzMeshData& meshData = modelData.meshData;

		const NzIndexBuffer* indexBuffer = meshData.indexBuffer;
		const NzVertexBuffer* vertexBuffer = meshData.vertexBuffer;

		// Gestion du draw call avant la boucle de rendu
		NzRenderer::DrawCall drawFunc;
		unsigned int indexCount;

		if (indexBuffer)
		{
			drawFunc = NzRenderer::DrawIndexedPrimitives;
			indexCount = indexBuffer->GetIndexCount();
		}
		else
		{
			drawFunc = NzRenderer::DrawPrimitives;
			indexCount = vertexBuffer->GetVertexCount();
		}

		NzRenderer::SetIndexBuffer(indexBuffer);
		NzRenderer::SetVertexBuffer(vertexBuffer);

		if (shaderUniforms->hasLightUniforms)
		{
			// Calcul des lumières les plus proches
			if (lightCount < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS && !m_lights.IsEmpty())
			{
				NzVector3f position = matrix.GetTranslation() + modelData.squaredBoundingSphere.GetPosition();
				float radius = modelData.squaredBoundingSphere.radius;
				unsigned int closestLightCount = m_lights.ComputeClosestLights(position, radius, NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS);

				unsigned int count = std::min(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS - lightCount, closestLightCount);
				for (unsigned int i = 0; i < count; ++i)
					m_lights.GetResult(i)->Enable(shader, shaderUniforms->lightUniforms, shaderUniforms->lightOffset*(lightCount++));
			}

			for (unsigned int i = lightCount; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
				NzLight::Disable(shader, shaderUniforms->lightUniforms, shaderUniforms->lightOffset*i);
		}

		NzRenderer::SetMatrix(nzMatrixType_World, matrix);
		drawFunc(meshData.primitiveMode, 0, indexCount);
	}
}

const NzForwardRenderTechnique::ShaderUniforms* NzForwardRenderTechnique::GetShaderUniforms(const NzShader* shader) const
{
	auto it = m_shaderUniforms.find(shader);
	if (it == m_shaderUniforms.end())
	{
		ShaderUniforms uniforms;
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
			uniforms.lightUniforms.locations.parameters1 = shader->GetUniformLocation("Lights[0].parameters1");
			uniforms.lightUniforms.locations.parameters2 = shader->GetUniformLocation("Lights[0].parameters2");
			uniforms.lightUniforms.locations.parameters3 = shader->GetUniformLocation("Lights[0].parameters3");
		}
		else
			uniforms.hasLightUniforms = false;

		it = m_shaderUniforms.emplace(shader, uniforms).first;
	}

	return &it->second;
}

NzIndexBuffer NzForwardRenderTechnique::s_quadIndexBuffer;
NzVertexBuffer NzForwardRenderTechnique::s_quadVertexBuffer;
NzVertexDeclaration NzForwardRenderTechnique::s_billboardInstanceDeclaration;
NzVertexDeclaration NzForwardRenderTechnique::s_billboardVertexDeclaration;
