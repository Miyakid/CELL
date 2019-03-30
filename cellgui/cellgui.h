#pragma once

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets>
#include <QMainWindow>
#include "ui_cellgui.h"
#include <string>
#include<opencv2/opencv.hpp>
#include<iostream>
#pragma execution_character_set("utf-8")

class cellgui : public QMainWindow
{
	Q_OBJECT

public:
	cellgui(QWidget *parent = Q_NULLPTR);
	~cellgui();

private:
	Ui::cellguiClass ui;

	QDockWidget *dock_Image;// 图像窗口
	QDockWidget* dock_Geom;// 几何变换窗口
	QDockWidget* dock_Output;// 输出窗口
	QSlider *slider;
	QLineEdit *linetext;
	//QSpinBox *spinbox;
	QImage Demo;
	double bili = 1;

	QString currentPath;                        // 当前图像的路径
	QLabel *imgLabel;                           // 图像显示框
	QLabel *imgLabel_demo;                      // 图像显示框
	void InitImage();                           // 初始化图像
	void Menu_File();                           // 文件菜单

private slots:             // 槽函数
	void Pic_open();      
	void Pic_detect();       
	void Pic_save();     
	void Pic_saveas();
	void autobatch();
	void next();
	void slot_slider(int);
	void setlinetextValue(int);
	void setsliderValue();
	void about();
};
