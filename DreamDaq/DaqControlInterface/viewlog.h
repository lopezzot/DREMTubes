#ifndef VIEWLOG_H
#define VIEWLOG_H

#include <QDialog>

namespace Ui {
    class ViewLog;
}

class ViewLog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewLog(QString path,QWidget *parent = 0);
    ~ViewLog();

private:
    QString logPath;
    void readLog();
    Ui::ViewLog *ui;
public slots:
    void updateLog();
    void changePath(QString path);
};

#endif // VIEWLOG_H
