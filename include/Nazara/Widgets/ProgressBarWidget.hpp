// Copyright (C) 2022 Samy Bensaid
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_PROGRESSBARWIDGET_HPP
#define NAZARA_WIDGETS_PROGRESSBARWIDGET_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/ClientPrerequisites.hpp>

namespace Nz
{
	class NDK_CLIENT_API ProgressBarWidget : public BaseWidget
	{
		friend class Sdk;

		public:
			ProgressBarWidget(BaseWidget* parent);
			ProgressBarWidget(const ProgressBarWidget&) = delete;
			ProgressBarWidget(ProgressBarWidget&&) = default;
			~ProgressBarWidget() = default;

			//virtual ProgressBarWidget* Clone() const = 0;

			inline void EnableText(bool enable = true);
			inline void EnableBorder(bool enable = true);

			inline bool IsTextEnabled() const;
			inline bool IsBorderEnabled() const;


			inline unsigned GetPercentageValue() const;
			inline Nz::Vector2f GetProgressBarSize() const;
			inline Nz::Vector2f GetProgressBarBorderSize() const;
			inline float GetTextMargin() const;


			inline const Nz::Color& GetBarBackgroundColor() const;
			inline const Nz::Color& GetBarBackgroundCornerColor() const;
			inline const Nz::Color& GetBarColor() const;
			inline const Nz::Color& GetBarCornerColor() const;

			inline const Nz::TextureRef& GetBarBackgroundTexture() const;
			inline const Nz::TextureRef& GetBarTexture() const;

			static const Nz::Color& GetDefaultBarColor();
			static const Nz::Color& GetDefaultBarCornerColor();
			static const Nz::Color& GetDefaultBarBackgroundColor();
			static const Nz::Color& GetDefaultBarBackgroundCornerColor();


			inline void SetBarBackgroundColor(const Nz::Color& globalColor, const Nz::Color& cornerColor);
			inline void SetBarBackgroundTexture(Nz::TextureRef texture, bool resetColors = true);
			inline void SetBarColor(const Nz::Color& globalColor, const Nz::Color& cornerColor);
			inline void SetBarTexture(Nz::TextureRef texture, bool resetColors = true);


			inline void SetPercentageValue(unsigned percentage);
			inline void SetTextMargin(float margin);
			inline void SetTextColor(const Nz::Color& color);

			NazaraSignal(OnValueChanged, const ProgressBarWidget* /*progressBar*/);

		private:
			void Layout() override;
			inline void UpdateText();


			EntityHandle m_borderEntity;
			EntityHandle m_barEntity;
			EntityHandle m_textEntity;

			static Nz::Color s_borderColor;
			static Nz::Color s_barBackgroundColor;
			static Nz::Color s_barBackgroundCornerColor;
			static Nz::Color s_barColor;
			static Nz::Color s_barCornerColor;
			Nz::Color m_textColor;

			Nz::SpriteRef m_borderSprite;
			Nz::SpriteRef m_barBackgroundSprite;
			Nz::SpriteRef m_barSprite;
			Nz::TextSpriteRef m_textSprite;

			static float s_borderScale;
			float m_textMargin;
			unsigned m_value;
	};
}

#include <NDK/Widgets/ProgressBarWidget.inl>

#endif // NAZARA_WIDGETS_PROGRESSBARWIDGET_HPP
