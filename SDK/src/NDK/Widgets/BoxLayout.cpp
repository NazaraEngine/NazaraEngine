// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/BoxLayout.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <cassert>

namespace Ndk
{
	void BoxLayout::Layout()
	{
		std::size_t axis1, axis2;

		switch (m_orientation)
		{
			case BoxLayoutOrientation_Horizontal:
				axis1 = 0; //< x
				axis2 = 1; //< y
				break;

			case BoxLayoutOrientation_Vertical:
				axis1 = 1; //< y
				axis2 = 0; //< x
				break;

			default:
				assert(false);
				break;
		}

		m_childInfos.clear();

		// Handle size
		ForEachWidgetChild([&](BaseWidget* child)
		{
			if (!child->IsVisible())
				return;

			m_childInfos.emplace_back();
			auto& info = m_childInfos.back();
			info.maximumSize = child->GetMaximumSize()[axis1];
			info.minimumSize = child->GetMinimumSize()[axis1];
			info.size = -1.f; //< Undecided
			info.widget = child;
		});

		Nz::Vector2f layoutSize = GetContentSize();

		float remainingSize = layoutSize[axis1] - m_spacing * (m_childInfos.size() - 2); //< FIXME: Why -2 instead of -1?
		for (auto& info : m_childInfos)
			remainingSize -= info.minimumSize;

		if (remainingSize > 0.f)
		{
			// Take maximum size into account
			float evenSize = remainingSize / m_childInfos.size();

			std::size_t unconstrainedChildCount = m_childInfos.size();
			for (auto& info : m_childInfos)
			{
				float widgetSize = info.minimumSize + evenSize;
				if (widgetSize > info.maximumSize)
				{
					unconstrainedChildCount--;

					evenSize += (widgetSize - info.maximumSize) / unconstrainedChildCount;
					info.size = info.maximumSize;
				}
			}

			// Resize widgets which are not constrained
			for (auto& info : m_childInfos)
			{
				if (info.size < 0.f)
					info.size = info.minimumSize + evenSize;
			}
		}

		for (auto& info : m_childInfos)
		{
			Nz::Vector2f newSize = info.widget->GetSize();
			newSize[axis1] = info.size;

			info.widget->Resize(newSize);
		}

		// Handle position
		float cursor = 0.f;
		bool first = true;
		for (auto& info : m_childInfos)
		{
			if (first)
				first = false;
			else
				cursor += m_spacing;

			Nz::Vector2f widgetSize = info.widget->GetSize();
			Nz::Vector2f position = GetContentOrigin();

			position[axis1] = cursor;

			info.widget->SetPosition(position);

			cursor += widgetSize[axis1];
		};
	}

	void BoxLayout::OnParentResized(const Nz::Vector2f& /*newSize*/)
	{
		Layout();
	}
}
