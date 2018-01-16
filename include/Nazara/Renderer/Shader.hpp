// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_HPP
#define NAZARA_SHADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	class Color;
	class Shader;
	class ShaderStage;

	using ShaderConstRef = ObjectRef<const Shader>;
	using ShaderLibrary = ObjectLibrary<Shader>;
	using ShaderRef = ObjectRef<Shader>;

	class NAZARA_RENDERER_API Shader : public RefCounted
	{
		friend ShaderLibrary;
		friend class Renderer;

		public:
			Shader();
			Shader(const Shader&) = delete;
			Shader(Shader&&) = delete;
			~Shader();

			void AttachStage(ShaderStageType stage, const ShaderStage& shaderStage);
			bool AttachStageFromFile(ShaderStageType stage, const String& filePath);
			bool AttachStageFromSource(ShaderStageType stage, const char* source, unsigned int length);
			bool AttachStageFromSource(ShaderStageType stage, const String& source);

			void Bind() const;

			bool Create();
			void Destroy();

			ByteArray GetBinary() const;
			String GetLog() const;
			String GetSourceCode(ShaderStageType stage) const;
			int GetUniformLocation(const String& name) const;
			int GetUniformLocation(ShaderUniform shaderUniform) const;

			bool HasStage(ShaderStageType stage) const;

			bool IsBinaryRetrievable() const;
			bool IsLinked() const;
			bool IsValid() const;

			bool Link();

			bool LoadFromBinary(const void* buffer, unsigned int size);
			bool LoadFromBinary(const ByteArray& byteArray);

			void SendBoolean(int location, bool value) const;
			void SendColor(int location, const Color& color) const;
			void SendDouble(int location, double value) const;
			void SendDoubleArray(int location, const double* values, unsigned int count) const;
			void SendFloat(int location, float value) const;
			void SendFloatArray(int location, const float* values, unsigned int count) const;
			void SendInteger(int location, int value) const;
			void SendIntegerArray(int location, const int* values, unsigned int count) const;
			void SendMatrix(int location, const Matrix4d& matrix) const;
			void SendMatrix(int location, const Matrix4f& matrix) const;
			void SendVector(int location, const Vector2d& vector) const;
			void SendVector(int location, const Vector2f& vector) const;
			void SendVector(int location, const Vector2i& vector) const;
			void SendVector(int location, const Vector3d& vector) const;
			void SendVector(int location, const Vector3f& vector) const;
			void SendVector(int location, const Vector3i& vector) const;
			void SendVector(int location, const Vector4d& vector) const;
			void SendVector(int location, const Vector4f& vector) const;
			void SendVector(int location, const Vector4i& vector) const;
			void SendVectorArray(int location, const Vector2d* vectors, unsigned int count) const;
			void SendVectorArray(int location, const Vector2f* vectors, unsigned int count) const;
			void SendVectorArray(int location, const Vector2i* vectors, unsigned int count) const;
			void SendVectorArray(int location, const Vector3d* vectors, unsigned int count) const;
			void SendVectorArray(int location, const Vector3f* vectors, unsigned int count) const;
			void SendVectorArray(int location, const Vector3i* vectors, unsigned int count) const;
			void SendVectorArray(int location, const Vector4d* vectors, unsigned int count) const;
			void SendVectorArray(int location, const Vector4f* vectors, unsigned int count) const;
			void SendVectorArray(int location, const Vector4i* vectors, unsigned int count) const;

			bool Validate() const;

			// Fonctions OpenGL
			unsigned int GetOpenGLID() const;

			Shader& operator=(const Shader&) = delete;
			Shader& operator=(Shader&&) = delete;

			static bool IsStageSupported(ShaderStageType stage);
			template<typename... Args> static ShaderRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnShaderDestroy, const Shader* /*shader*/);
			NazaraSignal(OnShaderRelease, const Shader* /*shader*/);
			NazaraSignal(OnShaderUniformInvalidated, const Shader* /*shader*/);

		private:
			bool PostLinkage();

			static bool Initialize();
			static void Uninitialize();

			std::vector<unsigned int> m_attachedShaders[ShaderStageType_Max+1];
			bool m_linked;
			int m_uniformLocations[ShaderUniform_Max+1];
			unsigned int m_program;

			static ShaderLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Renderer/Shader.inl>

#endif // NAZARA_SHADER_HPP
