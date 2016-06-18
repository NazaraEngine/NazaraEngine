// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Simplex.hpp>
#include <Nazara/Noise/NoiseTools.hpp>
#include <exception>
#include <stdexcept>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	Simplex::Simplex() :
	  gradient2{
		{1,1},{-1,1},{1,-1},{-1,-1},
		{1,0},{-1,0},{0,1},{0,-1}
	  },
	  gradient3{
		{1.f,1.f,0.f},{-1.f,1.f,0.f},{1.f,-1.f,0.f},{-1.f,-1.f,0.f},
			{1.f,0.f,1.f},{-1.f,0.f,1.f},{1.f,0.f,-1.f},{-1.f,0.f,-1.f},
			{0.f,1.f,1.f},{0.f,-1.f,1.f},{0.f,1.f,-1.f},{0.f,-1.f,-1.f}
	  },
	  gradient4{
		{0.f,1.f,1.f,1.f}, {0.f,1.f,1.f,-1.f}, {0.f,1.f,-1.f,1.f}, {0.f,1.f,-1.f,-1.f},
		{0.f,-1.f,1.f,1.f},{0.f,-1.f,1.f,-1.f},{0.f,-1.f,-1.f,1.f},{0.f,-1.f,-1.f,-1.f},
		{1.f,0.f,1.f,1.f}, {1.f,0.f,1.f,-1.f}, {1.f,0.f,-1.f,1.f}, {1.f,0.f,-1.f,-1.f},
		{-1.f,0.f,1.f,1.f},{-1.f,0.f,1.f,-1.f},{-1.f,0.f,-1.f,1.f},{-1.f,0.f,-1.f,-1.f},
		{1.f,1.f,0.f,1.f}, {1.f,1.f,0.f,-1.f}, {1.f,-1.f,0.f,1.f}, {1.f,-1.f,0.f,-1.f},
		{-1.f,1.f,0.f,1.f},{-1.f,1.f,0.f,-1.f},{-1.f,-1.f,0.f,1.f},{-1.f,-1.f,0.f,-1.f},
		{1.f,1.f,1.f,0.f}, {1.f,1.f,-1.f,0.f}, {1.f,-1.f,1.f,0.f}, {1.f,-1.f,-1.f,0.f},
		{-1.f,1.f,1.f,0.f},{-1.f,1.f,-1.f,0.f},{-1.f,-1.f,1.f,0.f},{-1.f,-1.f,-1.f,0.f}
	  },
	  lookupTable4D{
		{0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
		{0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
		{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		{1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
		{1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
		{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		{2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
		{2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
	  },
	  SkewCoeff2D (0.5f*(std::sqrt(3.f) - 1.f)),
	  UnskewCoeff2D((3.f-std::sqrt(3.f))/6.f),
	  SkewCoeff3D (1/3.f),
	  UnskewCoeff3D (1/6.f),
	  SkewCoeff4D ((std::sqrt(5.f) - 1.f)/4.f),
	  UnskewCoeff4D ((5.f - std::sqrt(5.f))/20.f)
	{

	}

	Simplex::Simplex(unsigned int seed) : Simplex()
	{
		SetSeed(seed);
		Shuffle();
	}

	float Simplex::Get(std::initializer_list<float> coordinates, float scale) const
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

	float Simplex::_2D(std::initializer_list<float> coordinates, float scale) const
	{
		thread_local float xc,yc;
		thread_local int ii,jj;
		thread_local int gi0,gi1,gi2;
		thread_local int skewedCubeOriginx,skewedCubeOriginy;
		thread_local int off1x,off1y;
		thread_local float n1,n2,n3;
		thread_local float c1,c2,c3;
		thread_local float sum;
		thread_local float unskewedCubeOriginx,unskewedCubeOriginy;
		thread_local float unskewedDistToOriginx,unskewedDistToOriginy;
		thread_local float d1x,d1y;
		thread_local float d2x,d2y;
		thread_local float d3x,d3y;

		std::initializer_list<float>::const_iterator it = coordinates.begin();

		xc = *(it  ) * scale;
		yc = *(++it) * scale;

		sum = (xc + yc) * SkewCoeff2D;
		skewedCubeOriginx = fastfloor(xc + sum);
		skewedCubeOriginy = fastfloor(yc + sum);

		sum = (skewedCubeOriginx + skewedCubeOriginy) * UnskewCoeff2D;
		unskewedCubeOriginx = skewedCubeOriginx - sum;
		unskewedCubeOriginy = skewedCubeOriginy - sum;

		unskewedDistToOriginx = xc - unskewedCubeOriginx;// Difference with 3d and 4d
		unskewedDistToOriginy = yc - unskewedCubeOriginy;

		if(unskewedDistToOriginx > unskewedDistToOriginy)
		{
			off1x = 1;
			off1y = 0;
		}
		else
		{
			off1x = 0;
			off1y = 1;
		}

		d1x = - unskewedDistToOriginx;
		d1y = - unskewedDistToOriginy;

		d2x = d1x + off1x - UnskewCoeff2D;
		d2y = d1y + off1y - UnskewCoeff2D;

		d3x = d1x + 1.f - 2.f * UnskewCoeff2D;
		d3y = d1y + 1.f - 2.f * UnskewCoeff2D;

		ii = skewedCubeOriginx & 255;
		jj = skewedCubeOriginy & 255;

		gi0 = perm[ii +         perm[jj         ]] & 7;
		gi1 = perm[ii + off1x + perm[jj + off1y ]] & 7;
		gi2 = perm[ii + 1 +     perm[jj + 1     ]] & 7;

		c1 = 0.5f - d1x * d1x - d1y * d1y;
		c2 = 0.5f - d2x * d2x - d2y * d2y;
		c3 = 0.5f - d3x * d3x - d3y * d3y;

		if(c1 < 0)
			n1 = 0;
		else
			n1 = c1*c1*c1*c1*(gradient2[gi0][0] * d1x + gradient2[gi0][1] * d1y);

		if(c2 < 0)
			n2 = 0;
		else
			n2 = c2*c2*c2*c2*(gradient2[gi1][0] * d2x + gradient2[gi1][1] * d2y);

		if(c3 < 0)
			n3 = 0;
		else
			n3 = c3*c3*c3*c3*(gradient2[gi2][0] * d3x + gradient2[gi2][1] * d3y);

		return (n1+n2+n3)*70.f;
	}

	float Simplex::_3D(std::initializer_list<float> coordinates, float scale) const
	{
		thread_local float xc, yc, zc;
		thread_local float x,y,z;
		thread_local int ii,jj,kk;
		thread_local int gi0,gi1,gi2,gi3;
		thread_local int skewedCubeOriginx,skewedCubeOriginy,skewedCubeOriginz;

		thread_local int off1x,off1y,off1z;
		thread_local int off2x,off2y,off2z;
		thread_local float n1,n2,n3,n4;
		thread_local float c1,c2,c3,c4;

		thread_local float sum;
		thread_local float unskewedCubeOriginx,unskewedCubeOriginy,unskewedCubeOriginz;
		thread_local float unskewedDistToOriginx,unskewedDistToOriginy,unskewedDistToOriginz;
		thread_local float d1x,d1y,d1z;
		thread_local float d2x,d2y,d2z;
		thread_local float d3x,d3y,d3z;
		thread_local float d4x,d4y,d4z;

		std::initializer_list<float>::const_iterator it = coordinates.begin();

		x = *(it  );
		y = *(++it);
		z = *(++it);

		xc = x * scale;
		yc = y * scale;
		zc = z * scale;

		sum = (xc + yc + zc) * SkewCoeff3D;
		skewedCubeOriginx = fastfloor(xc + sum);
		skewedCubeOriginy = fastfloor(yc + sum);
		skewedCubeOriginz = fastfloor(zc + sum);

		sum = (skewedCubeOriginx + skewedCubeOriginy + skewedCubeOriginz) * UnskewCoeff3D;
		unskewedCubeOriginx = skewedCubeOriginx - sum;
		unskewedCubeOriginy = skewedCubeOriginy - sum;
		unskewedCubeOriginz = skewedCubeOriginz - sum;

		unskewedDistToOriginx = xc - unskewedCubeOriginx;
		unskewedDistToOriginy = yc - unskewedCubeOriginy;
		unskewedDistToOriginz = zc - unskewedCubeOriginz;

		if(unskewedDistToOriginx >= unskewedDistToOriginy)
		{
			if(unskewedDistToOriginy >= unskewedDistToOriginz)
			{
				off1x = 1;
				off1y = 0;
				off1z = 0;
				off2x = 1;
				off2y = 1;
				off2z = 0;
			}
			else if(unskewedDistToOriginx >= unskewedDistToOriginz)
			{
				off1x = 1;
				off1y = 0;
				off1z = 0;
				off2x = 1;
				off2y = 0;
				off2z = 1;
			}
			else
			{
				off1x = 0;
				off1y = 0;
				off1z = 1;
				off2x = 1;
				off2y = 0;
				off2z = 1;
			}
		}
		else
		{
			if(unskewedDistToOriginy < unskewedDistToOriginz)
			{
				off1x = 0;
				off1y = 0;
				off1z = 1;
				off2x = 0;
				off2y = 1;
				off2z = 1;
			}
			else if(unskewedDistToOriginx < unskewedDistToOriginz)
			{
				off1x = 0;
				off1y = 1;
				off1z = 0;
				off2x = 0;
				off2y = 1;
				off2z = 1;
			}
			else
			{
				off1x = 0;
				off1y = 1;
				off1z = 0;
				off2x = 1;
				off2y = 1;
				off2z = 0;
			}
		}

		d1x = unskewedDistToOriginx;
		d1y = unskewedDistToOriginy;
		d1z = unskewedDistToOriginz;

		d2x = d1x - off1x + UnskewCoeff3D;
		d2y = d1y - off1y + UnskewCoeff3D;
		d2z = d1z - off1z + UnskewCoeff3D;

		d3x = d1x - off2x + 2.f*UnskewCoeff3D;
		d3y = d1y - off2y + 2.f*UnskewCoeff3D;
		d3z = d1z - off2z + 2.f*UnskewCoeff3D;

		d4x = d1x - 1.f + 3.f*UnskewCoeff3D;
		d4y = d1y - 1.f + 3.f*UnskewCoeff3D;
		d4z = d1z - 1.f + 3.f*UnskewCoeff3D;

		ii = skewedCubeOriginx & 255;
		jj = skewedCubeOriginy & 255;
		kk = skewedCubeOriginz & 255;

		gi0 = perm[ii +         perm[jj +         perm[kk         ]]] % 12;
		gi1 = perm[ii + off1x + perm[jj + off1y + perm[kk + off1z ]]] % 12;
		gi2 = perm[ii + off2x + perm[jj + off2y + perm[kk + off2z ]]] % 12;
		gi3 = perm[ii + 1 +     perm[jj + 1 +     perm[kk + 1     ]]] % 12;

		c1 = 0.6f - d1x * d1x - d1y * d1y - d1z * d1z;
		c2 = 0.6f - d2x * d2x - d2y * d2y - d2z * d2z;
		c3 = 0.6f - d3x * d3x - d3y * d3y - d3z * d3z;
		c4 = 0.6f - d4x * d4x - d4y * d4y - d4z * d4z;

		if(c1 < 0)
			n1 = 0;
		else
			n1 = c1*c1*c1*c1*(gradient3[gi0][0] * d1x + gradient3[gi0][1] * d1y + gradient3[gi0][2] * d1z);

		if(c2 < 0)
			n2 = 0;
		else
			n2 = c2*c2*c2*c2*(gradient3[gi1][0] * d2x + gradient3[gi1][1] * d2y + gradient3[gi1][2] * d2z);

		if(c3 < 0)
			n3 = 0;
		else
			n3 = c3*c3*c3*c3*(gradient3[gi2][0] * d3x + gradient3[gi2][1] * d3y + gradient3[gi2][2] * d3z);

		if(c4 < 0)
			n4 = 0;
		else
			n4 = c4*c4*c4*c4*(gradient3[gi3][0] * d4x + gradient3[gi3][1] * d4y + gradient3[gi3][2] * d4z);

		return (n1+n2+n3+n4)*32;
	}

	float Simplex::_4D(std::initializer_list<float> coordinates, float scale) const
	{
		thread_local float xc,yc,zc,wc;
		thread_local float x,y,z,w;
		thread_local int ii,jj,kk,ll;
		thread_local int gi0,gi1,gi2,gi3,gi4;
		thread_local int skewedCubeOriginx,skewedCubeOriginy,skewedCubeOriginz,skewedCubeOriginw;

		thread_local int off1x,off1y,off1z,off1w;
		thread_local int off2x,off2y,off2z,off2w;
		thread_local int off3x,off3y,off3z,off3w;

		thread_local int c;
		thread_local float n1,n2,n3,n4,n5;
		thread_local float c1,c2,c3,c4,c5,c6;

		thread_local float sum;
		thread_local float unskewedCubeOriginx,unskewedCubeOriginy,unskewedCubeOriginz,unskewedCubeOriginw;
		thread_local float unskewedDistToOriginx,unskewedDistToOriginy,unskewedDistToOriginz,unskewedDistToOriginw;
		thread_local float d1x,d2x,d3x,d4x,d5x;
		thread_local float d1y,d2y,d3y,d4y,d5y;
		thread_local float d1z,d2z,d3z,d4z,d5z;
		thread_local float d1w,d2w,d3w,d4w,d5w;

		std::initializer_list<float>::const_iterator it = coordinates.begin();

		x = *(it  );
		y = *(++it);
		z = *(++it);
		w = *(++it);

		xc = x * scale;
		yc = y * scale;
		zc = z * scale;
		wc = w * scale;

		sum = (xc + yc + zc + wc) * SkewCoeff4D;
		skewedCubeOriginx = fastfloor(xc + sum);
		skewedCubeOriginy = fastfloor(yc + sum);
		skewedCubeOriginz = fastfloor(zc + sum);
		skewedCubeOriginw = fastfloor(wc + sum);

		sum = (skewedCubeOriginx + skewedCubeOriginy + skewedCubeOriginz + skewedCubeOriginw) * UnskewCoeff4D;
		unskewedCubeOriginx = skewedCubeOriginx - sum;
		unskewedCubeOriginy = skewedCubeOriginy - sum;
		unskewedCubeOriginz = skewedCubeOriginz - sum;
		unskewedCubeOriginw = skewedCubeOriginw - sum;

		unskewedDistToOriginx = xc - unskewedCubeOriginx;
		unskewedDistToOriginy = yc - unskewedCubeOriginy;
		unskewedDistToOriginz = zc - unskewedCubeOriginz;
		unskewedDistToOriginw = wc - unskewedCubeOriginw;

		c1 = (unskewedDistToOriginx > unskewedDistToOriginy) ? 32 : 0;
		c2 = (unskewedDistToOriginx > unskewedDistToOriginz) ? 16 : 0;
		c3 = (unskewedDistToOriginy > unskewedDistToOriginz) ? 8  : 0;
		c4 = (unskewedDistToOriginx > unskewedDistToOriginw) ? 4  : 0;
		c5 = (unskewedDistToOriginy > unskewedDistToOriginw) ? 2  : 0;
		c6 = (unskewedDistToOriginz > unskewedDistToOriginw) ? 1  : 0;
		c = c1 + c2 + c3 + c4 + c5 + c6;

		off1x = lookupTable4D[c][0] >= 3 ? 1 : 0;
		off1y = lookupTable4D[c][1] >= 3 ? 1 : 0;
		off1z = lookupTable4D[c][2] >= 3 ? 1 : 0;
		off1w = lookupTable4D[c][3] >= 3 ? 1 : 0;

		off2x = lookupTable4D[c][0] >= 2 ? 1 : 0;
		off2y = lookupTable4D[c][1] >= 2 ? 1 : 0;
		off2z = lookupTable4D[c][2] >= 2 ? 1 : 0;
		off2w = lookupTable4D[c][3] >= 2 ? 1 : 0;

		off3x = lookupTable4D[c][0] >= 1 ? 1 : 0;
		off3y = lookupTable4D[c][1] >= 1 ? 1 : 0;
		off3z = lookupTable4D[c][2] >= 1 ? 1 : 0;
		off3w = lookupTable4D[c][3] >= 1 ? 1 : 0;

		d1x = unskewedDistToOriginx;
		d1y = unskewedDistToOriginy;
		d1z = unskewedDistToOriginz;
		d1w = unskewedDistToOriginw;

		d2x = d1x - off1x + UnskewCoeff4D;
		d2y = d1y - off1y + UnskewCoeff4D;
		d2z = d1z - off1z + UnskewCoeff4D;
		d2w = d1w - off1w + UnskewCoeff4D;

		d3x = d1x - off2x + 2.f*UnskewCoeff4D;
		d3y = d1y - off2y + 2.f*UnskewCoeff4D;
		d3z = d1z - off2z + 2.f*UnskewCoeff4D;
		d3w = d1w - off2w + 2.f*UnskewCoeff4D;

		d4x = d1x - off3x + 3.f*UnskewCoeff4D;
		d4y = d1y - off3y + 3.f*UnskewCoeff4D;
		d4z = d1z - off3z + 3.f*UnskewCoeff4D;
		d4w = d1w - off3w + 3.f*UnskewCoeff4D;

		d5x = d1x - 1.f + 4*UnskewCoeff4D;
		d5y = d1y - 1.f + 4*UnskewCoeff4D;
		d5z = d1z - 1.f + 4*UnskewCoeff4D;
		d5w = d1w - 1.f + 4*UnskewCoeff4D;

		ii = skewedCubeOriginx & 255;
		jj = skewedCubeOriginy & 255;
		kk = skewedCubeOriginz & 255;
		ll = skewedCubeOriginw & 255;

		gi0 = perm[ii +         perm[jj +         perm[kk +         perm[ll        ]]]] & 31;
		gi1 = perm[ii + off1x + perm[jj + off1y + perm[kk + off1z + perm[ll + off1w]]]] & 31;
		gi2 = perm[ii + off2x + perm[jj + off2y + perm[kk + off2z + perm[ll + off2w]]]] & 31;
		gi3 = perm[ii + off3x + perm[jj + off3y + perm[kk + off3z + perm[ll + off3w]]]] & 31;
		gi4 = perm[ii + 1 +     perm[jj + 1 +     perm[kk + 1 +     perm[ll + 1    ]]]] % 32;

		c1 = 0.6f - d1x*d1x - d1y*d1y - d1z*d1z - d1w*d1w;
		c2 = 0.6f - d2x*d2x - d2y*d2y - d2z*d2z - d2w*d2w;
		c3 = 0.6f - d3x*d3x - d3y*d3y - d3z*d3z - d3w*d3w;
		c4 = 0.6f - d4x*d4x - d4y*d4y - d4z*d4z - d4w*d4w;
		c5 = 0.6f - d5x*d5x - d5y*d5y - d5z*d5z - d5w*d5w;

		if(c1 < 0)
			n1 = 0;
		else
			n1 = c1*c1*c1*c1*(gradient4[gi0][0]*d1x + gradient4[gi0][1]*d1y + gradient4[gi0][2]*d1z + gradient4[gi0][3]*d1w);

		if(c2 < 0)
			n2 = 0;
		else
			n2 = c2*c2*c2*c2*(gradient4[gi1][0]*d2x + gradient4[gi1][1]*d2y + gradient4[gi1][2]*d2z + gradient4[gi1][3]*d2w);

		if(c3 < 0)
			n3 = 0;
		else
			n3 = c3*c3*c3*c3*(gradient4[gi2][0]*d3x + gradient4[gi2][1]*d3y + gradient4[gi2][2]*d3z + gradient4[gi2][3]*d3w);

		if(c4 < 0)
			n4 = 0;
		else
			n4 = c4*c4*c4*c4*(gradient4[gi3][0]*d4x + gradient4[gi3][1]*d4y + gradient4[gi3][2]*d4z + gradient4[gi3][3]*d4w);

		if(c5 < 0)
			n5 = 0;
		else
			n5 = c5*c5*c5*c5*(gradient4[gi4][0]*d5x + gradient4[gi4][1]*d5y + gradient4[gi4][2]*d5z + gradient4[gi4][3]*d5w);

		return (n1+n2+n3+n4+n5)*27.f;
	}
}
