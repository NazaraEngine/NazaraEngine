// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OCCLUSIONQUERY_HPP
#define NAZARA_OCCLUSIONQUERY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>

class NAZARA_API NzOcclusionQuery : NzNonCopyable
{
	public:
		NzOcclusionQuery();
		~NzOcclusionQuery();

		void Begin();
		void End();

		unsigned int GetResult() const;

		bool IsResultAvailable() const;

		static bool IsSupported();

	private:
		unsigned int m_id;
};

#endif // NAZARA_OCCLUSIONQUERY_HPP
