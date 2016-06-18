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
	namespace
	{
		constexpr float s_SkewCoeff2D   = 0.5f * (M_SQRT3 - 1.f);
		constexpr float s_UnskewCoeff2D = (3.f - M_SQRT3)/6.f;
		constexpr float s_SkewCoeff3D   = 1.f / 3.f;
		constexpr float s_UnskewCoeff3D = 1.f / 6.f;
		constexpr float s_SkewCoeff4D   = (M_SQRT5 - 1.f)/4.f;
		constexpr float s_UnskewCoeff4D = (5.f - M_SQRT5)/20.f;
	}

	Simplex::Simplex(unsigned int seed)
	{
		SetSeed(seed);
		Shuffle();
	}

	float Simplex::Get(float x, float y, float scale) const
	{
		float xc = x * scale;
		float yc = y * scale;

		float sum = (xc + yc) * s_SkewCoeff2D;
		Vector2i skewedCubeOrigin(fastfloor(xc + sum), fastfloor(yc + sum));

		sum = (skewedCubeOrigin.x + skewedCubeOrigin.y) * s_UnskewCoeff2D;
		Vector2f unskewedCubeOrigin(skewedCubeOrigin.x - sum, skewedCubeOrigin.y - sum);

		Vector2f unskewedDistToOrigin(xc - unskewedCubeOrigin.x, yc - unskewedCubeOrigin.y);

		Vector2f off1;
		if(unskewedDistToOrigin.x > unskewedDistToOrigin.y)
			off1.Set(1, 0);
		else
			off1.Set(0, 1);

		std::array<Vector2f, 3> d;
		d[0] = -unskewedDistToOrigin;
		d[1] = d[0] + off1 - Vector2f(s_UnskewCoeff2D);
		d[2] = d[0] + Vector2f(1.f - 2.f * s_UnskewCoeff2D);

		Vector2i offset(skewedCubeOrigin.x & 255, skewedCubeOrigin.y & 255);
		std::array<std::size_t, 3> gi =
		{
			m_permutations[offset.x + m_permutations[offset.y]] & 7,
			m_permutations[offset.x + off1.x + m_permutations[offset.y + off1.y]] & 7,
			m_permutations[offset.x + 1 + m_permutations[offset.y + 1]] & 7
		};

		float n = 0.f;
		for (unsigned int i = 0; i < 3; ++i)
		{
			float c = 0.5f - d[i].x * d[i].x - d[i].y *d[i].y;
			if (c > 0.f)
				n += c * c * c * c * (s_gradients2[gi[i]].x * d[i].x + s_gradients2[gi[i]].y * d[i].y);
		}

		return n*70.f;
	}

	float Simplex::Get(float x, float y, float z, float scale) const
	{
		float xc, yc, zc;
		int ii,jj,kk;
		int gi0,gi1,gi2,gi3;
		int skewedCubeOriginx,skewedCubeOriginy,skewedCubeOriginz;

		int off1x,off1y,off1z;
		int off2x,off2y,off2z;
		float n1,n2,n3,n4;
		float c1,c2,c3,c4;

		float sum;
		float unskewedCubeOriginx,unskewedCubeOriginy,unskewedCubeOriginz;
		float unskewedDistToOriginx,unskewedDistToOriginy,unskewedDistToOriginz;
		float d1x,d1y,d1z;
		float d2x,d2y,d2z;
		float d3x,d3y,d3z;
		float d4x,d4y,d4z;

		xc = x * scale;
		yc = y * scale;
		zc = z * scale;

		sum = (xc + yc + zc) * s_SkewCoeff3D;
		skewedCubeOriginx = fastfloor(xc + sum);
		skewedCubeOriginy = fastfloor(yc + sum);
		skewedCubeOriginz = fastfloor(zc + sum);

		sum = (skewedCubeOriginx + skewedCubeOriginy + skewedCubeOriginz) * s_UnskewCoeff3D;
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

		d2x = d1x - off1x + s_UnskewCoeff3D;
		d2y = d1y - off1y + s_UnskewCoeff3D;
		d2z = d1z - off1z + s_UnskewCoeff3D;

		d3x = d1x - off2x + 2.f*s_UnskewCoeff3D;
		d3y = d1y - off2y + 2.f*s_UnskewCoeff3D;
		d3z = d1z - off2z + 2.f*s_UnskewCoeff3D;

		d4x = d1x - 1.f + 3.f*s_UnskewCoeff3D;
		d4y = d1y - 1.f + 3.f*s_UnskewCoeff3D;
		d4z = d1z - 1.f + 3.f*s_UnskewCoeff3D;

		ii = skewedCubeOriginx & 255;
		jj = skewedCubeOriginy & 255;
		kk = skewedCubeOriginz & 255;

		gi0 = m_permutations[ii +         m_permutations[jj +         m_permutations[kk         ]]] % 12;
		gi1 = m_permutations[ii + off1x + m_permutations[jj + off1y + m_permutations[kk + off1z ]]] % 12;
		gi2 = m_permutations[ii + off2x + m_permutations[jj + off2y + m_permutations[kk + off2z ]]] % 12;
		gi3 = m_permutations[ii + 1 +     m_permutations[jj + 1 +     m_permutations[kk + 1     ]]] % 12;

		c1 = 0.6f - d1x * d1x - d1y * d1y - d1z * d1z;
		c2 = 0.6f - d2x * d2x - d2y * d2y - d2z * d2z;
		c3 = 0.6f - d3x * d3x - d3y * d3y - d3z * d3z;
		c4 = 0.6f - d4x * d4x - d4y * d4y - d4z * d4z;

		if(c1 < 0)
			n1 = 0;
		else
			n1 = c1*c1*c1*c1*(s_gradients3[gi0][0] * d1x + s_gradients3[gi0][1] * d1y + s_gradients3[gi0][2] * d1z);

		if(c2 < 0)
			n2 = 0;
		else
			n2 = c2*c2*c2*c2*(s_gradients3[gi1][0] * d2x + s_gradients3[gi1][1] * d2y + s_gradients3[gi1][2] * d2z);

		if(c3 < 0)
			n3 = 0;
		else
			n3 = c3*c3*c3*c3*(s_gradients3[gi2][0] * d3x + s_gradients3[gi2][1] * d3y + s_gradients3[gi2][2] * d3z);

		if(c4 < 0)
			n4 = 0;
		else
			n4 = c4*c4*c4*c4*(s_gradients3[gi3][0] * d4x + s_gradients3[gi3][1] * d4y + s_gradients3[gi3][2] * d4z);

		return (n1+n2+n3+n4)*32;
	}

	float Simplex::Get(float x, float y, float z, float w, float scale) const
	{
		static std::array<Vector4ui, 64> lookupTable =
		{
			{
				{0,1,2,3}, {0,1,3,2}, {0,0,0,0}, {0,2,3,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {1,2,3,0},
				{0,2,1,3}, {0,0,0,0}, {0,3,1,2}, {0,3,2,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {1,3,2,0},
				{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
				{1,2,0,3}, {0,0,0,0}, {1,3,0,2}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {2,3,0,1}, {2,3,1,0},
				{1,0,2,3}, {1,0,3,2}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {2,0,3,1}, {0,0,0,0}, {2,1,3,0},
				{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
				{2,0,1,3}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {3,0,1,2}, {3,0,2,1}, {0,0,0,0}, {3,1,2,0},
				{2,1,0,3}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {3,1,0,2}, {0,0,0,0}, {3,2,0,1}, {3,2,1,0}
			}
		};

		float xc,yc,zc,wc;
		int ii,jj,kk,ll;
		int gi0,gi1,gi2,gi3,gi4;
		int skewedCubeOriginx,skewedCubeOriginy,skewedCubeOriginz,skewedCubeOriginw;

		int off1x,off1y,off1z,off1w;
		int off2x,off2y,off2z,off2w;
		int off3x,off3y,off3z,off3w;

		int c;
		float n1,n2,n3,n4,n5;
		float c1,c2,c3,c4,c5,c6;

		float sum;
		float unskewedCubeOriginx,unskewedCubeOriginy,unskewedCubeOriginz,unskewedCubeOriginw;
		float unskewedDistToOriginx,unskewedDistToOriginy,unskewedDistToOriginz,unskewedDistToOriginw;
		float d1x,d2x,d3x,d4x,d5x;
		float d1y,d2y,d3y,d4y,d5y;
		float d1z,d2z,d3z,d4z,d5z;
		float d1w,d2w,d3w,d4w,d5w;

		xc = x * scale;
		yc = y * scale;
		zc = z * scale;
		wc = w * scale;

		sum = (xc + yc + zc + wc) * s_SkewCoeff4D;
		skewedCubeOriginx = fastfloor(xc + sum);
		skewedCubeOriginy = fastfloor(yc + sum);
		skewedCubeOriginz = fastfloor(zc + sum);
		skewedCubeOriginw = fastfloor(wc + sum);

		sum = (skewedCubeOriginx + skewedCubeOriginy + skewedCubeOriginz + skewedCubeOriginw) * s_UnskewCoeff4D;
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

		off1x = lookupTable[c][0] >= 3 ? 1 : 0;
		off1y = lookupTable[c][1] >= 3 ? 1 : 0;
		off1z = lookupTable[c][2] >= 3 ? 1 : 0;
		off1w = lookupTable[c][3] >= 3 ? 1 : 0;

		off2x = lookupTable[c][0] >= 2 ? 1 : 0;
		off2y = lookupTable[c][1] >= 2 ? 1 : 0;
		off2z = lookupTable[c][2] >= 2 ? 1 : 0;
		off2w = lookupTable[c][3] >= 2 ? 1 : 0;

		off3x = lookupTable[c][0] >= 1 ? 1 : 0;
		off3y = lookupTable[c][1] >= 1 ? 1 : 0;
		off3z = lookupTable[c][2] >= 1 ? 1 : 0;
		off3w = lookupTable[c][3] >= 1 ? 1 : 0;

		d1x = unskewedDistToOriginx;
		d1y = unskewedDistToOriginy;
		d1z = unskewedDistToOriginz;
		d1w = unskewedDistToOriginw;

		d2x = d1x - off1x + s_UnskewCoeff4D;
		d2y = d1y - off1y + s_UnskewCoeff4D;
		d2z = d1z - off1z + s_UnskewCoeff4D;
		d2w = d1w - off1w + s_UnskewCoeff4D;

		d3x = d1x - off2x + 2.f*s_UnskewCoeff4D;
		d3y = d1y - off2y + 2.f*s_UnskewCoeff4D;
		d3z = d1z - off2z + 2.f*s_UnskewCoeff4D;
		d3w = d1w - off2w + 2.f*s_UnskewCoeff4D;

		d4x = d1x - off3x + 3.f*s_UnskewCoeff4D;
		d4y = d1y - off3y + 3.f*s_UnskewCoeff4D;
		d4z = d1z - off3z + 3.f*s_UnskewCoeff4D;
		d4w = d1w - off3w + 3.f*s_UnskewCoeff4D;

		d5x = d1x - 1.f + 4*s_UnskewCoeff4D;
		d5y = d1y - 1.f + 4*s_UnskewCoeff4D;
		d5z = d1z - 1.f + 4*s_UnskewCoeff4D;
		d5w = d1w - 1.f + 4*s_UnskewCoeff4D;

		ii = skewedCubeOriginx & 255;
		jj = skewedCubeOriginy & 255;
		kk = skewedCubeOriginz & 255;
		ll = skewedCubeOriginw & 255;

		gi0 = m_permutations[ii +         m_permutations[jj +         m_permutations[kk +         m_permutations[ll        ]]]] & 31;
		gi1 = m_permutations[ii + off1x + m_permutations[jj + off1y + m_permutations[kk + off1z + m_permutations[ll + off1w]]]] & 31;
		gi2 = m_permutations[ii + off2x + m_permutations[jj + off2y + m_permutations[kk + off2z + m_permutations[ll + off2w]]]] & 31;
		gi3 = m_permutations[ii + off3x + m_permutations[jj + off3y + m_permutations[kk + off3z + m_permutations[ll + off3w]]]] & 31;
		gi4 = m_permutations[ii + 1 +     m_permutations[jj + 1 +     m_permutations[kk + 1 +     m_permutations[ll + 1    ]]]] % 32;

		c1 = 0.6f - d1x*d1x - d1y*d1y - d1z*d1z - d1w*d1w;
		c2 = 0.6f - d2x*d2x - d2y*d2y - d2z*d2z - d2w*d2w;
		c3 = 0.6f - d3x*d3x - d3y*d3y - d3z*d3z - d3w*d3w;
		c4 = 0.6f - d4x*d4x - d4y*d4y - d4z*d4z - d4w*d4w;
		c5 = 0.6f - d5x*d5x - d5y*d5y - d5z*d5z - d5w*d5w;

		if(c1 < 0)
			n1 = 0;
		else
			n1 = c1*c1*c1*c1*(s_gradients4[gi0][0]*d1x + s_gradients4[gi0][1]*d1y + s_gradients4[gi0][2]*d1z + s_gradients4[gi0][3]*d1w);

		if(c2 < 0)
			n2 = 0;
		else
			n2 = c2*c2*c2*c2*(s_gradients4[gi1][0]*d2x + s_gradients4[gi1][1]*d2y + s_gradients4[gi1][2]*d2z + s_gradients4[gi1][3]*d2w);

		if(c3 < 0)
			n3 = 0;
		else
			n3 = c3*c3*c3*c3*(s_gradients4[gi2][0]*d3x + s_gradients4[gi2][1]*d3y + s_gradients4[gi2][2]*d3z + s_gradients4[gi2][3]*d3w);

		if(c4 < 0)
			n4 = 0;
		else
			n4 = c4*c4*c4*c4*(s_gradients4[gi3][0]*d4x + s_gradients4[gi3][1]*d4y + s_gradients4[gi3][2]*d4z + s_gradients4[gi3][3]*d4w);

		if(c5 < 0)
			n5 = 0;
		else
			n5 = c5*c5*c5*c5*(s_gradients4[gi4][0]*d5x + s_gradients4[gi4][1]*d5y + s_gradients4[gi4][2]*d5z + s_gradients4[gi4][3]*d5w);

		return (n1+n2+n3+n4+n5)*27.f;
	}
}
