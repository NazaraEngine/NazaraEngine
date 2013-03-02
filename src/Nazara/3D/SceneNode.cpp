// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/SceneNode.hpp>
#include <Nazara/3D/Scene.hpp>
#include <Nazara/3D/Debug.hpp>

NzSceneNode::NzSceneNode() :
m_scene(nullptr)
{
}

NzSceneNode::NzSceneNode(const NzSceneNode& node) :
NzNode(node),
m_scene(node.m_scene)
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

		for (NzNode* child : m_childs)
		{
			if (child->GetNodeType() == nzNodeType_Scene)
				static_cast<NzSceneNode*>(child)->SetScene(scene);
		}
	}
}

bool NzSceneNode::ShouldUpdateWhenVisible()
{
	return false;
}

void NzSceneNode::Unregister()
{
}

void NzSceneNode::Update()
{
}
