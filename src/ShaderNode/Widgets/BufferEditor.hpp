#pragma once

#ifndef NAZARA_SHADERNODES_BUFFEREDITOR_HPP
#define NAZARA_SHADERNODES_BUFFEREDITOR_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QWidget>

class QLabel;
class QListWidget;
class QVBoxLayout;

class BufferEditor : public QWidget
{
	public:
		BufferEditor(ShaderGraph& graph);
		~BufferEditor() = default;

	private:
		void OnAddBuffer();
		void OnEditBuffer(int inputIndex);
		void OnBufferListUpdate(ShaderGraph* graph);
		void OnBufferUpdate(ShaderGraph* graph, std::size_t inputIndex);
		void RefreshBuffers();

		NazaraSlot(ShaderGraph, OnBufferListUpdate, m_onBufferListUpdateSlot);
		NazaraSlot(ShaderGraph, OnBufferUpdate, m_onBufferUpdateSlot);

		ShaderGraph& m_shaderGraph;
		QListWidget* m_bufferList;
		QVBoxLayout* m_layout;
};

#include <ShaderNode/Widgets/BufferEditor.inl>

#endif
