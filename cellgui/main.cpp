#include "cellgui.h"
#include <QtWidgets/QApplication>

extern int cvgo();
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	cellgui w;
	w.show();
	cvgo();
	return a.exec();
}
