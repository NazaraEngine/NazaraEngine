---
title: Window
description: Nothing
---

# Nz::Window

Class description

## Constructors

- `Window()`
- `Window(`[`VideoMode`](documentation/generated/Platform/VideoMode.md)` mode, std::string const& title, Nz::WindowStyleFlags style)`
- `Window(`[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` handle)`
- `Window(`Window` const&)`
- `Window(`Window`&& window)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Close()` |
| `bool` | `Create(`[`VideoMode`](documentation/generated/Platform/VideoMode.md)` mode, std::string const& title, Nz::WindowStyleFlags style)` |
| `bool` | `Create(`[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` handle)` |
| `void` | `Destroy()` |
| `void` | `EnableCloseOnQuit(bool closeOnQuit)` |
| `std::shared_ptr<`[`Cursor`](documentation/generated/Platform/Cursor.md)`> const&` | `GetCursor()` |
| [`CursorController`](documentation/generated/Platform/CursorController.md)`&` | `GetCursorController()` |
| [`WindowEventHandler`](documentation/generated/Platform/WindowEventHandler.md)`&` | `GetEventHandler()` |
| `Nz::WindowHandle` | `GetHandle()` |
| `Nz::Vector2i const&` | `GetPosition()` |
| `Nz::Vector2ui const&` | `GetSize()` |
| `Nz::WindowStyleFlags` | `GetStyle()` |
| `std::string` | `GetTitle()` |
| `void` | `HandleEvent(`[`WindowEvent`](documentation/generated/Platform/WindowEvent.md)` const& event)` |
| `bool` | `HasFocus()` |
| `bool` | `IsMinimized()` |
| `bool` | `IsOpen(bool checkClosed)` |
| `bool` | `IsOpen()` |
| `bool` | `IsValid()` |
| `bool` | `IsVisible()` |
| `void` | `SetCursor(std::shared_ptr<`[`Cursor`](documentation/generated/Platform/Cursor.md)`> cursor)` |
| `void` | `SetCursor(Nz::SystemCursor systemCursor)` |
| `void` | `SetFocus()` |
| `void` | `SetIcon(std::shared_ptr<`[`Icon`](documentation/generated/Platform/Icon.md)`> icon)` |
| `void` | `SetMaximumSize(Nz::Vector2i const& maxSize)` |
| `void` | `SetMaximumSize(int width, int height)` |
| `void` | `SetMinimumSize(Nz::Vector2i const& minSize)` |
| `void` | `SetMinimumSize(int width, int height)` |
| `void` | `SetPosition(Nz::Vector2i const& position)` |
| `void` | `SetPosition(int x, int y)` |
| `void` | `SetSize(Nz::Vector2i const& size)` |
| `void` | `SetSize(unsigned int width, unsigned int height)` |
| `void` | `SetStayOnTop(bool stayOnTop)` |
| `void` | `SetTitle(std::string const& title)` |
| `void` | `SetVisible(bool visible)` |
| Window`&` | `operator=(`Window` const&)` |
| Window`&` | `operator=(`Window`&& window)` |
