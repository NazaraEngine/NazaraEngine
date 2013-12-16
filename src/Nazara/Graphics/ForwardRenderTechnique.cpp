// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	static NzIndexBuffer* s_indexBuffer = nullptr;
	unsigned int maxLightCount = 3; ///TODO: Constante sur le nombre maximum de lumières
	unsigned int s_maxSprites = 8192;

	NzIndexBuffer* BuildIndexBuffer()
	{
		std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(false, s_maxSprites*6, nzBufferStorage_Hardware, nzBufferUsage_Static));
		indexBuffer->SetPersistent(false);

		NzBufferMapper<NzIndexBuffer> mapper(indexBuffer.get(), nzBufferAccess_WriteOnly);
		nzUInt16* indices = static_cast<nzUInt16*>(mapper.GetPointer());

		for (unsigned int i = 0; i < s_maxSprites; ++i)
		{
			*indices++ = i*4 + 0;
			*indices++ = i*4 + 2;
			*indices++ = i*4 + 1;

			*indices++ = i*4 + 2;
			*indices++ = i*4 + 3;
			*indices++ = i*4 + 1;
		}

		return indexBuffer.release();
	}
}

NzForwardRenderTechnique::NzForwardRenderTechnique() :
m_spriteBuffer(NzVertexDeclaration::Get(nzVertexLayout_XYZ_UV), s_maxSprites*4, nzBufferStorage_Hardware, nzBufferUsage_Dynamic),
m_maxLightsPerObject(maxLightCount)
{
	if (!s_indexBuffer)
		s_indexBuffer = BuildIndexBuffer();

	m_indexBuffer = s_indexBuffer;
}

NzForwardRenderTechnique::~NzForwardRenderTechnique()
{
	if (m_indexBuffer.Reset())
		s_indexBuffer = nullptr;
}

void NzForwardRenderTechnique::Clear(const NzScene* scene)
{
	NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
	NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
	NzRenderer::Clear(nzRendererClear_Depth);

	NzAbstractBackground* background = scene->GetBackground();
	if (background)
		background->Draw(scene);
}

bool NzForwardRenderTechnique::Draw(const NzScene* scene)
{
	m_directionalLights.SetLights(&m_renderQueue.directionalLights[0], m_renderQueue.directionalLights.size());
	m_lights.SetLights(&m_renderQueue.lights[0], m_renderQueue.lights.size());
	m_renderQueue.Sort(scene->GetViewer());

	if (!m_renderQueue.opaqueModels.empty())
		DrawOpaqueModels(scene);

	if (!m_renderQueue.sprites.empty())
		DrawSprites(scene);

	if (!m_renderQueue.transparentsModels.empty())
		DrawTransparentModels(scene);

	// Les autres drawables (Exemple: Terrain)
	for (const NzDrawable* drawable : m_renderQueue.otherDrawables)
		drawable->Draw();

	return true;

	// Les billboards
	/*if (!m_renderQueue.billboards.empty())
	{
		//NzRenderer::SetIndexBuffer(m_billboardIndexBuffer);
		NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
		NzRenderer::SetShader(m_billboardShader);
		NzRenderer::SetVertexBuffer(m_billboardVertexBuffer);

		m_billboardShader->SendVector(s_cameraForwardLocation, camera->GetForward());
		m_billboardShader->SendVector(s_cameraUpLocation, camera->GetUp());
		m_billboardShader->SendVector(s_worldUpLocation, NzVector3f::Up());

		// Couleur ambiante de la scène
		m_billboardShader->SendColor(m_billboardShader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
		// Position de la caméra
		m_billboardShader->SendVector(m_billboardShader->GetUniformLocation(nzShaderUniform_CameraPosition), camera->GetPosition());

		lightCount = 0;

		// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
		m_renderQueue.lights[0]->Apply(m_billboardShader, 0);

		for (auto& matIt : m_renderQueue.billboards)
		{
			const NzMaterial* material = matIt.first;
			auto& billboards = matIt.second;

			material->Apply(m_billboardShader);

			unsigned int billboardCount = billboards.size();
			const NzForwardRenderQueue::BillboardData* data = &billboards[0];
			while (billboardCount > 0)
			{
				unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboards);
				billboardCount -= renderedBillboardCount;

				m_billboardVertexBuffer->FillVertices(data, 0, renderedBillboardCount, true);
				data += renderedBillboardCount;

				NzRenderer::DrawPrimitives(nzPrimitiveMode_PointList, 0, renderedBillboardCount);
			}
			billboards.clear();
		}
	}*/
}

