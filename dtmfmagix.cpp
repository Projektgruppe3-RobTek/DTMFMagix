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

void DTMFMagix::on_browseButton_clicked()							//SLOT: called when browseButton is clicked
{
    QFileDialog dialog(this);										//Create QFileDialog object
    dialog.setFileMode(QFileDialog::ExistingFile);					//Only existing files are selectable
    dialog.setViewMode(QFileDialog::Detail);						//Details about files are shown in dialog
    if (dialog.exec())
    {
        filePath = dialog.selectedFiles();							//Show Dialog. Save path to selected file as filePath
    }
    if(filePath.size()>0) ui->pathEdit->setText(filePath[0]);		//If filePath is not empty, set text in pathEdit to filePath
}

void DTMFMagix::on_sendButton_clicked()								//SLOT: called when sendButton is clicked
{
	if(!filePath.size()) return;
    ui->sendButton->setEnabled(false);								//Disable buttons while sending
    ui->downloadButton->setEnabled(false);
    ui->requestButton->setEnabled(false);
    if(sendThread.joinable())										//If already sending, wait until done
    {
        sendThread.join();											
    }
    sending = true;
    filePath[0]=ui->pathEdit->text();								//Save text in pathEdit as filePath
    sendThread = std::thread(sendFileWrapper,appLayer,filePath[0].toStdString(),appLayer->stripPath(filePath[0].toStdString()),this); //Send file at filePath
}

void DTMFMagix::fileTreeSetup(string path)							//Setup contents of path in listWidget
{
	ui->listWidget->clear();										//Delete all elements in listWidget
	new QListWidgetItem("..", ui->listWidget);						//Create ".." element in start of list
    appLayer->requestFileTree(path);								//Request contents of path
    vector<string> fileTree=appLayer->getFileTree();				//Save contents of path as fileTree
    currentFolder = path;
    for(int i=0;i<fileTree.size()-1;i++)							//Create strings in fileTree, as seperate elements in listWidget
    {
        new QListWidgetItem(QString::fromStdString(appLayer->stripPath(fileTree[i])), ui->listWidget);
    }
}

void DTMFMagix::on_requestButton_clicked()
{
	fileTreeSetup("./");											//Setup contents of application folder in listWidget
}

void DTMFMagix::onNumberChanged(int max, int current)				//SLOT: called when a frame is received correctly
{
    ui->progressBar->setMaximum(max);								
    ui->progressBar->setValue(current);
}

void sendFileWrapper(AppLayer *appLayer,string path,string name,DTMFMagix *Magix)	//Wrapper for sendThread
{
    appLayer->sendFile(path,name,1);													//Send file
    Magix->setDone();																//Call setDone
}

void DTMFMagix::setDone()											//Enable all buttons
{
    ui->sendButton->setEnabled(true);
    ui->downloadButton->setEnabled(true);
    ui->requestButton->setEnabled(true);
}

void DTMFMagix::on_listWidget_itemClicked(QListWidgetItem *item)	//SLOT: called when item in listWidget is clicked
{
	if(item->text().toStdString().back() == '/')					//If item is folder (ends with '/') disable downloadsButton
	{
		ui->downloadButton->setEnabled(false);
	}
	else 															//Else enable downloadbutton and save item name as fileName
	{
		ui->downloadButton->setEnabled(true);
		fileName=item->text();
	}
}

void DTMFMagix::on_downloadButton_clicked()							//SLOT: called when downloadButton is clicked
{
    appLayer->requestFile(currentFolder+fileName.toStdString(),"./"+fileName.toStdString()); //Download file at fileName
}

void DTMFMagix::on_listWidget_itemDoubleClicked(QListWidgetItem *item) 	//SLOT: called when item in listWidget is double clicked
{
	if(item->text().toStdString().back() == '/')						//If item is folder (ends with '/') request content of folder
	{
		fileTreeSetup(currentFolder+item->text().toStdString());
	}
	else if(item==ui->listWidget->item(0))								//If item is '..' (first item) request content of parrentfolder
	{
		fileTreeSetup(currentFolder+"../");
	}
}

