#include "viewlog.h"
#include "ui_viewlog.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

ViewLog::ViewLog(QString path,QWidget *parent) :
    QDialog(parent), logPath(path),
    ui(new Ui::ViewLog)
{
    ui->setupUi(this);
    readLog();

}

void ViewLog::readLog(){
    QFile file(logPath);
    QString nofile="<b><p>log file not found:</p><p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+logPath+"</p></b>";
    ui->textEdit->setText(nofile);
    if (file.open(QFile::ReadOnly)){
        QTextStream stream(&file);
        ui->textEdit->setText(stream.readAll());
        file.close();
    }
    this->hide();
}

void ViewLog::changePath(QString path)
{
    logPath.clear();
    logPath=path;
    readLog();
}

void ViewLog::updateLog(){
    readLog();
}

ViewLog::~ViewLog()
{
    delete ui;
}
