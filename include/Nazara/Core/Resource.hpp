// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCE_HPP
#define NAZARA_RESOURCE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>

class NAZARA_API NzResource
{
	public:
		NzResource() = default;
		NzResource(const NzResource&) = default;
		NzResource(NzResource&&) = default;
		virtual ~NzResource();

		const NzString& GetFilePath() const;

		void SetFilePath(const NzString& filePath);

		NzResource& operator=(const NzResource&) = default;
		NzResource& operator=(NzResource&&) = default;

	private:
		NzString m_filePath;
};

#endif // NAZARA_RESOURCE_HPP
