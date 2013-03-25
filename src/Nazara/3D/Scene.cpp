// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/Scene.hpp>
#include <Nazara/2D/ColorBackground.hpp>
#include <Nazara/2D/Drawable.hpp>
#include <Nazara/3D/Camera.hpp>
#include <Nazara/3D/Light.hpp>
#include <Nazara/3D/Model.hpp>
#include <Nazara/3D/RenderQueue.hpp>
#include <Nazara/3D/SceneRoot.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include <Nazara/3D/Debug.hpp>

namespace
{
	const unsigned int maxLights = 8; ///TODO: Config

	struct LightComparator
	{
		bool operator()(const NzLight* light1, const NzLight* light2)
		{
			return light1->GetPosition().SquaredDistance(pos) < light2->GetPosition().SquaredDistance(pos);
		}

		NzVector3f pos;
	};
}

struct NzSceneImpl
{
	NzSceneImpl(NzScene* scene) :
	root(scene)
	{
	}

	std::unique_ptr<NzBackground> background;
	std::vector<NzUpdatable*> updateList;
	std::vector<NzUpdatable*> visibleUpdateList;
	std::vector<NzRenderer::InstancingData> instancingData;
	NzClock updateClock;
	NzColor ambientColor = NzColor(25,25,25);
	NzRenderQueue renderQueue;
	NzSceneRoot root;
	const NzCamera* activeCamera;
	NzVertexBuffer* skinningBuffer;
	bool update;
	float frameTime;
	float updateTime;
	unsigned int updatePerSecond = 60;
};

NzScene::NzScene()
{
	m_impl = new NzSceneImpl(this);
	m_impl->background.reset(new NzColorBackground);
	m_impl->skinningBuffer = new NzVertexBuffer(NzMesh::GetDeclaration(), 20000, nzBufferStorage_Hardware, nzBufferUsage_Dynamic);

	if (NzRenderer::HasCapability(nzRendererCap_Instancing))
		m_impl->instancingData.resize(NAZARA_RENDERER_INSTANCING_MAX);
}

NzScene::~NzScene()
{
	for (NzNode* child : m_impl->root.GetChilds())
	{
		if (child->GetNodeType() == nzNodeType_Scene)
			static_cast<NzSceneNode*>(child)->SetScene(nullptr);
	}

	delete m_impl->skinningBuffer;
	delete m_impl;
}

void NzScene::AddToVisibilityList(NzUpdatable* object)
{
	m_impl->visibleUpdateList.push_back(object);
}

void NzScene::Cull()
{
	m_impl->renderQueue.Clear();
	m_impl->visibleUpdateList.clear();

	// Frustum culling
	RecursiveFrustumCull(m_impl->renderQueue, m_impl->activeCamera->GetFrustum(), &m_impl->root);

	///TODO: Occlusion culling

	///TODO: Light culling
}

