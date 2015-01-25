// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENEROOT_HPP
#define NAZARA_SCENEROOT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/SceneNode.hpp>

class NzScene;

class NAZARA_API NzSceneRoot : public NzSceneNode
{
	friend NzScene;

	public:
		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		nzSceneNodeType GetSceneNodeType() const override;

		bool IsDrawable() const;

	private:
		NzSceneRoot(NzScene* scene);
		virtual ~NzSceneRoot();

		NzSceneRoot* Clone() const;
		NzSceneRoot* Create() const;

		void MakeBoundingVolume() const override;
		void Register();
		void Unregister();
};

#endif // NAZARA_SCENEROOT_HPP
