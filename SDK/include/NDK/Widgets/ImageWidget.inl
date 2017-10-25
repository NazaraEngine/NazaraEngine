// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/ImageWidget.hpp>

namespace Ndk
{
	inline const Nz::TextureRef& ImageWidget::GetTexture() const
	{
		return m_sprite->GetMaterial()->GetDiffuseMap();
	}

	inline void ImageWidget::SetTexture(const Nz::TextureRef& texture, bool resizeToContent)
	{
		m_sprite->SetTexture(texture, false);

		if (resizeToContent)
			ResizeToContent();
	}
}
