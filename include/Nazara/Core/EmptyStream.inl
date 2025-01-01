// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::EmptyStream
	* \brief Constructs an EmptyStream object by default
	*/
	inline EmptyStream::EmptyStream() :
	m_size(0)
	{
		m_openMode = Nz::OpenMode_ReadWrite;
	}
}