unsigned int NzForwardRenderTechnique::GetMaxLightsPerObject() const
{
	return m_maxLightsPerObject;
}

NzAbstractRenderQueue* NzForwardRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

nzRenderTechniqueType NzForwardRenderTechnique::GetType() const
{
	return nzRenderTechniqueType_BasicForward;
}

void NzForwardRenderTechnique::SetMaxLightsPerObject(unsigned int lightCount)
{
	#if NAZARA_GRAPHICS_SAFE
	if (lightCount > maxLightCount)
	{
		NazaraError("Light count is over maximum light count (" + NzString::Number(lightCount) + " > " + NzString::Number(lightCount) + ')');
		return;
	}
	#endif

	m_maxLightsPerObject = lightCount;
}

void NzForwardRenderTechnique::DrawOpaqueModels(const NzScene* scene)
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShaderProgram* lastProgram = nullptr;

	unsigned int lightCount = 0;

	for (auto& matIt : m_renderQueue.opaqueModels)
	{
		bool& used = std::get<0>(matIt.second);
		if (used)
		{
			bool& renderQueueInstancing = std::get<1>(matIt.second);
			NzForwardRenderQueue::BatchedSkeletalMeshContainer& skeletalContainer = std::get<2>(matIt.second);
			NzForwardRenderQueue::BatchedStaticMeshContainer& staticContainer = std::get<3>(matIt.second);

			if (!skeletalContainer.empty() || !staticContainer.empty())
			{
				const NzMaterial* material = matIt.first;

				// Nous utilisons de l'instancing que lorsqu'aucune lumière (autre que directionnelle) n'est active
				// Ceci car l'instancing n'est pas compatible avec la recherche des lumières les plus proches
				// (Le deferred shading n'a pas ce problème)
				bool instancing = m_instancingEnabled && m_lights.IsEmpty() && renderQueueInstancing;

				// On commence par récupérer le programme du matériau
				const NzShaderProgram* program = material->GetShaderProgram(nzShaderTarget_Model, (instancing) ? nzShaderFlags_Instancing : 0);

				// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
				if (program != lastProgram)
				{
					NzRenderer::SetShaderProgram(program);

					// Couleur ambiante de la scène
					program->SendColor(program->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
					// Position de la caméra
					program->SendVector(program->GetUniformLocation(nzShaderUniform_EyePosition), viewer->GetEyePosition());

					// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
					lightCount = std::min(m_directionalLights.GetLightCount(), 3U);
					for (unsigned int i = 0; i < lightCount; ++i)
						m_directionalLights.GetLight(i)->Enable(program, i);

					lastProgram = program;
				}

				material->Apply(program);

				// Meshs squelettiques
				/*if (!skeletalContainer.empty())
				{
					NzRenderer::SetVertexBuffer(m_skinningBuffer); // Vertex buffer commun
					for (auto& subMeshIt : container)
					{
						///TODO
					}
				}*/

				// Meshs statiques
				for (auto& subMeshIt : staticContainer)
				{
					const NzSpheref& boundingSphere = subMeshIt.second.first;
					const NzStaticMesh* mesh = subMeshIt.first;
					std::vector<NzForwardRenderQueue::StaticData>& staticData = subMeshIt.second.second;

					if (!staticData.empty())
					{
						const NzIndexBuffer* indexBuffer = mesh->GetIndexBuffer();
						const NzVertexBuffer* vertexBuffer = mesh->GetVertexBuffer();

						// Gestion du draw call avant la boucle de rendu
						std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> DrawFunc;
						std::function<void(unsigned int, nzPrimitiveMode, unsigned int, unsigned int)> InstancedDrawFunc;
						unsigned int indexCount;

						if (indexBuffer)
						{
							DrawFunc = NzRenderer::DrawIndexedPrimitives;
							InstancedDrawFunc = NzRenderer::DrawIndexedPrimitivesInstanced;
							indexCount = indexBuffer->GetIndexCount();
						}
						else
						{
							DrawFunc = NzRenderer::DrawPrimitives;
							InstancedDrawFunc = NzRenderer::DrawPrimitivesInstanced;
							indexCount = vertexBuffer->GetVertexCount();
						}

						NzRenderer::SetIndexBuffer(indexBuffer);
						NzRenderer::SetVertexBuffer(vertexBuffer);

						nzPrimitiveMode primitiveMode = mesh->GetPrimitiveMode();
						if (instancing)
						{
							NzVertexBuffer* instanceBuffer = NzRenderer::GetInstanceBuffer();

							instanceBuffer->SetVertexDeclaration(NzVertexDeclaration::Get(nzVertexLayout_Matrix4));

							unsigned int stride = instanceBuffer->GetStride();

							const NzForwardRenderQueue::StaticData* data = &staticData[0];
							unsigned int instanceCount = staticData.size();
							unsigned int maxInstanceCount = instanceBuffer->GetVertexCount();

							while (instanceCount > 0)
							{
								unsigned int renderedInstanceCount = std::min(instanceCount, maxInstanceCount);
								instanceCount -= renderedInstanceCount;

								NzBufferMapper<NzVertexBuffer> mapper(instanceBuffer, nzBufferAccess_DiscardAndWrite, 0, renderedInstanceCount);
								nzUInt8* ptr = reinterpret_cast<nzUInt8*>(mapper.GetPointer());

								for (unsigned int i = 0; i < renderedInstanceCount; ++i)
								{
									std::memcpy(ptr, data->transformMatrix, sizeof(float)*16);

									data++;
									ptr += stride;
								}

								mapper.Unmap();

								InstancedDrawFunc(renderedInstanceCount, primitiveMode, 0, indexCount);
							}
						}
						else
						{
							unsigned int originalLightCount = lightCount;
							for (const NzForwardRenderQueue::StaticData& data : staticData)
							{
								// Calcul des lumières les plus proches
								if (lightCount < m_maxLightsPerObject && !m_lights.IsEmpty())
								{
									unsigned int count = std::min(m_maxLightsPerObject-lightCount, m_lights.ComputeClosestLights(data.transformMatrix.GetTranslation() + boundingSphere.GetPosition(), boundingSphere.radius, maxLightCount));
									for (unsigned int i = 0; i < count; ++i)
										m_lights.GetResult(i)->Enable(program, lightCount++);
								}

								for (unsigned int i = lightCount; i < maxLightCount; ++i)
									NzLight::Disable(program, i);

								NzRenderer::SetMatrix(nzMatrixType_World, data.transformMatrix);
								DrawFunc(primitiveMode, 0, indexCount);

								lightCount = originalLightCount;
							}
						}
						staticData.clear();
					}
				}
			}

			// Et on remet à zéro les données
			renderQueueInstancing = false;
			used = false;
		}
	}
}

