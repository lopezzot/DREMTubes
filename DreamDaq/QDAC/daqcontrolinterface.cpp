#include "daqcontrolinterface.h"
#include "ui_daqcontrolinterface.h"

#include <stdint.h>
#include <cstdio>

#include <QTimer>
#include <qmath.h>

#include <QDir>

#include <QDate>
#include <QTime>
#include <QTest>
#include <QProcess>
#include <QFontMetrics>
#include <QStringList>

#include <QDebug>

void DaqControlInterface::firstRun(){
    QDir dir(QDir::homePath()+"/working/");
    if (dir.exists(".dreamdaqrc")) return;

    PreferenceDialog * dialog = new PreferenceDialog();
    dialog->set(QDir::homePath(),"INPUT","OUTPUT");
    dialog->show();
    connect(dialog,SIGNAL(dir(QString)),this,SLOT(chgDir(QString)));
    connect(dialog,SIGNAL(log(QString)),this,SLOT(chgLog(QString)));
    connect(dialog,SIGNAL(input(QString)),this,SLOT(chgInput(QString)));
    readValues();
}

DaqControlInterface::DaqControlInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DaqControlInterface)
{

    isReading=false;
    hasEvent=false;
    isStarted=false;
    isStopping =false;
    runTime=0;
    runNumber=0;

    ui->setupUi(this);
    this->setFixedSize(NW,NH);

    //EDIT LABEL
    QFont f;
    f.setPointSize(8);
    f.setUnderline(true);
    l1 = new Label(ui->centralWidget);
//    l1->setGeometry(280,31,31,16);
    l1->setGeometry(280,31,31,16);
    l1->setFont(f);
    l1->setStyleSheet("color: rgb(130,130,130);");
    l1->setStrings("Edit","Done");
    l1->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    l1->show();

    l2 = new Label(ui->centralWidget);
//    l2->setGeometry(280,291,31,16);
    l2->setGeometry(280,291,31,16);
    l2->setFont(f);
    l2->setStyleSheet("color: rgb(130,130,130);");
    l2->setStrings("Edit","Done");
    l2->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    l2->show();

    ui->toolBar->setMovable(false);

    //Time Label
    timeLabel= new QLabel(ui->statusBar);
    timeUpdate();
    ui->statusBar->addWidget(timeLabel);
    //Status Label;
    statusLabel=new QLabel("DAQ Stopped",ui->statusBar);
    statusLabel->setAlignment(Qt::AlignRight);
    ui->statusBar->addWidget(statusLabel,2);

    t1=new QTimer();
    t1->start(100);

    startStopButton=new QPushButton("Start",ui->toolBar);
    startStopButton->addAction(ui->actionStart);
    startStopButton->setStyleSheet("color : green;");
    startStopButton->setFont(QFont(startStopButton->font().family(),startStopButton->font().pointSize()+1,QFont::Bold));
    ui->toolBar->addWidget(startStopButton);
    ui->toolBar->addAction(ui->actionClean);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionPreferences);

    //CONNECTIONS
    connect(l1,SIGNAL(clicked()),this,SLOT(editNumber()));
    connect(l2,SIGNAL(clicked()),this,SLOT(editNumber()));
    connect(ui->runNumber,SIGNAL(textEdited(QString)),this, SLOT(checkRunNr(QString)));
    connect(ui->maxEvents,SIGNAL(textEdited(QString)),this, SLOT(checkEvtNr(QString)));
    connect(ui->energy,SIGNAL(textEdited(QString)),this, SLOT(checkEnergy(QString)));
    connect(ui->phy2ped,SIGNAL(textEdited(QString)),this,SLOT(checkPhy2Ped(QString)));
    connect(ui->editX,SIGNAL(textEdited(QString)),this,SLOT(checkXY(QString)));
    connect(ui->editY,SIGNAL(textEdited(QString)),this,SLOT(checkXY(QString)));
    connect(ui->editAngle,SIGNAL(textEdited(QString)),this,SLOT(checkAngle(QString)));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changed()));
    connect(ui->checkDrs,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->checkScope,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->T1_and_T2,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->T1_and_T2_and_T3,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->checkFiberM,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->checkLeakD,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->checkMatrix,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->checkVeto,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->checkPreshow,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->checkTestRun,SIGNAL(stateChanged(int)),this,SLOT(changed()));
    connect(ui->detectors,SIGNAL(textEdited(QString)),this,SLOT(changed()));
    connect(ui->actionStart,SIGNAL(triggered()),this,SLOT(startStopSlot()),Qt::DirectConnection);
    connect(startStopButton,SIGNAL(clicked()),this,SLOT(startStopSlot()),Qt::DirectConnection);
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->actionClean,SIGNAL(triggered()),this,SLOT(cleanUpped()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutSlot()));


    //UI SLOTS
    connect(t1,SIGNAL(timeout()),this,SLOT(timeUpdate()));

    //SHOW CONFIG
    firstRun();
    readConfigFile();
    dialog = new PreferenceDialog();
    dialog->set(workDir,dreamLogFile,guiInputFile,wrkDisk,bkupDisk);
    viewLog=new ViewLog(this->workDir+"/"+this->dreamLogFile);
    viewLog->hide();

    connect(ui->actionViewLog,SIGNAL(triggered()),viewLog,SLOT(show()));
    connect(ui->actionPreferences,SIGNAL(triggered()),dialog,SLOT(show()));
    connect(dialog,SIGNAL(dir(QString)),this,SLOT(chgDir(QString)));
    connect(dialog,SIGNAL(log(QString)),this,SLOT(chgLog(QString)));
    connect(dialog,SIGNAL(input(QString)),this,SLOT(chgInput(QString)));
    connect(dialog,SIGNAL(work(QString)),this,SLOT(chgWork(QString)));
    connect(dialog,SIGNAL(backup(QString)),this,SLOT(chgBackUp(QString)));
    showConfig();
    //CLOSE DIALOG
    em= new QMessageBox(this);
    em->setModal(true);
    this->isChanged=false;
}

