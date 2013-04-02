// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzSceneNode::NzSceneNode() :
m_scene(nullptr),
m_visible(false)
{
}

NzSceneNode::NzSceneNode(const NzSceneNode& node) :
NzNode(node),
m_scene(node.m_scene),
m_visible(false)
{
}

NzSceneNode::~NzSceneNode() = default;

nzNodeType NzSceneNode::GetNodeType() const
{
	return nzNodeType_Scene;
}

NzScene* NzSceneNode::GetScene() const
{
	return m_scene;
}

bool NzSceneNode::IsVisible() const
{
	return m_visible;
}

void NzSceneNode::OnParenting(const NzNode* parent)
{
	if (parent)
	{
		if (parent->GetNodeType() == nzNodeType_Scene)
			SetScene(static_cast<const NzSceneNode*>(parent)->m_scene);
	}
	else
		SetScene(nullptr);
}

void NzSceneNode::OnVisibilityChange(bool visibility)
{
	NazaraUnused(visibility);

	///TODO: Envoyer l'évènements aux listeners
}

void NzSceneNode::RecursiveSetScene(NzScene* scene, NzNode* node)
{
	for (NzNode* child : node->GetChilds())
	{
		if (child->GetNodeType() == nzNodeType_Scene)
		{
			NzSceneNode* sceneNode = static_cast<NzSceneNode*>(child);
			sceneNode->SetScene(scene);
		}

		if (node->HasChilds())
			RecursiveSetScene(scene, node);
	}
}

void NzSceneNode::Register()
{
}

void NzSceneNode::SetScene(NzScene* scene)
{
	if (m_scene != scene)
	{
		if (m_scene)
			Unregister();

		m_scene = scene;
		if (m_scene)
			Register();

		RecursiveSetScene(scene, this);
	}
}

void NzSceneNode::Unregister()
{
}

void NzSceneNode::Update()
{
}

void NzSceneNode::UpdateVisibility(const NzFrustumf& frustum)
{
	bool wasVisible = m_visible;

	m_visible = VisibilityTest(frustum);

	if (m_visible != wasVisible)
		OnVisibilityChange(m_visible);
}
