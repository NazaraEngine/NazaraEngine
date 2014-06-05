// Copyright (C) 2014 Jérôme Leclercq
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

const NzBoundingVolumef& NzSceneRoot::GetBoundingVolume() const
{
	static NzBoundingVolumef infinite(nzExtend_Infinite);
	return infinite;
}

nzSceneNodeType NzSceneRoot::GetSceneNodeType() const
{
	return nzSceneNodeType_Root;
}

bool NzSceneRoot::IsDrawable() const
{
	return true;
}

void NzSceneRoot::Register()
{
	NazaraInternalError("SceneNode::Register() called on SceneRoot");
}

void NzSceneRoot::Unregister()
{
	NazaraInternalError("SceneNode::Unregister() called on SceneRoot");
}
