// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Worley.hpp>
#include <Nazara/Noise/NoiseTools.hpp>
#include <exception>
#include <stdexcept>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	namespace
	{
		static constexpr std::array<float, 4> m_functionScales = {
			{
				1.f  / float(M_SQRT2),
				0.5f / float(M_SQRT2),
				0.5f / float(M_SQRT2),
				0.5f / float(M_SQRT2)
			}
		};
	}
	Worley::Worley() :
	m_function(WorleyFunction_F1)
	{
	}

	Worley::Worley(unsigned int seed) :
	Worley()
	{
		SetSeed(seed);
		Shuffle();
	}

	float Worley::Get(float x, float y, float scale) const
	{
		std::map<float, Vector2f> featurePoints;

		float xc, yc;
		int x0, y0;
		float fractx, fracty;

		xc = x * scale;
		yc = y * scale;

		x0 = fastfloor(xc);
		y0 = fastfloor(yc);

		fractx = xc - static_cast<float>(x0);
		fracty = yc - static_cast<float>(y0);

		featurePoints.clear();

		SquareTest(x0,y0,xc,yc,featurePoints);

		std::size_t functionIndex = static_cast<std::size_t>(m_function);

		auto it = featurePoints.begin();
		std::advance(it, functionIndex);

		if(fractx < it->first)
			SquareTest(x0 - 1,y0,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it, functionIndex);

		if(1.f - fractx < it->first)
			SquareTest(x0 + 1,y0,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it, functionIndex);

		if(fracty < it->first)
			SquareTest(x0,y0 - 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it, functionIndex);

		if (1.f - fracty < it->first)
			SquareTest(x0,y0 + 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it, functionIndex);

		if (fractx < it->first && fracty < it->first)
			SquareTest(x0 - 1, y0 - 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it, functionIndex);

		if (1.f - fractx < it->first && fracty < it->first)
		   SquareTest(x0 + 1, y0 - 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it, functionIndex);

		if (fractx < it->first && 1.f - fracty < it->first)
		   SquareTest(x0 - 1, y0 + 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it, functionIndex);

		if(1.f - fractx < it->first && 1.f - fracty < it->first)
		   SquareTest(x0 + 1, y0 + 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it, functionIndex);

		return it->first * m_functionScales[functionIndex];
	}

	float Worley::Get(float /*x*/, float /*y*/, float /*z*/, float /*scale*/) const
	{
		throw std::runtime_error("Worley 3D not available yet.");
	}

	float Worley::Get(float /*x*/, float /*y*/, float /*z*/, float /*w*/, float /*scale*/) const
	{
		throw std::runtime_error("Worley 4D not available yet.");
	}

	void Worley::Set(WorleyFunction func)
	{
		m_function = func;
	}

	void Worley::SquareTest(int xi, int yi, float x, float y, std::map<float, Vector2f>& featurePoints) const
	{
		int ii = xi & 255;
		int jj = yi & 255;

		std::size_t seed = m_permutations[ii + m_permutations[jj]];

		//On initialise notre rng avec seed
		std::minstd_rand0 randomNumberGenerator(static_cast<unsigned int>(seed));

		//On prend un nombre de points à déterminer dans le cube, compris entre 1 et 8
		std::size_t m = (seed & 7) + 1;

		//On calcule les emplacements des différents points
		for(std::size_t i(0) ; i < m; ++i)
		{
			Nz::Vector2f featurePoint;
			featurePoint.x = (randomNumberGenerator() & 1023) / 1023.f + static_cast<float>(xi);
			featurePoint.y = (randomNumberGenerator() & 1023) / 1023.f + static_cast<float>(yi);

			// TODO : Check order is correct
			float distance = std::sqrt((featurePoint.x - x) * (featurePoint.x - x) +
									   (featurePoint.y - y) * (featurePoint.y - y));

			//Insertion dans la liste triée
			featurePoints[distance] = featurePoint;
		}
	}
}
