// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COLOR_HPP
#define NAZARA_COLOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct SerializationContext;

	class Color
	{
		public:
			inline Color();
			inline Color(UInt8 red, UInt8 green, UInt8 blue, UInt8 alpha = 255);
			inline explicit Color(UInt8 lightness);
			inline Color(UInt8 color[3], UInt8 alpha = 255);
			inline Color(const Color& color) = default;
			inline ~Color() = default;

			inline bool IsOpaque() const;

			inline String ToString() const;

			inline Color operator+(const Color& angles) const;
			inline Color operator*(const Color& angles) const;

			inline Color operator+=(const Color& angles);
			inline Color operator*=(const Color& angles);

			inline bool operator==(const Color& angles) const;
			inline bool operator!=(const Color& angles) const;

			static inline Color FromCMY(float cyan, float magenta, float yellow);
			static inline Color FromCMYK(float cyan, float magenta, float yellow, float black);
			static inline Color FromHSL(UInt8 hue, UInt8 saturation, UInt8 lightness);
			static inline Color FromHSV(float hue, float saturation, float value);
			static inline Color FromXYZ(const Vector3f& vec);
			static inline Color FromXYZ(float x, float y, float z);
			static inline void ToCMY(const Color& color, float* cyan, float* magenta, float* yellow);
			static inline void ToCMYK(const Color& color, float* cyan, float* magenta, float* yellow, float* black);
			static inline void ToHSL(const Color& color, UInt8* hue, UInt8* saturation, UInt8* lightness);
			static inline void ToHSV(const Color& color, float* hue, float* saturation, float* value);
			static inline void ToXYZ(const Color& color, Vector3f* vec);
			static inline void ToXYZ(const Color& color, float* x, float* y, float* z);

			UInt8 r, g, b, a;

			static NAZARA_CORE_API const Color Black;
			static NAZARA_CORE_API const Color Blue;
			static NAZARA_CORE_API const Color Cyan;
			static NAZARA_CORE_API const Color Green;
			static NAZARA_CORE_API const Color Magenta;
			static NAZARA_CORE_API const Color Orange;
			static NAZARA_CORE_API const Color Red;
			static NAZARA_CORE_API const Color Yellow;
			static NAZARA_CORE_API const Color White;

		private:
			static float Hue2RGB(float v1, float v2, float vH);
	};

	inline bool Serialize(SerializationContext& context, const Color& color);
	inline bool Unserialize(SerializationContext& context, Color* color);
}

std::ostream& operator<<(std::ostream& out, const Nz::Color& color);

#include <Nazara/Core/Color.inl>

#endif // NAZARA_COLOR_HPP
