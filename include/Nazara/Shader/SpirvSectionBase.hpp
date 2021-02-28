// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVSECTIONBASE_HPP
#define NAZARA_SPIRVSECTIONBASE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/ShaderEnums.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_SHADER_API SpirvSectionBase
	{
		public:
			struct OpSize;
			struct Raw;

			SpirvSectionBase() = default;
			SpirvSectionBase(const SpirvSectionBase&) = default;
			SpirvSectionBase(SpirvSectionBase&&) = default;
			~SpirvSectionBase() = default;

			inline const std::vector<UInt32>& GetBytecode() const;
			inline std::size_t GetOutputOffset() const;

			SpirvSectionBase& operator=(const SpirvSectionBase&) = delete;
			SpirvSectionBase& operator=(SpirvSectionBase&&) = default;

			struct OpSize
			{
				unsigned int wc;
			};

			struct Raw
			{
				const void* ptr;
				std::size_t size;
			};

			static inline UInt32 BuildOpcode(SpirvOp opcode, unsigned int wordCount);

		protected:
			inline std::size_t Append(SpirvOp opcode, const OpSize& wordCount);
			template<typename... Args> std::size_t Append(SpirvOp opcode, const Args&... args);
			template<typename F> std::size_t AppendVariadic(SpirvOp opcode, F&& callback);
			inline std::size_t AppendRaw(const char* str);
			inline std::size_t AppendRaw(const std::string_view& str);
			inline std::size_t AppendRaw(const std::string& str);
			inline std::size_t AppendRaw(UInt32 value);
			std::size_t AppendRaw(const Raw& raw);
			inline std::size_t AppendRaw(std::initializer_list<UInt32> codepoints);
			inline std::size_t AppendSection(const SpirvSectionBase& section);
			template<typename T, typename = std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>>> std::size_t AppendRaw(T value);

			inline unsigned int CountWord(const char* str);
			inline unsigned int CountWord(const std::string_view& str);
			inline unsigned int CountWord(const std::string& str);
			inline unsigned int CountWord(const Raw& raw);
			template<typename T, typename = std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>>> unsigned int CountWord(const T& value);
			template<typename T1, typename T2, typename... Args> unsigned int CountWord(const T1& value, const T2& value2, const Args&... rest);

		private:
			std::vector<UInt32> m_bytecode;
	};
}

#include <Nazara/Shader/SpirvSectionBase.inl>

#endif
