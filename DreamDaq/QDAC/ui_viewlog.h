/********************************************************************************
** Form generated from reading UI file 'viewlog.ui'
**
** Created: Wed Oct 21 19:43:36 2015
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWLOG_H
#define UI_VIEWLOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ViewLog
{
public:
    QGridLayout *gridLayout;
    QTextEdit *textEdit;
    QPushButton *pushButton;

    void setupUi(QDialog *ViewLog)
    {
        if (ViewLog->objectName().isEmpty())
            ViewLog->setObjectName(QString::fromUtf8("ViewLog"));
        ViewLog->resize(614, 460);
        gridLayout = new QGridLayout(ViewLog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        textEdit = new QTextEdit(ViewLog);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        textEdit->setReadOnly(true);

        gridLayout->addWidget(textEdit, 0, 0, 1, 1);

        pushButton = new QPushButton(ViewLog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMaximumSize(QSize(100, 16777215));

        gridLayout->addWidget(pushButton, 1, 0, 1, 1);


        retranslateUi(ViewLog);
        QObject::connect(pushButton, SIGNAL(clicked()), ViewLog, SLOT(hide()));

        QMetaObject::connectSlotsByName(ViewLog);
    } // setupUi

    void retranslateUi(QDialog *ViewLog)
    {
        ViewLog->setWindowTitle(QApplication::translate("ViewLog", "Dialog", 0, QApplication::UnicodeUTF8));
        textEdit->setHtml(QApplication::translate("ViewLog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">No Log File found!!</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("ViewLog", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ViewLog: public Ui_ViewLog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWLOG_H
