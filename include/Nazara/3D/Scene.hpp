// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENE_HPP
#define NAZARA_SCENE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/2D/Background.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Updatable.hpp>
#include <Nazara/Math/Frustum.hpp>

class NzCamera;
class NzLight;
class NzModel;
class NzNode;
class NzRenderQueue;
class NzSceneNode;
struct NzSceneImpl;

class NAZARA_API NzScene
{
	friend NzCamera;

	public:
		NzScene();
		~NzScene();

		void AddToVisibilityList(NzUpdatable* object);

		void Cull();
		void Draw();

		NzBackground* GetBackground() const;
		NzSceneNode& GetRoot() const;
		float GetUpdateTime() const;
		unsigned int GetUpdatePerSecond() const;

		void RegisterForUpdate(NzUpdatable* object);

		void SetAmbientColor(const NzColor& color);
		void SetBackground(NzBackground* background);
		void SetUpdatePerSecond(unsigned int updatePerSecond);

		void UnregisterForUpdate(NzUpdatable* object);

		void Update();
		void UpdateVisible();

		operator const NzSceneNode&() const;

	private:
		void RecursiveFrustumCull(NzRenderQueue& renderQueue, const NzFrustumf& frustum, NzNode* node);
		void SetActiveCamera(const NzCamera* camera);

		NzSceneImpl* m_impl;
};

#endif // NAZARA_SCENE_HPP
