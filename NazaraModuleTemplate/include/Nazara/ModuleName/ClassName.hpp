// Copyright (C) 2014 AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CLASSNAME_HPP
#define NAZARA_CLASSNAME_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzClassName
{
	public:
		NzClassName();

		int GetAttribute() const;

		void SetAttribute(int attribute);

	private:
		int m_attribute;
};

#endif // NAZARA_CLASSNAME_HPP
