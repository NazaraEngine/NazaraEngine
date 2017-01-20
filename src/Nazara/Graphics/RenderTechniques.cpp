// Copyright (C) 2017 Jérôme Leclercq
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

		static_assert(sizeof(techniquesName) / sizeof(const char*) == RenderTechniqueType_Max + 1, "Render technique type name array is incomplete");

		struct RenderTechnique
		{
			RenderTechniques::RenderTechniqueFactory factory;
			int ranking;
		};

		std::unordered_map<String, RenderTechnique> s_renderTechniques;
	}

	/*!
	* \ingroup graphics
	* \class Nz::RenderTechniques
	* \brief Graphics class that represents the techniques used in rendering
	*/

	/*!
	* \brief Gets the technique by enumeration
	* \return A reference to the newly created technique
	*
	* \param renderTechnique Enumeration of the technique
	* \param techniqueRanking Ranking for the technique
	*
	* \remark Produces a NazaraError if renderTechnique does not exist
	*/

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

	/*!
	* \brief Gets the technique by index
	* \return A reference to the newly created technique
	*
	* \param index Index of the technique
	* \param techniqueRanking Ranking for the technique
	*
	* \remark Produces a NazaraError if index is out or range
	*/

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

	/*!
	* \brief Gets the technique by name
	* \return A reference to the newly created technique
	*
	* \param name Name of the technique
	* \param techniqueRanking Ranking for the technique
	*
	* \remark Produces a NazaraError if name does not exist or is invalid
	*/

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

	/*!
	* \brief Gets the technique by ranking
	* \return A reference to the newly created technique
	*
	* \param maxRanking Ranking maximum of the technique
	* \param techniqueRanking Ranking for the technique
	*
	* \remark Produces a NazaraError if name does not exist or is invalid
	*/

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

	/*!
	* \brief Gets the number of techniques available
	* \return Number of techniques
	*/

	std::size_t RenderTechniques::GetCount()
	{
		return s_renderTechniques.size();
	}

	/*!
	* \brief Registers a technique
	*
	* \param name Name of the technique
	* \param ranking Ranking of the technique
	* \param factory Factory to create the technique
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if name is empty
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if ranking is negative
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if factory is invalid is invalid
	*/

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

	/*!
	* \brief Converts the enumeration to string
	* \return String symbolizing the technique
	*
	* \param renderTechnique Enumeration of the technique
	*
	* \remark Produces a NazaraError if renderTechnique does not exist and returns "Error"
	*/

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

	/*!
	* \brief Unregisters a technique
	*
	* \param name Name of the technique
	*/

	void RenderTechniques::Unregister(const String& name)
	{
		s_renderTechniques.erase(name);
	}
}