void NzScene::Draw()
{
	NzRenderer::Clear(nzRendererClear_Depth);

	if (m_impl->background)
		m_impl->background->Draw();

	LightComparator lightComparator;

	// Pour les meshs squelettiques, on utilise un buffer commun
	NzRenderer::SetVertexBuffer(m_impl->skinningBuffer);
	for (auto matIt : m_impl->renderQueue.visibleSkeletalModels)
	{
		// On applique le shader du matériau
		nzUInt32 shaderFlags = matIt.first->GetShaderFlags();

		const NzShader* shader = NzShaderBuilder::Get(shaderFlags);

		NzRenderer::SetShader(shader);
		matIt.first->Apply(shader);

		// Position de la caméra
		int camPosLocation = shader->GetUniformLocation("CameraPosition");
		if (camPosLocation != -1)
			shader->SendVector(camPosLocation, m_impl->activeCamera->GetPosition());

		// Couleur ambiante de la scène
		int sceneAmbientLocation = shader->GetUniformLocation("SceneAmbient");
		if (sceneAmbientLocation != -1)
			shader->SendColor(sceneAmbientLocation, m_impl->ambientColor);

		// Gestion des lumières (D'abord directionnelles)
		int lightCountLocation = shader->GetUniformLocation("LightCount");

		unsigned int lightIndex = 0;
		if (lightCountLocation != -1)
		{
			for (const NzLight* light : m_impl->renderQueue.directionnalLights)
			{
				light->Apply(shader, lightIndex++);
				if (lightIndex > maxLights)
					break; // N'arrivera jamais mais pourrait résulter en un bug
			}
		}

		for (auto subMeshIt : matIt.second)
		{
			const NzSkeletalMesh* skeletalMesh = subMeshIt.first;
			const NzIndexBuffer* indexBuffer = skeletalMesh->GetIndexBuffer();

			unsigned int vertexCount = skeletalMesh->GetVertexCount();

			// Gestion du draw call avant la boucle de rendu
			std::function<void(nzPrimitiveType, unsigned int, unsigned int)> drawFunc;
			nzPrimitiveType primitiveType = skeletalMesh->GetPrimitiveType();
			unsigned int indexCount;
			if (indexBuffer)
			{
				drawFunc = NzRenderer::DrawIndexedPrimitives;
				indexCount = indexBuffer->GetIndexCount();
				NzRenderer::SetIndexBuffer(indexBuffer);
			}
			else
			{
				drawFunc = NzRenderer::DrawPrimitives;
				indexCount = skeletalMesh->GetVertexCount();
			}

			for (const NzRenderQueue::SkeletalData& data : subMeshIt.second)
			{
				// Transfert du résultat du skinning vers notre buffer hardware
				NzBufferMapper<NzVertexBuffer> outputMapper(m_impl->skinningBuffer, nzBufferAccess_DiscardAndWrite, 0, vertexCount);
				std::memcpy(outputMapper.GetPointer(), &data.skinnedVertices[0], vertexCount*sizeof(NzMeshVertex));
				outputMapper.Unmap();

				// Calcul des lumières les plus proches (TODO: LightManager ?)
				if (lightCountLocation != -1)
				{
					auto visibleLights = m_impl->renderQueue.visibleLights;
					lightComparator.pos = data.transformMatrix.GetTranslation();
					std::sort(visibleLights.begin(), visibleLights.end(), lightComparator);

					const unsigned int maxLightPerObject = 3; ///TODO: Config
					unsigned int max = std::min(std::min(maxLights - lightIndex, maxLightPerObject), static_cast<unsigned int>(visibleLights.size()));
					for (unsigned int i = 0; i < max; ++i)
						visibleLights[i]->Apply(shader, lightIndex + i);

					shader->SendInteger(lightCountLocation, lightIndex + max);
				}

				NzRenderer::SetMatrix(nzMatrixType_World, data.transformMatrix);

				drawFunc(primitiveType, 0, indexCount);
			}
		}
	}

	// Pour les meshs statiques, on utilise le buffer du mesh
	for (auto matIt : m_impl->renderQueue.visibleStaticModels)
	{
		// On applique le shader du matériau
		nzUInt32 shaderFlags = matIt.first->GetShaderFlags();
		if (NzRenderer::HasCapability(nzRendererCap_Instancing) && m_impl->renderQueue.visibleLights.empty())
			shaderFlags |= nzShaderFlags_Instancing;

		const NzShader* shader = NzShaderBuilder::Get(shaderFlags);

		NzRenderer::SetShader(shader);
		matIt.first->Apply(shader);

		bool instancing = shader->GetFlags() & nzShaderFlags_Instancing;

		// Position de la caméra
		int camPosLocation = shader->GetUniformLocation("CameraPosition");
		if (camPosLocation != -1)
			shader->SendVector(camPosLocation, m_impl->activeCamera->GetPosition());

		// Couleur ambiante de la scène
		int sceneAmbientLocation = shader->GetUniformLocation("SceneAmbient");
		if (sceneAmbientLocation != -1)
			shader->SendColor(sceneAmbientLocation, m_impl->ambientColor);

		// Gestion des lumières (D'abord directionnelles)
		int lightCountLocation = shader->GetUniformLocation("LightCount");

		unsigned int lightIndex = 0;
		if (lightCountLocation != -1)
		{
			for (const NzLight* light : m_impl->renderQueue.directionnalLights)
			{
				light->Apply(shader, lightIndex++);
				if (lightIndex > maxLights)
					break; // N'arrivera probablement jamais mais pourrait résulter en un bug
			}
		}

		for (auto subMeshIt : matIt.second)
		{
			NzStaticMesh* staticMesh = subMeshIt.first;

			const NzIndexBuffer* indexBuffer = staticMesh->GetIndexBuffer();
			const NzVertexBuffer* vertexBuffer = staticMesh->GetVertexBuffer();

			NzRenderer::SetVertexBuffer(vertexBuffer);

			// Gestion du draw call avant la boucle de rendu
			std::function<void(nzPrimitiveType, unsigned int, unsigned int)> draw;
			std::function<void(unsigned int, nzPrimitiveType, unsigned int, unsigned int)> instancedDraw;
			nzPrimitiveType primitiveType = staticMesh->GetPrimitiveType();
			unsigned int indexCount;
			if (indexBuffer)
			{
				draw = NzRenderer::DrawIndexedPrimitives;
				indexCount = indexBuffer->GetIndexCount();
				instancedDraw = NzRenderer::DrawIndexedPrimitivesInstanced;
				NzRenderer::SetIndexBuffer(indexBuffer);
			}
			else
			{
				draw = NzRenderer::DrawPrimitives;
				indexCount = vertexBuffer->GetVertexCount();
				instancedDraw = NzRenderer::DrawPrimitivesInstanced;
			}

			if (instancing)
			{
				if (lightCountLocation != -1)
					shader->SendInteger(lightCountLocation, lightIndex);

				unsigned int count = 0;
				for (const NzMatrix4f& matrix : subMeshIt.second)
				{
					m_impl->instancingData[count++].worldMatrix = matrix;
					if (count == m_impl->instancingData.size())
					{
						NzRenderer::SetInstancingData(&m_impl->instancingData[0], count);
						instancedDraw(count, primitiveType, 0, indexCount);

						count = 0;
					}
				}

				if (count > 0)
				{
					NzRenderer::SetInstancingData(&m_impl->instancingData[0], count);
					instancedDraw(count, primitiveType, 0, indexCount);
				}
			}
			else
			{
				for (const NzMatrix4f& matrix : subMeshIt.second)
				{
					// Calcul des lumières les plus proches (TODO: LightManager ?)
					if (lightCountLocation != -1)
					{
						std::vector<const NzLight*>& visibleLights = m_impl->renderQueue.visibleLights;
						lightComparator.pos = matrix.GetTranslation();
						std::sort(visibleLights.begin(), visibleLights.end(), lightComparator);

						const unsigned int maxLightPerObject = 3; ///TODO: Config
						unsigned int max = std::min(std::min(maxLights - lightIndex, maxLightPerObject), visibleLights.size());
						for (unsigned int i = 0; i < max; ++i)
							visibleLights[i]->Apply(shader, lightIndex + i);

						shader->SendInteger(lightCountLocation, lightIndex + max);
					}

					NzRenderer::SetMatrix(nzMatrixType_World, matrix);

					draw(primitiveType, 0, indexCount);
				}
			}
		}
	}

	// Les autres drawables (Exemple: Terrain)
	for (const NzDrawable* drawable : m_impl->renderQueue.otherDrawables)
		drawable->Draw();
}