void NzForwardRenderTechnique::DrawSprites(const NzScene* scene)
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShaderProgram* lastProgram = nullptr;

	NzRenderer::SetIndexBuffer(m_indexBuffer);
	NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
	NzRenderer::SetVertexBuffer(&m_spriteBuffer);

	for (auto& matIt : m_renderQueue.sprites)
	{
		const NzMaterial* material = matIt.first;
		auto& spriteVector = matIt.second;

		unsigned int spriteCount = spriteVector.size();
		if (spriteCount > 0)
		{
			// On commence par récupérer le programme du matériau
			const NzShaderProgram* program = material->GetShaderProgram(nzShaderTarget_Sprite, 0);

			// Les uniformes sont conservées au sein du shader, inutile de les renvoyer tant que le shader reste le même
			if (program != lastProgram)
			{
				NzRenderer::SetShaderProgram(program);

				// Couleur ambiante de la scène
				program->SendColor(program->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
				// Position de la caméra
				program->SendVector(program->GetUniformLocation(nzShaderUniform_EyePosition), viewer->GetEyePosition());

				lastProgram = program;
			}

			material->Apply(program);

			const NzSprite** spritePtr = &spriteVector[0];
			do
			{
				unsigned int renderedSpriteCount = std::min(spriteCount, 64U);
				spriteCount -= renderedSpriteCount;

				NzBufferMapper<NzVertexBuffer> vertexMapper(m_spriteBuffer, nzBufferAccess_DiscardAndWrite, 0, renderedSpriteCount*4);
				NzVertexStruct_XYZ_UV* vertices = reinterpret_cast<NzVertexStruct_XYZ_UV*>(vertexMapper.GetPointer());

				for (unsigned int i = 0; i < renderedSpriteCount; ++i)
				{
					const NzSprite* sprite = *spritePtr++;
					const NzRectf& textureCoords = sprite->GetTextureCoords();
					const NzVector2f& halfSize = sprite->GetSize()*0.5f;
					NzVector3f center = sprite->GetPosition();
					NzQuaternionf rotation = sprite->GetRotation();

					vertices->position = center + rotation * NzVector3f(-halfSize.x, -halfSize.y, 0.f);
					vertices->uv.Set(textureCoords.x, textureCoords.y + textureCoords.height);
					vertices++;

					vertices->position = center + rotation * NzVector3f(halfSize.x, -halfSize.y, 0.f);
					vertices->uv.Set(textureCoords.width, textureCoords.y + textureCoords.height);
					vertices++;

					vertices->position = center + rotation * NzVector3f(-halfSize.x, halfSize.y, 0.f);
					vertices->uv.Set(textureCoords.x, textureCoords.y);
					vertices++;

					vertices->position = center + rotation * NzVector3f(halfSize.x, halfSize.y, 0.f);
					vertices->uv.Set(textureCoords.width, textureCoords.y);
					vertices++;
				}

				vertexMapper.Unmap();

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, renderedSpriteCount*6);
			}
			while (spriteCount > 0);

			spriteVector.clear();
		}
	}
}

