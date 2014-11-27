#include "dtmfmagix.h"
#include "ui_dtmfmagix.h"

DTMFMagix::DTMFMagix(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DTMFMagix)
{
    ui->setupUi(this);

    mThread = new MyThread(this,&appLayer);
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
    filePath[0]=ui->pathEdit->text();
    appLayer.sendFile(filePath[0].toStdString());
}


void DTMFMagix::on_requestButton_clicked()
{

    appLayer.requestFileTree(filePath[0].toStdString());

}

void DTMFMagix::onNumberChanged(int max, int current)
{
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(current);
}
