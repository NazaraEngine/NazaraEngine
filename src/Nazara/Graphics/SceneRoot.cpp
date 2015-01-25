// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SceneRoot.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzSceneRoot::NzSceneRoot(NzScene* scene)
{
	m_scene = scene;
}

NzSceneRoot::~NzSceneRoot() = default;

void NzSceneRoot::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	NazaraUnused(renderQueue);

	NazaraInternalError("SceneNode::AddToRenderQueue() called on SceneRoot");
}

nzSceneNodeType NzSceneRoot::GetSceneNodeType() const
{
	return nzSceneNodeType_Root;
}

bool NzSceneRoot::IsDrawable() const
{
	return true;
}

NzSceneRoot* NzSceneRoot::Clone() const
{
	NazaraInternalError("SceneNode::Clone() called on SceneRoot");
	return nullptr;
}

NzSceneRoot* NzSceneRoot::Create() const
{
	NazaraInternalError("SceneNode::Create() called on SceneRoot");
	return nullptr;
}

void NzSceneRoot::MakeBoundingVolume() const
{
	m_boundingVolume.MakeInfinite();
}

void NzSceneRoot::Register()
{
	NazaraInternalError("SceneNode::Register() called on SceneRoot");
}

void NzSceneRoot::Unregister()
{
	NazaraInternalError("SceneNode::Unregister() called on SceneRoot");
}
