---
title: WebService
description: Nothing
---

# Nz::WebService

Class description

## Constructors

- `WebService(`[`CurlLibrary`](documentation/generated/Network/CurlLibrary.md)` const& library)`
- `WebService(`WebService` const&)`
- `WebService(`WebService`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::unique_ptr<`[`WebRequest`](documentation/generated/Network/WebRequest.md)`>` | `AllocateRequest()` |
| `std::unique_ptr<`[`WebRequest`](documentation/generated/Network/WebRequest.md)`>` | `CreateGetRequest(std::string const& url, WebRequest::ResultCallback callback)` |
| `std::unique_ptr<`[`WebRequest`](documentation/generated/Network/WebRequest.md)`>` | `CreatePostRequest(std::string const& url, WebRequest::ResultCallback callback)` |
| `std::string const&` | `GetUserAgent()` |
| `void` | `Poll()` |
| `void` | `QueueRequest(std::unique_ptr<`[`WebRequest`](documentation/generated/Network/WebRequest.md)`>&& request)` |
| WebService`&` | `operator=(`WebService` const&)` |
| WebService`&` | `operator=(`WebService`&&)` |
