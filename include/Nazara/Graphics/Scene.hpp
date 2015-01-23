// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENE_HPP
#define NAZARA_SCENE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Updatable.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/SceneRoot.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <memory>
#include <vector>

class NzAbstractRenderQueue;
class NzAbstractViewer;
class NzCamera;
class NzLight;
class NzModel;
class NzNode;
class NzRenderQueue;
class NzSceneNode;

class NAZARA_API NzScene
{
	friend NzCamera;

	public:
		NzScene();
		~NzScene() = default;

		void AddToVisibilityList(NzUpdatable* object);

		void Cull();
		void Draw();

		void EnableBackground(bool enable);

		NzColor GetAmbientColor() const;
		NzAbstractBackground* GetBackground() const;
		NzVector3f GetBackward() const;
		NzVector3f GetDown() const;
		NzVector3f GetForward() const;
		NzVector3f GetLeft() const;
		NzAbstractRenderTechnique* GetRenderTechnique() const;
		NzVector3f GetRight() const;
		NzSceneNode& GetRoot();
		const NzSceneNode& GetRoot() const;
		NzAbstractViewer* GetViewer() const;
		NzVector3f GetUp() const;
		float GetUpdateTime() const;
		unsigned int GetUpdatePerSecond() const;

		bool IsBackgroundEnabled() const;

		void RegisterForUpdate(NzUpdatable* object);

		void RenderFrame();

		void SetAmbientColor(const NzColor& color);
		void SetBackground(NzAbstractBackground* background);
		void SetRenderTechnique(NzAbstractRenderTechnique* renderTechnique);
		void SetViewer(NzAbstractViewer* viewer);
		void SetViewer(NzAbstractViewer& viewer);
		void SetUpdatePerSecond(unsigned int updatePerSecond);

		void UnregisterForUpdate(NzUpdatable* object);

		void Update();
		void UpdateVisible();

		operator const NzSceneNode&() const;

	private:
		void RecursiveFrustumCull(NzAbstractRenderQueue* renderQueue, const NzFrustumf& frustum, NzNode* node);

		mutable std::unique_ptr<NzAbstractBackground> m_background;
		mutable std::unique_ptr<NzAbstractRenderTechnique> m_renderTechnique;
		std::vector<NzUpdatable*> m_updateList;
		std::vector<NzUpdatable*> m_visibleUpdateList;
		NzClock m_updateClock;
		NzColor m_ambientColor;
		NzSceneRoot m_root;
		NzAbstractViewer* m_viewer;
		bool m_backgroundEnabled;
		bool m_update;
		float m_frameTime;
		float m_updateTime;
		mutable int m_renderTechniqueRanking;
		unsigned int m_updatePerSecond;
};

#endif // NAZARA_SCENE_HPP
