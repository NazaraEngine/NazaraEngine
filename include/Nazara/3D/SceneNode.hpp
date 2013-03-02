// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENENODE_HPP
#define NAZARA_SCENENODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/3D/Enums.hpp>
#include <Nazara/3D/RenderQueue.hpp>
#include <Nazara/3D/Scene.hpp>
#include <Nazara/Math/BoundingBox.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Utility/Node.hpp>

class NAZARA_API NzSceneNode : public NzNode
{
	friend class NzScene;

	public:
		NzSceneNode();
		NzSceneNode(const NzSceneNode& node);
		virtual ~NzSceneNode();

		virtual void AddToRenderQueue(NzRenderQueue& renderQueue) const = 0;

		virtual const NzBoundingBoxf& GetBoundingBox() const = 0;
		nzNodeType GetNodeType() const final;
		NzScene* GetScene() const;
		virtual nzSceneNodeType GetSceneNodeType() const = 0;

		virtual bool IsVisible(const NzFrustumf& frustum) const = 0;

	protected:
		virtual void OnParenting(const NzNode* parent) override;
		virtual void Register();
		void SetScene(NzScene* scene);
		virtual bool ShouldUpdateWhenVisible();
		virtual void Unregister();
		virtual void Update();

		NzScene* m_scene;
};

#endif // NAZARA_SCENENODE_HPP
