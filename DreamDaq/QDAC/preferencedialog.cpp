#include "preferencedialog.h"
#include "ui_preferencedialog.h"
#include <QDebug>

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(cancel()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(ok()));
}

void PreferenceDialog::set(QString dir, QString log, QString input, QString wrk, QString bkup){
    directory=dir;
    ui->workdirEdit->setText(dir);
    dreamlogfile=log;
    ui->outfileEdit->setText(log);
    guiinputfile=input;
    ui->infileEdit->setText(input);
    wrkdisk=wrk;
    ui->wrkdiskEdit->setText(wrk);
    bkupdisk=bkup;
    ui->bkupdiskEdit->setText(bkup);
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}
void PreferenceDialog::ok(){
    directory=ui->workdirEdit->text();
    dreamlogfile=ui->outfileEdit->text();
    guiinputfile=ui->infileEdit->text();
    wrkdisk=ui->wrkdiskEdit->text();
    bkupdisk=ui->bkupdiskEdit->text();
    emit dir(directory);
    emit log(dreamlogfile);
    emit input(guiinputfile);
    emit work(wrkdisk);
    emit backup(bkupdisk);
    close();
}

void PreferenceDialog::cancel(){
    ui->workdirEdit->setText(directory);
    ui->outfileEdit->setText(dreamlogfile);
    ui->infileEdit->setText(guiinputfile);
    ui->bkupdiskEdit->setText(bkupdisk);
    ui->wrkdiskEdit->setText(wrkdisk);
    close();
}
