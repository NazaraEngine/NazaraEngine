// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/SimpleWidgetStyles.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Widgets/AbstractLabelWidget.hpp>
#include <Nazara/Widgets/AbstractTextAreaWidget.hpp>
#include <Nazara/Widgets/ButtonWidget.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/CheckboxWidget.hpp>
#include <Nazara/Widgets/ImageButtonWidget.hpp>
#include <Nazara/Widgets/ProgressBarWidget.hpp>
#include <Nazara/Widgets/ScrollAreaWidget.hpp>
#include <Nazara/Widgets/ScrollbarButtonWidget.hpp>
#include <Nazara/Widgets/ScrollbarWidget.hpp>
#include <Nazara/Widgets/Widgets.hpp>

namespace Nz
{
	SimpleButtonWidgetStyle::SimpleButtonWidgetStyle(ButtonWidget* buttonWidget, StyleConfig config) :
	ButtonWidgetStyle(buttonWidget, 2),
	m_styleConfig(std::move(config)),
	m_isDisabled(false),
	m_isHovered(false),
	m_isPressed(false)
	{
		assert(m_styleConfig.material);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner corner;
		corner.size = Vector2f(config.cornerSize);
		corner.textureCoords = Vector2f(config.cornerTexCoords);

		m_sprite = std::make_shared<SlicedSprite>(m_styleConfig.material);
		m_sprite->SetCorners(corner, corner);
		m_sprite->SetTextureCoords(m_styleConfig.coords);

		m_textSprite = std::make_shared<TextSprite>(Widgets::Instance()->GetTransparentMaterial());

		m_spriteEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_spriteEntity).AttachRenderable(m_sprite, renderMask);

