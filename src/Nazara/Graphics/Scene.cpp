// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <functional>
#include <Nazara/Graphics/Debug.hpp>

NzScene::NzScene() :
m_ambientColor(25, 25, 25),
m_root(this),
m_viewer(nullptr),
m_backgroundEnabled(true),
m_update(false),
m_updatePerSecond(60)
{
}

void NzScene::AddToVisibilityList(NzUpdatable* object)
{
	m_visibleUpdateList.push_back(object);
}

void NzScene::Clear()
{
	m_nodeMap.clear();
	m_nodes.clear();
}

void NzScene::Cull()
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_viewer)
	{
		NazaraError("No viewer");
		return;
	}
	#endif

	NzAbstractRenderQueue* renderQueue = GetRenderTechnique()->GetRenderQueue();
	renderQueue->Clear(false);

	m_visibleUpdateList.clear();

	// Frustum culling
	RecursiveFrustumCull(renderQueue, m_viewer->GetFrustum(), &m_root);

	///TODO: Occlusion culling
}

void NzScene::Draw()
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_viewer)
	{
		NazaraError("No viewer");
		return;
	}
	#endif

	m_viewer->ApplyView();

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException, true);
		m_renderTechnique->Clear(this);
		m_renderTechnique->Draw(this);
	}
	catch (const std::exception& e)
	{
		NzString oldName = m_renderTechnique->GetName();

		if (m_renderTechniqueRanking > 0)
		{
			m_renderTechnique.reset(NzRenderTechniques::GetByRanking(m_renderTechniqueRanking-1, &m_renderTechniqueRanking));
			NazaraError("Render technique \"" + oldName + "\" failed (" + NzString(e.what()) + "), falling back to \"" + m_renderTechnique->GetName() + '"');
		}
		else
		{
			NzErrorFlags errFlags(nzErrorFlag_ThrowException);
			NazaraError("Render technique \"" + oldName + "\" failed (" + NzString(e.what()) + ") and no fallback is available");
		}

		return;
	}
}

void NzScene::EnableBackground(bool enable)
{
	m_backgroundEnabled = enable;
}

NzSceneNode* NzScene::FindNode(const NzString& name)
{
	auto it = m_nodeMap.find(name);
	if (it == m_nodeMap.end())
		return nullptr;

	return it->second;
}

const NzSceneNode* NzScene::FindNode(const NzString& name) const
{
	auto it = m_nodeMap.find(name);
	if (it == m_nodeMap.end())
		return nullptr;

	return it->second;
}

NzColor NzScene::GetAmbientColor() const
{
	return m_ambientColor;
}

NzAbstractBackground* NzScene::GetBackground() const
{
	if (!m_background)
		m_background.reset(new NzColorBackground);

	return m_background.get();
}

NzVector3f NzScene::GetBackward() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_viewer)
	{
		NazaraError("No viewer");
		return NzVector3f::Backward();
	}
	#endif

	return -m_viewer->GetGlobalForward();
}

NzVector3f NzScene::GetDown() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_viewer)
	{
		NazaraError("No viewer");
		return NzVector3f::Down();
	}
	#endif

	return -m_viewer->GetGlobalUp();
}

NzVector3f NzScene::GetForward() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_viewer)
	{
		NazaraError("No viewer");
		return NzVector3f::Forward();
	}
	#endif

	return m_viewer->GetGlobalForward();
}

NzVector3f NzScene::GetLeft() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_viewer)
	{
		NazaraError("No viewer");
		return NzVector3f::Left();
	}
	#endif

	return -m_viewer->GetGlobalRight();
}

NzAbstractRenderTechnique* NzScene::GetRenderTechnique() const
{
	if (!m_renderTechnique)
		m_renderTechnique.reset(NzRenderTechniques::GetByRanking(-1, &m_renderTechniqueRanking));

	return m_renderTechnique.get();
}

NzVector3f NzScene::GetRight() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_viewer)
	{
		NazaraError("No viewer");
		return NzVector3f::Right();
	}
	#endif

	return m_viewer->GetGlobalRight();
}

NzSceneNode& NzScene::GetRoot()
{
	return m_root;
}

const NzSceneNode& NzScene::GetRoot() const
{
	return m_root;
}

NzAbstractViewer* NzScene::GetViewer() const
{
	return m_viewer;
}

NzVector3f NzScene::GetUp() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_viewer)
	{
		NazaraError("No viewer");
		return NzVector3f::Up();
	}
	#endif

	return m_viewer->GetGlobalUp();
}

float NzScene::GetUpdateTime() const
{
	return m_updateTime;
}

unsigned int NzScene::GetUpdatePerSecond() const
{
	return m_updatePerSecond;
}

