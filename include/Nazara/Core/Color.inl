// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://www.easyrgb.com/index.php?X=MATH

#include <Nazara/Core/StringStream.hpp>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

inline NzColor::NzColor()
{
}

inline NzColor::NzColor(nzUInt8 red, nzUInt8 green, nzUInt8 blue, nzUInt8 alpha) :
r(red),
g(green),
b(blue),
a(alpha)
{
}

inline NzColor::NzColor(nzUInt8 lightness) :
r(lightness),
g(lightness),
b(lightness),
a(255)
{
}

inline NzColor::NzColor(nzUInt8 vec[3], nzUInt8 alpha) :
r(vec[0]),
g(vec[1]),
b(vec[2]),
a(alpha)
{
}

inline NzString NzColor::ToString() const
{
	NzStringStream ss;
	ss << "Color(" << static_cast<int>(r) << ", " << static_cast<int>(g) << ", " << static_cast<int>(b);

	if (a != 255)
		ss << ", " << static_cast<int>(a);

	ss << ')';

	return ss;
}

inline NzColor NzColor::operator+(const NzColor& color) const
{
	return NzColor(std::min(static_cast<nzUInt16>(r) + color.r, 255),
	               std::min(static_cast<nzUInt16>(r) + color.r, 255),
				   std::min(static_cast<nzUInt16>(r) + color.r, 255),
	               std::min(static_cast<nzUInt16>(r) + color.r, 255));
}

inline NzColor NzColor::operator*(const NzColor& color) const
{
	return NzColor(static_cast<nzUInt16>(r) * color.r/255,
	               static_cast<nzUInt16>(r) * color.r/255,
				   static_cast<nzUInt16>(r) * color.r/255,
	               static_cast<nzUInt16>(r) * color.r/255);
}

inline NzColor NzColor::operator+=(const NzColor& color)
{
	return operator=(operator+(color));
}

inline NzColor NzColor::operator*=(const NzColor& color)
{
	return operator=(operator+(color));
}

inline bool NzColor::operator==(const NzColor& color) const
{
	return r == color.r && g == color.g && b == color.b && a == color.a;
}

inline bool NzColor::operator!=(const NzColor& color) const
{
	return !operator==(color);
}

// Algorithmes venant de http://www.easyrgb.com/index.php?X=MATH

inline NzColor NzColor::FromCMY(float cyan, float magenta, float yellow)
{
	return NzColor((1.f-cyan)*255.f, (1.f-magenta)*255.f, (1.f-yellow)*255.f);
}

inline NzColor NzColor::FromCMYK(float cyan, float magenta, float yellow, float black)
{
	return FromCMY(cyan * (1.f - black) + black,
	               magenta * (1.f - black) + black,
	               yellow * (1.f - black) + black);
}

inline NzColor NzColor::FromHSL(nzUInt8 hue, nzUInt8 saturation, nzUInt8 lightness)
{
	// Norme Windows
	float l = lightness/240.f;

	if (saturation == 0)
	{
		// RGB results from 0 to 255
		return NzColor(lightness * 255.f,
		               lightness * 255.f,
		               lightness * 255.f);
	}
	else
	{
		float h = hue/240.f;
		float s = saturation/240.f;

		float v2;
		if (l < 0.5f)
			v2 = l * (1.f + s);
		else
			v2 = (l + s) - (s*l);

		float v1 = 2.f * l - v2;

		return NzColor(255.f * Hue2RGB(v1, v2, h + (1.f/3.f)),
		               255.f * Hue2RGB(v1, v2, h),
		               255.f * Hue2RGB(v1, v2, h - (1.f/3.f)));
	}
}

