---
title: StateMachine
description: Nothing
---

# Nz::StateMachine

Class description

## Constructors

- `StateMachine(std::shared_ptr<`[`State`](documentation/generated/Core/State.md)`> initialState)`
- `StateMachine(`StateMachine` const&)`
- `StateMachine(`StateMachine`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `ChangeState(std::shared_ptr<`[`State`](documentation/generated/Core/State.md)`> state)` |
| `void` | `Disable(std::shared_ptr<`[`State`](documentation/generated/Core/State.md)`> state)` |
| `void` | `Enable(std::shared_ptr<`[`State`](documentation/generated/Core/State.md)`> state)` |
| `bool` | `IsStateEnabled(`[`State`](documentation/generated/Core/State.md)` const* state)` |
| `bool` | `IsTopState(`[`State`](documentation/generated/Core/State.md)` const* state)` |
| `void` | `PopState()` |
| `void` | `PopStatesUntil(std::shared_ptr<`[`State`](documentation/generated/Core/State.md)`> state)` |
| `void` | `PushState(std::shared_ptr<`[`State`](documentation/generated/Core/State.md)`> state, bool enabled)` |
| `void` | `ResetState(std::shared_ptr<`[`State`](documentation/generated/Core/State.md)`> state)` |
| `bool` | `Update(`[`Time`](documentation/generated/Core/Time.md)` elapsedTime)` |
| StateMachine`&` | `operator=(`StateMachine`&& fsm)` |
| StateMachine`&` | `operator=(`StateMachine` const&)` |
