/****************************************************************************
** Meta object code from reading C++ file 'daqcontrolinterface.h'
**
** Created: Wed Oct 21 19:43:39 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "daqcontrolinterface.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'daqcontrolinterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DaqControlInterface[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x08,
      36,   34,   20,   20, 0x08,
      56,   34,   20,   20, 0x08,
      76,   34,   20,   20, 0x08,
      97,   34,   20,   20, 0x08,
     119,   34,   20,   20, 0x08,
     136,   34,   20,   20, 0x08,
     156,   20,   20,   20, 0x08,
     166,   20,   20,   20, 0x08,
     176,   20,   20,   20, 0x08,
     189,   20,   20,   20, 0x08,
     202,   20,   20,   20, 0x08,
     214,   20,   20,   20, 0x08,
     231,  228,   20,   20, 0x08,
     252,   20,   20,   20, 0x08,
     268,   20,   20,   20, 0x08,
     282,  280,   20,   20, 0x08,
     298,  280,   20,   20, 0x08,
     314,  280,   20,   20, 0x08,
     332,  280,   20,   20, 0x08,
     349,  280,   20,   20, 0x08,
     368,  280,   20,   20, 0x08,
     382,   20,   20,   20, 0x28,

       0        // eod
};

static const char qt_meta_stringdata_DaqControlInterface[] = {
    "DaqControlInterface\0\0editNumber()\0t\0"
    "checkRunNr(QString)\0checkEvtNr(QString)\0"
    "checkEnergy(QString)\0checkPhy2Ped(QString)\0"
    "checkXY(QString)\0checkAngle(QString)\0"
    "started()\0stopped()\0cleanUpped()\0"
    "timeUpdate()\0eventInit()\0eventUpdate()\0"
    "qs\0updatePanel(QString)\0startStopSlot()\0"
    "aboutSlot()\0a\0chgDir(QString)\0"
    "chgLog(QString)\0chgInput(QString)\0"
    "chgWork(QString)\0chgBackUp(QString)\0"
    "changed(bool)\0changed()\0"
};

const QMetaObject DaqControlInterface::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_DaqControlInterface,
      qt_meta_data_DaqControlInterface, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DaqControlInterface::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DaqControlInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DaqControlInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DaqControlInterface))
        return static_cast<void*>(const_cast< DaqControlInterface*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int DaqControlInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: editNumber(); break;
        case 1: checkRunNr((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: checkEvtNr((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: checkEnergy((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: checkPhy2Ped((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: checkXY((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: checkAngle((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: started(); break;
        case 8: stopped(); break;
        case 9: cleanUpped(); break;
        case 10: timeUpdate(); break;
        case 11: eventInit(); break;
        case 12: eventUpdate(); break;
        case 13: updatePanel((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: startStopSlot(); break;
        case 15: aboutSlot(); break;
        case 16: chgDir((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 17: chgLog((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: chgInput((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 19: chgWork((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: chgBackUp((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 21: changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: changed(); break;
        default: ;
        }
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
