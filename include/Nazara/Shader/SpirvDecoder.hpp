// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVDECODER_HPP
#define NAZARA_SPIRVDECODER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <functional>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_SHADER_API SpirvDecoder
	{
		public:
			SpirvDecoder() = default;
			SpirvDecoder(const SpirvDecoder&) = default;
			SpirvDecoder(SpirvDecoder&&) = default;
			~SpirvDecoder() = default;

			void Decode(const UInt32* codepoints, std::size_t count);

			SpirvDecoder& operator=(const SpirvDecoder&) = default;
			SpirvDecoder& operator=(SpirvDecoder&&) = default;

		protected:
			struct SpirvHeader;

			inline const UInt32* GetCurrentPtr() const;

			virtual bool HandleHeader(const SpirvHeader& header);
			virtual bool HandleOpcode(const SpirvInstruction& instruction, UInt32 wordCount) = 0;

			std::string ReadString();
			UInt32 ReadWord();

			struct SpirvHeader
			{
				UInt32 generatorId;
				UInt32 bound;
				UInt32 schema;
				UInt32 versionNumber;
			};

		private:
			const UInt32* m_currentCodepoint;
			const UInt32* m_codepointEnd;
	};
}

#include <Nazara/Shader/SpirvDecoder.inl>

#endif