DaqControlInterface::~DaqControlInterface()
{
    delete ui;
}

void DaqControlInterface::editNumber(){
    if (sender()==l1){
        ui->runNumber->setReadOnly(!ui->runNumber->isReadOnly());
        ui->runNumber->setFocus();
    }
    if (sender()==l2){
        ui->phy2ped->setReadOnly(!ui->phy2ped->isReadOnly());
        ui->phy2ped->setFocus();
    }
}

void DaqControlInterface::lockInterface(bool yes){
    l1->setEnabled(!yes);
    if (!l1->text().compare("Done")) l1->switchText();
    l2->setEnabled(!yes);
    if (!l2->text().compare("Done")) l2->switchText();
    ui->checkDrs->setEnabled(!yes);
    ui->checkScope->setEnabled(!yes);
    ui->T1_and_T2->setEnabled(!yes);
    ui->T1_and_T2_and_T3->setEnabled(!yes);
    // ui->checkFiberM->setEnabled(!yes);
    // ui->checkLeakD->setEnabled(!yes);
    // ui->checkMatrix->setEnabled(!yes);
    // ui->checkVeto->setEnabled(!yes);
    // ui->checkPreshow->setEnabled(!yes);
    // ui->checkTestRun->setEnabled(!yes);
    ui->runNumber->setEnabled(!yes);
    ui->maxEvents->setEnabled(!yes);
    ui->phy2ped->setEnabled(!yes);
    // ui->energy->setEnabled(!yes);
    // ui->beam_file->setEnabled(!yes);
    // ui->detectors->setEnabled(!yes);
    // ui->comboBox->setEnabled(!yes);
    // ui->editX->setEnabled(!yes);
    // ui->editY->setEnabled(!yes);
    // ui->editAngle->setEnabled(!yes);

    if (!yes){
                ui->actionStart->setText("Start");
                startStopButton->setText("Start");
                startStopButton->setStyleSheet("color: green;");
                ui->statusBar->setStyleSheet("background-color: rgb(255,80,80);");
                statusLabel->setText("DAQ Stopped");
                ui->runNumber->setReadOnly(true);
                ui->phy2ped->setReadOnly(true);
    } else if (yes){
        ui->actionStart->setText("Stop");
        startStopButton->setText("Stop");
        startStopButton->setStyleSheet("color: rgb(255,0,0);");
        ui->statusBar->setStyleSheet("background-color: rgb(0,255,0);");
        statusLabel->setText("DAQ Running");
    }
}

void DaqControlInterface::lockInfo(bool yes){
    ui->checkFiberM->setEnabled(!yes);
    ui->checkLeakD->setEnabled(!yes);
    ui->checkMatrix->setEnabled(!yes);
    ui->checkVeto->setEnabled(!yes);
    ui->checkPreshow->setEnabled(!yes);
    ui->checkTestRun->setEnabled(!yes);
    ui->energy->setEnabled(!yes);
    ui->beam_file->setEnabled(!yes);
    ui->detectors->setEnabled(!yes);
    ui->comboBox->setEnabled(!yes);
    ui->editX->setEnabled(!yes);
    ui->editY->setEnabled(!yes);
    ui->editAngle->setEnabled(!yes);
}

bool DaqControlInterface::checkValidity(){
    if (ui->runNumber->text().isEmpty()) return false;
    if (ui->maxEvents->text().isEmpty()) return false;
    if (ui->energy->text().isEmpty()) return false;
    return true;
}

quint32 DaqControlInterface::spillRate(){
    if (countRate.size()==0) return 0;
    if (countRate.count(0))
     {
      spillCounted -= countRate[0];
      countRate.erase(0);
     }
    if (countRate.size()==0) return 0;
    if (countRate.size()<3) return countRate.rbegin()->first;
    std::map<quint32, quint32>::reverse_iterator rit;
    // rit = countRate.rbegin();
    // spillCounted -= (*rit).second;
    // countRate.erase((*rit).first);
    quint32 goods(0), totevt(0);
    for (rit=countRate.rbegin();rit!=countRate.rend();rit++)
     {
      if (goods*2<spillCounted)
       {
        goods+=(*rit).second;
        totevt+=((*rit).second)*((*rit).first);
       }
     }
    return (goods) ? totevt/goods : 0;
}

