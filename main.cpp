#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	QFile qss(":/style/lightblue.css");

	if( qss.open(QFile::ReadOnly))
	{
		qDebug("open success");
		QString style = QLatin1String(qss.readAll());
		a.setStyleSheet(style);
		qss.close();
	}
	else
	{
		qDebug("Open failed");
	}
	w.show();

	return a.exec();
}
