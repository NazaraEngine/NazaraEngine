// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	NoiseBase::NoiseBase(unsigned int seed)
	{
		SetSeed(seed);
		m_scale = 0.05f;

		for(unsigned int i(0) ; i < 512; ++i)
			perm[i] = i & 255;
	}

	float NoiseBase::GetScale()
	{
		return m_scale;
	}

	void NoiseBase::SetScale(float scale)
	{
		m_scale = scale;
	}

	void NoiseBase::SetSeed(unsigned int seed)
	{
		generator.seed(seed);
	}

	void NoiseBase::Shuffle()
	{
		int xchanger;
		unsigned int ncase;

		for(unsigned int i(0) ; i < 256 ; i++)
			perm[i] = i;

		for (unsigned int i(0); i < 256 ; ++i)
		{
			ncase = generator() & 255;
			xchanger = perm[i];
			perm[i] = perm[ncase];
			perm[ncase] = xchanger;
		}

		for(unsigned int i(256) ; i < 512; ++i)
			perm[i] = perm[i & 255];
	}

	void NoiseBase::Shuffle(unsigned int amount)
	{
		for(unsigned int j(0) ; j < amount ; ++j)
			Shuffle();
	}
}
