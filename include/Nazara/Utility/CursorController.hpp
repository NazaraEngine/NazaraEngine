// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSORCONTROLLER_HPP
#define NAZARA_CURSORCONTROLLER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Cursor.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class CursorController;

	using CursorControllerHandle = ObjectHandle<CursorController>;

	class CursorController : public HandledObject<CursorController>
	{
		public:
			CursorController() = default;
			CursorController(const CursorController&) = delete;
			CursorController(CursorController&&) = default;
			~CursorController() = default;

			inline void UpdateCursor(const CursorRef& cursor);

			CursorController& operator=(const CursorController&) = delete;
			CursorController& operator=(CursorController&&) = default;

			NazaraSignal(OnCursorUpdated, const CursorController* /*cursorController*/, const CursorRef& /*cursor*/);
	};
}

#include <Nazara/Utility/CursorController.inl>

#endif // NAZARA_CURSORCONTROLLER_HPP
