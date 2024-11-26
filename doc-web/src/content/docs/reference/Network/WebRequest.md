---
title: WebRequest
description: Nothing
---

# Nz::WebRequest

Class description

## Constructors

- `WebRequest(`[`WebService`](documentation/generated/Network/WebService.md)`& owner)`
- `WebRequest(`WebRequest` const&)`
- `WebRequest(`WebRequest`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `ForceProtocol(Nz::NetProtocol protocol)` |
| `void` | `SetDataCallback(Nz::WebRequest::DataCallback callback)` |
| `void` | `SetHeader(std::string header, std::string value)` |
| `void` | `SetJSonContent(std::string encodedJSon)` |
| `void` | `SetMaximumFileSize(Nz::UInt64 maxFileSize)` |
| `void` | `SetResultCallback(Nz::WebRequest::ResultCallback callback)` |
| `void` | `SetServiceName(std::string serviceName)` |
| `void` | `SetURL(std::string const& url)` |
| `void` | `SetupGet()` |
| `void` | `SetupPost()` |
| WebRequest`&` | `operator=(`WebRequest` const&)` |
| WebRequest`&` | `operator=(`WebRequest`&&)` |
