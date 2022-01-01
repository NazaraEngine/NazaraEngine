// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SPIRVPRINTER_HPP
#define NAZARA_SHADER_SPIRVPRINTER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/SpirvDecoder.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_SHADER_API SpirvPrinter : SpirvDecoder
	{
		public:
			struct Settings;

			inline SpirvPrinter();
			SpirvPrinter(const SpirvPrinter&) = default;
			SpirvPrinter(SpirvPrinter&&) = default;
			~SpirvPrinter() = default;

			inline std::string Print(const std::vector<UInt32>& codepoints);
			inline std::string Print(const UInt32* codepoints, std::size_t count);
			inline std::string Print(const std::vector<UInt32>& codepoints, const Settings& settings);
			std::string Print(const UInt32* codepoints, std::size_t count, const Settings& settings);

			SpirvPrinter& operator=(const SpirvPrinter&) = default;
			SpirvPrinter& operator=(SpirvPrinter&&) = default;

			struct Settings
			{
				bool printHeader = true;
				bool printParameters = true;
			};

		private:
			bool HandleHeader(const SpirvHeader& header) override;
			bool HandleOpcode(const SpirvInstruction& instruction, UInt32 wordCount) override;

			struct State;

			State* m_currentState;
	};
}

#include <Nazara/Shader/SpirvPrinter.inl>

#endif // NAZARA_SHADER_SPIRVPRINTER_HPP
