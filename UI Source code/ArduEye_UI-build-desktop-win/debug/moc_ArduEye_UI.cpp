/****************************************************************************
** Meta object code from reading C++ file 'ArduEye_UI.h'
**
** Created: Mon Oct 3 22:18:13 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ArduEye_UI/ArduEye_UI.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ArduEye_UI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ArduEyeUI[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   11,   10,   10, 0x08,
      52,   11,   10,   10, 0x08,
      82,   10,   10,   10, 0x08,
     109,   10,   10,   10, 0x08,
     127,   10,   10,   10, 0x08,
     154,   10,   10,   10, 0x08,
     184,   10,   10,   10, 0x08,
     203,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ArduEyeUI[] = {
    "ArduEyeUI\0\0newport\0onDeviceDiscovered(QextPortInfo)\0"
    "onDeviceRemoved(QextPortInfo)\0"
    "on_ConnectButton_clicked()\0onDataAvailable()\0"
    "on_CmdEdit_returnPressed()\0"
    "on_FileRecordButton_clicked()\0"
    "CheckCmdReceived()\0on_enterButton_clicked()\0"
};

const QMetaObject ArduEyeUI::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ArduEyeUI,
      qt_meta_data_ArduEyeUI, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ArduEyeUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ArduEyeUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ArduEyeUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArduEyeUI))
        return static_cast<void*>(const_cast< ArduEyeUI*>(this));
    return QWidget::qt_metacast(_clname);
}

int ArduEyeUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onDeviceDiscovered((*reinterpret_cast< const QextPortInfo(*)>(_a[1]))); break;
        case 1: onDeviceRemoved((*reinterpret_cast< const QextPortInfo(*)>(_a[1]))); break;
        case 2: on_ConnectButton_clicked(); break;
        case 3: onDataAvailable(); break;
        case 4: on_CmdEdit_returnPressed(); break;
        case 5: on_FileRecordButton_clicked(); break;
        case 6: CheckCmdReceived(); break;
        case 7: on_enterButton_clicked(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
