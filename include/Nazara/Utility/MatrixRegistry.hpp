// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATRIXREGISTRY_HPP
#define NAZARA_MATRIXREGISTRY_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/UniformBuffer.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API MatrixRegistry
	{
		public:
			inline MatrixRegistry(std::size_t entrySize, std::size_t matrixOffset, std::size_t inverseMatrixOffset);
			~MatrixRegistry() = default;

			void Clear();

			void Freeze();

			const UniformBufferRef& GetUbo(std::size_t uboIndex) const;

			std::size_t PushMatrix(std::size_t index, const Nz::Matrix4f& matrix);

			std::size_t Register();
			void Unregister(std::size_t index);

		private:
			Bitset<Nz::UInt64> m_availableIndices;
			BufferMapper<Buffer> m_mapper;
			std::size_t m_currentUbo;
			std::size_t m_entrySize;
			std::size_t m_inverseMatrixOffset;
			std::size_t m_matrixOffset;
			std::vector<UniformBufferRef> m_ubos;
			UInt8* m_mappedPtr;
	};
}

#include <Nazara/Utility/MatrixRegistry.inl>

#endif // NAZARA_MATRIXREGISTRY_HPP
