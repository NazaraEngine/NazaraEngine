// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <limits>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const char* techniquesName[] =
		{
			"Advanced Forward",
			"Basic Forward",
			"Deferred Shading",
			"Depth Pass",
			"Light Pre-Pass",
			"User"
		};

		static_assert(sizeof(techniquesName)/sizeof(const char*) == RenderTechniqueType_Max+1, "Render technique type name array is incomplete");

		struct RenderTechnique
		{
			RenderTechniques::RenderTechniqueFactory factory;
			int ranking;
		};

		std::unordered_map<String, RenderTechnique> s_renderTechniques;
	}

	AbstractRenderTechnique* RenderTechniques::GetByEnum(RenderTechniqueType renderTechnique, int* techniqueRanking)
	{
		#ifdef NAZARA_DEBUG
		if (renderTechnique > RenderTechniqueType_Max)
		{
			NazaraError("Render technique type out of enum");
			return nullptr;
		}
		#endif

		return GetByName(techniquesName[renderTechnique], techniqueRanking);
	}

	AbstractRenderTechnique* RenderTechniques::GetByIndex(unsigned int index, int* techniqueRanking)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (index >= s_renderTechniques.size())
		{
			NazaraError("Technique index out of range (" + String::Number(index) + " >= " + String::Number(s_renderTechniques.size()) + ')');
			return nullptr;
		}
		#endif

		auto it = s_renderTechniques.begin();
		std::advance(it, index);

		if (techniqueRanking)
			*techniqueRanking = it->second.ranking;

		return it->second.factory();
	}

	AbstractRenderTechnique* RenderTechniques::GetByName(const String& name, int* techniqueRanking)
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

	AbstractRenderTechnique* RenderTechniques::GetByRanking(int maxRanking, int* techniqueRanking)
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

	unsigned int RenderTechniques::GetCount()
	{
		return s_renderTechniques.size();
	}

	void RenderTechniques::Register(const String& name, int ranking, RenderTechniqueFactory factory)
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

	String RenderTechniques::ToString(RenderTechniqueType renderTechnique)
	{
		#ifdef NAZARA_DEBUG
		if (renderTechnique > RenderTechniqueType_Max)
		{
			NazaraError("Render technique type out of enum");
			return String("Error");
		}
		#endif

		return techniquesName[renderTechnique];
	}

	void RenderTechniques::Unregister(const String& name)
	{
		s_renderTechniques.erase(name);
	}
}
