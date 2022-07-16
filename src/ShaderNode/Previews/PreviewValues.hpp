#pragma once

#ifndef NAZARA_SHADERNODES_PREVIEWVALUES_HPP
#define NAZARA_SHADERNODES_PREVIEWVALUES_HPP

#include <Nazara/Prerequisites.hpp>
#include <ShaderNode/Enums.hpp>
#include <NZSL/Math/Vector.hpp>
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

		void Fill(const nzsl::Vector4f32& value);

		QImage GenerateImage() const;

		inline nzsl::Vector4f32* GetData();
		inline const nzsl::Vector4f32* GetData() const;
		inline std::size_t GetHeight() const;
		inline std::size_t GetWidth() const;

		PreviewValues Resized(std::size_t newWidth, std::size_t newHeight) const;

		nzsl::Vector4f32 Sample(float u, float v) const;

		nzsl::Vector4f32& operator()(std::size_t x, std::size_t y);
		nzsl::Vector4f32 operator()(std::size_t x, std::size_t y) const;

		PreviewValues& operator=(const PreviewValues&) = default;
		PreviewValues& operator=(PreviewValues&&) = default;

	private:
		std::size_t m_height;
		std::size_t m_width;
		std::vector<nzsl::Vector4f32> m_values;
};

#include <ShaderNode/Previews/PreviewValues.inl>

#endif