void DaqControlInterface::saveLog(){
    QString logpath=dreamLogFile;

    logpath = workDir+"/"+logpath;
    QFile file(logpath);
    if (file.open(QFile::WriteOnly | QFile::Append)) {
        QTextStream fout(&file);
        if (file.size()==0) {
            QString help(HELPLOG);
            fout<<help<<"\n";
            QString helpsep(HELPSEP);
            fout<<helpsep<<"\n";
        }
        char cbuf[64];
        snprintf(cbuf, 24, "%7d", uint32_t(runNumber));
        fout<<cbuf<<"  |  ";
        fout<<QDate::currentDate().toString("yyyy MMM dd")<<" "<<QTime::currentTime().toString("hh:mm:ss")<<"  |  ";
        snprintf(cbuf, 24, "%6d", uint32_t(runTime));
        fout<<cbuf<<"  |  ";
        snprintf(cbuf, 24, "%6d", uint32_t(this->spillRate()));
        fout<<cbuf<<"  |  ";
        snprintf(cbuf, 24, "%9d", uint32_t(phyNumber));
        fout<<cbuf<<"  |  ";
        snprintf(cbuf, 24, "%8d", uint32_t(pedNumber));
        fout<<cbuf<<"  |  ";
        // std::string utf8_type = type.toUtf8().constData();
        // snprintf(cbuf, 24, "%9s", utf8_type.c_str());
        snprintf(cbuf, 24, "%9s", type.toStdString().c_str());
        fout<<cbuf<<"  |  ";
        snprintf(cbuf, 24, "%8d", int32_t(energy));
        fout<<cbuf<<"  |  ";
        snprintf(cbuf, 24, "%10s", beamfile.toStdString().c_str());
        fout<<cbuf<<"  |  ";
        snprintf(cbuf, 24, "%6.2f", double(X));
        fout<<cbuf<<"  |  ";
        snprintf(cbuf, 24, "%6.2f", double(Y));
        fout<<cbuf<<"  |  ";
        snprintf(cbuf, 24, "%6.2f", double(Angle));
        fout<<cbuf<<"  |  ";
        fout<<(testRun ? "T " : "P ");
        fout<<(drs ? "1" : "0");
        fout<<(oscilloscope ? "1" : "0");
        fout<<(t1t2 ? "1" : "0");
        fout<<(t1t2t3 ? "1" : "0");
        fout<<(fiberModule ? "1" : "0");
        fout<<(leakageDets ? "1" : "0");
        fout<<(crystalMatrix ? "1" : "0");
        fout<<(veto ? "1" : "0");
        fout<<(preshower ? "1" : "0");
        fout<<"  |  "<<detectors<<"\n";
        isChanged=false;
        file.close();
    }

}

void DaqControlInterface::readValues(){
    runNumber=ui->runNumber->text().toInt();
    eventsNumber=ui->maxEvents->text().toLong();
    type=ui->comboBox->currentText();
    energy=ui->energy->text().toLong();
    beamfile=ui->beam_file->text();
    X=ui->editX->text().toDouble();
    Y=ui->editY->text().toDouble();
    Angle=ui->editAngle->text().toDouble();
    detectors=ui->detectors->text();
    double p2p=ui->phy2ped->text().toDouble();
    if (p2p>0) phyToPed=p2p; else phyToPed=1;
    drs=ui->checkDrs->isChecked();
    oscilloscope=ui->checkScope->isChecked();
    t1t2=ui->T1_and_T2->isChecked();
    t1t2t3=ui->T1_and_T2_and_T3->isChecked();
    fiberModule=ui->checkFiberM->isChecked();
    leakageDets=ui->checkLeakD->isChecked();
    crystalMatrix=ui->checkMatrix->isChecked();
    veto=ui->checkVeto->isChecked();
    preshower=ui->checkPreshow->isChecked();
    testRun=ui->checkTestRun->isChecked();
    this->saveConfigFile();
}

bool DaqControlInterface::isANumber(char c){
    if (c>=48 && c<=57) return true;
    return false;
}

void DaqControlInterface::checkRunNr(QString t){
    if (t.length()==0) return;
    QLineEdit * line=dynamic_cast<QLineEdit*>(sender());
    if (line){
        bool ok=false;
        t.toUInt(&ok);
        if (ok){
            isChanged=true;
            line->setText(t);
        }
        else line->undo();
    }
}

void DaqControlInterface::checkEvtNr(QString t){
    if (t.length()==0) return;
    QLineEdit * line=dynamic_cast<QLineEdit*>(sender());
    if (line){
        bool ok=false;
        t.toUInt(&ok);
        if (ok){
            isChanged=true;
            line->setText(t);
        }
        else line->undo();
    }
}

void DaqControlInterface::checkEnergy(QString t){
    if (t.length()==0) return;
    if (t.length()==1 && (t[0]=='-' || t[0]=='+'))return;
    QLineEdit * line=dynamic_cast<QLineEdit*>(sender());
    if (line){
        bool ok=false;
        t.toLong(&ok);
        if (ok){
            isChanged=true;
            line->setText(t);
        }
        else line->undo();
    }
}

