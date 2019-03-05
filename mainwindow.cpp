#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QLineEdit>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	//QApplication::setStyle(QStyleFactory::create("Fusion"));
	setWindowIcon(QIcon(QStringLiteral(":/ico/ico")));
//	QFile qss("qrc:/style/lightblue.css");
//	qss.open(QFile::ReadOnly);
//	//qApp->setStyleSheet(qss.readAll());
//	qss.close();
//	parent->setStyleSheet(qss.readAll());

	ui->setupUi(this);
	this->setFixedSize( this->width (),this->height ());
	//setWindowFlags (Qt::FramelessWindowHint);
	//this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint); // 去掉标题栏,去掉任务栏显示，窗口置顶
	//setWindowFlags(Qt::CustomizeWindowHint);
	this->setWindowFlags(Qt::FramelessWindowHint);
	//this->setAttribute(Qt::WA_TranslucentBackground, true);//设置透明2-窗体标题栏不透明,背景透明
	//this->setWindowOpacity(0.98);
	last.setX(this->x());
	last.setY(this->y());


	mTcpServer = new QTcpServer(this);
	ui->btnSend->setEnabled(false);
	ui->btnDisconnect->setEnabled(false);
	initStatusBar();


	connect(ui->quit, SIGNAL(triggered()), this, SLOT(quit()));
	ui->menuBar->addAction(tr("&Help"), this, SLOT(about()));
	ui->menuBar->addAction(tr("&Quit"), this, SLOT(quit()));

	//端口设置
	QRegExp rx("^[0-9]{1,10}$");
	QValidator *validator = new QRegExpValidator(rx,0);
	ui->comboBoxServerPort->lineEdit()->setValidator(validator);

	ui->comboBoxServerAddr->clear();
	ui->comboBoxClientAddr->clear();
	fresh();
	connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	qDebug() << QStyleFactory::keys();
}

void MainWindow::on_btnListen_clicked()
{
	mHost = ui->comboBoxServerAddr->currentText();
	mPort = ui->comboBoxServerPort->currentText().toInt();
	qDebug() << "Host : " << mHost;
	qDebug() << "Port : " << mPort;

	if(ui->btnListen->text() == "监听")
	{
		if(mTcpServer->listen(QHostAddress(mHost), mPort))
		{
			ui->btnListen->setText("取消");
		}
	}
	else if (ui->btnListen->text() == "取消")
	{
		for(int i=0; i< mTcpClients.length(); i++)//断开所有连接
		{
			mTcpClients[i]->disconnectFromHost();
			bool ok = mTcpClients[i]->waitForDisconnected(1000);
			if(!ok)
			{
				// 处理异常
			}
			mTcpClients.removeAt(i);  //从保存的客户端列表中取去除
		}
		mTcpServer->close();     //不再监听端口
		ui->btnListen->setText("监听");
		ui->btnSend->setEnabled(false);
	}
}

void MainWindow::on_btnSend_clicked()
{
	if(ui->btnListen->text()=="")
	{

	}


	//	if(ui->btnConnect->text()=="连接")
	//    {
	//        tcpClient->connectToHost(ui->edtIP->text(), ui->edtPort->text().toInt());
	//        if (tcpClient->waitForConnected(1000))  // 连接成功则进入if{}
	//        {
	//            ui->btnConnect->setText("断开");
	//            ui->btnSend->setEnabled(true);
	//        }
	//    }
	//    else
	//    {
	//        tcpClient->disconnectFromHost();
	//        if (tcpClient->state() == QAbstractSocket::UnconnectedState \
	//                || tcpClient->waitForDisconnected(1000))  //已断开连接则进入if{}
	//        {
	//            ui->btnConnect->setText("连接");
	//            ui->btnSend->setEnabled(false);
	//        }
	//    }
}

void MainWindow::on_comboBoxServerPort_currentTextChanged(const QString &arg1)
{
	mPort = arg1.toInt();
}

void MainWindow::on_comboBoxServerAddr_currentTextChanged(const QString &arg1)
{
	mHost = arg1;
}

void MainWindow::quit()
{
	close();
}