		m_textEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_textEntity).AttachRenderable(m_textSprite, renderMask);
	}

	void SimpleButtonWidgetStyle::Layout(const Vector2f& size)
	{
		m_sprite->SetSize(size);

		entt::registry& registry = GetRegistry();

		Boxf textBox = m_textSprite->GetAABB();
		registry.get<NodeComponent>(m_textEntity).SetPosition({ size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f });
	}

	void SimpleButtonWidgetStyle::OnDisabled()
	{
		UpdateMaterial(m_isHovered, m_isPressed, true);
		m_isDisabled = true;
	}

	void SimpleButtonWidgetStyle::OnEnabled()
	{
		UpdateMaterial(m_isHovered, m_isPressed, false);
		m_isDisabled = false;
	}

	void SimpleButtonWidgetStyle::OnHoverBegin()
	{
		UpdateMaterial(true, m_isPressed, m_isDisabled);
		m_isHovered = true;
	}

	void SimpleButtonWidgetStyle::OnHoverEnd()
	{
		UpdateMaterial(false, m_isPressed, m_isDisabled);
		m_isHovered = false;
	}

	void SimpleButtonWidgetStyle::OnPress()
	{
		UpdateMaterial(m_isHovered, true, m_isDisabled);
		m_isPressed = true;
	}

	void SimpleButtonWidgetStyle::OnRelease()
	{
		UpdateMaterial(m_isHovered, false, m_isDisabled);
		m_isPressed = false;
	}

	void SimpleButtonWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_sprite->UpdateRenderLayer(baseRenderLayer);
		m_textSprite->UpdateRenderLayer(baseRenderLayer + 1);
	}

	void SimpleButtonWidgetStyle::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);
	}

	void SimpleButtonWidgetStyle::UpdateMaterial(bool hovered, bool pressed, bool disabled)
	{
		if (disabled)
		{
			if (m_styleConfig.disabledCoords)
				m_sprite->SetTextureCoords(*m_styleConfig.disabledCoords);
			else
				m_sprite->SetColor(Color(0.5f));
		}
		else
		{
			m_sprite->SetColor(Color::White());

			if (pressed && hovered && m_styleConfig.pressedHoveredCoords)
				m_sprite->SetTextureCoords(*m_styleConfig.pressedHoveredCoords);
			else if (pressed && m_styleConfig.pressedCoords)
				m_sprite->SetTextureCoords(*m_styleConfig.pressedCoords);
			else if (hovered && m_styleConfig.hoveredCoords)
				m_sprite->SetTextureCoords(*m_styleConfig.hoveredCoords);
			else
				m_sprite->SetTextureCoords(m_styleConfig.coords);
		}
	}

	/************************************************************************/

	SimpleCheckboxWidgetStyle::SimpleCheckboxWidgetStyle(CheckboxWidget* buttonWidget, StyleConfig config) :
	CheckboxWidgetStyle(buttonWidget, 2),
	m_styleConfig(std::move(config)),
	m_isHovered(false)
	{
		assert(m_styleConfig.material);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner corner;
		corner.size = Vector2f(config.backgroundCornerSize);
		corner.textureCoords = Vector2f(config.backgroundCornerTexCoords);

		m_backgroundSprite = std::make_shared<SlicedSprite>(m_styleConfig.material);
		m_backgroundSprite->SetTextureCoords(m_styleConfig.backgroundCoords);
		m_backgroundSprite->SetCorners(corner, corner);

		m_checkSprite = std::make_shared<Sprite>(m_styleConfig.material);
		m_checkSprite->SetTextureCoords(m_styleConfig.checkCoords);

		m_backgroundEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_backgroundEntity).AttachRenderable(m_backgroundSprite, renderMask);

		m_checkEntity = CreateGraphicsEntity();
		{
			auto& gfxComponent = registry.get<GraphicsComponent>(m_checkEntity);
			gfxComponent.AttachRenderable(m_checkSprite, renderMask);
			gfxComponent.Hide();
		}
	}

	void SimpleCheckboxWidgetStyle::Layout(const Vector2f& size)
	{
		m_backgroundSprite->SetSize(size);

		Vector2f checkSize = size * 0.66f;
		m_checkSprite->SetSize(checkSize);

		GetRegistry().get<NodeComponent>(m_checkEntity).SetPosition({ size.x / 2.f - checkSize.x / 2.f, size.y / 2.f - checkSize.y / 2.f });
	}

	void SimpleCheckboxWidgetStyle::OnHoverBegin()
	{
		m_isHovered = true;
		UpdateMaterial(m_isHovered);
	}

	void SimpleCheckboxWidgetStyle::OnHoverEnd()
	{
		m_isHovered = false;
		UpdateMaterial(m_isHovered);
	}

	void SimpleCheckboxWidgetStyle::OnNewState(CheckboxState newState)
	{
		switch (newState)
		{
			case CheckboxState::Unchecked:
			{
				GetRegistry().get<GraphicsComponent>(m_checkEntity).Hide();
				break;
			}

			case CheckboxState::Tristate:
			case CheckboxState::Checked:
			{
				m_checkSprite->SetTextureCoords((newState == CheckboxState::Checked) ? m_styleConfig.checkCoords : m_styleConfig.tristateCoords);
				GetRegistry().get<GraphicsComponent>(m_checkEntity).Show();
				break;
			}
		}
	}

	void SimpleCheckboxWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_backgroundSprite->UpdateRenderLayer(baseRenderLayer);
		m_checkSprite->UpdateRenderLayer(baseRenderLayer + 1);
	}

	void SimpleCheckboxWidgetStyle::UpdateMaterial(bool hovered)
	{
		if (hovered && m_styleConfig.backgroundHoveredCoords)
			m_backgroundSprite->SetTextureCoords(*m_styleConfig.backgroundHoveredCoords);
		else
			m_backgroundSprite->SetTextureCoords(m_styleConfig.backgroundCoords);
	}

	/************************************************************************/

	SimpleImageButtonWidgetStyle::SimpleImageButtonWidgetStyle(ImageButtonWidget* imageButtonWidget, StyleConfig config) :
	ImageButtonWidgetStyle(imageButtonWidget, 1),
	m_isHovered(false),
	m_isPressed(false)
	{
		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner hoveredCorner;
		hoveredCorner.size = Vector2f(config.hoveredCornerSize, config.hoveredCornerSize);
		hoveredCorner.textureCoords = Vector2f(config.hoveredCornerTexCoords, config.hoveredCornerTexCoords);

		if (config.hoveredCoords)
		{
			m_hoveredSprite = std::make_shared<SlicedSprite>(config.material);
			m_hoveredSprite->SetCorners(hoveredCorner, hoveredCorner);
			m_hoveredSprite->SetTextureCoords(*config.hoveredCoords);
		}

		float imageCornerSize = imageButtonWidget->GetCornerSize();
		float imageCornerTexCoords = imageButtonWidget->GetCornerTexCoords();

		SlicedSprite::Corner corner;
		corner.size = Vector2f(imageCornerSize, imageCornerSize);
		corner.textureCoords = Vector2f(imageCornerTexCoords, imageCornerTexCoords);

		m_sprite = std::make_shared<SlicedSprite>(imageButtonWidget->GetMaterial());
		m_sprite->SetCorners(corner, corner);
		m_sprite->SetTextureCoords(imageButtonWidget->GetTextureCoords());

		m_entity = CreateGraphicsEntity();

		GraphicsComponent& gfxComponent = registry.get<GraphicsComponent>(m_entity);
		gfxComponent.AttachRenderable(m_sprite, renderMask);
	}

	void SimpleImageButtonWidgetStyle::Layout(const Vector2f& size)
	{
		m_sprite->SetSize(size);
		if (m_hoveredSprite)
			m_hoveredSprite->SetSize(size);
	}

	void SimpleImageButtonWidgetStyle::OnHoverBegin()
	{
		Update(true, m_isPressed);
		m_isHovered = true;
	}

	void SimpleImageButtonWidgetStyle::OnHoverEnd()
	{
		Update(false, m_isPressed);
		m_isHovered = false;
	}

	void SimpleImageButtonWidgetStyle::OnPress()
	{
		Update(m_isHovered, true);
		m_isPressed = true;
	}

	void SimpleImageButtonWidgetStyle::OnRelease()
	{
		Update(m_isHovered, false);
		m_isPressed = false;
	}

	void SimpleImageButtonWidgetStyle::OnUpdate()
	{
		ImageButtonWidget* owner = GetOwnerWidget<ImageButtonWidget>();

		// If a hovering material was added while we're being hovered, we need to detach the hovering sprite
		if (owner->GetHoveredMaterial() && m_hoveredSprite)
		{
			GraphicsComponent& gfxComponent = GetRegistry().get<GraphicsComponent>(m_entity);
			gfxComponent.DetachRenderable(m_hoveredSprite);
		}

		m_sprite->SetTextureCoords(owner->GetTextureCoords());

		Update(m_isHovered, m_isPressed);
	}

	void SimpleImageButtonWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_sprite->UpdateRenderLayer(baseRenderLayer);
		if (m_hoveredSprite)
			m_hoveredSprite->UpdateRenderLayer(baseRenderLayer + 1);
	}

	void SimpleImageButtonWidgetStyle::Update(bool hovered, bool pressed)
	{
		ImageButtonWidget* owner = GetOwnerWidget<ImageButtonWidget>();

		if (pressed)
		{
			if (const auto& pressedMaterial = owner->GetPressedMaterial())
			{
				m_sprite->SetColor(owner->GetColor());
				m_sprite->SetMaterial(pressedMaterial);
			}
			else
			{
				m_sprite->SetColor(owner->GetColor() * Color::FromRGB8(120, 120, 120));
				m_sprite->SetMaterial(owner->GetMaterial());
			}

			m_sprite->SetTextureCoords(owner->GetPressedTextureCoords());
		}
		else
		{
			m_sprite->SetColor(owner->GetColor());
			m_sprite->SetMaterial(owner->GetMaterial());
			m_sprite->SetTextureCoords(owner->GetTextureCoords());
		}

		if (hovered)
		{
			if (const auto& hoveredMaterial = owner->GetHoveredMaterial())
			{
				if (!pressed)
				{
					m_sprite->SetMaterial(hoveredMaterial);
					m_sprite->SetTextureCoords(owner->GetHoveredTextureCoords());
				}
			}
			else
			{
				if (!pressed)
				{
					m_sprite->SetMaterial(owner->GetMaterial());
					m_sprite->SetTextureCoords(owner->GetTextureCoords());
				}

				if (!m_isHovered && m_hoveredSprite)
				{
					GraphicsComponent& gfxComponent = GetRegistry().get<GraphicsComponent>(m_entity);
					gfxComponent.AttachRenderable(m_hoveredSprite, GetRenderMask());
				}
			}
		}
		else
		{
			if (!pressed)
			{
				m_sprite->SetMaterial(owner->GetMaterial());
				m_sprite->SetTextureCoords(owner->GetTextureCoords());
			}

			if (m_isHovered && m_hoveredSprite)
			{
				GraphicsComponent& gfxComponent = GetRegistry().get<GraphicsComponent>(m_entity);
				gfxComponent.DetachRenderable(m_hoveredSprite);
			}
		}
	}

	/************************************************************************/

	SimpleLabelWidgetStyle::SimpleLabelWidgetStyle(AbstractLabelWidget* labelWidget, StyleConfig styleConfig) :
	LabelWidgetStyle(labelWidget, 1),
	m_styleConfig(std::move(styleConfig))
	{
		assert(m_styleConfig.material);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		m_textSprite = std::make_shared<TextSprite>(m_styleConfig.material);

		m_entity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_entity).AttachRenderable(m_textSprite, renderMask);
	}

	void SimpleLabelWidgetStyle::Layout(const Vector2f& size)
	{
		entt::registry& registry = GetRegistry();

		Boxf textBox = m_textSprite->GetAABB();
		registry.get<NodeComponent>(m_entity).SetPosition({ size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f });
	}

	void SimpleLabelWidgetStyle::OnHoverBegin()
	{
		UpdateMaterial(true);
	}

	void SimpleLabelWidgetStyle::OnHoverEnd()
	{
		UpdateMaterial(false);
	}

	void SimpleLabelWidgetStyle::UpdateMaterial(bool hovered)
	{
		if (hovered && m_styleConfig.hoveredMaterial)
			m_textSprite->SetMaterial(m_styleConfig.hoveredMaterial);
		else
			m_textSprite->SetMaterial(m_styleConfig.material);
	}

	void SimpleLabelWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_textSprite->UpdateRenderLayer(baseRenderLayer);
	}

	void SimpleLabelWidgetStyle::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);
	}

	/************************************************************************/

	SimpleProgressBarWidgetStyle::SimpleProgressBarWidgetStyle(ProgressBarWidget* progressBarWidget, StyleConfig config) :
	ProgressBarWidgetStyle(progressBarWidget, 2),
	m_styleConfig(std::move(config))
	{
		assert(m_styleConfig.material);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner backgroundCorner;
		backgroundCorner.size = Vector2f(config.backgroundCornerSize, config.backgroundCornerSize);
		backgroundCorner.textureCoords = Vector2f(config.backgroundCornerTexCoords, config.backgroundCornerTexCoords);

		m_backgroundSprite = std::make_shared<SlicedSprite>(m_styleConfig.material);
		m_backgroundSprite->SetCorners(backgroundCorner, backgroundCorner);
		m_backgroundSprite->SetTextureCoords(m_styleConfig.backgroundCoords);

		m_backgroundEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_backgroundEntity).AttachRenderable(m_backgroundSprite, renderMask);

		m_progressBarSprite = std::make_shared<Sprite>(Widgets::Instance()->GetTransparentMaterial());
		m_progressBarSprite->SetCornerColor(RectCorner::LeftBottom, m_styleConfig.progressBarBeginColor);
		m_progressBarSprite->SetCornerColor(RectCorner::LeftTop, m_styleConfig.progressBarBeginColor);
		m_progressBarSprite->SetCornerColor(RectCorner::RightBottom, m_styleConfig.progressBarEndColor);
		m_progressBarSprite->SetCornerColor(RectCorner::RightTop, m_styleConfig.progressBarEndColor);

		m_barEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_barEntity).AttachRenderable(m_progressBarSprite, renderMask);
		registry.get<NodeComponent>(m_barEntity).SetPosition({ m_styleConfig.barOffset, m_styleConfig.barOffset });
	}

	void SimpleProgressBarWidgetStyle::Layout(const Vector2f& size)
	{
		float fraction = GetOwnerWidget<ProgressBarWidget>()->GetFraction();
		float width = std::max(fraction * (size.x - m_styleConfig.barOffset * 2.f), 0.f);

		Color endColor = Lerp(m_styleConfig.progressBarBeginColor, m_styleConfig.progressBarEndColor, fraction);
		m_progressBarSprite->SetCornerColor(RectCorner::RightBottom, endColor);
		m_progressBarSprite->SetCornerColor(RectCorner::RightTop, endColor);

		m_backgroundSprite->SetSize(size);
		m_progressBarSprite->SetSize({ width, size.y - m_styleConfig.barOffset * 2.f });
	}

	void SimpleProgressBarWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_backgroundSprite->UpdateRenderLayer(baseRenderLayer);
		m_progressBarSprite->UpdateRenderLayer(baseRenderLayer + 1);
	}

	/************************************************************************/

	SimpleScrollAreaWidgetStyle::SimpleScrollAreaWidgetStyle(ScrollAreaWidget* scrollAreaWidget) :
	ScrollAreaWidgetStyle(scrollAreaWidget, 0)
	{
	}

	void SimpleScrollAreaWidgetStyle::Layout(const Vector2f& /*size*/)
	{
	}

	void SimpleScrollAreaWidgetStyle::UpdateRenderLayer(int /*baseRenderLayer*/)
	{
	}


	SimpleScrollbarWidgetStyle::SimpleScrollbarWidgetStyle(ScrollbarWidget* scrollBarWidget, StyleConfig config) :
	ScrollbarWidgetStyle(scrollBarWidget, 1),
	m_config(std::move(config))
	{
		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		m_backgroundScrollbarSprite = std::make_shared<Sprite>(m_config.material);
		m_backgroundScrollbarSprite->SetTextureCoords((scrollBarWidget->GetOrientation() == ScrollbarOrientation::Horizontal) ? m_config.backgroundHorizontalCoords : m_config.backgroundVerticalCoords);

		m_backgroundScrollbarSpriteEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_backgroundScrollbarSpriteEntity).AttachRenderable(m_backgroundScrollbarSprite, renderMask);

		m_scrollbarSpriteEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_scrollbarSpriteEntity).AttachRenderable(m_scrollbarSprite, renderMask);
	}

	std::unique_ptr<ImageButtonWidget> SimpleScrollbarWidgetStyle::CreateBackButton(ScrollbarWidget* widget, ScrollbarOrientation orientation)
	{
		if (orientation == ScrollbarOrientation::Horizontal)
			return std::make_unique<ImageButtonWidget>(widget, m_config.material, m_config.buttonLeftCoords, m_config.buttonLeftHoveredCoords, m_config.buttonLeftPressedCoords, m_config.buttonCornerSize, m_config.buttonCornerTexcoords);
		else
			return std::make_unique<ImageButtonWidget>(widget, m_config.material, m_config.buttonUpCoords, m_config.buttonUpHoveredCoords, m_config.buttonUpPressedCoords, m_config.buttonCornerSize, m_config.buttonCornerTexcoords);
	}

	std::unique_ptr<ImageButtonWidget> SimpleScrollbarWidgetStyle::CreateForwardButton(ScrollbarWidget* widget, ScrollbarOrientation orientation)
	{
		if (orientation == ScrollbarOrientation::Horizontal)
			return std::make_unique<ImageButtonWidget>(widget, m_config.material, m_config.buttonRightCoords, m_config.buttonRightHoveredCoords, m_config.buttonRightPressedCoords, m_config.buttonCornerSize, m_config.buttonCornerTexcoords);
		else
			return std::make_unique<ImageButtonWidget>(widget, m_config.material, m_config.buttonDownCoords, m_config.buttonDownHoveredCoords, m_config.buttonDownPressedCoords, m_config.buttonCornerSize, m_config.buttonCornerTexcoords);
	}

	void SimpleScrollbarWidgetStyle::Layout(const Vector2f& size)
	{
		m_backgroundScrollbarSprite->SetSize(size);
	}

	void SimpleScrollbarWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_backgroundScrollbarSprite->UpdateRenderLayer(baseRenderLayer);
	}

	/************************************************************************/

	SimpleScrollbarButtonWidgetStyle::SimpleScrollbarButtonWidgetStyle(ScrollbarButtonWidget* scrollbarButtonWidget, StyleConfig config) :
	ScrollbarButtonWidgetStyle(scrollbarButtonWidget, 1),
	m_config(std::move(config)),
	m_isHovered(false),
	m_isPressed(false)
	{
		assert(m_config.material);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner corner;
		corner.size = Vector2f(config.cornerSize);
		corner.textureCoords = Vector2f(config.cornerTexCoords);

		m_sprite = std::make_shared<SlicedSprite>(m_config.material);
		m_sprite->SetCorners(corner, corner);
		m_sprite->SetTextureCoords(m_config.coords);

		m_entity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_entity).AttachRenderable(m_sprite, renderMask);
	}

	void SimpleScrollbarButtonWidgetStyle::Layout(const Vector2f& size)
	{
		m_sprite->SetSize(size);
	}

	void SimpleScrollbarButtonWidgetStyle::OnHoverBegin()
	{
		Update(true, m_isPressed);
		m_isHovered = true;
	}

	void SimpleScrollbarButtonWidgetStyle::OnHoverEnd()
	{
		Update(false, m_isPressed);
		m_isHovered = false;
	}

	void SimpleScrollbarButtonWidgetStyle::OnGrab()
	{
		Update(m_isHovered, true);
		m_isPressed = true;
	}

	void SimpleScrollbarButtonWidgetStyle::OnRelease()
	{
		Update(m_isHovered, false);
		m_isPressed = false;
	}

	void SimpleScrollbarButtonWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_sprite->UpdateRenderLayer(baseRenderLayer);
	}

	void SimpleScrollbarButtonWidgetStyle::Update(bool hovered, bool pressed)
	{
		if (pressed && hovered && m_config.grabbedHoveredCoords)
			m_sprite->SetTextureCoords(*m_config.grabbedHoveredCoords);
		else if (pressed && m_config.grabbedCoords)
			m_sprite->SetTextureCoords(*m_config.grabbedCoords);
		else if (hovered && m_config.hoveredCoords)
			m_sprite->SetTextureCoords(*m_config.hoveredCoords);
		else
			m_sprite->SetTextureCoords(m_config.coords);
	}


	SimpleTextAreaWidgetStyle::SimpleTextAreaWidgetStyle(AbstractTextAreaWidget* textAreaWidget, StyleConfig config) :
	TextAreaWidgetStyle(textAreaWidget, 2),
	m_backgroundEntity(entt::null),
	m_backgroundColor(Color::White()),
	m_config(std::move(config)),
	m_hasFocus(false),
	m_isDisabled(false)
	{
		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		m_textEntity = CreateGraphicsEntity();
		m_textSprite = std::make_shared<TextSprite>(Widgets::Instance()->GetTransparentMaterial());

		auto& textGfx = registry.get<GraphicsComponent>(m_textEntity);
		textGfx.AttachRenderable(m_textSprite, renderMask);
	}

	void SimpleTextAreaWidgetStyle::EnableBackground(bool enable)
	{
		if ((m_backgroundEntity != entt::null) == enable)
			return;

		if (enable)
		{
			auto& registry = GetRegistry();
			UInt32 renderMask = GetRenderMask();

			if (!m_backgroundSprite)
			{
				AbstractTextAreaWidget* textAreaWidget = GetOwnerWidget<AbstractTextAreaWidget>();

				SlicedSprite::Corner backgroundCorner;
				backgroundCorner.size = Vector2f(m_config.backgroundCornerSize);
				backgroundCorner.textureCoords = Vector2f(m_config.backgroundCornerTexCoords);

				m_backgroundSprite = std::make_shared<SlicedSprite>(m_config.material);
				m_backgroundSprite->SetColor(m_backgroundColor);
				m_backgroundSprite->SetCorners(backgroundCorner, backgroundCorner);
				m_backgroundSprite->SetSize(textAreaWidget->GetSize());
				m_backgroundSprite->SetTextureCoords((m_config.backgroundDisabledCoords) ? *m_config.backgroundDisabledCoords : m_config.backgroundCoords);
				m_backgroundSprite->UpdateRenderLayer(m_baseRenderLayer);
				if (!textAreaWidget->IsEnabled() && !m_config.backgroundDisabledCoords)
					m_backgroundSprite->SetColor(Color(0.5f));
			}

			m_backgroundEntity = CreateGraphicsEntity();

			auto& backgroundGfx = registry.get<GraphicsComponent>(m_backgroundEntity);
			backgroundGfx.AttachRenderable(m_backgroundSprite, renderMask);
		}
		else
		{
			m_backgroundSprite.reset();

			DestroyEntity(m_backgroundEntity);
			m_backgroundEntity = entt::null;
		}
	}

	void SimpleTextAreaWidgetStyle::Layout(const Vector2f& size)
	{
		if (m_backgroundSprite)
			m_backgroundSprite->SetSize(size);
	}

	void SimpleTextAreaWidgetStyle::OnDisabled()
	{
		if (m_backgroundSprite)
		{
			if (m_config.backgroundDisabledCoords)
				m_backgroundSprite->SetTextureCoords(*m_config.backgroundDisabledCoords);
			else
				m_backgroundSprite->SetColor(Color(0.5f));
		}
	}

	void SimpleTextAreaWidgetStyle::OnEnabled()
	{
		if (m_backgroundSprite)
		{
			m_backgroundSprite->SetColor(Color::White());
			m_backgroundSprite->SetTextureCoords(m_config.backgroundCoords);
		}
	}

	void SimpleTextAreaWidgetStyle::OnFocusLost()
	{
		UpdateCursorColor(false);
		m_hasFocus = false;
	}

	void SimpleTextAreaWidgetStyle::OnFocusReceived()
	{
		UpdateCursorColor(true);
		m_hasFocus = true;
	}

	void SimpleTextAreaWidgetStyle::UpdateBackgroundColor(const Color& color)
	{
		m_backgroundColor = color;

		if (m_backgroundSprite)
			m_backgroundSprite->SetColor(color);
	}

	void SimpleTextAreaWidgetStyle::UpdateCursors(std::span<const Rectf> cursorRects)
	{
		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		// Create/destroy cursor entities and sprites
		auto& textNode = registry.get<NodeComponent>(m_textEntity);

		std::size_t selectionLineCount = cursorRects.size();
		std::size_t oldSpriteCount = m_cursors.size();
		if (m_cursors.size() < selectionLineCount)
		{
			m_cursors.resize(selectionLineCount);
			for (std::size_t i = oldSpriteCount; i < m_cursors.size(); ++i)
			{
				m_cursors[i].sprite = std::make_shared<Sprite>(Widgets::Instance()->GetTransparentMaterial());
				m_cursors[i].sprite->UpdateRenderLayer(m_baseRenderLayer + 1);

				m_cursors[i].entity = CreateGraphicsEntity(&textNode);

				auto& cursorGfx = registry.get<GraphicsComponent>(m_cursors[i].entity);
				cursorGfx.AttachRenderable(m_cursors[i].sprite, renderMask);
			}
		}
		else if (m_cursors.size() > selectionLineCount)
		{
			for (std::size_t i = selectionLineCount; i < m_cursors.size(); ++i)
				DestroyEntity(m_cursors[i].entity);

			m_cursors.resize(selectionLineCount);
		}

		// Resize every cursor sprite
		for (std::size_t i = 0; i < cursorRects.size(); ++i)
		{
			const Rectf& cursorRect = cursorRects[i];

			auto& cursor = m_cursors[i];
			cursor.sprite->SetSize(Vector2f(cursorRect.width, cursorRect.height));
			registry.get<NodeComponent>(cursor.entity).SetPosition({ cursorRect.x, cursorRect.y });
		}

		UpdateCursorColor(m_hasFocus);
	}

	void SimpleTextAreaWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_baseRenderLayer = baseRenderLayer;
		m_textSprite->UpdateRenderLayer(baseRenderLayer + 1);

		if (m_backgroundSprite)
			m_backgroundSprite->UpdateRenderLayer(baseRenderLayer);

		for (Cursor& cursor : m_cursors)
			cursor.sprite->UpdateRenderLayer(baseRenderLayer + 2);
	}

	void SimpleTextAreaWidgetStyle::UpdateText(const AbstractTextDrawer& drawer)
	{
		AbstractTextAreaWidget* textAreaWidget = GetOwnerWidget<AbstractTextAreaWidget>();
		Vector2f textPadding = textAreaWidget->GetTextPadding();

		m_textSprite->Update(drawer);

		Vector2f textSize = Vector2f(m_textSprite->GetAABB().GetLengths());

		auto& textNode = GetRegistry().get<NodeComponent>(m_textEntity);
		textNode.SetPosition({ textPadding.x, textAreaWidget->GetHeight() - textPadding.y - textSize.y });

		UpdateCursorColor(textAreaWidget->HasFocus());
	}

	void SimpleTextAreaWidgetStyle::UpdateTextOffset(float offset)
	{
		AbstractTextAreaWidget* textAreaWidget = GetOwnerWidget<AbstractTextAreaWidget>();
		Vector2f textPadding = textAreaWidget->GetTextPadding();

		auto& textNode = GetRegistry().get<NodeComponent>(m_textEntity);
		textNode.SetPosition({ textPadding.x + offset, textNode.GetPosition().y });
	}

	void SimpleTextAreaWidgetStyle::UpdateCursorColor(bool hasFocus)
	{
		AbstractTextAreaWidget* textAreaWidget = GetOwnerWidget<AbstractTextAreaWidget>();

		auto [cursorBegin, cursorEnd] = textAreaWidget->GetSelection();

		if (cursorBegin == cursorEnd)
		{
			auto& registry = GetRegistry();

			// Show or hide cursor depending on state
			if (hasFocus)
			{
				if (!textAreaWidget->IsReadOnly())
				{
					for (auto& cursor : m_cursors)
					{
						cursor.sprite->SetColor(m_config.insertionCursorColor);
						registry.get<GraphicsComponent>(cursor.entity).Show();
					}
				}
			}
			else
			{
				for (auto& cursor : m_cursors)
					registry.get<GraphicsComponent>(cursor.entity).Hide();
			}
		}
		else
		{
			const Color& color = (hasFocus) ? m_config.selectionCursorColor : m_config.selectionCursorColorNoFocus;
			for (auto& cursor : m_cursors)
				cursor.sprite->SetColor(color);
		}
	}
}
