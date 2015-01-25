// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENE_HPP
#define NAZARA_SCENE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Updatable.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/SceneRoot.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

class NzAbstractRenderQueue;
class NzAbstractViewer;
class NzNode;
class NzSceneNode;

class NAZARA_API NzScene
{
	friend NzSceneNode;

	public:
		NzScene();
		~NzScene() = default;

		void AddToVisibilityList(NzUpdatable* object);

		template<typename T, typename... Args> T* CreateNode(Args&&... args);
		template<typename T, typename... Args> T* CreateNode(const NzString& name, Args&&... args);
		template<typename T, typename... Args> T* CreateNode(const NzString& name, const NzString& templateNodeName);

		void Clear();

		void Cull();
		void Draw();

		void EnableBackground(bool enable);

		NzSceneNode* FindNode(const NzString& name);
		const NzSceneNode* FindNode(const NzString& name) const;
		template<typename T> T* FindNodeAs(const NzString& name);
		template<typename T> const T* FindNodeAs(const NzString& name) const;

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

		void RemoveNode(NzSceneNode* node);
		void RemoveNode(const NzString& name);

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
		bool ChangeNodeName(NzSceneNode* node, const NzString& newName);
		bool RegisterSceneNode(const NzString& name, NzSceneNode* node);
		void RecursiveFrustumCull(NzAbstractRenderQueue* renderQueue, const NzFrustumf& frustum, NzNode* node);

		mutable std::unique_ptr<NzAbstractBackground> m_background;
		mutable std::unique_ptr<NzAbstractRenderTechnique> m_renderTechnique;
		std::unordered_map<NzString, NzSceneNode*> m_nodeMap;
		std::vector<std::unique_ptr<NzSceneNode>> m_nodes;
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

#include <Nazara/Graphics/Scene.inl>

#endif // NAZARA_SCENE_HPP
