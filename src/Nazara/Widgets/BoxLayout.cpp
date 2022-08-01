// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/BoxLayout.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utils/StackVector.hpp>
#include <kiwi/kiwi.h>
#include <cassert>
#include <vector>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	struct BoxLayout::State
	{
		std::vector<kiwi::Variable> sizeVar;
		kiwi::Solver solver;
	};

	BoxLayout::BoxLayout(BaseWidget* parent, BoxLayoutOrientation orientation) :
	BaseWidget(parent),
	m_orientation(orientation),
	m_spacing(5.f)
	{
		m_state = std::make_unique<State>();
	}

	BoxLayout::~BoxLayout() = default;

	void BoxLayout::Layout()
	{
		BaseWidget::Layout();

		std::size_t axis;

		switch (m_orientation)
		{
			case BoxLayoutOrientation::Horizontal:
				axis = 0; //< x
				break;

			case BoxLayoutOrientation::Vertical:
				axis = 1; //< y
				break;

			default:
				assert(false);
				break;
		}

		//TODO: Keep solver state when widgets don't change
		std::size_t widgetChildCount = GetWidgetChildCount();
		if (widgetChildCount == 0)
			return;

		m_state->solver.reset();

		m_state->sizeVar.clear();
		m_state->sizeVar.reserve(widgetChildCount);

		kiwi::Expression sizeSum;

		Nz::Vector2f layoutSize = GetSize();
		float availableSpace = layoutSize[axis] - m_spacing * (widgetChildCount - 1);
		float perfectSpacePerWidget = availableSpace / widgetChildCount;

		// Handle size
		ForEachWidgetChild([&](BaseWidget* child)
		{
			if (!child->IsVisible())
				return;

			float maximumSize = child->GetMaximumSize()[axis];
			float minimumSize = child->GetMinimumSize()[axis];

			m_state->sizeVar.emplace_back();
			auto& sizeVar = m_state->sizeVar.back();

			m_state->solver.addConstraint({ sizeVar >= minimumSize | kiwi::strength::required });

			if (maximumSize < std::numeric_limits<float>::infinity())
				m_state->solver.addConstraint({ sizeVar <= maximumSize | kiwi::strength::required });

			m_state->solver.addConstraint({ sizeVar >= perfectSpacePerWidget | kiwi::strength::medium });

			sizeSum = sizeSum + sizeVar;
		});

		kiwi::Variable targetSize("LayoutSize");

		m_state->solver.addConstraint(sizeSum <= targetSize | kiwi::strength::strong);

		m_state->solver.addEditVariable(targetSize, kiwi::strength::strong);
		m_state->solver.suggestValue(targetSize, availableSpace);

		m_state->solver.updateVariables();

		std::size_t varIndex = 0;

		float remainingSize = availableSpace;

		ForEachWidgetChild([&](BaseWidget* child)
		{
			if (!child->IsVisible())
				return;

			Nz::Vector2f newSize = layoutSize;
			newSize[axis] = m_state->sizeVar[varIndex].value();

			child->Resize(newSize);
			remainingSize -= newSize[axis];

			varIndex++;
		});

		float spacing = m_spacing + remainingSize / (widgetChildCount - 1);

		// Handle position
		float cursor = 0.f;
		bool first = true;
		ForEachWidgetChild([&](BaseWidget* child)
		{
			if (first)
				first = false;
			else
				cursor += spacing;

			Nz::Vector2f position = Nz::Vector2f(0.f, 0.f);
			position[axis] = cursor;

			child->SetPosition(position);

			cursor += child->GetSize()[axis];
		});
	}
}
