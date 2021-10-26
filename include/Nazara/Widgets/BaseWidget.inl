// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <limits>

namespace Nz
{
	inline BaseWidget::BaseWidget() :
	m_canvasIndex(InvalidCanvasIndex),
	m_registry(nullptr),
	m_canvas(nullptr),
	m_backgroundColor(Color(230, 230, 230, 255)),
	m_renderingRect(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()),
	m_cursor(Nz::SystemCursor::Default),
	m_maximumSize(std::numeric_limits<float>::infinity()),
	m_minimumSize(0.f),
	m_preferredSize(-1),
	m_size(50.f, 50.f),
	m_widgetParent(nullptr),
	m_visible(true)
	{
	}

	template<typename T, typename... Args>
	inline T* BaseWidget::Add(Args&&... args)
	{
		std::unique_ptr<T> widget = std::make_unique<T>(this, std::forward<Args>(args)...);
		T* widgetPtr = widget.get();
		AddChild(std::move(widget));

		return widgetPtr;
	}

	inline void BaseWidget::AddChild(std::unique_ptr<BaseWidget>&& widget)
	{
		widget->Show(m_visible);
		widget->SetParent(this);
		m_children.emplace_back(std::move(widget));
	}

	inline void BaseWidget::Center()
	{
		NazaraAssert(m_widgetParent, "Widget has no parent");

		Nz::Vector2f parentSize = m_widgetParent->GetSize();
		Nz::Vector2f mySize = GetSize();
		SetPosition((parentSize.x - mySize.x) / 2.f, (parentSize.y - mySize.y) / 2.f);
	}

	inline void BaseWidget::CenterHorizontal()
	{
		NazaraAssert(m_widgetParent, "Widget has no parent");

		Nz::Vector2f parentSize = m_widgetParent->GetSize();
		Nz::Vector2f mySize = GetSize();
		SetPosition((parentSize.x - mySize.x) / 2.f, GetPosition(Nz::CoordSys::Local).y);
	}

	inline void BaseWidget::CenterVertical()
	{
		NazaraAssert(m_widgetParent, "Widget has no parent");

		Nz::Vector2f parentSize = m_widgetParent->GetSize();
		Nz::Vector2f mySize = GetSize();
		SetPosition(GetPosition(Nz::CoordSys::Local).x, (parentSize.y - mySize.y) / 2.f);
	}

	inline void BaseWidget::ClearRenderingRect()
	{
		SetRenderingRect(Nz::Rectf(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
	}

	template<typename F>
	inline void BaseWidget::ForEachWidgetChild(F iterator)
	{
		for (const auto& child : m_children)
			iterator(child.get());
	}

	template<typename F>
	inline void BaseWidget::ForEachWidgetChild(F iterator) const
	{
		for (const auto& child : m_children)
			iterator(static_cast<const BaseWidget*>(child.get()));
	}

	inline const Color& BaseWidget::GetBackgroundColor() const
	{
		return m_backgroundColor;
	}

	inline Canvas* BaseWidget::GetCanvas()
	{
		return m_canvas;
	}

	inline Nz::SystemCursor BaseWidget::GetCursor() const
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

	inline Nz::Vector2f BaseWidget::GetMaximumSize() const
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

	inline Nz::Vector2f BaseWidget::GetMinimumSize() const
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

	inline Nz::Vector2f BaseWidget::GetPreferredSize() const
	{
		return m_preferredSize;
	}

	inline float BaseWidget::GetPreferredWidth() const
	{
		return m_preferredSize.x;
	}

	inline const Nz::Rectf& BaseWidget::GetRenderingRect() const
	{
		return m_renderingRect;
	}

	inline Nz::Vector2f BaseWidget::GetSize() const
	{
		return Nz::Vector2f(GetWidth(), GetHeight());
	}

	inline float BaseWidget::GetWidth() const
	{
		return m_size.x;
	}

	inline std::size_t BaseWidget::GetWidgetChildCount() const
	{
		return m_children.size();
	}

	inline void BaseWidget::Hide()
	{
		return Show(false);
	}

	inline bool BaseWidget::IsVisible() const
	{
		return m_visible;
	}

	inline void BaseWidget::SetFixedHeight(float fixedHeight)
	{
		SetMaximumHeight(fixedHeight);
		SetMinimumHeight(fixedHeight);
	}

	inline void BaseWidget::SetFixedSize(const Nz::Vector2f& fixedSize)
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
		Nz::Vector2f maximumSize = GetMaximumSize();
		maximumSize.y = maximumHeight;

		SetMaximumSize(maximumSize);
	}

	inline void BaseWidget::SetMaximumSize(const Nz::Vector2f& maximumSize)
	{
		m_maximumSize = maximumSize;

		Nz::Vector2f size = GetSize();
		if (size.x > m_maximumSize.x || size.y > m_maximumSize.y)
			Resize(size); //< Will clamp automatically
	}

	inline void BaseWidget::SetMaximumWidth(float maximumWidth)
	{
		Nz::Vector2f maximumSize = GetMaximumSize();
		maximumSize.x = maximumWidth;

		SetMaximumSize(maximumSize);
	}

	inline void BaseWidget::SetMinimumHeight(float minimumHeight)
	{
		Nz::Vector2f minimumSize = GetMinimumSize();
		minimumSize.y = minimumHeight;

		SetMinimumSize(minimumSize);
	}

	inline void BaseWidget::SetMinimumSize(const Nz::Vector2f& minimumSize)
	{
		m_minimumSize = minimumSize;

		Nz::Vector2f size = GetSize();
		if (size.x < m_minimumSize.x || size.y < m_minimumSize.y)
			Resize(size); //< Will clamp automatically
	}

	inline void BaseWidget::SetMinimumWidth(float minimumWidth)
	{
		Nz::Vector2f minimumSize = GetMinimumSize();
		minimumSize.x = minimumWidth;

		SetMinimumSize(minimumSize);
	}

	inline void BaseWidget::SetPreferredSize(const Nz::Vector2f& preferredSize)
	{
		m_preferredSize = preferredSize;

		//Resize(m_preferredSize);
	}

	inline bool BaseWidget::IsRegisteredToCanvas() const
	{
		return m_canvas && m_canvasIndex != InvalidCanvasIndex;
	}

	inline void BaseWidget::NotifyParentResized(const Nz::Vector2f& newSize)
	{
		for (const auto& widgetPtr : m_children)
			widgetPtr->OnParentResized(newSize);
	}

	inline void BaseWidget::UpdateCanvasIndex(std::size_t index)
	{
		m_canvasIndex = index;
	}
}
