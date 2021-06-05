#pragma once

#ifndef NAZARA_SHADERNODES_MAINWINDOW_HPP
#define NAZARA_SHADERNODES_MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>

class NodeEditor;

class MainWindow : public QMainWindow
{
	public:
		MainWindow(ShaderGraph& graph);
		~MainWindow();

	private:
		void BuildMenu();
		void OnCompile();
		void OnLoad();
		void OnSave();
		void OnUpdateInfo();

		NazaraSlot(ShaderGraph, OnConditionUpdate, m_onConditionUpdate);
		NazaraSlot(ShaderGraph, OnSelectedNodeUpdate, m_onSelectedNodeUpdate);

		NodeEditor* m_nodeEditor;
		ShaderGraph& m_shaderGraph;
};

#include <ShaderNode/Widgets/MainWindow.inl>

#endif
