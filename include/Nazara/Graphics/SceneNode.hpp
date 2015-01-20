// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENENODE_HPP
#define NAZARA_SCENENODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Utility/Node.hpp>

class NzScene;

class NAZARA_API NzSceneNode : public NzNode
{
	friend class NzScene;

	public:
		NzSceneNode();
		NzSceneNode(const NzSceneNode& sceneNode);
		NzSceneNode(NzSceneNode& sceneNode) = delete;
		virtual ~NzSceneNode();

		virtual void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const = 0;

		void EnableDrawing(bool drawingEnabled);

		NzVector3f GetBackward() const;
		virtual const NzBoundingVolumef& GetBoundingVolume() const;
		NzVector3f GetDown() const;
		NzVector3f GetForward() const;
		NzVector3f GetLeft() const;
		nzNodeType GetNodeType() const final;
		NzVector3f GetRight() const;
		NzScene* GetScene() const;
		virtual nzSceneNodeType GetSceneNodeType() const = 0;
		NzVector3f GetUp() const;

		void InvalidateAABB();

		virtual bool IsDrawable() const = 0;
		bool IsDrawingEnabled() const;
		bool IsVisible() const;

		NzSceneNode& operator=(const NzSceneNode& sceneNode);
		NzSceneNode& operator=(NzSceneNode&& sceneNode) = delete;

	protected:
		virtual bool FrustumCull(const NzFrustumf& frustum) const;
		virtual void InvalidateNode() override;
		virtual void MakeBoundingVolume() const = 0;
		virtual void OnParenting(const NzNode* parent) override;
		virtual void OnVisibilityChange(bool visibility);
		void RecursiveSetScene(NzScene* scene, NzNode* node);
		virtual void Register();
		void SetScene(NzScene* scene);
		virtual void Unregister();
		virtual void Update();
		virtual void UpdateBoundingVolume() const;

		mutable NzBoundingVolumef m_boundingVolume;
		NzScene* m_scene;
		mutable bool m_boundingVolumeUpdated;
		bool m_drawingEnabled;
		bool m_visible;

	private:
		void UpdateVisibility(const NzFrustumf& frustum);
};

#endif // NAZARA_SCENENODE_HPP
