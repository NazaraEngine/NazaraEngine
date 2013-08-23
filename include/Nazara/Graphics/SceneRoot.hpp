// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENEROOT_HPP
#define NAZARA_SCENEROOT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/SceneNode.hpp>

class NAZARA_API NzSceneRoot : public NzSceneNode
{
	friend struct NzSceneImpl;

	public:
		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		const NzBoundingVolumef& GetBoundingVolume() const override;
		nzSceneNodeType GetSceneNodeType() const override;

		bool IsDrawable() const;

	private:
		NzSceneRoot(NzScene* scene);
		virtual ~NzSceneRoot();

		bool FrustumCull(const NzFrustumf& frustum) override;
		void Register();
		void Unregister();
};

#endif // NAZARA_SCENEROOT_HPP
