// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <limits>

namespace Nz
{
	inline BaseWidget::BaseWidget(std::shared_ptr<WidgetTheme> theme) :
	m_canvasIndex(InvalidCanvasIndex),
	m_theme(std::move(theme)),
	m_registry(nullptr),
	m_canvas(nullptr),
	m_backgroundColor(Color(0.9f, 0.9f, 0.9f, 1.f)),
	m_renderingRect(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()),
	m_cursor(SystemCursor::Default),
	m_maximumSize(std::numeric_limits<float>::infinity()),
	m_minimumSize(0.f),
	m_preferredSize(-1),
	m_size(50.f, 50.f),
	m_parentWidget(nullptr),
	m_disableVisibilitySignal(false),
	m_followPreferredSize(true),
	m_isMouseInputEnabled(true),
	m_visible(true),
	m_baseRenderLayer(0),
	m_renderLayerCount(1),
	m_renderLayoutOffset(0)
	{
	}

	template<std::derived_from<BaseWidget> T, typename... Args>
	T* BaseWidget::Add(Args&&... args)
	{
		return AddChild(std::make_unique<T>(this, std::forward<Args>(args)...));
	}

	template<std::derived_from<BaseWidget> T>
	T* BaseWidget::AddChild(std::unique_ptr<T>&& widget)
	{
		widget->SetParent(this);
		widget->SetBaseRenderLayer(m_baseRenderLayer + m_renderLayerCount);
		widget->Show(widget->IsVisible() && m_visible);

		T* widgetPtr = widget.get();
		m_widgetChilds.emplace_back(std::move(widget));
		OnChildAdded(widgetPtr);

		return widgetPtr;
	}

	inline void BaseWidget::Center()
	{
		NazaraAssertMsg(m_parentWidget, "Widget has no parent");

		Vector2f parentSize = m_parentWidget->GetSize();
		Vector2f mySize = GetSize();
		SetPosition({ (parentSize.x - mySize.x) / 2.f, (parentSize.y - mySize.y) / 2.f });
	}

	inline void BaseWidget::CenterHorizontal()
	{
		NazaraAssertMsg(m_parentWidget, "Widget has no parent");

		Vector2f parentSize = m_parentWidget->GetSize();
		Vector2f mySize = GetSize();
		SetPosition({ (parentSize.x - mySize.x) / 2.f, GetPosition().y });
	}

	inline void BaseWidget::CenterVertical()
	{
		NazaraAssertMsg(m_parentWidget, "Widget has no parent");

		Vector2f parentSize = m_parentWidget->GetSize();
		Vector2f mySize = GetSize();
		SetPosition({ GetPosition().x, (parentSize.y - mySize.y) / 2.f });
	}

