/********************************************************************************
** Form generated from reading UI file 'preferencedialog.ui'
**
** Created: Wed Oct 21 19:43:36 2015
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCEDIALOG_H
#define UI_PREFERENCEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_PreferenceDialog
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QLineEdit *workdirEdit;
    QLineEdit *outfileEdit;
    QLineEdit *infileEdit;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *wrkdiskEdit;
    QLineEdit *bkupdiskEdit;
    QLabel *label_4;
    QLabel *label_5;

    void setupUi(QDialog *PreferenceDialog)
    {
        if (PreferenceDialog->objectName().isEmpty())
            PreferenceDialog->setObjectName(QString::fromUtf8("PreferenceDialog"));
        PreferenceDialog->resize(348, 219);
        pushButton = new QPushButton(PreferenceDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(240, 180, 101, 27));
        pushButton_2 = new QPushButton(PreferenceDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(130, 180, 101, 27));
        workdirEdit = new QLineEdit(PreferenceDialog);
        workdirEdit->setObjectName(QString::fromUtf8("workdirEdit"));
        workdirEdit->setGeometry(QRect(140, 10, 201, 27));
        outfileEdit = new QLineEdit(PreferenceDialog);
        outfileEdit->setObjectName(QString::fromUtf8("outfileEdit"));
        outfileEdit->setGeometry(QRect(140, 40, 201, 27));
        infileEdit = new QLineEdit(PreferenceDialog);
        infileEdit->setObjectName(QString::fromUtf8("infileEdit"));
        infileEdit->setGeometry(QRect(140, 70, 201, 27));
        label = new QLabel(PreferenceDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 14, 131, 17));
        label_2 = new QLabel(PreferenceDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 45, 131, 17));
        label_3 = new QLabel(PreferenceDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 76, 131, 17));
        wrkdiskEdit = new QLineEdit(PreferenceDialog);
        wrkdiskEdit->setObjectName(QString::fromUtf8("wrkdiskEdit"));
        wrkdiskEdit->setGeometry(QRect(140, 100, 201, 27));
        bkupdiskEdit = new QLineEdit(PreferenceDialog);
        bkupdiskEdit->setObjectName(QString::fromUtf8("bkupdiskEdit"));
        bkupdiskEdit->setGeometry(QRect(140, 130, 201, 27));
        label_4 = new QLabel(PreferenceDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 108, 131, 17));
        label_5 = new QLabel(PreferenceDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 138, 131, 17));

        retranslateUi(PreferenceDialog);

        QMetaObject::connectSlotsByName(PreferenceDialog);
    } // setupUi

    void retranslateUi(QDialog *PreferenceDialog)
    {
        PreferenceDialog->setWindowTitle(QApplication::translate("PreferenceDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("PreferenceDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("PreferenceDialog", "Ok", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PreferenceDialog", "Working directory", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PreferenceDialog", "Dream log file", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PreferenceDialog", "GUI input file", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("PreferenceDialog", "Working Disk", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("PreferenceDialog", "Backup Disk", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PreferenceDialog: public Ui_PreferenceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCEDIALOG_H
