// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COLOR_HPP
#define NAZARA_COLOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>

class NzColor
{
	public:
		NzColor();
		NzColor(nzUInt8 red, nzUInt8 green, nzUInt8 blue, nzUInt8 alpha = 255);
		explicit NzColor(nzUInt8 lightness);
		NzColor(nzUInt8 color[3], nzUInt8 alpha = 255);
		NzColor(const NzColor& color) = default;
		~NzColor() = default;

		NzString ToString() const;

		NzColor operator+(const NzColor& angles) const;
		NzColor operator*(const NzColor& angles) const;

		NzColor operator+=(const NzColor& angles);
		NzColor operator*=(const NzColor& angles);

		bool operator==(const NzColor& angles) const;
		bool operator!=(const NzColor& angles) const;

		static NzColor FromCMY(float cyan, float magenta, float yellow);
		static NzColor FromCMYK(float cyan, float magenta, float yellow, float black);
		static NzColor FromHSL(nzUInt8 hue, nzUInt8 saturation, nzUInt8 lightness);
		static NzColor FromHSV(float hue, float saturation, float value);
		static NzColor FromXYZ(const NzVector3f& vec);
		static NzColor FromXYZ(float x, float y, float z);
		static void ToCMY(const NzColor& color, float* cyan, float* magenta, float* yellow);
		static void ToCMYK(const NzColor& color, float* cyan, float* magenta, float* yellow, float* black);
		static void ToHSL(const NzColor& color, nzUInt8* hue, nzUInt8* saturation, nzUInt8* lightness);
		static void ToHSV(const NzColor& color, float* hue, float* saturation, float* value);
		static void ToXYZ(const NzColor& color, NzVector3f* vec);
		static void ToXYZ(const NzColor& color, float* x, float* y, float* z);

		nzUInt8 r, g, b, a;

		static NAZARA_API const NzColor Black;
		static NAZARA_API const NzColor Blue;
		static NAZARA_API const NzColor Cyan;
		static NAZARA_API const NzColor Green;
		static NAZARA_API const NzColor Magenta;
		static NAZARA_API const NzColor Orange;
		static NAZARA_API const NzColor Red;
		static NAZARA_API const NzColor Yellow;
		static NAZARA_API const NzColor White;

	private:
		static float Hue2RGB(float v1, float v2, float vH);
};

std::ostream& operator<<(std::ostream& out, const NzColor& color);

#include <Nazara/Core/Color.inl>

#endif // NAZARA_COLOR_HPP
