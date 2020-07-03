#pragma once

#ifndef NAZARA_SHADERNODES_QUADPREVIEW_HPP
#define NAZARA_SHADERNODES_QUADPREVIEW_HPP

#include <ShaderNode/Previews/PreviewModel.hpp>
#include <ShaderNode/Previews/PreviewValues.hpp>

class QuadPreview : public PreviewModel
{
	public:
		QuadPreview() = default;
		~QuadPreview() = default;

		PreviewValues GetPreview(InputRole role, std::size_t roleIndex) const override;
};

#include <ShaderNode/Previews/QuadPreview.inl>

#endif
