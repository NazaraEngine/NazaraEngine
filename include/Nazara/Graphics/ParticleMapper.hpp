// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEMAPPER_HPP
#define NAZARA_PARTICLEMAPPER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/ParticleDeclaration.hpp>

class NAZARA_API NzParticleMapper
{
	public:
		NzParticleMapper(void* buffer, const NzParticleDeclaration* declaration);
		~NzParticleMapper();

		template<typename T> NzSparsePtr<T> GetComponentPtr(nzParticleComponent component);
		template<typename T> NzSparsePtr<const T> GetComponentPtr(nzParticleComponent component) const;

	private:
		const NzParticleDeclaration* m_declaration;
		nzUInt8* m_ptr;
};

#include <Nazara/Graphics/ParticleMapper.inl>

#endif // NAZARA_PARTICLEMAPPER_HPP