void DaqControlInterface::checkPhy2Ped(QString t){
    if (t.length()==0) return;
    QLineEdit * line=dynamic_cast<QLineEdit*>(sender());
    if (line){
        bool ok=false;
        double test = t.toDouble(&ok);
        if (ok && (test>=0)){
            isChanged=true;
            line->setText(t);
        }
        else line->undo();
    }
}

void DaqControlInterface::checkXY(QString t){
    if (t.length()==0) return;
    QLineEdit * line=dynamic_cast<QLineEdit*>(sender());
    if (line){
        bool ok=false;
        double test = t.toDouble(&ok);
        if (ok && (test>=0) && (test<=200)){
            isChanged=true;
            line->setText(t);
        }
        else line->undo();
    }
}

void DaqControlInterface::checkAngle(QString t){
    if (t.length()==0) return;
    if (t.length()==1 && (t[0]=='-' || t[0]=='+'))return;
    QLineEdit * line=dynamic_cast<QLineEdit*>(sender());
    if (line){
        bool ok=false;
        double test = t.toDouble(&ok);
        if (ok && (test>=-90) && (test<=90)){
            isChanged=true;
            line->setText(t);
        }
        else line->undo();
    }
}

void DaqControlInterface::started(){
    if (!checkValidity()) return;
    this->readValues();
    runNumber++;
    this->saveConfigFile();
    this->showConfig();
    isStarted=true;

    lockInterface();
    this->setFixedHeight(EH);
    this->setFixedWidth(EW);

    this->runTime=0;
    //EXEC SCRIPT AND LUNCH THE CONTROL THREAD
    t0=new QTimer();
    t0->start(TI/10);
    if (this->eventsNumber>0) {
        this->setFixedHeight(EH+40);
        ui->progressBar->setValue(0);
    }
    connect(t0,SIGNAL(timeout()),this,SLOT(eventUpdate()));

    saveConfigFile();
    eventInit();

    //INSERT HERE THE START FUNCTIONS
    QString rdo = (drs) ? "drs" : "vme";
    QString prgr = "dreamfunct.sh start"+QString(" %1 ").arg(runNumber)+rdo+QString(" %1 %2").arg(eventsNumber).arg(phyToPed);
    qWarning()<<prgr;

    QProcess qp;
    qp.execute(prgr);
    startTime = QDateTime::currentDateTime();
    qp.startDetached("dreamfunct.sh sync");
    lastSpillEvents = 0;
    spillCounted = 0;
    countRate.clear();
    openFile();
}

void DaqControlInterface::stopped(){
    //INSERT HERE THE STOP FUNCTIONS
    if (!isStarted) return;
    isStopping = true;
    isStarted=false;
    lockInfo(true);
    runTime = startTime.secsTo(QDateTime::currentDateTime());
    QString prgr="dreamfunct.sh stop";
    qWarning()<<prgr;

    QProcess qp;
    qp.execute(prgr);

    prgr="dreamfunct.sh log";
    qWarning()<<prgr;

    QTest::qSleep(500);
    if (isReading)
     {
      QString temp;
      do {
        temp=stream->readLine();
      } while (!temp.isNull() && temp.compare("DREAM RUN CLOSED")!=0);
      do {
        temp=stream->readLine();
      } while (!temp.isNull() && !temp.contains("Events taken -- Total"));
      if (!temp.isEmpty() && temp.contains("Events taken -- Total")){
        updatePanel(temp);
      }
      closeFile();
     }

    qp.startDetached(prgr);
    this->readValues(); // added on 5.11.11
    //SAVE THE LOG
    saveLog();
    t0->stop();
    delete t0;
    viewLog->updateLog();
    QTest::qSleep(7000);
    lockInterface(false);
    lockInfo(false);
    isStopping = false;
}

