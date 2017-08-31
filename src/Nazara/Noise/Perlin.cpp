// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Perlin.hpp>
#include <Nazara/Noise/NoiseTools.hpp>
#include <exception>
#include <stdexcept>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	Perlin::Perlin(unsigned int seed) :
	Perlin()
	{
		SetSeed(seed);
		Shuffle();
	}

	float Perlin::Get(float x, float y, float scale) const
	{
		float xc, yc;
		int x0, y0;
		int gi0,gi1,gi2,gi3;
		int ii, jj;

		float s,t,u,v;
		float Cx,Cy;
		float Li1, Li2;
		float tempx,tempy;

		xc = x * scale;
		yc = y * scale;

		x0 = fastfloor(xc);
		y0 = fastfloor(yc);

		ii = x0 & 255;
		jj = y0 & 255;

		gi0 = m_permutations[ii +     m_permutations[jj]] & 7;
		gi1 = m_permutations[ii + 1 + m_permutations[jj]] & 7;
		gi2 = m_permutations[ii +     m_permutations[jj + 1]] & 7;
		gi3 = m_permutations[ii + 1 + m_permutations[jj + 1]] & 7;

		tempx = xc - x0;
		tempy = yc - y0;

		Cx = tempx * tempx * tempx * (tempx * (tempx * 6 - 15) + 10);
		Cy = tempy * tempy * tempy * (tempy * (tempy * 6 - 15) + 10);

		s = s_gradients2[gi0][0]*tempx + s_gradients2[gi0][1]*tempy;

		tempx = xc - (x0 + 1);
		t = s_gradients2[gi1][0]*tempx + s_gradients2[gi1][1]*tempy;

		tempy = yc - (y0 + 1);
		v = s_gradients2[gi3][0]*tempx + s_gradients2[gi3][1]*tempy;

		tempx = xc - x0;
		u = s_gradients2[gi2][0]*tempx + s_gradients2[gi2][1]*tempy;

		Li1 = s + Cx*(t-s);
		Li2 = u + Cx*(v-u);

		return Li1 + Cy*(Li2-Li1);
	}

	float Perlin::Get(float x, float y, float z, float scale) const
	{
		float xc, yc, zc;
		int x0, y0, z0;
		int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7;
		int ii, jj, kk;

		float Li1,Li2,Li3,Li4,Li5,Li6;
		float s[2],t[2],u[2],v[2];
		float Cx,Cy,Cz;
		float tempx,tempy,tempz;

		xc = x * scale;
		yc = y * scale;
		zc = z * scale;

		x0 = fastfloor(xc);
		y0 = fastfloor(yc);
		z0 = fastfloor(zc);

		ii = x0 & 255;
		jj = y0 & 255;
		kk = z0 & 255;

		gi0 = m_permutations[ii +     m_permutations[jj +     m_permutations[kk]]] & 15;
		gi1 = m_permutations[ii + 1 + m_permutations[jj +     m_permutations[kk]]] & 15;
		gi2 = m_permutations[ii +     m_permutations[jj + 1 + m_permutations[kk]]] & 15;
		gi3 = m_permutations[ii + 1 + m_permutations[jj + 1 + m_permutations[kk]]] & 15;

		gi4 = m_permutations[ii +     m_permutations[jj +     m_permutations[kk + 1]]] & 15;
		gi5 = m_permutations[ii + 1 + m_permutations[jj +     m_permutations[kk + 1]]] & 15;
		gi6 = m_permutations[ii +     m_permutations[jj + 1 + m_permutations[kk + 1]]] & 15;
		gi7 = m_permutations[ii + 1 + m_permutations[jj + 1 + m_permutations[kk + 1]]] & 15;

		tempx = xc - x0;
		tempy = yc - y0;
		tempz = zc - z0;

		Cx = tempx * tempx * tempx * (tempx * (tempx * 6 - 15) + 10);
		Cy = tempy * tempy * tempy * (tempy * (tempy * 6 - 15) + 10);
		Cz = tempz * tempz * tempz * (tempz * (tempz * 6 - 15) + 10);

		s[0] = s_gradients3[gi0][0]*tempx + s_gradients3[gi0][1]*tempy + s_gradients3[gi0][2]*tempz;

		tempx = xc - (x0 + 1);
		t[0] = s_gradients3[gi1][0]*tempx + s_gradients3[gi1][1]*tempy + s_gradients3[gi1][2]*tempz;

		tempy = yc - (y0 + 1);
		v[0] = s_gradients3[gi3][0]*tempx + s_gradients3[gi3][1]*tempy + s_gradients3[gi3][2]*tempz;

		tempx = xc - x0;
		u[0] = s_gradients3[gi2][0]*tempx + s_gradients3[gi2][1]*tempy + s_gradients3[gi2][2]*tempz;

		tempy = yc - y0;
		tempz = zc - (z0 + 1);
		s[1] = s_gradients3[gi4][0]*tempx + s_gradients3[gi4][1]*tempy + s_gradients3[gi4][2]*tempz;

		tempx = xc - (x0 + 1);
		t[1] = s_gradients3[gi5][0]*tempx + s_gradients3[gi5][1]*tempy + s_gradients3[gi5][2]*tempz;

		tempy = yc - (y0 + 1);
		v[1] = s_gradients3[gi7][0]*tempx + s_gradients3[gi7][1]*tempy + s_gradients3[gi7][2]*tempz;

		tempx = xc - x0;
		u[1] = s_gradients3[gi6][0]*tempx + s_gradients3[gi6][1]*tempy + s_gradients3[gi6][2]*tempz;

		Li1 = s[0] + Cx*(t[0]-s[0]);
		Li2 = u[0] + Cx*(v[0]-u[0]);
		Li3 = s[1] + Cx*(t[1]-s[1]);
		Li4 = u[1] + Cx*(v[1]-u[1]);

		Li5 = Li1 + Cy * (Li2-Li1);
		Li6 = Li3 + Cy * (Li4-Li3);

		return Li5 + Cz * (Li6-Li5);
	}

	float Perlin::Get(float x, float y, float z, float w, float scale) const
	{
		float xc,yc,zc,wc;
		int x0,y0,z0,w0;
		int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7,gi8,gi9,gi10,gi11,gi12,gi13,gi14,gi15;
		int ii,jj,kk,ll;

		float Li1,Li2,Li3,Li4,Li5,Li6,Li7,Li8,Li9,Li10,Li11,Li12,Li13,Li14;
		float s[4],t[4],u[4],v[4];
		float Cx,Cy,Cz,Cw;

		float tempx,tempy,tempz,tempw;

		xc = x * scale;
		yc = y * scale;
		zc = z * scale;
		wc = w * scale;

		x0 = fastfloor(xc);
		y0 = fastfloor(yc);
		z0 = fastfloor(zc);
		w0 = fastfloor(wc);

		ii = x0 & 255;
		jj = y0 & 255;
		kk = z0 & 255;
		ll = w0 & 255;

		gi0 =  m_permutations[ii     + m_permutations[jj     + m_permutations[kk     + m_permutations[ll]]]] & 31;
		gi1 =  m_permutations[ii + 1 + m_permutations[jj     + m_permutations[kk     + m_permutations[ll]]]] & 31;
		gi2 =  m_permutations[ii     + m_permutations[jj + 1 + m_permutations[kk     + m_permutations[ll]]]] & 31;
		gi3 =  m_permutations[ii + 1 + m_permutations[jj + 1 + m_permutations[kk     + m_permutations[ll]]]] & 31;

		gi4 =  m_permutations[ii     + m_permutations[jj +   + m_permutations[kk + 1 + m_permutations[ll]]]] & 31;
		gi5 =  m_permutations[ii + 1 + m_permutations[jj +   + m_permutations[kk + 1 + m_permutations[ll]]]] & 31;
		gi6 =  m_permutations[ii     + m_permutations[jj + 1 + m_permutations[kk + 1 + m_permutations[ll]]]] & 31;
		gi7 =  m_permutations[ii + 1 + m_permutations[jj + 1 + m_permutations[kk + 1 + m_permutations[ll]]]] & 31;

		gi8 =  m_permutations[ii     + m_permutations[jj     + m_permutations[kk     + m_permutations[ll + 1]]]] & 31;
		gi9 =  m_permutations[ii + 1 + m_permutations[jj     + m_permutations[kk     + m_permutations[ll + 1]]]] & 31;
		gi10 = m_permutations[ii     + m_permutations[jj + 1 + m_permutations[kk     + m_permutations[ll + 1]]]] & 31;
		gi11 = m_permutations[ii + 1 + m_permutations[jj + 1 + m_permutations[kk     + m_permutations[ll + 1]]]] & 31;

		gi12 = m_permutations[ii     + m_permutations[jj     + m_permutations[kk + 1 + m_permutations[ll + 1]]]] & 31;
		gi13 = m_permutations[ii + 1 + m_permutations[jj     + m_permutations[kk + 1 + m_permutations[ll + 1]]]] & 31;
		gi14 = m_permutations[ii     + m_permutations[jj + 1 + m_permutations[kk + 1 + m_permutations[ll + 1]]]] & 31;
		gi15 = m_permutations[ii + 1 + m_permutations[jj + 1 + m_permutations[kk + 1 + m_permutations[ll + 1]]]] & 31;

		tempx = xc - x0;
		tempy = yc - y0;
		tempz = zc - z0;
		tempw = wc - w0;

		Cx = tempx * tempx * tempx * (tempx * (tempx * 6 - 15) + 10);
		Cy = tempy * tempy * tempy * (tempy * (tempy * 6 - 15) + 10);
		Cz = tempz * tempz * tempz * (tempz * (tempz * 6 - 15) + 10);
		Cw = tempw * tempw * tempw * (tempw * (tempw * 6 - 15) + 10);

		s[0] = s_gradients4[gi0][0]*tempx + s_gradients4[gi0][1]*tempy + s_gradients4[gi0][2]*tempz + s_gradients4[gi0][3]*tempw;

		tempx = xc - (x0+1);
		t[0] = s_gradients4[gi1][0]*tempx + s_gradients4[gi1][1]*tempy + s_gradients4[gi1][2]*tempz + s_gradients4[gi1][3]*tempw;

		tempy = yc - (y0+1);
		v[0] = s_gradients4[gi3][0]*tempx + s_gradients4[gi3][1]*tempy + s_gradients4[gi3][2]*tempz + s_gradients4[gi3][3]*tempw;

		tempx = xc - x0;
		u[0] = s_gradients4[gi2][0]*tempx + s_gradients4[gi2][1]*tempy + s_gradients4[gi2][2]*tempz + s_gradients4[gi2][3]*tempw;

		tempy = yc - y0;
		tempz = zc - (z0+1);
		s[1] = s_gradients4[gi4][0]*tempx + s_gradients4[gi4][1]*tempy + s_gradients4[gi4][2]*tempz + s_gradients4[gi4][3]*tempw;

		tempx = xc - (x0+1);
		t[1] = s_gradients4[gi5][0]*tempx + s_gradients4[gi5][1]*tempy + s_gradients4[gi5][2]*tempz + s_gradients4[gi5][3]*tempw;

		tempy = yc - (y0+1);
		v[1] = s_gradients4[gi7][0]*tempx + s_gradients4[gi7][1]*tempy + s_gradients4[gi7][2]*tempz + s_gradients4[gi7][3]*tempw;

		tempx = xc - x0;
		u[1] = s_gradients4[gi6][0]*tempx + s_gradients4[gi6][1]*tempy + s_gradients4[gi6][2]*tempz + s_gradients4[gi6][3]*tempw;


		tempy = yc - y0;
		tempz = zc - z0;
		tempw = wc - (w0+1);
		s[2] = s_gradients4[gi8][0]*tempx + s_gradients4[gi8][1]*tempy + s_gradients4[gi8][2]*tempz + s_gradients4[gi8][3]*tempw;

		tempx = xc - (x0+1);
		t[2] = s_gradients4[gi9][0]*tempx + s_gradients4[gi9][1]*tempy + s_gradients4[gi9][2]*tempz + s_gradients4[gi9][3]*tempw;

		tempy = yc - (y0+1);
		v[2] = s_gradients4[gi11][0]*tempx + s_gradients4[gi11][1]*tempy + s_gradients4[gi11][2]*tempz + s_gradients4[gi11][3]*tempw;

		tempx = xc - x0;
		u[2] = s_gradients4[gi10][0]*tempx + s_gradients4[gi10][1]*tempy + s_gradients4[gi10][2]*tempz + s_gradients4[gi10][3]*tempw;


		tempy = yc - y0;
		tempz = zc - (z0+1);
		s[3] = s_gradients4[gi12][0]*tempx + s_gradients4[gi12][1]*tempy + s_gradients4[gi12][2]*tempz + s_gradients4[gi12][3]*tempw;

		tempx = xc - (x0+1);
		t[3] = s_gradients4[gi13][0]*tempx + s_gradients4[gi13][1]*tempy + s_gradients4[gi13][2]*tempz + s_gradients4[gi13][3]*tempw;

		tempy = yc - (y0+1);
		v[3] = s_gradients4[gi15][0]*tempx + s_gradients4[gi15][1]*tempy + s_gradients4[gi15][2]*tempz + s_gradients4[gi15][3]*tempw;

		tempx = xc - x0;
		u[3] = s_gradients4[gi14][0]*tempx + s_gradients4[gi14][1]*tempy + s_gradients4[gi14][2]*tempz + s_gradients4[gi14][3]*tempw;

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
