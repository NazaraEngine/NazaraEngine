#pragma once

#ifndef NAZARA_SHADERNODES_PREVIEWVALUES_HPP
#define NAZARA_SHADERNODES_PREVIEWVALUES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <ShaderNode/Enums.hpp>
#include <array>
#include <vector>

class QImage;

class PreviewValues
{
	public:
		PreviewValues();
		PreviewValues(std::size_t width, std::size_t height);
		PreviewValues(const PreviewValues&) = default;
		PreviewValues(PreviewValues&&) = default;
		~PreviewValues() = default;

		void Fill(const Nz::Vector4f& value);

		QImage GenerateImage() const;

		inline Nz::Vector4f* GetData();
		inline const Nz::Vector4f* GetData() const;
		inline std::size_t GetHeight() const;
		inline std::size_t GetWidth() const;

		PreviewValues Resized(std::size_t newWidth, std::size_t newHeight) const;

		Nz::Vector4f Sample(float u, float v) const;

		Nz::Vector4f& operator()(std::size_t x, std::size_t y);
		Nz::Vector4f operator()(std::size_t x, std::size_t y) const;

		PreviewValues& operator=(const PreviewValues&) = default;
		PreviewValues& operator=(PreviewValues&&) = default;

	private:
		std::size_t m_height;
		std::size_t m_width;
		std::vector<Nz::Vector4f> m_values;
};

#include <ShaderNode/Previews/PreviewValues.inl>

#endif
