#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

namespace Ui {
    class PreferenceDialog;
}

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = 0);
    ~PreferenceDialog();
    void set(QString dir, QString log, QString input, QString wrk="/mnt/wrk", QString bkup="/mnt/bkup");

private:
    QString directory,dreamlogfile,guiinputfile, wrkdisk,bkupdisk;
    Ui::PreferenceDialog *ui;
signals:
    void dir(QString a);
    void log(QString a);
    void input(QString a);
    void work(QString a);
    void backup(QString a);
private slots:
    void ok();
    void cancel();
};

#endif // PREFERENCEDIALOG_H
