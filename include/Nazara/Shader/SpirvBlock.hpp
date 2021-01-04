// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVBLOCK_HPP
#define NAZARA_SPIRVBLOCK_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/SpirvSection.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_SHADER_API SpirvBlock : public SpirvSection
	{
		public:
			inline SpirvBlock(SpirvWriter& writer);
			SpirvBlock(const SpirvBlock&) = default;
			SpirvBlock(SpirvBlock&&) = default;
			~SpirvBlock() = default;

			inline UInt32 GetLabelId() const;

			SpirvBlock& operator=(const SpirvBlock&) = delete;
			SpirvBlock& operator=(SpirvBlock&&) = default;

		private:
			UInt32 m_labelId;
	};
}

#include <Nazara/Shader/SpirvBlock.inl>

#endif
