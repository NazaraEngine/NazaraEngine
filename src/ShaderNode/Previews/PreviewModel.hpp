#pragma once

#ifndef NAZARA_SHADERNODES_PREVIEWMODEL_HPP
#define NAZARA_SHADERNODES_PREVIEWMODEL_HPP

#include <ShaderNode/Enums.hpp>

class PreviewValues;

class PreviewModel
{
	public:
		PreviewModel() = default;
		virtual ~PreviewModel();

		virtual PreviewValues GetPreview(InputRole role, std::size_t roleIndex) const = 0;
};

#include <ShaderNode/Previews/PreviewModel.inl>

#endif
