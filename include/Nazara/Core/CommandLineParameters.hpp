// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_COMMANDLINEPARAMETERS_HPP
#define NAZARA_CORE_COMMANDLINEPARAMETERS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace Nz
{
	class CommandLineParameters
	{
		public:
			inline bool GetParameter(const std::string& name, std::string_view* value) const;

			inline bool HasFlag(const std::string& flag) const;

			inline bool operator==(const CommandLineParameters& params) const;
			inline bool operator!=(const CommandLineParameters& params) const;

			static inline CommandLineParameters Parse(int argc, char** argv);
			static inline CommandLineParameters Parse(int argc, const Pointer<const char>* argv);

		private:
			std::unordered_map<std::string, std::string> m_parameters;
			std::unordered_set<std::string> m_flags;
	};
}

#include <Nazara/Core/CommandLineParameters.inl>

#endif // NAZARA_CORE_COMMANDLINEPARAMETERS_HPP