inline NzColor NzColor::FromHSV(float hue, float saturation, float value)
{
	if (NzNumberEquals(saturation, 0.f))
		return NzColor(static_cast<nzUInt8>(value * 255.f));
	else
	{
		float h = hue/360.f * 6.f;
		float s = saturation/360.f;

		if (NzNumberEquals(h, 6.f))
			h = 0; // hue must be < 1

		int i = h;
		float v1 = value * (1.f - s);
		float v2 = value * (1.f - s * (h - i));
		float v3 = value * (1.f - s * (1.f - (h - i)));

		float r, g, b;
		switch (i)
		{
			case 0:
				r = value;
				g = v3;
				b = v1;

			case 1:
				r = v2;
				g = value;
				b = v1;

			case 2:
				r = v1;
				g = value;
				b = v3;

			case 3:
				r = v1;
				g = v2;
				b = value;

			case 4:
				r = v3;
				g = v1;
				b = value;

			default:
				r = value;
				g = v1;
				b = v2;
		}

		// RGB results from 0 to 255
		return NzColor(r*255.f, g*255.f, b*255.f);
	}
}
inline NzColor NzColor::FromXYZ(const NzVector3f& vec)
{
	return FromXYZ(vec.x, vec.y, vec.z);
}

inline NzColor NzColor::FromXYZ(float x, float y, float z)
{
	x /= 100; // X from 0 to  95.047
	y /= 100; // Y from 0 to 100.000
	z /= 100; // Z from 0 to 108.883

	float r = x *  3.2406f + y * -1.5372f + z * -0.4986f;
	float g = x * -0.9689f + y *  1.8758f + z *  0.0415f;
	float b = x *  0.0557f + y * -0.2040f + z *  1.0570f;

	if (r > 0.0031308f)
		r = 1.055f * (std::pow(r, 1.f/2.4f)) - 0.055f;
	else
		r *= 12.92f;

	if (g > 0.0031308f)
		g = 1.055f * (std::pow(r, 1.f/2.4f)) - 0.055f;
	else
		g *= 12.92f;

	if (b > 0.0031308f)
		b = 1.055f * (std::pow(r, 1.f/2.4f)) - 0.055f;
	else
		b *= 12.92f;

	return NzColor(r * 255.f, g * 255.f, b * 255.f);
}

inline void NzColor::ToCMY(const NzColor& color, float* cyan, float* magenta, float* yellow)
{
	*cyan = 1.f - color.r/255.f;
	*magenta = 1.f - color.g/255.f;
	*yellow = 1.f - color.b/255.f;
}

inline void NzColor::ToCMYK(const NzColor& color, float* cyan, float* magenta, float* yellow, float* black)
{
	float c, m, y;
	ToCMY(color, &c, &m, &y);

	float k = std::min(std::min(std::min(1.f, c), m), y);

	if (NzNumberEquals(k, 1.f))
	{
		//Black
		*cyan = 0.f;
		*magenta = 0.f;
		*yellow = 0.f;
	}
	else
	{
		*cyan = (c-k)/(1.f-k);
		*magenta = (m-k)/(1.f-k);
		*yellow = (y-k)/(1.f-k);
	}

	*black = k;
}

inline void NzColor::ToHSL(const NzColor& color, nzUInt8* hue, nzUInt8* saturation, nzUInt8* lightness)
{
	float r = color.r / 255.f;
	float g = color.g / 255.f;
	float b = color.b / 255.f;

	float min = std::min(std::min(r, g), b); // Min. value of RGB
	float max = std::max(std::max(r, g), b); // Max. value of RGB

	float deltaMax = max - min; //Delta RGB value

	float l = (max + min)/2.f;

	if (NzNumberEquals(deltaMax, 0.f))
	{
		//This is a gray, no chroma...
		*hue = 0; //HSL results from 0 to 1
		*saturation = 0;
	}
	else
	{
		//Chromatic data...
		if (l < 0.5f)
			*saturation = deltaMax/(max+min)*240.f;
		else
			*saturation = deltaMax/(2.f-max-min)*240.f;

		*lightness = l*240.f;

		float deltaR = ((max - r)/6.f + deltaMax/2.f)/deltaMax;
		float deltaG = ((max - g)/6.f + deltaMax/2.f)/deltaMax;
		float deltaB = ((max - b)/6.f + deltaMax/2.f)/deltaMax;

		float h;

		if (NzNumberEquals(r, max))
			h = deltaB - deltaG;
		else if (NzNumberEquals(g, max))
			h = (1.f/3.f) + deltaR - deltaB;
		else if (NzNumberEquals(b, max))
			h = (2.f/3.f) + deltaG - deltaR;

		if (h < 0.f)
			h += 1.f;
		else if (h > 1.f)
			h -= 1.f;

		*hue = h*240.f;
	}
}

