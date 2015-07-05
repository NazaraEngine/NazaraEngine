// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderTechnique.hpp>
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

namespace
{
	struct BillboardPoint
	{
		NzColor color;
		NzVector3f position;
		NzVector2f size;
		NzVector2f sinCos; // must follow `size` (both will be sent as a Vector4f)
		NzVector2f uv;
	};

	unsigned int s_maxQuads = std::numeric_limits<nzUInt16>::max()/6;
	unsigned int s_vertexBufferSize = 4*1024*1024; // 4 MiB
}

NzDepthRenderTechnique::NzDepthRenderTechnique() :
m_vertexBuffer(nzBufferType_Vertex)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	m_vertexBuffer.Create(s_vertexBufferSize, nzDataStorage_Hardware, nzBufferUsage_Dynamic);

	m_billboardPointBuffer.Reset(&s_billboardVertexDeclaration, &m_vertexBuffer);
	m_spriteBuffer.Reset(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Color_UV), &m_vertexBuffer);
}

bool NzDepthRenderTechnique::Draw(const NzSceneData& sceneData) const
{
	NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
	NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
	NzRenderer::Clear(nzRendererBuffer_Depth);

	// Just in case the background does render depth
	if (sceneData.background)
		sceneData.background->Draw(sceneData.viewer);

	if (!m_renderQueue.opaqueModels.empty())
		DrawOpaqueModels(sceneData);

	if (!m_renderQueue.basicSprites.empty())
		DrawBasicSprites(sceneData);

	if (!m_renderQueue.billboards.empty())
		DrawBillboards(sceneData);

	// Other custom drawables
	for (const NzDrawable* drawable : m_renderQueue.otherDrawables)
		drawable->Draw();

	return true;
}

NzAbstractRenderQueue* NzDepthRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

nzRenderTechniqueType NzDepthRenderTechnique::GetType() const
{
	return nzRenderTechniqueType_Depth;
}

bool NzDepthRenderTechnique::Initialize()
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

void NzDepthRenderTechnique::Uninitialize()
{
	s_quadIndexBuffer.Reset();
	s_quadVertexBuffer.Reset();
}

void NzDepthRenderTechnique::DrawBasicSprites(const NzSceneData& sceneData) const
{
	NazaraUnused(sceneData);

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

						// Overlay
						shader->SendInteger(shaderUniforms->textureOverlay, overlayUnit);

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

void NzDepthRenderTechnique::DrawBillboards(const NzSceneData& sceneData) const
{
	NazaraUnused(sceneData);

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
				// On commence par appliquer du matériau
				material->Apply(nzShaderFlags_Billboard | nzShaderFlags_Instancing | nzShaderFlags_VertexColor);

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
				// On commence par appliquer du matériau
				material->Apply(nzShaderFlags_Billboard | nzShaderFlags_VertexColor);

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

void NzDepthRenderTechnique::DrawOpaqueModels(const NzSceneData& sceneData) const
{
	NazaraUnused(sceneData);

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
				bool noPointSpotLight = m_renderQueue.pointLights.empty() && m_renderQueue.spotLights.empty();
				bool instancing = m_instancingEnabled && (!material->IsLightingEnabled() || noPointSpotLight) && matEntry.instancingEnabled;

				// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
				material->Apply((instancing) ? nzShaderFlags_Instancing : 0);

				// Meshes
				for (auto& meshIt : meshInstances)
				{
					const NzMeshData& meshData = meshIt.first;
					auto& meshEntry = meshIt.second;

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

const NzDepthRenderTechnique::ShaderUniforms* NzDepthRenderTechnique::GetShaderUniforms(const NzShader* shader) const
{
	auto it = m_shaderUniforms.find(shader);
	if (it == m_shaderUniforms.end())
	{
		ShaderUniforms uniforms;
		uniforms.shaderReleaseSlot.Connect(shader->OnShaderRelease, this, &NzDepthRenderTechnique::OnShaderInvalidated);
		uniforms.shaderUniformInvalidatedSlot.Connect(shader->OnShaderUniformInvalidated, this, &NzDepthRenderTechnique::OnShaderInvalidated);

		uniforms.textureOverlay = shader->GetUniformLocation("TextureOverlay");

		it = m_shaderUniforms.emplace(shader, std::move(uniforms)).first;
	}

	return &it->second;
}

void NzDepthRenderTechnique::OnShaderInvalidated(const NzShader* shader) const
{
	m_shaderUniforms.erase(shader);
}

NzIndexBuffer NzDepthRenderTechnique::s_quadIndexBuffer;
NzVertexBuffer NzDepthRenderTechnique::s_quadVertexBuffer;
NzVertexDeclaration NzDepthRenderTechnique::s_billboardInstanceDeclaration;
NzVertexDeclaration NzDepthRenderTechnique::s_billboardVertexDeclaration;
