// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LIGHTMANAGER_HPP
#define NAZARA_LIGHTMANAGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>

class NzLight;

class NAZARA_API NzLightManager
{
	public:
		NzLightManager();
		NzLightManager(const NzLight** lights, unsigned int lightCount);
		~NzLightManager() = default;

		void AddLights(const NzLight** lights, unsigned int lightCount);

		void Clear();

		unsigned int ComputeClosestLights(const NzVector3f& position, float squaredRadius, unsigned int maxResults);

		const NzLight* GetLight(unsigned int index) const;
		unsigned int GetLightCount() const;
		const NzLight* GetResult(unsigned int i) const;

		bool IsEmpty() const;

		void SetLights(const NzLight** lights, unsigned int lightCount);

	private:
		struct Light
		{
			const NzLight* light;
			unsigned int score;
		};

		std::vector<std::pair<const NzLight**, unsigned int>> m_lights;
		std::vector<Light> m_results;
		unsigned int m_lightCount;
};

#endif // NAZARA_LIGHTMANAGER_HPP