void DaqControlInterface::cleanUpped(){
    QMessageBox msgBox;
    msgBox.setText("You asked to stop and clean-up the DAQ processes.");
    msgBox.setInformativeText("Is this really what you want?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    QProcess qx;
    QString prgr="dreamfunct.sh clean";
    switch(ret){
    case QMessageBox::Yes :
            hasEvent=false;
            isReading=false;
            stopped();
            //CLEANUP
            //INSERT HERE THE FUNCTIONS TO CLEANUP
            qWarning()<<prgr;

            qx.execute(prgr);
            ui->actionStart->setText("Start");
        break;
    case QMessageBox::No :
            return;
        break;
    default:
            return;
        break;
    }
}

void DaqControlInterface::timeUpdate(){
    QString text=QDate::currentDate().toString("dd.MM.yyyy")+" "+QTime::currentTime().toString("hh:mm:ss");
    timeLabel->setText(text);
}

void shellcommand ( std::string cmd, QStringList & qslist)
 {
    qslist.clear();
    FILE* dfc=popen(cmd.c_str(), "r");
    char* str(NULL);
    size_t sz(0);
    ssize_t ssz;
    while ((ssz=getline(&str, &sz, dfc)) != -1)
     {
      if (str[ssz]=='\n') str[ssz]='\0';
      qslist.append(QString(str));
     }
    if (str) free(str);

    pclose(dfc);
 }
    
void DaqControlInterface::eventInit(){
    ui->startTm->setText("0000.00.00 00:00:00");
    ui->runTm->setText("00:00:00");
    ui->evRate->setText("0");
    ui->nSpill->setText("0");
    ui->nEvents->setText("0");
    ui->nPhys->setText("0");
    ui->nPedestals->setText("0");
    
    QString qcmd;
    QStringList qslist;
    QStringList pcoc;

    bool ok;
    uint oc;

    if (QDir(wrkDisk).exists())
     {
      qcmd="df -h "+wrkDisk+" | grep -v ^Filesystem | grep -v ^' '";
      shellcommand( qcmd.toStdString(), qslist );
      qcmd="df -h "+wrkDisk+" | grep -v ^Filesystem | grep '%' | awk '{print $4}' | tr '%' ' '";
      shellcommand( qcmd.toStdString(), pcoc );
      oc=pcoc[0].toUInt(&ok);
      QString ocstr;
      ocstr.setNum(oc);
      ocstr.append('%');
      QString qstr=qslist[0];
      qstr.remove('\n');
      qstr.append(' ');
      if (ok && (oc>90)) qstr+=" Storage disk almost full ";
      if (ok) qstr.append(ocstr);
      ui->label_24->setText(qstr);
     }
    else
     {
      ui->label_24->setText(wrkDisk+" doesn't exist");
     }

    if (QDir(bkupDisk).exists())
     {
      qcmd="df -h "+bkupDisk+" | grep -v ^Filesystem | grep -v ^' '";
      shellcommand( qcmd.toStdString(), qslist );
      qcmd="df -h "+bkupDisk+" | grep -v ^Filesystem | grep '%' | awk '{print $4}' | tr '%' ' '";
      shellcommand( qcmd.toStdString(), pcoc );
      oc=pcoc[0].toUInt(&ok);
      QString ocstr;
      ocstr.setNum(oc);
      ocstr.append('%');
      QString qstr=qslist[0];
      qstr.remove('\n');
      qstr.append(' ');
      if (ok && (oc>90)) qstr+=" Storage disk almost full ";
      if (ok) qstr.append(ocstr);
      ui->label_25->setText(qstr);
     }
    else
     {
      ui->label_25->setText(bkupDisk+" doesn't exist");
     }

}

void DaqControlInterface::eventUpdate(){
    QString temp;
    temp=readString();
    if (temp.compare("none")==0) return;
    updatePanel(temp);
}

void DaqControlInterface::updatePanel(QString qs){
    parseString(qs);
    QString text=startTime.toString("yyyy.MM.dd hh:mm:ss");
    ui->startTm->setText(text);
    runTime = startTime.secsTo(QDateTime::currentDateTime());
    QTime n(0,0,0);
    QTime t=n.addSecs(runTime);
    ui->runTm->setText(t.toString("hh:mm:ss"));
    qint64 xrate = phyNumber-lastSpillEvents;
    lastSpillEvents = phyNumber;
    countRate[xrate] ++;
    spillCounted ++;
    ui->evRate->setText(QString::number(xrate));
    ui->nSpill->setText(QString::number(spillNumber));
    ui->nEvents->setText(QString::number(realEventsNumber));
    ui->nPhys->setText(QString::number(phyNumber));
    ui->nPedestals->setText(QString::number(pedNumber));
    
    QString fname;
    QString finfo;
    QString qcmd;
    QStringList qslist;
    QStringList pcoc;

    QString dirname=wrkDisk+"/data";
    QString dataF="datafile_run"+QString("%1").arg(runNumber)+"_*.dat";

    qcmd="cd "+dirname+"; ls -lht "+dataF+" 2>/dev/null | colrm 1 26";
    shellcommand( qcmd.toStdString(), qslist );
    ui->listWidget_2->clear();
    ui->listWidget_2->addItems(qslist);

    dirname=wrkDisk+"/pedestal";
    QString pedF="pedestal_run"+QString("%1").arg(runNumber)+"_*.dat";

    qcmd="cd "+dirname+"; ls -lht "+pedF+" 2>/dev/null | colrm 1 26";
    shellcommand( qcmd.toStdString(), qslist );
    ui->listWidget->clear();
    ui->listWidget->addItems(qslist);

    dirname=wrkDisk+"/hbook";
    QString histF="datafile_histo_run"+QString("%1").arg(runNumber)+".root";

    qcmd="cd "+dirname+"; ls -lht "+histF+" 2>/dev/null | colrm 1 26";
    shellcommand( qcmd.toStdString(), qslist );
    ui->listWidget_3->clear();
    ui->listWidget_3->addItems(qslist);

    bool ok;
    uint oc;

    if (QDir(wrkDisk).exists())
     {
      qcmd="df -h "+wrkDisk+" | grep -v ^Filesystem | grep -v ^' '";
      shellcommand( qcmd.toStdString(), qslist );
      qcmd="df -h "+wrkDisk+" | grep -v ^Filesystem | grep '%' | awk '{print $4}' | tr '%' ' '";
      shellcommand( qcmd.toStdString(), pcoc );
      oc=pcoc[0].toUInt(&ok);
      QString ocstr;
      ocstr.setNum(oc);
      ocstr.append('%');
      QString qstr=qslist[0];
      qstr.remove('\n');
      qstr.append(' ');
      if (ok && (oc>90)) qstr+=" Storage disk almost full ";
      if (ok) qstr.append(ocstr);
      ui->label_24->setText(qstr);
     }
    else
     {
      ui->label_24->setText(wrkDisk+" doesn't exist");
     }

    if (QDir(bkupDisk).exists())
     {
      qcmd="df -h "+bkupDisk+" | grep -v ^Filesystem | grep -v ^' '";
      shellcommand( qcmd.toStdString(), qslist );
      qcmd="df -h "+bkupDisk+" | grep -v ^Filesystem | grep '%' | awk '{print $4}' | tr '%' ' '";
      shellcommand( qcmd.toStdString(), pcoc );
      oc=pcoc[0].toUInt(&ok);
      QString ocstr;
      ocstr.setNum(oc);
      ocstr.append('%');
      QString qstr=qslist[0];
      qstr.remove('\n');
      qstr.append(' ');
      if (ok && (oc>90)) qstr+=" Storage disk almost full ";
      if (ok) qstr.append(ocstr);
      ui->label_25->setText(qstr);
     }
    else
     {
      ui->label_25->setText(bkupDisk+" doesn't exist");
     }

}

void DaqControlInterface::parseString(QString str){
    QList <QString> toP=str.split(" ");
    if (toP.count()<16) return;
    this->spillNumber= toP[0].toLong();
    this->realEventsNumber=toP[7].toLong();
    this->phyNumber=toP[10].toLong();
    this->pedNumber=toP[15].toLong();
    if (this->eventsNumber>0){
        double value=double(phyNumber)*100.0/double(eventsNumber);
        uint32_t uval=uint32_t(value+0.5);
        if (uval>100) uval=100;
        ui->progressBar->setValue(uval);
    }
    toP.clear();
}

QString DaqControlInterface::readString(){
    if (hasEvent){
//        timeUpdate();
    }
    if (!isReading){
            hasEvent = false;
    }
    else if (!stream->atEnd()){
            QString temp;
            temp=stream->readLine();
            if (!temp.isEmpty() && temp.contains("Events taken -- Total")){
                hasEvent=true;
                t0->setInterval(TI);
                return temp;
            }
            else if (hasEvent && (temp.compare("DREAM RUN CLOSED")==0)) {
                // qWarning()<<temp;
                hasEvent=false;
                stopped();
                ui->actionStart->setText("Start");
            }
    }
    return "none";
}

void DaqControlInterface::openFile(){
    if (isReading) closeFile();
    QString readfile=guiInputFile;
    readfile=workDir+"/"+readfile;
    readingFile.setFileName(readfile);
    if (readingFile.open(QFile::ReadOnly)){
        stream= new QTextStream(&readingFile);
        isReading=true;
	// stream->readAll();
        QTest::qSleep(500);
        QString temp;
	do
         { temp=stream->readLine(); }
        while (!temp.contains("Event fifo is valid") && !temp.isNull()
            && temp.compare("DREAM RUN CLOSED")!=0);
        return;
    }
    else {
        qWarning()<<"ERROR: file '" << readfile << "' not found!";
        isStarted=false;
        lockInterface(false);
        t0->stop();
        delete t0;
    }
    return;
}

void DaqControlInterface::closeFile(){
    readingFile.close();
    delete stream;
    isReading=false;
}

void DaqControlInterface::readConfigFile(){
    this->workDir="/home/dreamtest/working";
    this->dreamLogFile="summary.txt";
    this->guiInputFile="logfile.txt";
    //this->wrkDisk="/mnt/working";
    this->wrkDisk="/home/dreamtest/storage";
    this->bkupDisk="/mnt/wd";
    this->runNumber=6000;
    this->eventsNumber=1000000;
    this->phyToPed=10;
    this->type="Electrons";
    this->energy=40;
    this->X=100;
    this->Y=100;
    this->Angle=0;
    this->detectors="";
    this->drs=false;
    this->oscilloscope=false;
    this->t1t2=true;
    this->t1t2t3=false;
    this->fiberModule=false;
    this->leakageDets=false;
    this->crystalMatrix=false;
    this->veto=false;
    this->preshower=false;
    this->testRun=false;

    QString readfile=QDir::homePath()+"/working/.dreamdaqrc";
    QFile configFile(readfile);
    if (configFile.open(QFile::ReadOnly)){
        QString txt;
        QTextStream cstream(&configFile);
        while (!cstream.atEnd()){
            txt=cstream.readLine();
            parseConfig(txt);
        }
        configFile.close();
    }
}

void DaqControlInterface::parseConfig(QString line){
    QStringList list;
    list = line.split(" ",QString::SkipEmptyParts);
    if (list.count()<=1) return;
    QString name=list.at(0);
    QString value=list.at(1);
    value.remove(QChar(' '));
    value.remove(QChar('\t'));
    if (name.indexOf("WorkingDir=", 0) != -1)
      {
        this->workDir=value;
      }
    else if (name.indexOf("DreamLogFile=", 0) != -1)
      {
        this->dreamLogFile=value;
      }
    else if (name.indexOf("GuiInputFile=", 0) != -1)
      {
        this->guiInputFile=value;
      }
    else if (name.indexOf("WorkDisk=", 0)!= -1)
      {
        this->wrkDisk=value;
      }
    else if (name.indexOf("BackupDisk=", 0)!= -1)
      {
        this->bkupDisk=value;
      }
    else if (name.indexOf("RunNumber=", 0) != -1)
      {
        this->runNumber=value.toInt();
      }
    else if (name.indexOf("NrOfPhysicsEvents=", 0) != -1)
      {
        this->eventsNumber=value.toInt();
      }
    else if (name.indexOf("PhysToPedRatio=", 0) != -1)
      {
        this->phyToPed=value.toDouble();
      }
    else if (name.indexOf("Beam=", 0) != -1)
      {
        this->type=value;
      }
    else if (name.indexOf("Energy=", 0) != -1)
      {
        this->energy=value.toInt();
      }
    else if (name.indexOf("BeamFile=", 0) != -1)
      {
        this->beamfile=value;
      }
    else if (name.indexOf("X=", 0) != -1)
      {
        this->X=value.toDouble();
      }
    else if (name.indexOf("Y=", 0) != -1)
      {
        this->Y=value.toDouble();
      }
    else if (name.indexOf("Angle=", 0) != -1)
      {
        this->Angle=value.toDouble();
      }
    else if (name.indexOf("Detectors=", 0) != -1)
      {
          for (int i=2;i<list.count();i++) value+=" "+list.at(i);
          this->detectors=value;
      }
    else if (name.indexOf("DRSReadout=", 0) != -1)
     {
        if (!value.compare("ON")) this->drs=true;
        else if (!value.compare("OFF")) this->drs=false;
        else qWarning()<<"Error on drs value";
     }
    else if (name.indexOf("ScopeReadout=", 0) != -1)
     {
        if (!value.compare("ON")) this->oscilloscope=true;
        else if (!value.compare("OFF")) this->oscilloscope=false;
        else qWarning()<<"Error on scope value";
     }
    else if (name.indexOf("T1*T2=", 0) != -1)
     {
        if (!value.compare("ON")) this->t1t2=true;
        else if (!value.compare("OFF")) this->t1t2=false;
        else qWarning()<<"Error on T1*T2 value";
     }
    else if (name.indexOf("T1*T2*T3=", 0) != -1)
     {
        if (!value.compare("ON")) this->t1t2t3=true;
        else if (!value.compare("OFF")) this->t1t2t3=false;
        else qWarning()<<"Error on T1*T2*T3 value";
     }
    else if (name.indexOf("FiberModule=", 0) != -1)
     {
        if (!value.compare("ON")) this->fiberModule=true;
        else if (!value.compare("OFF")) this->fiberModule=false;
        else qWarning()<<"Error on fiberModule value";
     }
    else if (name.indexOf("LeakageCounters=", 0) != -1)
     {
        if (!value.compare("ON")) this->leakageDets=true;
        else if (!value.compare("OFF")) this->leakageDets=false;
        else qWarning()<<"Error on leakageDets value";
     }
    else if (name.indexOf("CrystalMatrix=", 0) != -1)
     {
        if (!value.compare("ON")) this->crystalMatrix=true;
        else if (!value.compare("OFF")) this->crystalMatrix=false;
        else qWarning()<<"Error on crystalMatrix value";
     }
    else if (name.indexOf("Veto=", 0) != -1)
     {
        if (!value.compare("ON")) this->veto=true;
        else if (!value.compare("OFF")) this->veto=false;
        else qWarning()<<"Error on veto value";
     }
    else if (name.indexOf("Preshower=", 0) != -1)
     {
        if (!value.compare("ON")) this->preshower=true;
        else if (!value.compare("OFF")) this->preshower=false;
        else qWarning()<<"Error on preshower value";
     }
    else if (name.indexOf("TestRun=", 0) != -1)
     {
        if (!value.compare("ON")) this->testRun=true;
        else if (!value.compare("OFF")) this->testRun=false;
        else qWarning()<<"Error on test_run value";
     }
}

void DaqControlInterface::saveConfigFile(){
    QString readfile=QDir::homePath()+"/working/.dreamdaqrc";
    QFile configFile(readfile);
    if (configFile.open(QFile::WriteOnly|QFile::Truncate)){
        QTextStream fout(&configFile);
        fout<<"WorkingDir= "<<this->workDir<<"\n";
        fout<<"DreamLogFile= "<<this->dreamLogFile<<"\n";
        fout<<"GuiInputFile= "<<this->guiInputFile<<"\n";
        fout<<"WorkDisk= "<<this->wrkDisk<<"\n";
        fout<<"BackupDisk= "<<this->bkupDisk<<"\n";
        fout<<"RunNumber= "<<this->runNumber<<"\n";
        fout<<"NrOfPhysicsEvents= "<<this->eventsNumber<<"\n";
        fout<<"PhysToPedRatio= "<<this->phyToPed<<"\n";
        fout<<"Beam= "<<this->type<<"\n";
        fout<<"Energy= "<<this->energy<<"\n";
        fout<<"BeamFile= "<<this->beamfile<<"\n";
        fout<<"X= "<<this->X<<"\n";
        fout<<"Y= "<<this->Y<<"\n";
        fout<<"Angle= "<<this->Angle<<"\n";
        fout<<"Detectors= "<<this->detectors<<"\n";
        fout<<"DRSReadout= "<<(this->drs ? "ON":"OFF")<<"\n";
        fout<<"ScopeReadout= "<<(this->oscilloscope ? "ON" : "OFF")<<"\n";
        fout<<"T1*T2= "<<(this->t1t2 ? "ON" : "OFF")<<"\n";
        fout<<"T1*T2*T3= "<<(this->t1t2t3 ? "ON" : "OFF")<<"\n";
        fout<<"FiberModule= "<<(this->fiberModule ? "ON" : "OFF")<<"\n";
        fout<<"LeakageCounters= "<<(this->leakageDets ? "ON" : "OFF")<<"\n";
        fout<<"CrystalMatrix= "<<(this->crystalMatrix ? "ON" : "OFF")<<"\n";
        fout<<"Veto= "<<(this->veto ? "ON" : "OFF")<<"\n";
        fout<<"Preshower= "<<(this->preshower ? "ON" : "OFF")<<"\n";
        fout<<"TestRun= "<<(this->testRun ? "ON" : "OFF")<<"\n";
        configFile.close();
    }
    else qWarning()<<"ERROR: No way to save config!";
}

void DaqControlInterface::startStopSlot(){
    if (!isStarted && !isStopping) {
        started();
    } else if (!isStopping){
        stopped();
    }
}

void DaqControlInterface::showConfig(){
    ui->runNumber->setText(QString::number(runNumber));
    ui->maxEvents->setText(QString::number(eventsNumber));
    ui->energy->setText(QString::number(energy));
    ui->beam_file->setText(beamfile);
    ui->editX->setText(QString::number(X));
    ui->editY->setText(QString::number(Y));
    ui->editAngle->setText(QString::number(Angle));
    ui->detectors->setText(detectors);
    int ind = ui->comboBox->findText(type);
    ui->comboBox->setCurrentIndex(ind);
    ui->checkDrs->setChecked(drs);
    ui->checkScope->setChecked(oscilloscope);
    ui->T1_and_T2->setChecked(t1t2);
    ui->T1_and_T2_and_T3->setChecked(t1t2t3);
    ui->checkFiberM->setChecked(fiberModule);
    ui->checkLeakD->setChecked(leakageDets);
    ui->checkMatrix->setChecked(crystalMatrix);
    ui->checkVeto->setChecked(veto);
    ui->checkPreshow->setChecked(preshower);
    ui->checkTestRun->setChecked(testRun);
    ui->phy2ped->setText(QString::number(phyToPed));
}

void DaqControlInterface::closeEvent(QCloseEvent *e){
    if (this->isStarted) {
        em->setText("DAQ is running!<br>Stop it and close again!");
        em->exec();
        e->ignore();
    } else {
        viewLog->close();
        dialog->close();
        if (!isChanged) {
            e->accept();
            return;
        }

        QMessageBox msgBox;
        msgBox.setText("The configuration has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Save:
                readValues();
                saveConfigFile();
                e->accept();
                break;
          case QMessageBox::Discard:
                e->accept();
                break;
          case QMessageBox::Cancel:
                e->ignore();
                break;
          default:
              // should never be reached
              break;
        }

    }
}

void DaqControlInterface::aboutSlot(){
    QMessageBox m(this);
    m.setModal(true);
    m.setText("<b>DAQ Control Interface 1.0.7</b><br>Based on Qt 4.7<p>Written by <a href=\"mailto:manda.mgf@gmail.com\">Martino G. Ferrari</a><br>License <a href=\"http://www.gnu.org/licenses/\">GPL-V2</a><p>");
    m.exec();
}

void DaqControlInterface::chgDir(QString a){
    workDir=a;
    saveConfigFile();
    viewLog->changePath(this->workDir+"/"+this->dreamLogFile);
}

void DaqControlInterface::chgLog(QString a){
    dreamLogFile=a;
    saveConfigFile();
    viewLog->changePath(this->workDir+"/"+this->dreamLogFile);
}

void DaqControlInterface::chgInput(QString a){
    guiInputFile=a;
    saveConfigFile();
}

void DaqControlInterface::chgBackUp(QString a){
    bkupDisk=a;
    saveConfigFile();
}

void DaqControlInterface::chgWork(QString a){
    wrkDisk=a;
    saveConfigFile();
}

void DaqControlInterface::changed(bool a){
    if (a) isChanged=a;
}
