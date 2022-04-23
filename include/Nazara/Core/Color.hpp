// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_COLOR_HPP
#define NAZARA_CORE_COLOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	class Color
	{
		public:
			inline Color();
			inline Color(float red, float green, float blue, float alpha = 1.f);
			inline explicit Color(float lightness);
			inline Color(const Color& color) = default;
			inline Color(Color&& color) = default;
			inline ~Color() = default;

			inline bool IsOpaque() const;

			inline std::string ToString() const;

			inline Color operator+(const Color& angles) const;
			inline Color operator*(const Color& angles) const;

			inline Color& operator=(const Color& other) = default;
			inline Color& operator=(Color&& other) = default;

			inline Color operator+=(const Color& angles);
			inline Color operator*=(const Color& angles);

			inline bool operator==(const Color& angles) const;
			inline bool operator!=(const Color& angles) const;

			static inline Color FromCMY(float cyan, float magenta, float yellow);
			static inline Color FromCMYK(float cyan, float magenta, float yellow, float black);
			static inline Color FromHSL(float hue, float saturation, float lightness);
			static inline Color FromHSV(float hue, float saturation, float value);
			static inline Color FromRGB8(UInt8 r, UInt8 g, UInt8 b);
			static inline Color FromRGBA8(UInt8 r, UInt8 g, UInt8 b, UInt8 a);
			static inline Color FromXYZ(const Vector3f& vec);
			static inline Color FromXYZ(float x, float y, float z);
			static inline void ToCMY(const Color& color, float* cyan, float* magenta, float* yellow);
			static inline void ToCMYK(const Color& color, float* cyan, float* magenta, float* yellow, float* black);
			static inline void ToHSL(const Color& color, float* hue, float* saturation, float* lightness);
			static inline void ToHSV(const Color& color, float* hue, float* saturation, float* value);
			static inline void ToRGB8(const Color& color, UInt8* r, UInt8* g, UInt8* b);
			static inline void ToRGBA8(const Color& color, UInt8* r, UInt8* g, UInt8* b, UInt8* a = nullptr);
			static inline void ToXYZ(const Color& color, Vector3f* vec);
			static inline void ToXYZ(const Color& color, float* x, float* y, float* z);

			float r, g, b, a;

			static NAZARA_CORE_API const Color Black;
			static NAZARA_CORE_API const Color Blue;
			static NAZARA_CORE_API const Color Cyan;
			static NAZARA_CORE_API const Color Gray;
			static NAZARA_CORE_API const Color Green;
			static NAZARA_CORE_API const Color Magenta;
			static NAZARA_CORE_API const Color Orange;
			static NAZARA_CORE_API const Color Red;
			static NAZARA_CORE_API const Color Yellow;
			static NAZARA_CORE_API const Color White;

		private:
			static float Hue2RGB(float v1, float v2, float vH);
	};

	inline bool Serialize(SerializationContext& context, const Color& color, TypeTag<Color>);
	inline bool Unserialize(SerializationContext& context, Color* color, TypeTag<Color>);
}

std::ostream& operator<<(std::ostream& out, const Nz::Color& color);

#include <Nazara/Core/Color.inl>

#endif // NAZARA_CORE_COLOR_HPP
