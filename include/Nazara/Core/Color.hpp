// Copyright (C) 2015 Jérôme Leclercq
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
		inline NzColor();
		inline NzColor(nzUInt8 red, nzUInt8 green, nzUInt8 blue, nzUInt8 alpha = 255);
		inline explicit NzColor(nzUInt8 lightness);
		inline NzColor(nzUInt8 color[3], nzUInt8 alpha = 255);
		inline NzColor(const NzColor& color) = default;
		inline ~NzColor() = default;

		inline NzString ToString() const;

		inline NzColor operator+(const NzColor& angles) const;
		inline NzColor operator*(const NzColor& angles) const;

		inline NzColor operator+=(const NzColor& angles);
		inline NzColor operator*=(const NzColor& angles);

		inline bool operator==(const NzColor& angles) const;
		inline bool operator!=(const NzColor& angles) const;

		static inline NzColor FromCMY(float cyan, float magenta, float yellow);
		static inline NzColor FromCMYK(float cyan, float magenta, float yellow, float black);
		static inline NzColor FromHSL(nzUInt8 hue, nzUInt8 saturation, nzUInt8 lightness);
		static inline NzColor FromHSV(float hue, float saturation, float value);
		static inline NzColor FromXYZ(const NzVector3f& vec);
		static inline NzColor FromXYZ(float x, float y, float z);
		static inline void ToCMY(const NzColor& color, float* cyan, float* magenta, float* yellow);
		static inline void ToCMYK(const NzColor& color, float* cyan, float* magenta, float* yellow, float* black);
		static inline void ToHSL(const NzColor& color, nzUInt8* hue, nzUInt8* saturation, nzUInt8* lightness);
		static inline void ToHSV(const NzColor& color, float* hue, float* saturation, float* value);
		static inline void ToXYZ(const NzColor& color, NzVector3f* vec);
		static inline void ToXYZ(const NzColor& color, float* x, float* y, float* z);

		nzUInt8 r, g, b, a;

		static NAZARA_CORE_API const NzColor Black;
		static NAZARA_CORE_API const NzColor Blue;
		static NAZARA_CORE_API const NzColor Cyan;
		static NAZARA_CORE_API const NzColor Green;
		static NAZARA_CORE_API const NzColor Magenta;
		static NAZARA_CORE_API const NzColor Orange;
		static NAZARA_CORE_API const NzColor Red;
		static NAZARA_CORE_API const NzColor Yellow;
		static NAZARA_CORE_API const NzColor White;

	private:
		static float Hue2RGB(float v1, float v2, float vH);
};

std::ostream& operator<<(std::ostream& out, const NzColor& color);

#include <Nazara/Core/Color.inl>

#endif // NAZARA_COLOR_HPP
