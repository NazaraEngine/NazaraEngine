#pragma once

#ifndef NAZARA_SHADERNODES_QUADPREVIEW_HPP
#define NAZARA_SHADERNODES_QUADPREVIEW_HPP

#include <Previews/PreviewModel.hpp>
#include <QtGui/QImage>

class QuadPreview : public PreviewModel
{
	public:
		QuadPreview() = default;
		~QuadPreview() = default;

		QImage GetImage(InputRole role, std::size_t roleIndex) const override;
};

#include <Previews/QuadPreview.inl>

#endif
