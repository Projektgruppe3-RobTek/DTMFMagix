#ifndef DTMFMAGIX_H
#define DTMFMAGIX_H

#include <QFileDialog>
#include <QMainWindow>
#include <thread>
#include <unistd.h>
#include "mythread.h"
#include "AppLayer.h"

namespace Ui {
class DTMFMagix;
}

class DTMFMagix : public QMainWindow
{
    Q_OBJECT

public:
    explicit DTMFMagix(QWidget *parent = 0);
    ~DTMFMagix();
    MyThread *mThread;
    void setDone();

public slots:
    void onNumberChanged(int, int);

private slots:

    void on_browseButton_clicked();

    void on_sendButton_clicked();

    void on_requestButton_clicked();

private:
    QStringList filePath;
    Ui::DTMFMagix *ui;
    AppLayer *appLayer;
    std::thread sendThread;
    bool sending;
};
void sendFileWrapper(AppLayer * appLayer,string,string,DTMFMagix *magix);

#endif // DTMFMAGIX_H
