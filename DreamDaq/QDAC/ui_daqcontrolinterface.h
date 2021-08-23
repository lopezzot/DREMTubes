/********************************************************************************
** Form generated from reading UI file 'daqcontrolinterface.ui'
**
** Created: Wed Oct 21 19:43:36 2015
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DAQCONTROLINTERFACE_H
#define UI_DAQCONTROLINTERFACE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DaqControlInterface
{
public:
    QAction *actionAbout;
    QAction *actionStart;
    QAction *actionClean;
    QAction *actionViewLog;
    QAction *actionPreferences;
    QAction *actionExit;
    QAction *actionHelp;
    QWidget *centralWidget;
    QLabel *label;
    QLineEdit *runNumber;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QCheckBox *checkDrs;
    QCheckBox *checkScope;
    QLineEdit *maxEvents;
    QLineEdit *energy;
    QLineEdit *detectors;
    QComboBox *comboBox;
    QWidget *widget;
    QProgressBar *progressBar;
    QLabel *nPedestals;
    QLabel *label_8;
    QLabel *nPhys;
    QLabel *label_10;
    QLabel *label_2;
    QLabel *nSpill;
    QLabel *nEvents;
    QLabel *label_9;
    QLabel *label_17;
    QLabel *evRate;
    QLabel *label_16;
    QLabel *label_15;
    QLabel *runTm;
    QLabel *startTm;
    QFrame *line_2;
    QFrame *line_3;
    QFrame *line_4;
    QLabel *label_24;
    QLabel *label_25;
    QLabel *label_29;
    QFrame *line_5;
    QListWidget *listWidget;
    QListWidget *listWidget_2;
    QLabel *label_18;
    QLabel *label_19;
    QListWidget *listWidget_3;
    QLabel *label_23;
    QLabel *label_11;
    QLineEdit *phy2ped;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLineEdit *editX;
    QLineEdit *editY;
    QLineEdit *editAngle;
    QFrame *line;
    QLineEdit *beam_file;
    QLabel *label_20;
    QCheckBox *checkFiberM;
    QCheckBox *checkLeakD;
    QCheckBox *checkMatrix;
    QCheckBox *checkVeto;
    QCheckBox *checkPreshow;
    QCheckBox *checkTestRun;
    QLabel *label_21;
    QLabel *label_22;
    QCheckBox *T1_and_T2;
    QCheckBox *T1_and_T2_and_T3;
    QStatusBar *statusBar;
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QMenu *menuHelp;
    QMenu *menuHelp_2;
    QButtonGroup *buttonGroup;

    void setupUi(QMainWindow *DaqControlInterface)
    {
        if (DaqControlInterface->objectName().isEmpty())
            DaqControlInterface->setObjectName(QString::fromUtf8("DaqControlInterface"));
        DaqControlInterface->resize(781, 740);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DaqControlInterface->sizePolicy().hasHeightForWidth());
        DaqControlInterface->setSizePolicy(sizePolicy);
        DaqControlInterface->setMinimumSize(QSize(0, 30));
        DaqControlInterface->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        actionAbout = new QAction(DaqControlInterface);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionStart = new QAction(DaqControlInterface);
        actionStart->setObjectName(QString::fromUtf8("actionStart"));
        actionClean = new QAction(DaqControlInterface);
        actionClean->setObjectName(QString::fromUtf8("actionClean"));
        actionViewLog = new QAction(DaqControlInterface);
        actionViewLog->setObjectName(QString::fromUtf8("actionViewLog"));
        actionPreferences = new QAction(DaqControlInterface);
        actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
        actionExit = new QAction(DaqControlInterface);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionHelp = new QAction(DaqControlInterface);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        centralWidget = new QWidget(DaqControlInterface);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 14, 91, 20));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        runNumber = new QLineEdit(centralWidget);
        runNumber->setObjectName(QString::fromUtf8("runNumber"));
        runNumber->setGeometry(QRect(160, 10, 161, 22));
        runNumber->setReadOnly(true);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 58, 121, 16));
        label_3->setFont(font);
        label_3->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 90, 121, 16));
        label_4->setFont(font);
        label_4->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 120, 121, 16));
        label_5->setFont(font);
        label_5->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 500, 321, 16));
        label_6->setFont(font);
        label_6->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 251, 121, 16));
        label_7->setFont(font);
        label_7->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        checkDrs = new QCheckBox(centralWidget);
        checkDrs->setObjectName(QString::fromUtf8("checkDrs"));
        checkDrs->setGeometry(QRect(10, 270, 141, 20));
        checkScope = new QCheckBox(centralWidget);
        checkScope->setObjectName(QString::fromUtf8("checkScope"));
        checkScope->setGeometry(QRect(10, 290, 141, 20));
        maxEvents = new QLineEdit(centralWidget);
        maxEvents->setObjectName(QString::fromUtf8("maxEvents"));
        maxEvents->setGeometry(QRect(160, 54, 161, 22));
        maxEvents->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        maxEvents->setReadOnly(false);
        energy = new QLineEdit(centralWidget);
        energy->setObjectName(QString::fromUtf8("energy"));
        energy->setGeometry(QRect(160, 116, 161, 22));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(energy->sizePolicy().hasHeightForWidth());
        energy->setSizePolicy(sizePolicy1);
        energy->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        energy->setReadOnly(false);
        detectors = new QLineEdit(centralWidget);
        detectors->setObjectName(QString::fromUtf8("detectors"));
        detectors->setGeometry(QRect(9, 520, 311, 22));
        detectors->setStyleSheet(QString::fromUtf8(""));
        detectors->setReadOnly(false);
        comboBox = new QComboBox(centralWidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(160, 84, 161, 24));
        comboBox->setAutoFillBackground(false);
        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(330, 0, 451, 551));
        widget->setStyleSheet(QString::fromUtf8("background-color: rgb(252, 255, 178);"));
        progressBar = new QProgressBar(widget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(10, 518, 431, 23));
        progressBar->setValue(0);
        nPedestals = new QLabel(widget);
        nPedestals->setObjectName(QString::fromUtf8("nPedestals"));
        nPedestals->setGeometry(QRect(140, 139, 301, 20));
        nPedestals->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_8 = new QLabel(widget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 99, 101, 16));
        nPhys = new QLabel(widget);
        nPhys->setObjectName(QString::fromUtf8("nPhys"));
        nPhys->setGeometry(QRect(140, 119, 301, 20));
        nPhys->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_10 = new QLabel(widget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(10, 139, 101, 17));
        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 79, 101, 16));
        nSpill = new QLabel(widget);
        nSpill->setObjectName(QString::fromUtf8("nSpill"));
        nSpill->setGeometry(QRect(140, 79, 301, 20));
        nSpill->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        nEvents = new QLabel(widget);
        nEvents->setObjectName(QString::fromUtf8("nEvents"));
        nEvents->setGeometry(QRect(140, 100, 301, 20));
        nEvents->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_9 = new QLabel(widget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 120, 101, 16));
        label_17 = new QLabel(widget);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(10, 59, 101, 16));
        evRate = new QLabel(widget);
        evRate->setObjectName(QString::fromUtf8("evRate"));
        evRate->setGeometry(QRect(140, 59, 301, 20));
        evRate->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_16 = new QLabel(widget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(10, 30, 101, 16));
        label_15 = new QLabel(widget);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(10, 11, 101, 16));
        runTm = new QLabel(widget);
        runTm->setObjectName(QString::fromUtf8("runTm"));
        runTm->setGeometry(QRect(110, 30, 201, 20));
        runTm->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        startTm = new QLabel(widget);
        startTm->setObjectName(QString::fromUtf8("startTm"));
        startTm->setGeometry(QRect(110, 10, 231, 20));
        startTm->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        line_2 = new QFrame(widget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(0, 160, 451, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(widget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(0, 52, 451, 4));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(widget);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(0, 410, 451, 16));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        label_24 = new QLabel(widget);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setGeometry(QRect(10, 451, 431, 23));
        label_25 = new QLabel(widget);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setGeometry(QRect(10, 475, 431, 23));
        label_29 = new QLabel(widget);
        label_29->setObjectName(QString::fromUtf8("label_29"));
        label_29->setGeometry(QRect(10, 425, 111, 17));
        label_29->setFont(font);
        label_29->setStyleSheet(QString::fromUtf8("color: rgb(86, 86, 86);"));
        line_5 = new QFrame(widget);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(0, 498, 451, 16));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);
        listWidget = new QListWidget(widget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(10, 180, 431, 71));
        listWidget_2 = new QListWidget(widget);
        listWidget_2->setObjectName(QString::fromUtf8("listWidget_2"));
        listWidget_2->setGeometry(QRect(10, 260, 431, 71));
        label_18 = new QLabel(widget);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(310, 180, 131, 17));
        label_18->setFont(font);
        label_19 = new QLabel(widget);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(310, 260, 131, 17));
        label_19->setFont(font);
        listWidget_3 = new QListWidget(widget);
        listWidget_3->setObjectName(QString::fromUtf8("listWidget_3"));
        listWidget_3->setGeometry(QRect(10, 340, 431, 71));
        label_23 = new QLabel(widget);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setGeometry(QRect(310, 340, 131, 17));
        label_23->setFont(font);
        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(170, 251, 151, 16));
        label_11->setFont(font);
        label_11->setLayoutDirection(Qt::RightToLeft);
        label_11->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_11->setAlignment(Qt::AlignCenter);
        phy2ped = new QLineEdit(centralWidget);
        phy2ped->setObjectName(QString::fromUtf8("phy2ped"));
        phy2ped->setGeometry(QRect(170, 270, 151, 22));
        sizePolicy1.setHeightForWidth(phy2ped->sizePolicy().hasHeightForWidth());
        phy2ped->setSizePolicy(sizePolicy1);
        phy2ped->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        phy2ped->setReadOnly(true);
        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(0, 191, 91, 17));
        label_12->setFont(font);
        label_12->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_12->setAlignment(Qt::AlignCenter);
        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(110, 191, 101, 20));
        label_13->setFont(font);
        label_13->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_13->setAlignment(Qt::AlignCenter);
        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(220, 191, 101, 20));
        label_14->setFont(font);
        label_14->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_14->setAlignment(Qt::AlignCenter);
        editX = new QLineEdit(centralWidget);
        editX->setObjectName(QString::fromUtf8("editX"));
        editX->setGeometry(QRect(10, 211, 91, 27));
        editY = new QLineEdit(centralWidget);
        editY->setObjectName(QString::fromUtf8("editY"));
        editY->setGeometry(QRect(110, 211, 101, 27));
        editAngle = new QLineEdit(centralWidget);
        editAngle->setObjectName(QString::fromUtf8("editAngle"));
        editAngle->setGeometry(QRect(220, 211, 101, 27));
        line = new QFrame(centralWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(321, 0, 20, 551));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        beam_file = new QLineEdit(centralWidget);
        beam_file->setObjectName(QString::fromUtf8("beam_file"));
        beam_file->setGeometry(QRect(160, 150, 161, 22));
        sizePolicy1.setHeightForWidth(beam_file->sizePolicy().hasHeightForWidth());
        beam_file->setSizePolicy(sizePolicy1);
        beam_file->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        beam_file->setReadOnly(false);
        label_20 = new QLabel(centralWidget);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setGeometry(QRect(10, 154, 121, 16));
        label_20->setFont(font);
        label_20->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        checkFiberM = new QCheckBox(centralWidget);
        checkFiberM->setObjectName(QString::fromUtf8("checkFiberM"));
        checkFiberM->setGeometry(QRect(10, 390, 141, 20));
        checkLeakD = new QCheckBox(centralWidget);
        checkLeakD->setObjectName(QString::fromUtf8("checkLeakD"));
        checkLeakD->setGeometry(QRect(10, 410, 141, 20));
        checkMatrix = new QCheckBox(centralWidget);
        checkMatrix->setObjectName(QString::fromUtf8("checkMatrix"));
        checkMatrix->setGeometry(QRect(10, 430, 141, 20));
        checkVeto = new QCheckBox(centralWidget);
        checkVeto->setObjectName(QString::fromUtf8("checkVeto"));
        checkVeto->setGeometry(QRect(10, 450, 141, 20));
        checkPreshow = new QCheckBox(centralWidget);
        checkPreshow->setObjectName(QString::fromUtf8("checkPreshow"));
        checkPreshow->setGeometry(QRect(10, 470, 141, 20));
        checkTestRun = new QCheckBox(centralWidget);
        checkTestRun->setObjectName(QString::fromUtf8("checkTestRun"));
        checkTestRun->setGeometry(QRect(210, 370, 111, 20));
        label_21 = new QLabel(centralWidget);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setGeometry(QRect(10, 370, 181, 16));
        label_21->setFont(font);
        label_21->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        label_22 = new QLabel(centralWidget);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setGeometry(QRect(10, 320, 181, 16));
        label_22->setFont(font);
        label_22->setStyleSheet(QString::fromUtf8("color: rgb(81, 81, 81);"));
        T1_and_T2 = new QCheckBox(centralWidget);
        buttonGroup = new QButtonGroup(DaqControlInterface);
        buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
        buttonGroup->addButton(T1_and_T2);
        T1_and_T2->setObjectName(QString::fromUtf8("T1_and_T2"));
        T1_and_T2->setGeometry(QRect(10, 340, 81, 20));
        T1_and_T2_and_T3 = new QCheckBox(centralWidget);
        buttonGroup->addButton(T1_and_T2_and_T3);
        T1_and_T2_and_T3->setObjectName(QString::fromUtf8("T1_and_T2_and_T3"));
        T1_and_T2_and_T3->setGeometry(QRect(140, 340, 171, 20));
        DaqControlInterface->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(DaqControlInterface);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        statusBar->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 80	, 80);"));
        DaqControlInterface->setStatusBar(statusBar);
        toolBar = new QToolBar(DaqControlInterface);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMinimumSize(QSize(0, 30));
        toolBar->setAutoFillBackground(true);
        toolBar->setStyleSheet(QString::fromUtf8(""));
        DaqControlInterface->addToolBar(Qt::TopToolBarArea, toolBar);
        menuBar = new QMenuBar(DaqControlInterface);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 781, 23));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuHelp_2 = new QMenu(menuBar);
        menuHelp_2->setObjectName(QString::fromUtf8("menuHelp_2"));
        DaqControlInterface->setMenuBar(menuBar);

        menuBar->addAction(menuHelp->menuAction());
        menuBar->addAction(menuHelp_2->menuAction());
        menuHelp->addAction(actionStart);
        menuHelp->addAction(actionClean);
        menuHelp->addSeparator();
        menuHelp->addAction(actionViewLog);
        menuHelp->addAction(actionPreferences);
        menuHelp->addSeparator();
        menuHelp->addAction(actionExit);
        menuHelp_2->addAction(actionAbout);

        retranslateUi(DaqControlInterface);

        QMetaObject::connectSlotsByName(DaqControlInterface);
    } // setupUi

    void retranslateUi(QMainWindow *DaqControlInterface)
    {
        DaqControlInterface->setWindowTitle(QApplication::translate("DaqControlInterface", "DAQ Control Interface", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("DaqControlInterface", "About", 0, QApplication::UnicodeUTF8));
        actionAbout->setShortcut(QApplication::translate("DaqControlInterface", "F2", 0, QApplication::UnicodeUTF8));
        actionStart->setText(QApplication::translate("DaqControlInterface", "Start", 0, QApplication::UnicodeUTF8));
        actionStart->setShortcut(QApplication::translate("DaqControlInterface", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionClean->setText(QApplication::translate("DaqControlInterface", "CleanUp", 0, QApplication::UnicodeUTF8));
        actionClean->setShortcut(QApplication::translate("DaqControlInterface", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        actionViewLog->setText(QApplication::translate("DaqControlInterface", "View Log", 0, QApplication::UnicodeUTF8));
        actionViewLog->setShortcut(QApplication::translate("DaqControlInterface", "Ctrl+L", 0, QApplication::UnicodeUTF8));
        actionPreferences->setText(QApplication::translate("DaqControlInterface", "Preferences", 0, QApplication::UnicodeUTF8));
        actionPreferences->setShortcut(QApplication::translate("DaqControlInterface", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("DaqControlInterface", "Exit", 0, QApplication::UnicodeUTF8));
        actionExit->setShortcut(QApplication::translate("DaqControlInterface", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionHelp->setText(QApplication::translate("DaqControlInterface", "Help", 0, QApplication::UnicodeUTF8));
        actionHelp->setShortcut(QApplication::translate("DaqControlInterface", "F1", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DaqControlInterface", "Run Number", 0, QApplication::UnicodeUTF8));
        runNumber->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DaqControlInterface", "# of Beam Events", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DaqControlInterface", "Beam", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DaqControlInterface", "Energy (GeV)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DaqControlInterface", "Anything else you may want to log", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("DaqControlInterface", "Readout Options", 0, QApplication::UnicodeUTF8));
        checkDrs->setText(QApplication::translate("DaqControlInterface", "Drs", 0, QApplication::UnicodeUTF8));
        checkScope->setText(QApplication::translate("DaqControlInterface", "Scope", 0, QApplication::UnicodeUTF8));
        maxEvents->setText(QApplication::translate("DaqControlInterface", "100000", 0, QApplication::UnicodeUTF8));
        energy->setText(QApplication::translate("DaqControlInterface", "+500", 0, QApplication::UnicodeUTF8));
        detectors->setText(QString());
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("DaqControlInterface", "Electrons", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DaqControlInterface", "Muons", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DaqControlInterface", "Pions", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DaqControlInterface", "Kaons", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DaqControlInterface", "Protons", 0, QApplication::UnicodeUTF8)
        );
        nPedestals->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("DaqControlInterface", "Total Events:", 0, QApplication::UnicodeUTF8));
        nPhys->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("DaqControlInterface", "Ped. Events:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DaqControlInterface", "Spill number:", 0, QApplication::UnicodeUTF8));
        nSpill->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        nEvents->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("DaqControlInterface", "Beam Events:", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("DaqControlInterface", "Beam Rate:", 0, QApplication::UnicodeUTF8));
        evRate->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("DaqControlInterface", "Run Time:", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("DaqControlInterface", "Start Time:", 0, QApplication::UnicodeUTF8));
        runTm->setText(QApplication::translate("DaqControlInterface", "run_time", 0, QApplication::UnicodeUTF8));
        startTm->setText(QApplication::translate("DaqControlInterface", "start_time", 0, QApplication::UnicodeUTF8));
        label_24->setText(QApplication::translate("DaqControlInterface", "/dev/hdb1  459G   61G  375G  14% /mnt/working", 0, QApplication::UnicodeUTF8));
        label_25->setText(QApplication::translate("DaqControlInterface", "/dev/sda1   1.8T  960G  781G  56% /mnt/wd", 0, QApplication::UnicodeUTF8));
        label_29->setText(QApplication::translate("DaqControlInterface", "Disk usage", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("DaqControlInterface", "Pedestal files", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("DaqControlInterface", "Beam files", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("DaqControlInterface", "Histogram files", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("DaqControlInterface", "Phy to Ped ratio", 0, QApplication::UnicodeUTF8));
        phy2ped->setText(QApplication::translate("DaqControlInterface", "10.00", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("DaqControlInterface", "X", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("DaqControlInterface", "Y", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("DaqControlInterface", "Angle", 0, QApplication::UnicodeUTF8));
        editX->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        editY->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        editAngle->setText(QApplication::translate("DaqControlInterface", "0", 0, QApplication::UnicodeUTF8));
        beam_file->setText(QApplication::translate("DaqControlInterface", "H372.34", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("DaqControlInterface", "Beam File", 0, QApplication::UnicodeUTF8));
        checkFiberM->setText(QApplication::translate("DaqControlInterface", "Fiber Module", 0, QApplication::UnicodeUTF8));
        checkLeakD->setText(QApplication::translate("DaqControlInterface", "Leakage Detectors", 0, QApplication::UnicodeUTF8));
        checkMatrix->setText(QApplication::translate("DaqControlInterface", "Crystal Matrix", 0, QApplication::UnicodeUTF8));
        checkVeto->setText(QApplication::translate("DaqControlInterface", "Veto", 0, QApplication::UnicodeUTF8));
        checkPreshow->setText(QApplication::translate("DaqControlInterface", "Preshower", 0, QApplication::UnicodeUTF8));
        checkTestRun->setText(QApplication::translate("DaqControlInterface", "Test Run", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("DaqControlInterface", "Detectors", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("DaqControlInterface", "Trigger", 0, QApplication::UnicodeUTF8));
        T1_and_T2->setText(QApplication::translate("DaqControlInterface", "T1 * T2", 0, QApplication::UnicodeUTF8));
        T1_and_T2_and_T3->setText(QApplication::translate("DaqControlInterface", "T1 * T2 * T3", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("DaqControlInterface", "toolBar", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("DaqControlInterface", "File", 0, QApplication::UnicodeUTF8));
        menuHelp_2->setTitle(QApplication::translate("DaqControlInterface", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DaqControlInterface: public Ui_DaqControlInterface {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAQCONTROLINTERFACE_H
