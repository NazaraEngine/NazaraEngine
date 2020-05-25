#pragma once

#ifndef NAZARA_SHADERNODES_MAINWINDOW_HPP
#define NAZARA_SHADERNODES_MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>
#include <ShaderNode/DataModels/ShaderNode.hpp>

class ShaderGraph;

class MainWindow : public QMainWindow
{
	public:
		MainWindow(ShaderGraph& graph);
		~MainWindow() = default;

	private:
		void BuildMenu();
		void OnCompileToGLSL();

		ShaderGraph& m_shaderGraph;
};

#include <ShaderNode/Widgets/MainWindow.inl>

#endif
