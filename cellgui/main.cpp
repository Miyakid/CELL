#include "cellgui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	cellgui w;
	w.show();
	return a.exec();
}