bool NzScene::IsBackgroundEnabled() const
{
	return m_backgroundEnabled;
}

void NzScene::RegisterForUpdate(NzUpdatable* object)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!object)
	{
		NazaraError("Invalid object");
		return;
	}
	#endif

	m_updateList.push_back(object);
}

void NzScene::RemoveNode(NzSceneNode* node)
{
	if (!node)
		return;

	// C'est moche mais je n'ai pas d'autre choix que d'utiliser un std::unique_ptr pour utiliser std::find
	std::unique_ptr<NzSceneNode> ptr(node);
	auto it = std::find(m_nodes.begin(), m_nodes.end(), ptr);
	ptr.release();

	if (it == m_nodes.end())
	{
		NazaraError("This scene node doesn't belong to this scene");
		return;
	}

	NzString nodeName = node->GetName();
	if (!nodeName.IsEmpty())
		m_nodeMap.erase(nodeName);

	m_nodes.erase(it);
}

void NzScene::RemoveNode(const NzString& name)
{
	RemoveNode(FindNode(name));
}

void NzScene::RenderFrame()
{
	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException, true);
		Update();
		Cull();
		UpdateVisible();
		Draw();
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to render frame: " + NzString(e.what()));
	}
}

void NzScene::SetAmbientColor(const NzColor& color)
{
	m_ambientColor = color;
}

void NzScene::SetBackground(NzAbstractBackground* background)
{
	m_background.reset(background);
}

void NzScene::SetRenderTechnique(NzAbstractRenderTechnique* renderTechnique)
{
	m_renderTechnique.reset(renderTechnique);
}

void NzScene::SetViewer(NzAbstractViewer* viewer)
{
	if (m_viewer != viewer)
	{
		m_viewer = viewer;

		// Invalidation de tous les nodes de la scène (utile pour la régénération des sommets dépendant du viewer)
		m_root.InvalidateNode();
	}
}

void NzScene::SetViewer(NzAbstractViewer& viewer)
{
	SetViewer(&viewer);
}

void NzScene::SetUpdatePerSecond(unsigned int updatePerSecond)
{
	m_updatePerSecond = updatePerSecond;
}

void NzScene::UnregisterForUpdate(NzUpdatable* object)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!object)
	{
		NazaraError("Invalid object");
		return;
	}
	#endif

	auto it = std::find(m_updateList.begin(), m_updateList.end(), object);
	if (it != m_updateList.end())
		m_updateList.erase(it);
}

void NzScene::Update()
{
	m_update = (m_updatePerSecond == 0 || m_updateClock.GetMilliseconds() > 1000/m_updatePerSecond);
	if (m_update)
	{
		m_updateTime = m_updateClock.GetSeconds();
		m_updateClock.Restart();

		for (NzUpdatable* updatable : m_updateList)
			///TODO: Multihreading
			updatable->Update();
	}
}

void NzScene::UpdateVisible()
{
	NzSkinningManager::Skin();

	if (m_update)
	{
		for (NzUpdatable* node : m_visibleUpdateList)
			node->Update();
	}
}

NzScene::operator const NzSceneNode&() const
{
	return m_root;
}

bool NzScene::ChangeNodeName(NzSceneNode* node, const NzString& newName)
{
	#ifdef NAZARA_DEBUG
	std::unique_ptr<NzSceneNode> ptr(node);
	auto it = std::find(m_nodes.begin(), m_nodes.end(), ptr);
	ptr.release();

	if (it == m_nodes.end())
	{
		NazaraInternalError("Node isn't part of the scene");
		return false;
	}
	#endif

	if (!newName.IsEmpty())
	{
		auto pair = m_nodeMap.insert(std::make_pair(newName, node));
		if (!pair.second)
		{
			NazaraError("Name \"" + newName + "\" is already in use");
			return false;
		}
	}

	NzString oldName = node->GetName();
	if (!oldName.IsEmpty())
		m_nodeMap.erase(oldName);

	node->SetNameInternal(newName);
	return true;
}

bool NzScene::RegisterSceneNode(const NzString& name, NzSceneNode* node)
{
	if (!name.IsEmpty())
	{
		if (m_nodeMap.find(name) != m_nodeMap.end())
		{
			NazaraError("Node " + name + " is already registred");
			return false;
		}

		m_nodeMap[name] = node;
	}

	node->SetNameInternal(name);
	node->SetParent(m_root, true);

	m_nodes.emplace_back(node);
	return true;
}

void NzScene::RecursiveFrustumCull(NzAbstractRenderQueue* renderQueue, const NzFrustumf& frustum, NzNode* node)
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
