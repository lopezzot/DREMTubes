/****************************************************************************
** Meta object code from reading C++ file 'preferencedialog.h'
**
** Created: Wed Oct 21 19:43:41 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "preferencedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'preferencedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PreferenceDialog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   18,   17,   17, 0x05,
      33,   18,   17,   17, 0x05,
      46,   18,   17,   17, 0x05,
      61,   18,   17,   17, 0x05,
      75,   18,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      91,   17,   17,   17, 0x08,
      96,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PreferenceDialog[] = {
    "PreferenceDialog\0\0a\0dir(QString)\0"
    "log(QString)\0input(QString)\0work(QString)\0"
    "backup(QString)\0ok()\0cancel()\0"
};

const QMetaObject PreferenceDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PreferenceDialog,
      qt_meta_data_PreferenceDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PreferenceDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PreferenceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PreferenceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PreferenceDialog))
        return static_cast<void*>(const_cast< PreferenceDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PreferenceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dir((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: log((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: input((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: work((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: backup((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: ok(); break;
        case 6: cancel(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void PreferenceDialog::dir(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PreferenceDialog::log(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PreferenceDialog::input(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PreferenceDialog::work(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PreferenceDialog::backup(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
