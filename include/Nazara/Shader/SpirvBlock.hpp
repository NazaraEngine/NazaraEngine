// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVBLOCK_HPP
#define NAZARA_SPIRVBLOCK_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/SpirvSectionBase.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_SHADER_API SpirvBlock : public SpirvSectionBase
	{
		public:
			inline SpirvBlock(SpirvWriter& writer);
			SpirvBlock(const SpirvBlock&) = default;
			SpirvBlock(SpirvBlock&&) = default;
			~SpirvBlock() = default;

			inline std::size_t Append(SpirvOp opcode, const OpSize& wordCount);
			template<typename... Args> std::size_t Append(SpirvOp opcode, Args&&... args);
			template<typename F> std::size_t AppendVariadic(SpirvOp opcode, F&& callback);

			inline UInt32 GetLabelId() const;

			inline bool IsTerminated() const;

			SpirvBlock& operator=(const SpirvBlock&) = delete;
			SpirvBlock& operator=(SpirvBlock&&) = default;

			static inline bool IsTerminationInstruction(SpirvOp op);

		private:
			inline void HandleSpirvOp(SpirvOp op);

			UInt32 m_labelId;
			bool m_isTerminated;
	};
}

#include <Nazara/Shader/SpirvBlock.inl>

#endif
