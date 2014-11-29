#include "dtmfmagix.h"
#include "ui_dtmfmagix.h"
#include <thread>

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
    delete ui;
}

void DTMFMagix::on_browseButton_clicked()
{

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec())
        filePath = dialog.selectedFiles();
    ui->pathEdit->setText(filePath[0]);
}

void DTMFMagix::on_sendButton_clicked()
{
    ui->sendButton->setEnabled(false);
    if(sendThread.joinable())
    {
        sendThread.join();
    }
    sending = true;
    filePath[0]=ui->pathEdit->text();
    //appLayer->sendFile(filePath[0].toStdString());
    string path=filePath[0].toStdString();
    string stripedpath=appLayer->stripPath(path);
    sendThread = std::thread(sendFileWrapper,appLayer,filePath[0].toStdString(),stripedpath,this);
}

void DTMFMagix::on_requestButton_clicked()
{
	string path=filePath[0].toStdString();
    appLayer->requestFileTree(path);

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