const NzCamera* NzScene::GetActiveCamera() const
{
	return m_impl->activeCamera;
}

NzBackground* NzScene::GetBackground() const
{
	return m_impl->background.get();
}

NzSceneNode& NzScene::GetRoot() const
{
	return m_impl->root;
}

float NzScene::GetUpdateTime() const
{
	return m_impl->updateTime;
}

unsigned int NzScene::GetUpdatePerSecond() const
{
	return m_impl->updatePerSecond;
}

void NzScene::RegisterForUpdate(NzUpdatable* object)
{
	#if NAZARA_3D_SAFE
	if (!object)
	{
		NazaraError("Invalid object");
		return;
	}
	#endif

	m_impl->updateList.push_back(object);
}

void NzScene::SetAmbientColor(const NzColor& color)
{
	m_impl->ambientColor = color;
}

void NzScene::SetBackground(NzBackground* background)
{
	m_impl->background.reset(background);
}

void NzScene::SetUpdatePerSecond(unsigned int updatePerSecond)
{
	m_impl->updatePerSecond = updatePerSecond;
}

void NzScene::UnregisterForUpdate(NzUpdatable* object)
{
	#if NAZARA_3D_SAFE
	if (!object)
	{
		NazaraError("Invalid object");
		return;
	}
	#endif

	auto it = std::find(m_impl->updateList.begin(), m_impl->updateList.end(), object);
	if (it != m_impl->updateList.end())
		m_impl->updateList.erase(it);
}

void NzScene::Update()
{
	m_impl->update = (m_impl->updatePerSecond == 0 || m_impl->updateClock.GetMilliseconds() > 1000/m_impl->updatePerSecond);
	if (m_impl->update)
	{
		m_impl->updateTime = m_impl->updateClock.GetSeconds();
		m_impl->updateClock.Restart();

		for (NzUpdatable* updatable : m_impl->updateList)
			///TODO: Multihreading
			updatable->Update();
	}
}

void NzScene::UpdateVisible()
{
	if (m_impl->update)
	{
		for (NzUpdatable* node : m_impl->visibleUpdateList)
			node->Update();
	}
}

NzScene::operator const NzSceneNode&() const
{
	return m_impl->root;
}

void NzScene::RecursiveFrustumCull(NzRenderQueue& renderQueue, const NzFrustumf& frustum, NzNode* node)
{
	for (NzNode* child : node->GetChilds())
	{
		if (child->GetNodeType() == nzNodeType_Scene)
		{
			NzSceneNode* sceneNode = static_cast<NzSceneNode*>(child);
			///TODO: Empêcher le rendu des enfants si le parent est cullé selon un flag
			sceneNode->UpdateVisibility(frustum);
			if (sceneNode->IsVisible())
				sceneNode->AddToRenderQueue(renderQueue);
		}

		if (child->HasChilds())
			RecursiveFrustumCull(renderQueue, frustum, child);
	}
}

void NzScene::SetActiveCamera(const NzCamera* camera)
{
	m_impl->activeCamera = camera;
}
