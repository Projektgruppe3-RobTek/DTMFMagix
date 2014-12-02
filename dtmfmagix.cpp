#include "dtmfmagix.h"
#include "ui_dtmfmagix.h"
#include <iostream>

DTMFMagix::DTMFMagix(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DTMFMagix)
{
	appLayer=AppLayer::getInstance();
    ui->setupUi(this);
    sending = false;
    mThread = new MyThread(this,appLayer,&sending);
    connect(mThread,SIGNAL(NumberChanged(int,int)),this,SLOT(onNumberChanged(int,int)));
    mThread->start();
}

DTMFMagix::~DTMFMagix()
{
	mThread->Stop();
	mThread->wait();
    delete ui;
}

void DTMFMagix::on_browseButton_clicked()
{

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec())
    {
        filePath = dialog.selectedFiles();
    }
    if(filePath.size()>0) ui->pathEdit->setText(filePath[0]);
}

void DTMFMagix::on_sendButton_clicked()
{
    if(!filePath.size()) return;
    ui->sendButton->setEnabled(false);
    if(sendThread.joinable())
    {
        sendThread.join();
    }
    sending = true;
    filePath[0]=ui->pathEdit->text();
    sendThread = std::thread(sendFileWrapper,appLayer,filePath[0].toStdString(),appLayer->stripPath(filePath[0].toStdString()),this);
}

void DTMFMagix::fileTreeSetup(string path)
{
	ui->listWidget->clear();
	new QListWidgetItem("..", ui->listWidget);
    appLayer->requestFileTree(path);
    vector<string> fileTree=appLayer->getFileTree();
    currentFolder = path;
	cout << currentFolder << endl;
    for(int i=0;i<fileTree.size();i++)
    {
        new QListWidgetItem(QString::fromStdString(appLayer->stripPath(fileTree[i])), ui->listWidget);
    }
}

void DTMFMagix::on_requestButton_clicked()
{
	fileTreeSetup("./");
}

void DTMFMagix::onNumberChanged(int max, int current)
{
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(current);
}

void sendFileWrapper(AppLayer *appLayer,string path,string name,DTMFMagix *Magix)
{
    appLayer->sendFile(path,name);
    Magix->setDone();
}

void DTMFMagix::setDone()
{
    ui->sendButton->setEnabled(true);
}

void DTMFMagix::on_listWidget_itemClicked(QListWidgetItem *item)
{
    fileName=item->text();
}

void DTMFMagix::on_downloadButton_clicked()
{
    appLayer->requestFile(currentFolder+fileName.toStdString(),"./"+fileName.toStdString());
}

void DTMFMagix::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
	if(item->text().toStdString().back() == '/')
	{
		fileTreeSetup(currentFolder+item->text().toStdString());
	}
	else if(item==ui->listWidget->item(0))
	{
		fileTreeSetup(currentFolder+"../");
	}
}

