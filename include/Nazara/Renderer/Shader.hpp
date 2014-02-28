// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_HPP
#define NAZARA_SHADER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Renderer/Enums.hpp>

class NzShader;
class NzShaderStage;

using NzShaderConstRef = NzResourceRef<const NzShader>;
using NzShaderRef = NzResourceRef<NzShader>;

class NAZARA_API NzShader : public NzResource, NzNonCopyable
{
	friend class NzRenderer;

	public:
		NzShader();
		NzShader(NzShader&& shader);
		~NzShader();

		void AttachStage(nzShaderStage stage, const NzShaderStage& shaderStage);
		bool AttachStageFromFile(nzShaderStage stage, const NzString& filePath);
		bool AttachStageFromSource(nzShaderStage stage, const char* source, unsigned int length);
		bool AttachStageFromSource(nzShaderStage stage, const NzString& source);

		void Bind() const;

		bool Create();
		void Destroy();

		NzByteArray GetBinary() const;
		NzString GetLog() const;
		NzString GetSourceCode(nzShaderStage stage) const;
		int GetUniformLocation(const NzString& name) const;
		int GetUniformLocation(nzShaderUniform shaderUniform) const;

		bool HasStage(nzShaderStage stage) const;

		bool IsBinaryRetrievable() const;
		bool IsLinked() const;
		bool IsValid() const;

		bool Link();

		bool LoadFromBinary(const void* buffer, unsigned int size);
		bool LoadFromBinary(const NzByteArray& byteArray);

		void SendBoolean(int location, bool value) const;
		void SendColor(int location, const NzColor& color) const;
		void SendDouble(int location, double value) const;
		void SendDoubleArray(int location, const double* values, unsigned int count) const;
		void SendFloat(int location, float value) const;
		void SendFloatArray(int location, const float* values, unsigned int count) const;
		void SendInteger(int location, int value) const;
		void SendIntegerArray(int location, const int* values, unsigned int count) const;
		void SendMatrix(int location, const NzMatrix4d& matrix) const;
		void SendMatrix(int location, const NzMatrix4f& matrix) const;
		void SendVector(int location, const NzVector2d& vector) const;
		void SendVector(int location, const NzVector2f& vector) const;
		void SendVector(int location, const NzVector2i& vector) const;
		void SendVector(int location, const NzVector3d& vector) const;
		void SendVector(int location, const NzVector3f& vector) const;
		void SendVector(int location, const NzVector3i& vector) const;
		void SendVector(int location, const NzVector4d& vector) const;
		void SendVector(int location, const NzVector4f& vector) const;
		void SendVector(int location, const NzVector4i& vector) const;
		void SendVectorArray(int location, const NzVector2d* vectors, unsigned int count) const;
		void SendVectorArray(int location, const NzVector2f* vectors, unsigned int count) const;
		void SendVectorArray(int location, const NzVector2i* vectors, unsigned int count) const;
		void SendVectorArray(int location, const NzVector3d* vectors, unsigned int count) const;
		void SendVectorArray(int location, const NzVector3f* vectors, unsigned int count) const;
		void SendVectorArray(int location, const NzVector3i* vectors, unsigned int count) const;
		void SendVectorArray(int location, const NzVector4d* vectors, unsigned int count) const;
		void SendVectorArray(int location, const NzVector4f* vectors, unsigned int count) const;
		void SendVectorArray(int location, const NzVector4i* vectors, unsigned int count) const;

		// Fonctions OpenGL
		unsigned int GetOpenGLID() const;

		NzShader& operator=(NzShader&& shader);

		static bool IsStageSupported(nzShaderStage stage);

	private:
		bool PostLinkage();

		std::vector<unsigned int> m_attachedShaders[nzShaderStage_Max+1];
		bool m_linked;
		int m_uniformLocations[nzShaderUniform_Max+1];
		unsigned int m_program;
};

#endif // NAZARA_SHADER_HPP
