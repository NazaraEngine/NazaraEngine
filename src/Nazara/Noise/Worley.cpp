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
	Worley::Worley() :
	  scales{
		1.f / std::sqrt(2),
		0.5f / std::sqrt(2),
		0.5f / std::sqrt(2),
		0.5f / std::sqrt(2)
	  }
	{
	  function = WorleyFunction_F1;
	}

	Worley::Worley(unsigned int seed) : Worley()
	{
		SetSeed(seed);
		Shuffle();
	}

	void Worley::Set(WorleyFunction func)
	{
	  function = func;
	}

	float Worley::Get(std::initializer_list<float> coordinates, float scale) const
	{
		switch(coordinates.size())
		{
			case 2:
			  return this->_2D(coordinates,scale);
			case 3:
			  return this->_3D(coordinates,scale);
			case 4:
			  return this->_4D(coordinates,scale);
			default:
			  throw std::invalid_argument("Number of coordinates elements not comprised between 2 and 4");
		}
	}

	float Worley::_2D(std::initializer_list<float> coordinates, float scale) const
	{
		std::map<float, Vector2f> featurePoints;
		std::map<float, Vector2f>::iterator it;

		float xc, yc;
		int x0, y0;
		float fractx, fracty;

		std::initializer_list<float>::const_iterator c = coordinates.begin();

		xc = *(c  ) * scale;
		yc = *(++c) * scale;

		x0 = fastfloor(xc);
		y0 = fastfloor(yc);

		fractx = xc - static_cast<float>(x0);
		fracty = yc - static_cast<float>(y0);

		featurePoints.clear();

		_SquareTest(x0,y0,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		if(fractx < it->first)
			_SquareTest(x0 - 1,y0,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		if(1.f - fractx < it->first)
			_SquareTest(x0 + 1,y0,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		if(fracty < it->first)
			_SquareTest(x0,y0 - 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		if(1.f - fracty < it->first)
			_SquareTest(x0,y0 + 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		if(fractx < it->first &&
		   fracty < it->first)
		   _SquareTest(x0 - 1, y0 - 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		if(1.f - fractx < it->first &&
		   fracty < it->first)
		   _SquareTest(x0 + 1, y0 - 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		if(fractx < it->first &&
		   1.f - fracty < it->first)
		   _SquareTest(x0 - 1, y0 + 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		if(1.f - fractx < it->first &&
		   1.f - fracty < it->first)
		   _SquareTest(x0 + 1, y0 + 1,xc,yc,featurePoints);

		it = featurePoints.begin();
		std::advance(it,function);

		return it->first * scales[function];
	}

	float Worley::_3D(std::initializer_list<float> coordinates, float scale) const
	{
		throw std::runtime_error("Worley 3D not available yet.");
	}

	float Worley::_4D(std::initializer_list<float> coordinates, float scale) const
	{
		throw std::runtime_error("Worley 4D not available yet.");
	}


	void Worley::_SquareTest(int xi, int yi, float x, float y, std::map<float, Vector2f> & featurePoints) const
	{
		int ii = xi & 255;
		int jj = yi & 255;

		int seed = perm[ii +     perm[jj]];

		//On initialise notre rng avec seed
		std::minstd_rand0 randomNumberGenerator(seed);

		//On prend un nombre de points à déterminer dans le cube, compris entre 1 et 8
		unsigned int m = (seed & 7) + 1;

		//On calcule les emplacements des différents points
		for(unsigned int i(0) ; i < m ; ++i)
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
