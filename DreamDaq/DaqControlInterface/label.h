#ifndef LABEL_H
#define LABEL_H

#include <QLabel>
#include<QMouseEvent>

class Label : public QLabel
{
    Q_OBJECT
public:
    explicit Label(QWidget *parent = 0);
    void setStrings(QString s1,QString s2);
    void switchText();
private:
    QString str1;
    QString str2;
    bool status;
protected:
    void mouseReleaseEvent(QMouseEvent *ev);

signals:
    void clicked();
public slots:

};

#endif // LABEL_H
