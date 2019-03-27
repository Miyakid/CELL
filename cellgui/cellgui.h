#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_cellgui.h"

class cellgui : public QMainWindow
{
	Q_OBJECT

public:
	cellgui(QWidget *parent = Q_NULLPTR);

private:
	Ui::cellguiClass ui;
};
