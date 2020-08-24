// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVPRINTER_HPP
#define NAZARA_SPIRVPRINTER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <string>

namespace Nz
{
	class NAZARA_SHADER_API SpirvPrinter
	{
		public:
			struct Settings;

			inline SpirvPrinter();
			SpirvPrinter(const SpirvPrinter&) = default;
			SpirvPrinter(SpirvPrinter&&) = default;
			~SpirvPrinter() = default;

			std::string Print(const UInt32* codepoints, std::size_t count, const Settings& settings = Settings());

			SpirvPrinter& operator=(const SpirvPrinter&) = default;
			SpirvPrinter& operator=(SpirvPrinter&&) = default;

			struct Settings
			{
				bool printHeader = true;
				bool printParameters = true;
			};

		private:
			void AppendInstruction();
			std::string ReadString();
			UInt32 ReadWord();

			struct State;

			State* m_currentState;
	};
}

#include <Nazara/Shader/SpirvPrinter.inl>

#endif