	inline void BaseWidget::ClearRenderingRect()
	{
		SetRenderingRect(Rectf(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
	}

	inline void BaseWidget::EnableMouseInput(bool enable)
	{
		if (m_isMouseInputEnabled == enable)
			return;

		m_isMouseInputEnabled = enable;

		if (!enable)
			ClearMouseFocus();
	}

	template<typename F>
	void BaseWidget::ForEachWidgetChild(F&& iterator, bool onlyVisible)
	{
		for (const auto& child : m_widgetChilds)
		{
			if (onlyVisible && !child->IsVisible())
				continue;

			iterator(child.get());
		}
	}

	template<typename F>
	void BaseWidget::ForEachWidgetChild(F&& iterator, bool onlyVisible) const
	{
		for (const auto& child : m_widgetChilds)
		{
			if (onlyVisible && !child->IsVisible())
				continue;

			iterator(child.get());
		}
	}

	inline const Color& BaseWidget::GetBackgroundColor() const
	{
		return m_backgroundColor;
	}

	inline Canvas* BaseWidget::GetCanvas()
	{
		return m_canvas;
	}

	inline const Canvas* BaseWidget::GetCanvas() const
	{
		return m_canvas;
	}

	inline SystemCursor BaseWidget::GetCursor() const
	{
		return m_cursor;
	}

	inline float BaseWidget::GetHeight() const
	{
		return m_size.y;
	}

	inline float BaseWidget::GetMaximumHeight() const
	{
		return m_maximumSize.y;
	}

	inline Vector2f BaseWidget::GetMaximumSize() const
	{
		return m_maximumSize;
	}

	inline float BaseWidget::GetMaximumWidth() const
	{
		return m_maximumSize.x;
	}

	inline float BaseWidget::GetMinimumHeight() const
	{
		return m_minimumSize.y;
	}

	inline Vector2f BaseWidget::GetMinimumSize() const
	{
		return m_minimumSize;
	}

	inline float BaseWidget::GetMinimumWidth() const
	{
		return m_minimumSize.x;
	}

	inline float BaseWidget::GetPreferredHeight() const
	{
		return m_preferredSize.y;
	}

	inline Vector2f BaseWidget::GetPreferredSize() const
	{
		return m_preferredSize;
	}

	inline float BaseWidget::GetPreferredWidth() const
	{
		return m_preferredSize.x;
	}

	inline const Rectf& BaseWidget::GetRenderingRect() const
	{
		return m_renderingRect;
	}

	inline Vector2f BaseWidget::GetSize() const
	{
		return Vector2f(GetWidth(), GetHeight());
	}

	inline const std::shared_ptr<WidgetTheme>& BaseWidget::GetTheme() const
	{
		return m_theme;
	}

	inline std::size_t BaseWidget::GetVisibleWidgetChildCount() const
	{
		std::size_t visibleChild = 0;
		for (const auto& child : m_widgetChilds)
			visibleChild += (child->IsVisible()) ? 1 : 0;

		return visibleChild;
	}

	inline float BaseWidget::GetWidth() const
	{
		return m_size.x;
	}

	inline std::size_t BaseWidget::GetWidgetChildCount() const
	{
		return m_widgetChilds.size();
	}

	inline void BaseWidget::Hide()
	{
		return Show(false);
	}

	inline bool BaseWidget::IsInside(float x, float y) const
	{
		Rectf rect(0.f, 0.f, m_size.x, m_size.y);
		return rect.Contains(x, y);
	}

	inline bool BaseWidget::IsMouseInputEnabled() const
	{
		if (m_parentWidget && !m_parentWidget->IsMouseInputEnabled())
			return false;

		return m_isMouseInputEnabled;
	}

	inline bool BaseWidget::IsVisible() const
	{
		return m_visible;
	}

	inline void BaseWidget::ResizeToPreferredSize()
	{
		m_followPreferredSize = true;
		if (m_preferredSize.x > 0.f && m_preferredSize.y > 0.f)
			InternalResize(m_preferredSize);
	}

	inline void BaseWidget::SetFixedHeight(float fixedHeight)
	{
		SetMaximumHeight(fixedHeight);
		SetMinimumHeight(fixedHeight);
	}

	inline void BaseWidget::SetFixedSize(const Vector2f& fixedSize)
	{
		SetMaximumSize(fixedSize);
		SetMinimumSize(fixedSize);
	}

	inline void BaseWidget::SetFixedWidth(float fixedWidth)
	{
		SetMaximumWidth(fixedWidth);
		SetMinimumWidth(fixedWidth);
	}

	inline void BaseWidget::SetMaximumHeight(float maximumHeight)
	{
		Vector2f maximumSize = GetMaximumSize();
		maximumSize.y = maximumHeight;

		SetMaximumSize(maximumSize);
	}

	inline void BaseWidget::SetMaximumSize(const Vector2f& maximumSize)
	{
		m_maximumSize.x = std::max(m_minimumSize.x, maximumSize.x);
		m_maximumSize.y = std::max(m_minimumSize.y, maximumSize.y);

		Vector2f size = GetSize();
		if (size.x > m_maximumSize.x || size.y > m_maximumSize.y)
			InternalResize(size); //< Will clamp automatically
	}

	inline void BaseWidget::SetMaximumWidth(float maximumWidth)
	{
		Vector2f maximumSize = GetMaximumSize();
		maximumSize.x = maximumWidth;

		SetMaximumSize(maximumSize);
	}

	inline void BaseWidget::SetMinimumHeight(float minimumHeight)
	{
		Vector2f minimumSize = GetMinimumSize();
		minimumSize.y = minimumHeight;

		SetMinimumSize(minimumSize);
	}

	inline void BaseWidget::SetMinimumSize(const Vector2f& minimumSize)
	{
		m_minimumSize.x = std::min(minimumSize.x, m_maximumSize.x);
		m_minimumSize.y = std::min(minimumSize.y, m_maximumSize.y);

		Vector2f size = GetSize();
		if (size.x < m_minimumSize.x || size.y < m_minimumSize.y)
			InternalResize(size); //< Will clamp automatically
	}

	inline void BaseWidget::SetMinimumWidth(float minimumWidth)
	{
		Vector2f minimumSize = GetMinimumSize();
		minimumSize.x = minimumWidth;

		SetMinimumSize(minimumSize);
	}

	inline void BaseWidget::SetRenderLayerOffset(int renderLayerOffset)
	{
		if (m_renderLayoutOffset == renderLayerOffset)
			return;

		m_renderLayoutOffset = renderLayerOffset;
		UpdateRenderLayers();
	}

	inline int BaseWidget::GetBaseRenderLayer() const
	{
		return m_baseRenderLayer + ((m_backgroundEntity.has_value()) ? 1 : 0) + m_renderLayoutOffset;
	}

	inline entt::registry& BaseWidget::GetRegistry()
	{
		assert(m_registry);
		return *m_registry;
	}

	inline const entt::registry& BaseWidget::GetRegistry() const
	{
		assert(m_registry);
		return *m_registry;
	}

	inline void BaseWidget::SetBaseRenderLayer(int baseRenderLayer)
	{
		if (m_baseRenderLayer != baseRenderLayer)
		{
			m_baseRenderLayer = baseRenderLayer;
			UpdateRenderLayers();
		}
	}

	inline void BaseWidget::SetPreferredSize(const Vector2f& preferredSize)
	{
		if (m_preferredSize != preferredSize)
		{
			m_preferredSize = preferredSize;

			if (m_parentWidget)
				m_parentWidget->OnChildPreferredSizeUpdated(this);

			if (m_followPreferredSize && m_preferredSize.x > 0.f && m_preferredSize.y > 0.f)
				InternalResize(m_preferredSize);
		}
	}

	inline void BaseWidget::SetRenderLayerCount(int renderLayerCount)
	{
		if (m_renderLayerCount != renderLayerCount)
		{
			m_renderLayerCount = renderLayerCount;
			for (const auto& widgetPtr : m_widgetChilds)
				widgetPtr->SetBaseRenderLayer(m_baseRenderLayer + m_renderLayerCount);
		}
	}

	inline bool BaseWidget::IsRegisteredToCanvas() const
	{
		return m_canvas && m_canvasIndex != InvalidCanvasIndex;
	}

	inline void BaseWidget::NotifyParentResized(const Vector2f& newSize)
	{
		for (const auto& widgetPtr : m_widgetChilds)
			widgetPtr->OnParentResized(newSize);
	}

	inline void BaseWidget::UpdateCanvasIndex(std::size_t index)
	{
		m_canvasIndex = index;
	}
}