void MainWindow::newConnection()
{
	mCurrentClient = mTcpServer->nextPendingConnection();
	mTcpClients.append(mCurrentClient);

	qDebug()<<mCurrentClient->peerAddress().toString()<<mCurrentClient->peerPort();

	ui->comboBoxClientAddr->clear();
	foreach (QTcpSocket *client, mTcpClients)
	{
		ui->comboBoxClientAddr->addItem(client->peerAddress().toString()+":"+QString::number(client->peerPort()));
	}

	ui->labelConnecttCnt->setText(QString::number(mTcpClients.size()));

	connect(mCurrentClient, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(mCurrentClient, SIGNAL(disconnected()), this, SLOT(disconnected()));

	ui->btnSend->setEnabled(true);
	ui->btnDisconnect->setEnabled(true);
}

void MainWindow::disconnected()
{
	for(int i=0; i<mTcpClients.length(); i++)
	{
		if(mTcpClients[i]->state() == QAbstractSocket::UnconnectedState)
		{
			ui->comboBoxClientAddr->removeItem(ui->comboBoxClientAddr->findText(mTcpClients[i]->peerAddress().toString()+":"+QString::number(mTcpClients[i]->peerPort())));
			mTcpClients[i]->destroyed();
			mTcpClients.removeAt(i);
		}
	}
	ui->labelConnecttCnt->setText(QString::number(mTcpClients.size()));
	if(0==QString::number(mTcpClients.size())){
		ui->btnSend->setEnabled(false);
		ui->btnDisconnect->setEnabled(false);
	}
}

void MainWindow::readData()
{
	qDebug()<<"mTcpClients sz:"<<mTcpClients.size();
	for(int i = 0; i < mTcpClients.length(); i++)
	{
		qDebug()<<"readData:"<<i;
		QByteArray buffer = mTcpClients[i]->readAll();
		if(buffer.isEmpty())
			continue;
		ui->textEditRecv->append(mTcpClients[i]->peerAddress().toString()+"_"+QString::number(mTcpClients[i]->peerPort())+":"+buffer);
	}
}

void MainWindow::initUi()
{

}
void MainWindow::on_btnFresh_clicked()
{
	fresh();
}

void MainWindow::fresh()
{
	ui->comboBoxServerAddr->clear();
	QList<QHostAddress> list = QNetworkInterface::allAddresses();
	foreach (QHostAddress address, list)
	{
		// 主机地址为空
		if (address.isNull())
			continue;

		qDebug() << "********************";
		QAbstractSocket::NetworkLayerProtocol nProtocol = address.protocol();
		QString strScopeId = address.scopeId();
		QString strAddress = address.toString();
		bool bLoopback = address.isLoopback();

		//IPv4
		if (nProtocol == QAbstractSocket::IPv4Protocol) {
			bool bOk = false;
			quint32 nIPV4 = address.toIPv4Address(&bOk);
			if (bOk && !bLoopback){
				qDebug() << "IPV4 : " << nIPV4;
				ui->comboBoxServerAddr->addItem(strAddress);
			}
		}
		//IPv6
		else if (nProtocol == QAbstractSocket::IPv6Protocol) {
			QStringList IPV6List("");
			Q_IPV6ADDR IPV6 = address.toIPv6Address();
			for (int i = 0; i < 16; ++i) {
				quint8 nC = IPV6[i];
				IPV6List << QString::number(nC);
			}
			qDebug() << "IPV6 : " << IPV6List.join(" ");
		}
		qDebug() << "Protocol : " << nProtocol;
		qDebug() << "ScopeId : " << strScopeId;
		qDebug() << "Address : " << strAddress;
		qDebug() << "IsLoopback  : " << bLoopback;
	}
}

void MainWindow::initStatusBar()
{
	statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 设置不显示label的边框

	statusBar()->setSizeGripEnabled(false); //设置是否显示右边的大小控制点

	statusBar()->addPermanentWidget(new QLabel("zlgorithmy@gmail.com", this)); //现实永久信息
}

void MainWindow::on_btnDisconnect_clicked()
{
	for(int i=0; i<mTcpClients.length(); i++)
	{
		QString s=mTcpClients[i]->peerAddress().toString()+":"+QString::number(mTcpClients[i]->peerPort());
		QString p = ui->comboBoxClientAddr->currentText();
		qDebug()<<s<<" : "<<p;
		if(s == p){
			mTcpClients[i]->disconnectFromHost();
			mTcpClients.removeAt(i);  //从保存的客户端列表中取去除
		}
	}
	ui->labelConnecttCnt->setText(QString::number(mTcpClients.size()));
}

void MainWindow::about()
{
	qDebug()<<"about";
	QMessageBox::aboutQt(this, "About Qt");
	QMessageBox::about(this,"about","1.0");
}
//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void MainWindow::mousePressEvent(QMouseEvent *e)
{
	last = e->globalPos();
}
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
	int dx = e->globalX() - last.x();
	int dy = e->globalY() - last.y();
	last = e->globalPos();
	move(x()+dx, y()+dy);
}
void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
	int dx = e->globalX() - last.x();
	int dy = e->globalY() - last.y();
	move(x()+dx, y()+dy);
}
