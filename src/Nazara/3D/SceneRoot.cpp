// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/SceneRoot.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/3D/Debug.hpp>

NzSceneRoot::NzSceneRoot(NzScene* scene)
{
	m_scene = scene;
}

NzSceneRoot::~NzSceneRoot() = default;

void NzSceneRoot::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
	NazaraUnused(renderQueue);

	NazaraInternalError("SceneNode::AddToRenderQueue() called on SceneRoot");
}

const NzBoundingBoxf& NzSceneRoot::GetBoundingBox() const
{
	static NzBoundingBoxf infinite(nzExtend_Infinite);
	return infinite;
}

nzSceneNodeType NzSceneRoot::GetSceneNodeType() const
{
	return nzSceneNodeType_Root;
}

void NzSceneRoot::Register()
{
	NazaraInternalError("SceneNode::Register() called on SceneRoot");
}

void NzSceneRoot::Unregister()
{
	NazaraInternalError("SceneNode::Unregister() called on SceneRoot");
}

bool NzSceneRoot::VisibilityTest(const NzFrustumf& frustum)
{
	NazaraUnused(frustum);

	return true; // Toujours visible
}
