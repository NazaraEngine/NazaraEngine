#pragma once

#ifndef NAZARA_SHADERNODES_MAINWINDOW_HPP
#define NAZARA_SHADERNODES_MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>

class NodeEditor;

namespace Nz
{
	class ShaderAst;
}

class MainWindow : public QMainWindow
{
	public:
		MainWindow(ShaderGraph& graph);
		~MainWindow() = default;

	private:
		void BuildMenu();
		void OnCompile();
		void OnGenerateGLSL();
		void OnLoad();
		void OnSave();
		Nz::ShaderAst ToShader();

		NazaraSlot(ShaderGraph, OnSelectedNodeUpdate, m_onSelectedNodeUpdate);

		NodeEditor* m_nodeEditor;
		ShaderGraph& m_shaderGraph;
};

#include <ShaderNode/Widgets/MainWindow.inl>

#endif
