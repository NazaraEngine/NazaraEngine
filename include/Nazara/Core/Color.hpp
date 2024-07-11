// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_COLOR_HPP
#define NAZARA_CORE_COLOR_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Serialization.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <limits>
#include <string>

namespace Nz
{
	struct SerializationContext;

	class Color
	{
		public:
			Color() = default;
			constexpr Color(float red, float green, float blue, float alpha = 1.f);
			constexpr explicit Color(float lightness);
			constexpr Color(const Color&) = default;
			constexpr Color(Color&&) = default;
			~Color() = default;

			constexpr bool ApproxEqual(const Color& color, float maxDifference = std::numeric_limits<float>::epsilon()) const;

			constexpr bool IsOpaque() const;

			inline std::string ToString() const;

			constexpr Color operator+(const Color& color) const;
			constexpr Color operator-(const Color& color) const;
			constexpr Color operator*(float scale) const;
			constexpr Color operator*(const Color& color) const;

			constexpr Color& operator=(const Color&) = default;
			constexpr Color& operator=(Color&&) = default;

			constexpr Color operator+=(const Color& color);
			constexpr Color operator*=(const Color& color);

			constexpr bool operator==(const Color& color) const;
			constexpr bool operator!=(const Color& color) const;

			static constexpr bool ApproxEqual(const Color& lhs, const Color& rhs, float maxDifference = std::numeric_limits<float>::epsilon());
			static constexpr Color FromCMY(float cyan, float magenta, float yellow);
			static constexpr Color FromCMYK(float cyan, float magenta, float yellow, float black);
			static constexpr Color FromHSL(float hue, float saturation, float lightness);
			static constexpr Color FromHSV(float hue, float saturation, float value);
			static constexpr Color FromRGB8(UInt8 r, UInt8 g, UInt8 b);
			static constexpr Color FromRGBA8(UInt8 r, UInt8 g, UInt8 b, UInt8 a);
			static constexpr Color FromTemperature(float temperature);
			static constexpr Color FromXYZ(const Vector3f& vec);
			static constexpr Color FromXYZ(float x, float y, float z);
			static constexpr void ToCMY(const Color& color, float* cyan, float* magenta, float* yellow);
			static constexpr void ToCMYK(const Color& color, float* cyan, float* magenta, float* yellow, float* black);
			static constexpr void ToHSL(const Color& color, float* hue, float* saturation, float* lightness);
			static constexpr void ToHSV(const Color& color, float* hue, float* saturation, float* value);
			static constexpr void ToRGB8(const Color& color, UInt8* r, UInt8* g, UInt8* b);
			static constexpr void ToRGBA8(const Color& color, UInt8* r, UInt8* g, UInt8* b, UInt8* a = nullptr);
			static constexpr void ToXYZ(const Color& color, Vector3f* vec);
			static constexpr void ToXYZ(const Color& color, float* x, float* y, float* z);

			// Color-space handling
			static constexpr float LinearTosRGB(float component);
			static constexpr Color LinearTosRGB(const Color& color);
			static constexpr float sRGBToLinear(float component);
			static constexpr Color sRGBToLinear(const Color& color);

			static constexpr Color Black();
			static constexpr Color Blue();
			static constexpr Color Cyan();
			static constexpr Color DarkBlue();
			static constexpr Color DarkGreen();
			static constexpr Color DarkRed();
			static constexpr Color Gray();
			static constexpr Color Green();
			static constexpr Color Magenta();
			static constexpr Color Orange();
			static constexpr Color Red();
			static constexpr Color Yellow();
			static constexpr Color White();

			float r, g, b, a;

		private:
			static constexpr float Hue2RGB(float v1, float v2, float vH);
	};

	inline bool Serialize(SerializationContext& context, const Color& color, TypeTag<Color>);
	inline bool Deserialize(SerializationContext& context, Color* color, TypeTag<Color>);

	inline std::ostream& operator<<(std::ostream& out, const Color& color);

	constexpr Color operator*(float scale, const Color& color);
}

#include <Nazara/Core/Color.inl>

#endif // NAZARA_CORE_COLOR_HPP
