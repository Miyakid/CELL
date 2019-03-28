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

	QDockWidget *dock_Image;                    // ͼ�񴰿�
	QString currentPath;                        // ��ǰͼ���·��
	QLabel *imgLabel;                           // ͼ����ʾ��
	void InitImage();                           // ��ʼ��ͼ��
	void Menu_File();                           // �ļ��˵�

private slots:             // �ۺ���
	void Pic_open();      
	void Pic_detect();       
	void Pic_save();     
	void Pic_saveas();
	void autobatch();
};
