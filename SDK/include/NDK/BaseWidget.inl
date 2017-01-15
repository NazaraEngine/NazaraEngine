// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseWidget.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <limits>

namespace Ndk
{
	inline BaseWidget::BaseWidget() :
	m_canvasIndex(std::numeric_limits<std::size_t>::max()),
	m_backgroundColor(Nz::Color(230, 230, 230, 255)),
	m_canvas(nullptr),
	m_contentSize(50.f, 50.f),
	m_widgetParent(nullptr)
	{
		SetPadding(5.f, 5.f, 5.f, 5.f);
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
		m_children.emplace_back(std::move(widget));
	}

	inline void BaseWidget::Center()
	{
		NazaraAssert(m_widgetParent, "Widget has no parent");

		Nz::Vector2f parentSize = m_widgetParent->GetSize();
		Nz::Vector2f mySize = GetSize();
		SetPosition((parentSize.x - mySize.x) / 2.f, (parentSize.y - mySize.y) / 2.f);
	}

	inline const Nz::Color& BaseWidget::GetBackgroundColor() const
	{
		return m_backgroundColor;
	}

	inline Canvas* BaseWidget::GetCanvas()
	{
		return m_canvas;
	}

	inline const BaseWidget::Padding& BaseWidget::GetPadding() const
	{
		return m_padding;
	}

	inline const Nz::Vector2f& BaseWidget::GetContentSize() const
	{
		return m_contentSize;
	}

	inline Nz::Vector2f BaseWidget::GetSize() const
	{
		return Nz::Vector2f(m_contentSize.x + m_padding.left + m_padding.right, m_contentSize.y + m_padding.top + m_padding.bottom);
	}

	inline void BaseWidget::SetContentSize(const Nz::Vector2f& size)
	{
		NotifyParentResized(size);
		m_contentSize = size;

		Layout();
	}

	inline void BaseWidget::SetPadding(float left, float top, float right, float bottom)
	{
		m_padding.left = left;
		m_padding.top = top;
		m_padding.bottom = bottom;
		m_padding.right = right;

		Layout();
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
