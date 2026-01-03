// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_COMMANDLINEPARAMETERS_HPP
#define NAZARA_CORE_COMMANDLINEPARAMETERS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <NazaraUtils/TypeTraits.hpp>
#include <unordered_map>
#include <unordered_set>

namespace Nz
{
	class CommandLineParameters
	{
		public:
			inline bool GetParameter(std::string_view name, std::string_view* value) const;

			inline bool HasFlag(std::string_view flag) const;

			inline bool operator==(const CommandLineParameters& params) const;
			inline bool operator!=(const CommandLineParameters& params) const;

			static inline CommandLineParameters Parse(int argc, char** argv);
			static inline CommandLineParameters Parse(int argc, const Pointer<const char>* argv);

		private:
			std::unordered_map<std::string, std::string, StringHash<>, std::equal_to<>> m_parameters;
			std::unordered_set<std::string, StringHash<>, std::equal_to<>> m_flags;
	};
}

#include <Nazara/Core/CommandLineParameters.inl>

#endif // NAZARA_CORE_COMMANDLINEPARAMETERS_HPP
