// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Perlin.hpp>
#include <Nazara/Noise/NoiseTools.hpp>
#include <exception>
#include <stdexcept>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	Perlin::Perlin() :
	  gradient2{
		{1.f,1.f},{-1.f,1.f},{1.f,-1.f},{-1.f,-1.f},
		{1.f,0.f},{-1.f,0.f},{0.f,1.f},{0.f,-1.f}
	  },
	  gradient3{
		{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
		{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
		{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1},
		{1,1,0},{-1,1,0},{0,-1,1},{0,-1,-1}
	  },
	  gradient4{
		{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1},
		{0,-1,1,1},{0,-1,1,-1},{0,-1,-1,1},{0,-1,-1,-1},
		{1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
		{-1,0,1,1},{-1,0,1,-1},{-1,0,-1,1},{-1,0,-1,-1},
		{1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
		{-1,1,0,1},{-1,1,0,-1},{-1,-1,0,1},{-1,-1,0,-1},
		{1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
		{-1,1,1,0},{-1,1,-1,0},{-1,-1,1,0},{-1,-1,-1,0}
	  }
	{

	}

	Perlin::Perlin(unsigned int seed) : Perlin()
	{
		SetSeed(seed);
		Shuffle();
	}

	float Perlin::Get(std::initializer_list<float> coordinates, float scale) const
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

	float Perlin::_2D(std::initializer_list<float> coordinates, float scale) const
	{
		thread_local float xc, yc;
		thread_local int x0, y0;
		thread_local int gi0,gi1,gi2,gi3;
		thread_local int ii, jj;

		thread_local float s,t,u,v;
		thread_local float Cx,Cy;
		thread_local float Li1, Li2;
		thread_local float tempx,tempy;

		std::initializer_list<float>::const_iterator it = coordinates.begin();

		xc = *(it  ) * scale;
		yc = *(++it) * scale;

		x0 = fastfloor(xc);
		y0 = fastfloor(yc);

		ii = x0 & 255;
		jj = y0 & 255;

		gi0 = perm[ii +     perm[jj]] & 7;
		gi1 = perm[ii + 1 + perm[jj]] & 7;
		gi2 = perm[ii +     perm[jj + 1]] & 7;
		gi3 = perm[ii + 1 + perm[jj + 1]] & 7;

		tempx = xc - x0;
		tempy = yc - y0;

		Cx = tempx * tempx * tempx * (tempx * (tempx * 6 - 15) + 10);
		Cy = tempy * tempy * tempy * (tempy * (tempy * 6 - 15) + 10);

		s = gradient2[gi0][0]*tempx + gradient2[gi0][1]*tempy;

		tempx = xc - (x0 + 1);
		t = gradient2[gi1][0]*tempx + gradient2[gi1][1]*tempy;

		tempy = yc - (y0 + 1);
		v = gradient2[gi3][0]*tempx + gradient2[gi3][1]*tempy;

		tempx = xc - x0;
		u = gradient2[gi2][0]*tempx + gradient2[gi2][1]*tempy;

		Li1 = s + Cx*(t-s);
		Li2 = u + Cx*(v-u);

		return Li1 + Cy*(Li2-Li1);
	}

	float Perlin::_3D(std::initializer_list<float> coordinates, float scale) const
	{
		thread_local float xc, yc, zc;
		thread_local int x0, y0, z0;
		thread_local int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7;
		thread_local int ii, jj, kk;

		thread_local float Li1,Li2,Li3,Li4,Li5,Li6;
		thread_local float s[2],t[2],u[2],v[2];
		thread_local float Cx,Cy,Cz;
		thread_local float nx,ny,nz;

		thread_local float tmp;
		thread_local float tempx,tempy,tempz;

		std::initializer_list<float>::const_iterator it = coordinates.begin();

		xc = *(it  ) * scale;
		yc = *(++it) * scale;
		zc = *(++it) * scale;

		x0 = fastfloor(xc);
		y0 = fastfloor(yc);
		z0 = fastfloor(zc);

		ii = x0 & 255;
		jj = y0 & 255;
		kk = z0 & 255;

		gi0 = perm[ii +     perm[jj +     perm[kk]]] & 15;
		gi1 = perm[ii + 1 + perm[jj +     perm[kk]]] & 15;
		gi2 = perm[ii +     perm[jj + 1 + perm[kk]]] & 15;
		gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk]]] & 15;

		gi4 = perm[ii +     perm[jj +     perm[kk + 1]]] & 15;
		gi5 = perm[ii + 1 + perm[jj +     perm[kk + 1]]] & 15;
		gi6 = perm[ii +     perm[jj + 1 + perm[kk + 1]]] & 15;
		gi7 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] & 15;

		tempx = xc - x0;
		tempy = yc - y0;
		tempz = zc - z0;

		Cx = tempx * tempx * tempx * (tempx * (tempx * 6 - 15) + 10);
		Cy = tempy * tempy * tempy * (tempy * (tempy * 6 - 15) + 10);
		Cz = tempz * tempz * tempz * (tempz * (tempz * 6 - 15) + 10);

		s[0] = gradient3[gi0][0]*tempx + gradient3[gi0][1]*tempy + gradient3[gi0][2]*tempz;

		tempx = xc - (x0 + 1);
		t[0] = gradient3[gi1][0]*tempx + gradient3[gi1][1]*tempy + gradient3[gi1][2]*tempz;

		tempy = yc - (y0 + 1);
		v[0] = gradient3[gi3][0]*tempx + gradient3[gi3][1]*tempy + gradient3[gi3][2]*tempz;

		tempx = xc - x0;
		u[0] = gradient3[gi2][0]*tempx + gradient3[gi2][1]*tempy + gradient3[gi2][2]*tempz;

		tempy = yc - y0;
		tempz = zc - (z0 + 1);
		s[1] = gradient3[gi4][0]*tempx + gradient3[gi4][1]*tempy + gradient3[gi4][2]*tempz;

		tempx = xc - (x0 + 1);
		t[1] = gradient3[gi5][0]*tempx + gradient3[gi5][1]*tempy + gradient3[gi5][2]*tempz;

		tempy = yc - (y0 + 1);
		v[1] = gradient3[gi7][0]*tempx + gradient3[gi7][1]*tempy + gradient3[gi7][2]*tempz;

		tempx = xc - x0;
		u[1] = gradient3[gi6][0]*tempx + gradient3[gi6][1]*tempy + gradient3[gi6][2]*tempz;

		Li1 = s[0] + Cx*(t[0]-s[0]);
		Li2 = u[0] + Cx*(v[0]-u[0]);
		Li3 = s[1] + Cx*(t[1]-s[1]);
		Li4 = u[1] + Cx*(v[1]-u[1]);

		Li5 = Li1 + Cy * (Li2-Li1);
		Li6 = Li3 + Cy * (Li4-Li3);

		return Li5 + Cz * (Li6-Li5);
	}

	float Perlin::_4D(std::initializer_list<float> coordinates, float scale) const
	{
		thread_local float xc,yc,zc,wc;
		thread_local int x0,y0,z0,w0;
		thread_local int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7,gi8,gi9,gi10,gi11,gi12,gi13,gi14,gi15;
		thread_local int ii,jj,kk,ll;

		thread_local float Li1,Li2,Li3,Li4,Li5,Li6,Li7,Li8,Li9,Li10,Li11,Li12,Li13,Li14;
		thread_local float s[4],t[4],u[4],v[4];
		thread_local float Cx,Cy,Cz,Cw;

		thread_local float tmp;
		thread_local float tempx,tempy,tempz,tempw;

		std::initializer_list<float>::const_iterator it = coordinates.begin();

		xc = *(it  ) * scale;
		yc = *(++it) * scale;
		zc = *(++it) * scale;
		wc = *(++it) * scale;

		x0 = fastfloor(xc);
		y0 = fastfloor(yc);
		z0 = fastfloor(zc);
		w0 = fastfloor(wc);

		ii = x0 & 255;
		jj = y0 & 255;
		kk = z0 & 255;
		ll = w0 & 255;

		gi0 =  perm[ii     + perm[jj     + perm[kk     + perm[ll]]]] & 31;
		gi1 =  perm[ii + 1 + perm[jj     + perm[kk     + perm[ll]]]] & 31;
		gi2 =  perm[ii     + perm[jj + 1 + perm[kk     + perm[ll]]]] & 31;
		gi3 =  perm[ii + 1 + perm[jj + 1 + perm[kk     + perm[ll]]]] & 31;

		gi4 =  perm[ii     + perm[jj +   + perm[kk + 1 + perm[ll]]]] & 31;
		gi5 =  perm[ii + 1 + perm[jj +   + perm[kk + 1 + perm[ll]]]] & 31;
		gi6 =  perm[ii     + perm[jj + 1 + perm[kk + 1 + perm[ll]]]] & 31;
		gi7 =  perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll]]]] & 31;

		gi8 =  perm[ii     + perm[jj     + perm[kk     + perm[ll + 1]]]] & 31;
		gi9 =  perm[ii + 1 + perm[jj     + perm[kk     + perm[ll + 1]]]] & 31;
		gi10 = perm[ii     + perm[jj + 1 + perm[kk     + perm[ll + 1]]]] & 31;
		gi11 = perm[ii + 1 + perm[jj + 1 + perm[kk     + perm[ll + 1]]]] & 31;

		gi12 = perm[ii     + perm[jj     + perm[kk + 1 + perm[ll + 1]]]] & 31;
		gi13 = perm[ii + 1 + perm[jj     + perm[kk + 1 + perm[ll + 1]]]] & 31;
		gi14 = perm[ii     + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] & 31;
		gi15 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] & 31;

		tempx = xc - x0;
		tempy = yc - y0;
		tempz = zc - z0;
		tempw = wc - w0;

		Cx = tempx * tempx * tempx * (tempx * (tempx * 6 - 15) + 10);
		Cy = tempy * tempy * tempy * (tempy * (tempy * 6 - 15) + 10);
		Cz = tempz * tempz * tempz * (tempz * (tempz * 6 - 15) + 10);
		Cw = tempw * tempw * tempw * (tempw * (tempw * 6 - 15) + 10);

		s[0] = gradient4[gi0][0]*tempx + gradient4[gi0][1]*tempy + gradient4[gi0][2]*tempz + gradient4[gi0][3]*tempw;

		tempx = xc - (x0+1);
		t[0] = gradient4[gi1][0]*tempx + gradient4[gi1][1]*tempy + gradient4[gi1][2]*tempz + gradient4[gi1][3]*tempw;

		tempy = yc - (y0+1);
		v[0] = gradient4[gi3][0]*tempx + gradient4[gi3][1]*tempy + gradient4[gi3][2]*tempz + gradient4[gi3][3]*tempw;

		tempx = xc - x0;
		u[0] = gradient4[gi2][0]*tempx + gradient4[gi2][1]*tempy + gradient4[gi2][2]*tempz + gradient4[gi2][3]*tempw;

		tempy = yc - y0;
		tempz = zc - (z0+1);
		s[1] = gradient4[gi4][0]*tempx + gradient4[gi4][1]*tempy + gradient4[gi4][2]*tempz + gradient4[gi4][3]*tempw;

		tempx = xc - (x0+1);
		t[1] = gradient4[gi5][0]*tempx + gradient4[gi5][1]*tempy + gradient4[gi5][2]*tempz + gradient4[gi5][3]*tempw;

		tempy = yc - (y0+1);
		v[1] = gradient4[gi7][0]*tempx + gradient4[gi7][1]*tempy + gradient4[gi7][2]*tempz + gradient4[gi7][3]*tempw;

		tempx = xc - x0;
		u[1] = gradient4[gi6][0]*tempx + gradient4[gi6][1]*tempy + gradient4[gi6][2]*tempz + gradient4[gi6][3]*tempw;


		tempy = yc - y0;
		tempz = zc - z0;
		tempw = wc - (w0+1);
		s[2] = gradient4[gi8][0]*tempx + gradient4[gi8][1]*tempy + gradient4[gi8][2]*tempz + gradient4[gi8][3]*tempw;

		tempx = xc - (x0+1);
		t[2] = gradient4[gi9][0]*tempx + gradient4[gi9][1]*tempy + gradient4[gi9][2]*tempz + gradient4[gi9][3]*tempw;

		tempy = yc - (y0+1);
		v[2] = gradient4[gi11][0]*tempx + gradient4[gi11][1]*tempy + gradient4[gi11][2]*tempz + gradient4[gi11][3]*tempw;

		tempx = xc - x0;
		u[2] = gradient4[gi10][0]*tempx + gradient4[gi10][1]*tempy + gradient4[gi10][2]*tempz + gradient4[gi10][3]*tempw;


		tempy = yc - y0;
		tempz = zc - (z0+1);
		s[3] = gradient4[gi12][0]*tempx + gradient4[gi12][1]*tempy + gradient4[gi12][2]*tempz + gradient4[gi12][3]*tempw;

		tempx = xc - (x0+1);
		t[3] = gradient4[gi13][0]*tempx + gradient4[gi13][1]*tempy + gradient4[gi13][2]*tempz + gradient4[gi13][3]*tempw;

		tempy = yc - (y0+1);
		v[3] = gradient4[gi15][0]*tempx + gradient4[gi15][1]*tempy + gradient4[gi15][2]*tempz + gradient4[gi15][3]*tempw;

		tempx = xc - x0;
		u[3] = gradient4[gi14][0]*tempx + gradient4[gi14][1]*tempy + gradient4[gi14][2]*tempz + gradient4[gi14][3]*tempw;

		Li1 = s[0] + Cx*(t[0]-s[0]);
		Li2 = u[0] + Cx*(v[0]-u[0]);
		Li3 = s[1] + Cx*(t[1]-s[1]);
		Li4 = u[1] + Cx*(v[1]-u[1]);
		Li5 = s[2] + Cx*(t[2]-s[2]);
		Li6 = u[2] + Cx*(v[2]-u[2]);
		Li7 = s[3] + Cx*(t[3]-s[3]);
		Li8 = u[3] + Cx*(v[3]-u[3]);

		Li9 = Li1 + Cy*(Li2-Li1);
		Li10 = Li3 + Cy*(Li4-Li3);
		Li11 = Li5 + Cy*(Li6-Li5);
		Li12 = Li7 + Cy*(Li8-Li7);

		Li13 = Li9 + Cz*(Li10-Li9);
		Li14 = Li11 + Cz*(Li12-Li11);

		return Li13 + Cw*(Li14-Li13);
	}
}
