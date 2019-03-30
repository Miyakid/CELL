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

	QDockWidget *dock_Image;// ͼ�񴰿�
	QDockWidget* dock_Geom;// ���α任����
	QDockWidget* dock_Output;// �������
	QSlider *slider;
	QLineEdit *linetext;
	//QSpinBox *spinbox;
	QImage Demo;
	double bili = 1;

	QString currentPath;                        // ��ǰͼ���·��
	QLabel *imgLabel;                           // ͼ����ʾ��
	QLabel *imgLabel_demo;                      // ͼ����ʾ��
	void InitImage();                           // ��ʼ��ͼ��
	void Menu_File();                           // �ļ��˵�

private slots:             // �ۺ���
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
