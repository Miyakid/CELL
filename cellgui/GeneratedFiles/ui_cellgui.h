/********************************************************************************
** Form generated from reading UI file 'cellgui.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CELLGUI_H
#define UI_CELLGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cellguiClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *cellguiClass)
    {
        if (cellguiClass->objectName().isEmpty())
            cellguiClass->setObjectName(QStringLiteral("cellguiClass"));
        cellguiClass->resize(600, 400);
        menuBar = new QMenuBar(cellguiClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        cellguiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(cellguiClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        cellguiClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(cellguiClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        cellguiClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(cellguiClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        cellguiClass->setStatusBar(statusBar);

        retranslateUi(cellguiClass);

        QMetaObject::connectSlotsByName(cellguiClass);
    } // setupUi

    void retranslateUi(QMainWindow *cellguiClass)
    {
        cellguiClass->setWindowTitle(QApplication::translate("cellguiClass", "cellgui", nullptr));
    } // retranslateUi

};

namespace Ui {
    class cellguiClass: public Ui_cellguiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CELLGUI_H
