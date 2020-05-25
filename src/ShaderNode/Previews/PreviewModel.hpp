#pragma once

#ifndef NAZARA_SHADERNODES_PREVIEWMODEL_HPP
#define NAZARA_SHADERNODES_PREVIEWMODEL_HPP

#include <ShaderNode/Enums.hpp>

class QImage;

class PreviewModel
{
	public:
		PreviewModel() = default;
		virtual ~PreviewModel();

		virtual QImage GetImage(InputRole role, std::size_t roleIndex) const = 0;
};

#include <ShaderNode/Previews/PreviewModel.inl>

#endif