void NzForwardRenderTechnique::DrawTransparentModels(const NzScene* scene)
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShaderProgram* lastProgram = nullptr;
	unsigned int lightCount = 0;

	for (const std::pair<unsigned int, bool>& pair : m_renderQueue.transparentsModels)
	{
		// Matériau
		const NzMaterial* material = (pair.second) ?
		                              m_renderQueue.transparentStaticModels[pair.first].material :
		                              m_renderQueue.transparentSkeletalModels[pair.first].material;

		// On commence par récupérer le shader du matériau
		const NzShaderProgram* program = material->GetShaderProgram(nzShaderTarget_Model, 0);

		// Les uniformes sont conservées au sein du shader, inutile de les renvoyer tant que le shader reste le même
		if (program != lastProgram)
		{
			NzRenderer::SetShaderProgram(program);

			// Couleur ambiante de la scène
			program->SendColor(program->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
			// Position de la caméra
			program->SendVector(program->GetUniformLocation(nzShaderUniform_EyePosition), viewer->GetEyePosition());

			// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
			lightCount = std::min(m_directionalLights.GetLightCount(), 3U);
			for (unsigned int i = 0; i < lightCount; ++i)
				m_directionalLights.GetLight(i)->Enable(program, i);

			lastProgram = program;
		}

		material->Apply(program);

		// Mesh
		if (pair.second)
		{
			NzForwardRenderQueue::TransparentStaticModel& staticModel = m_renderQueue.transparentStaticModels[pair.first];

			const NzMatrix4f& matrix = staticModel.transformMatrix;
			const NzStaticMesh* mesh = staticModel.mesh;

			const NzIndexBuffer* indexBuffer = mesh->GetIndexBuffer();
			const NzVertexBuffer* vertexBuffer = mesh->GetVertexBuffer();

			// Gestion du draw call avant la boucle de rendu
			std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> DrawFunc;
			unsigned int indexCount;

			if (indexBuffer)
			{
				DrawFunc = NzRenderer::DrawIndexedPrimitives;
				indexCount = indexBuffer->GetIndexCount();
			}
			else
			{
				DrawFunc = NzRenderer::DrawPrimitives;
				indexCount = vertexBuffer->GetVertexCount();
			}

			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(vertexBuffer);

			// Calcul des lumières les plus proches
			if (lightCount < m_maxLightsPerObject && !m_lights.IsEmpty())
			{
				unsigned int count = std::min(m_maxLightsPerObject-lightCount, m_lights.ComputeClosestLights(matrix.GetTranslation() + staticModel.boundingSphere.GetPosition(), staticModel.boundingSphere.radius, maxLightCount));
				for (unsigned int i = 0; i < count; ++i)
					m_lights.GetResult(i)->Enable(program, lightCount++);
			}

			for (unsigned int i = lightCount; i < maxLightCount; ++i)
				NzLight::Disable(program, i);

			NzRenderer::SetMatrix(nzMatrixType_World, matrix);
			DrawFunc(mesh->GetPrimitiveMode(), 0, indexCount);
		}
		else
		{
			///TODO
		}
	}
}
