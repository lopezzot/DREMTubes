#ifndef DAQCONTROLINTERFACE_H
#define DAQCONTROLINTERFACE_H

#include <map>
#include <QMainWindow>
#include <QLabel>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QList>
#include <QCloseEvent>
#include <QMessageBox>
#include <QPushButton>

#include "label.h"
#include "viewlog.h"
#include "preferencedialog.h"

#define HELPLOG "   Run   |          Date          |  Length  |  <Rate>  |  Beam Evts  |  Ped Evts  |  Beam type  |  Q*E(GeV)  |   Beam File  |     X    |     Y    |   Angle  |  R DSFLCVP  |  A.O.Info"
#define HELPSEP "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
//HEIGHT AND WIDTH OF THE WINDOW
#define NH 640
#define NW 330
//HEIGHT AND WIDTH OF THE WINDOW WHEN EVENTS HAPPEN
#define EH 587
#define EW 781
//TIME INTERVAL
#define TI 100

namespace Ui {
    class DaqControlInterface;
}

class DaqControlInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit DaqControlInterface(QWidget *parent = 0);
    ~DaqControlInterface();

private:
    QString workDir, dreamLogFile, guiInputFile;
    Ui::DaqControlInterface *ui;
    QLabel * timeLabel;
    QLabel * statusLabel;
    QPushButton * startStopButton;
    Label * l1, *l2;
    ViewLog * viewLog;
    PreferenceDialog * dialog;
    QMessageBox * em;

    void lockInterface(bool yes = true);
    void lockInfo(bool yes = true);
    bool checkValidity();

    //DAQ Variable
    qint64 runNumber,eventsNumber,energy;
    qreal X,Y,Angle;
    bool testRun,drs,oscilloscope,t1t2,t1t2t3,fiberModule,leakageDets,crystalMatrix,veto,preshower;
    bool isReading, hasEvent;
    bool isStarted;
    bool isChanged;
    QString beamfile,type,detectors;
    qint64 runTime;

    QString bkupDisk,wrkDisk;

    QDateTime startTime;

    QTimer * t0;
    QTimer * t1;

    //READ Variable
    qint64 spillNumber;
    qint64 realEventsNumber;
    qint64 phyNumber;
    qint64 pedNumber;
    qint64 lastSpillEvents;
    double phyToPed;
    std::map<quint32, quint32>countRate;
    quint32 spillCounted;

    quint32 spillRate();

    //DAQ Function
    void saveLog();
    QString readString();
    void parseString(QString str);

    void readConfigFile();
    void saveConfigFile();
    void parseConfig(QString line);

    void readValues();

    bool isANumber(char c);
    void openFile();
    void closeFile();
    QFile readingFile;
    QTextStream *stream;
    bool isStopping;
    void showConfig();
    void firstRun();
protected:
    void closeEvent(QCloseEvent *);
private slots:
    //Interface slots
    void editNumber();
    void checkRunNr(QString t);
    void checkEvtNr(QString t);
    void checkEnergy(QString t);
    void checkPhy2Ped(QString t);
    void checkXY(QString t);
    void checkAngle(QString t);
    //Control slots
    void started();
    void stopped();
    void cleanUpped();
    //Reading slots
    void timeUpdate();
    void eventInit();
    void eventUpdate();
    void updatePanel(QString qs);
    void startStopSlot();
    void aboutSlot();
    void chgDir(QString a);
    void chgLog(QString a);
    void chgInput(QString a);
    void chgWork(QString a);
    void chgBackUp(QString a);
    void changed(bool a=true);
};

#endif // DAQCONTROLINTERFACE_H