inline void NzColor::ToHSV(const NzColor& color, float* hue, float* saturation, float* value)
{
	float r = color.r / 255.f;
	float g = color.g / 255.f;
	float b = color.b / 255.f;

	float min = std::min(std::min(r, g), b);    //Min. value of RGB
	float max = std::max(std::max(r, g), b);    //Max. value of RGB

	float deltaMax = max - min; //Delta RGB value

	*value = max;

	if (NzNumberEquals(deltaMax, 0.f))
	{
		//This is a gray, no chroma...
		*hue = 0; //HSV results from 0 to 1
		*saturation = 0;
	}
	else
	{
		//Chromatic data...
		*saturation = deltaMax/max*360.f;

		float deltaR = ((max - r)/6.f + deltaMax/2.f)/deltaMax;
		float deltaG = ((max - g)/6.f + deltaMax/2.f)/deltaMax;
		float deltaB = ((max - b)/6.f + deltaMax/2.f)/deltaMax;

		float h;

		if (NzNumberEquals(r, max))
			h = deltaB - deltaG;
		else if (NzNumberEquals(g, max))
			h = (1.f/3.f) + deltaR - deltaB;
		else if (NzNumberEquals(b, max))
			h = (2.f/3.f) + deltaG - deltaR;

		if (h < 0.f)
			h += 1.f;
		else if (h > 1.f)
			h -= 1.f;

		*hue = h*360.f;
	}
}

inline void NzColor::ToXYZ(const NzColor& color, NzVector3f* vec)
{
	return ToXYZ(color, &vec->x, &vec->y, &vec->z);
}

inline void NzColor::ToXYZ(const NzColor& color, float* x, float* y, float* z)
{
	double r = color.r/255.0;        //R from 0 to 255
	double g = color.g/255.0;        //G from 0 to 255
	double b = color.b/255.0;        //B from 0 to 255

	if (r > 0.04045)
		r = std::pow((r + 0.055)/1.055, 2.4);
	else
		r /= 12.92;

	if (g > 0.04045)
		g = std::pow((g + 0.055)/1.055, 2.4);
	else
		g /= 12.92;

	if (b > 0.04045)
		b = std::pow((b + 0.055)/1.055, 2.4);
	else
		b /= 12.92;

	r *= 100.0;
	g *= 100.0;
	b *= 100.0;

	//Observer. = 2°, Illuminant = D65
	*x = r*0.4124 + g*0.3576 + b*0.1805;
	*y = r*0.2126 + g*0.7152 + b*0.0722;
	*z = r*0.0193 + g*0.1192 + b*0.9505;
}

inline float NzColor::Hue2RGB(float v1, float v2, float vH)
{
	if (vH < 0.f)
		vH += 1;

	if (vH > 1.f)
		vH -= 1;

	if ((6.f * vH) < 1.f)
		return v1 + (v2-v1)*6*vH;

	if ((2.f * vH) < 1.f)
		return v2;

	if ((3.f * vH) < 2.f)
		return v1 + (v2 - v1)*(2.f/3.f - vH)*6;

	return v1;
}

inline std::ostream& operator<<(std::ostream& out, const NzColor& color)
{
	return out << color.ToString();
}

#include <Nazara/Core/DebugOff.hpp>
