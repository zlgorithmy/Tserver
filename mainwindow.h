#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow()
		{
			delete ui;
		}

	private slots:
		void on_btnListen_clicked();

		void on_btnSend_clicked();

		void on_btnFresh_clicked();

		void on_comboBoxServerPort_currentTextChanged(const QString &arg1);

		void on_comboBoxServerAddr_currentTextChanged(const QString &arg1);

		void quit();

		void newConnection();
		void disconnected();
		void readData();
		void on_btnDisconnect_clicked();

		void about();

	private:
		Ui::MainWindow *ui;
		QString mHost;
		int mPort;

		QTcpServer *mTcpServer;
		QList<QTcpSocket*> mTcpClients;
		QTcpSocket *mCurrentClient;

		QLabel *mLocationLabel;
		QLabel* aixLabel;

		QPoint last;
	private:
		void initUi();
		void fresh();
		void initStatusBar();

	protected:
		void mousePressEvent(QMouseEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
};

#endif // MAINWINDOW_H
