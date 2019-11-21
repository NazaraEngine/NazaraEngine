// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/MatrixRegistry.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	void MatrixRegistry::Clear()
	{
		m_currentUbo = 0;
		m_mapper.Unmap();
	}

	void MatrixRegistry::Freeze()
	{
		m_mapper.Unmap();
	}

	const UniformBufferRef& MatrixRegistry::GetUbo(std::size_t uboIndex) const
	{
		NazaraAssert(uboIndex < m_currentUbo, "Ubo index out of bounds");
		return m_ubos[uboIndex];
	}

	std::size_t MatrixRegistry::PushMatrix(std::size_t index, const Matrix4f& matrix)
	{
		constexpr std::size_t entryPerBuffer = 1024;

		std::size_t uboIndex = m_currentUbo++;
		if (uboIndex >= m_ubos.size())
		{
			// Allocate a new buffer
			BufferRef uniformBuffer = Buffer::New(BufferType_Uniform, UInt32(m_entrySize * entryPerBuffer), DataStorage_Hardware, BufferUsage_Dynamic);

			// Prepare all ubos
			m_ubos.reserve(m_ubos.size() + entryPerBuffer);
			for (std::size_t i = 0; i < entryPerBuffer; ++i)
				m_ubos.push_back(UniformBuffer::New(uniformBuffer, UInt32(i * m_entrySize), UInt32(m_entrySize)));
		}

		if (uboIndex % entryPerBuffer == 0)
		{
			m_mapper.Map(m_ubos[uboIndex]->GetBuffer(), BufferAccess_DiscardAndWrite);
			m_mappedPtr = static_cast<UInt8*>(m_mapper.GetPointer());
		}

		Matrix4f* matrixPtr = AccessByOffset<Matrix4f>(m_mappedPtr, m_matrixOffset);
		Matrix4f* inverseMatrixPtr = AccessByOffset<Matrix4f>(m_mappedPtr, m_inverseMatrixOffset);

		*matrixPtr = matrix;
		matrix.GetInverseAffine(inverseMatrixPtr);

		m_mappedPtr += m_entrySize;

		return uboIndex;
	}

	std::size_t MatrixRegistry::Register()
	{
		std::size_t index = m_availableIndices.FindFirst();
		if (index != m_availableIndices.npos)
			m_availableIndices.Reset(index);
		else
		{
			// No index available, add a new one
			index = m_availableIndices.GetSize();
			m_availableIndices.Resize(index + 1);
		}

		return index;
	}

	void MatrixRegistry::Unregister(std::size_t index)
	{
		NazaraAssert(m_availableIndices.UnboundedTest(index), "Index was not registered");

		m_availableIndices.Reset(index);
	}
}
