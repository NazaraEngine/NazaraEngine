// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <limits>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	const char* techniquesName[] =
	{
		"Advanced Forward",
		"Basic Forward",
		"Deferred Shading",
		"Light Pre-Pass",
		"User"
	};

	static_assert(sizeof(techniquesName)/sizeof(const char*) == nzRenderTechniqueType_Max+1, "Render technique type name array is incomplete");

	struct RenderTechnique
	{
		NzRenderTechniques::RenderTechniqueFactory factory;
		int ranking;
	};

	std::unordered_map<NzString, RenderTechnique> s_renderTechniques;
}

NzAbstractRenderTechnique* NzRenderTechniques::GetByEnum(nzRenderTechniqueType renderTechnique, int* techniqueRanking)
{
	#ifdef NAZARA_DEBUG
	if (renderTechnique > nzRenderTechniqueType_Max)
	{
		NazaraError("Render technique type out of enum");
		return nullptr;
	}
	#endif

	return GetByName(techniquesName[renderTechnique], techniqueRanking);
}

NzAbstractRenderTechnique* NzRenderTechniques::GetByIndex(unsigned int index, int* techniqueRanking)
{
	#if NAZARA_GRAPHICS_SAFE
	if (index >= s_renderTechniques.size())
	{
		NazaraError("Technique index out of range (" + NzString::Number(index) + " >= " + NzString::Number(s_renderTechniques.size()) + ')');
		return nullptr;
	}
	#endif

	auto it = s_renderTechniques.begin();
	std::advance(it, index);

	if (techniqueRanking)
		*techniqueRanking = it->second.ranking;

	return it->second.factory();
}

NzAbstractRenderTechnique* NzRenderTechniques::GetByName(const NzString& name, int* techniqueRanking)
{
	#if NAZARA_GRAPHICS_SAFE
	if (name.IsEmpty())
	{
		NazaraError("Technique name cannot be empty");
		return nullptr;
	}
	#endif

	auto it = s_renderTechniques.find(name);
	if (it == s_renderTechniques.end())
	{
		NazaraError("Technique not found");
		return nullptr;
	}

	if (techniqueRanking)
		*techniqueRanking = it->second.ranking;

	return it->second.factory();
}

NzAbstractRenderTechnique* NzRenderTechniques::GetByRanking(int maxRanking, int* techniqueRanking)
{
	if (maxRanking < 0)
		maxRanking = std::numeric_limits<int>::max();

	int currentRanking = -1;
	RenderTechnique* technique = nullptr;

	for (auto it = s_renderTechniques.begin(); it != s_renderTechniques.end(); ++it)
	{
		int ranking = it->second.ranking;
		if (ranking > currentRanking && ranking <= maxRanking)
		{
			currentRanking = ranking;
			technique = &(it->second);
		}
	}

	if (!technique)
	{
		NazaraError("No technique found");
		return nullptr;
	}

	if (techniqueRanking)
		*techniqueRanking = currentRanking;

	return technique->factory();
}

unsigned int NzRenderTechniques::GetCount()
{
	return s_renderTechniques.size();
}

void NzRenderTechniques::Register(const NzString& name, int ranking, RenderTechniqueFactory factory)
{
	#if NAZARA_GRAPHICS_SAFE
	if (name.IsEmpty())
	{
		NazaraError("Technique name cannot be empty");
		return;
	}

	if (ranking < 0)
	{
		NazaraError("Technique ranking cannot be negative");
		return;
	}

	if (!factory)
	{
		NazaraError("Technique function must be valid");
		return;
	}
	#endif

	s_renderTechniques[name] = {factory, ranking};
}

NzString NzRenderTechniques::ToString(nzRenderTechniqueType renderTechnique)
{
	#ifdef NAZARA_DEBUG
	if (renderTechnique > nzRenderTechniqueType_Max)
	{
		NazaraError("Render technique type out of enum");
		return NzString("Error");
	}
	#endif

	return techniquesName[renderTechnique];
}

void NzRenderTechniques::Unregister(const NzString& name)
{
	s_renderTechniques.erase(name);
}
