// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/MatrixRegistry.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline MatrixRegistry::MatrixRegistry(std::size_t entrySize, std::size_t matrixOffset, std::size_t inverseMatrixOffset) :
	m_currentUbo(0),
	m_entrySize(entrySize),
	m_inverseMatrixOffset(inverseMatrixOffset),
	m_matrixOffset(matrixOffset)
	{
	}
}

#include <Nazara/Utility/DebugOff.hpp>
