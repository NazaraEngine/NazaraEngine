// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/LightManager.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzLightManager::NzLightManager() :
m_lightCount(0)
{
}

NzLightManager::NzLightManager(const NzLight** lights, unsigned int lightCount)
{
	SetLights(lights, lightCount);
}

void NzLightManager::AddLights(const NzLight** lights, unsigned int lightCount)
{
	m_lights.push_back(std::make_pair(lights, lightCount));
	m_lightCount += lightCount;
}

void NzLightManager::Clear()
{
	m_lights.clear();
	m_lightCount = 0;
}

unsigned int NzLightManager::ComputeClosestLights(const NzVector3f& position, float squaredRadius, unsigned int maxResults)
{
	m_results.resize(maxResults);
	for (Light& light : m_results)
	{
		light.light = nullptr;
		light.score = std::numeric_limits<unsigned int>::max(); // Nous jouons au Golf
	}

	for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		const NzLight** lights = it->first;
		unsigned int lightCount = it->second;

		for (unsigned int j = 0; j < lightCount; ++j)
		{
			const NzLight* light = *lights++;

			unsigned int score = std::numeric_limits<unsigned int>::max();
			switch (light->GetLightType())
			{
				case nzLightType_Directional:
					score = 0; // Lumière choisie d'office
					break;

				case nzLightType_Point:
				{
					float lightRadius = light->GetRadius();

					float squaredDistance = position.SquaredDistance(light->GetPosition());
					if (squaredDistance - squaredRadius <= lightRadius*lightRadius)
						score = static_cast<unsigned int>(squaredDistance*1000.f);

					break;
				}

				case nzLightType_Spot:
				{
					float lightRadius = light->GetRadius();

					///TODO: Attribuer bonus/malus selon l'angle du spot ?
					float squaredDistance = position.SquaredDistance(light->GetPosition());
					if (squaredDistance - squaredRadius <= lightRadius*lightRadius)
						score = static_cast<unsigned int>(squaredDistance*1000.f);

					break;
				}
			}

			if (score < m_results[0].score)
			{
				unsigned int k;
				for (k = 1; k < maxResults; ++k)
				{
					if (score > m_results[k].score)
						break;
				}

				k--; // Position de la nouvelle lumière

				// Décalage
				std::memcpy(&m_results[0], &m_results[1], k*sizeof(Light));

				m_results[k].light = light;
				m_results[k].score = score;
			}
		}
	}

	unsigned int i;
	for (i = 0; i < maxResults; ++i)
	{
		if (m_results[i].light)
			break;
	}

	return maxResults-i;
}

const NzLight* NzLightManager::GetLight(unsigned int index) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (index >= m_lightCount)
	{
		NazaraError("Light index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_lightCount) + ')');
		return nullptr;
	}
	#endif

	for (unsigned int i = 0; i < m_lights.size(); ++i)
	{
		unsigned int lightCount = m_lights[i].second;
		if (index > lightCount)
			index -= lightCount;
		else
		{
			const NzLight** lights = m_lights[i].first;
			return lights[i];
		}
	}

	#if NAZARA_GRAPHICS_SAFE
	NazaraInternalError("Light not found");
	#else
	NazaraError("Light not found");
	#endif

	return nullptr;
}

unsigned int NzLightManager::GetLightCount() const
{
	return m_lightCount;
}

const NzLight* NzLightManager::GetResult(unsigned int i) const
{
	return m_results[m_results.size()-i-1].light;
}

bool NzLightManager::IsEmpty() const
{
	return m_lightCount == 0;
}

void NzLightManager::SetLights(const NzLight** lights, unsigned int lightCount)
{
	Clear();
	AddLights(lights, lightCount);
}
