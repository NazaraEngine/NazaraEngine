// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEMAPPER_HPP
#define NAZARA_PARTICLEMAPPER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/ParticleDeclaration.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API ParticleMapper
	{
		public:
			ParticleMapper(void* buffer, const ParticleDeclaration* declaration);
			~ParticleMapper();

			template<typename T> SparsePtr<T> GetComponentPtr(ParticleComponent component);
			template<typename T> SparsePtr<const T> GetComponentPtr(ParticleComponent component) const;
			inline void* GetPointer();

		private:
			const ParticleDeclaration* m_declaration;
			UInt8* m_ptr;
	};
}

#include <Nazara/Graphics/ParticleMapper.inl>

#endif // NAZARA_PARTICLEMAPPER_HPP
