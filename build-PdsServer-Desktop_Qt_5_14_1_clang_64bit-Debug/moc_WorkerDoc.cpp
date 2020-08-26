/****************************************************************************
** Meta object code from reading C++ file 'WorkerDoc.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../Server2/WorkerDoc.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkerDoc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WorkerDoc_t {
    QByteArrayData data[21];
    char stringdata0[255];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WorkerDoc_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WorkerDoc_t qt_meta_stringdata_WorkerDoc = {
    {
QT_MOC_LITERAL(0, 0, 9), // "WorkerDoc"
QT_MOC_LITERAL(1, 10, 13), // "SigEsitoOpDoc"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 5), // "esito"
QT_MOC_LITERAL(4, 31, 12), // "DocOperation"
QT_MOC_LITERAL(5, 44, 5), // "docOp"
QT_MOC_LITERAL(6, 50, 15), // "SigEsitoCreaDoc"
QT_MOC_LITERAL(7, 66, 4), // "CRDT"
QT_MOC_LITERAL(8, 71, 3), // "doc"
QT_MOC_LITERAL(9, 75, 15), // "SigEsitoApriDoc"
QT_MOC_LITERAL(10, 91, 26), // "SigNessunClientStaEditando"
QT_MOC_LITERAL(11, 118, 8), // "nomeFile"
QT_MOC_LITERAL(12, 127, 16), // "workerDocCreaDoc"
QT_MOC_LITERAL(13, 144, 13), // "WorkerSocket*"
QT_MOC_LITERAL(14, 158, 3), // "wsP"
QT_MOC_LITERAL(15, 162, 25), // "workerDocPrimaAperturaDoc"
QT_MOC_LITERAL(16, 188, 25), // "workerDocNsimaAperturaDoc"
QT_MOC_LITERAL(17, 214, 7), // "QUtente"
QT_MOC_LITERAL(18, 222, 4), // "user"
QT_MOC_LITERAL(19, 227, 5), // "opDoc"
QT_MOC_LITERAL(20, 233, 21) // "unClientHaChiusoIlDoc"

    },
    "WorkerDoc\0SigEsitoOpDoc\0\0esito\0"
    "DocOperation\0docOp\0SigEsitoCreaDoc\0"
    "CRDT\0doc\0SigEsitoApriDoc\0"
    "SigNessunClientStaEditando\0nomeFile\0"
    "workerDocCreaDoc\0WorkerSocket*\0wsP\0"
    "workerDocPrimaAperturaDoc\0"
    "workerDocNsimaAperturaDoc\0QUtente\0"
    "user\0opDoc\0unClientHaChiusoIlDoc"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WorkerDoc[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x06 /* Public */,
       6,    2,   64,    2, 0x06 /* Public */,
       9,    2,   69,    2, 0x06 /* Public */,
      10,    1,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    2,   77,    2, 0x0a /* Public */,
      15,    2,   82,    2, 0x0a /* Public */,
      16,    2,   87,    2, 0x0a /* Public */,
      19,    1,   92,    2, 0x0a /* Public */,
      20,    0,   95,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 7,    3,    8,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 7,    3,    8,
    QMetaType::Void, QMetaType::QString,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 13,   11,   14,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 13,   11,   14,
    QMetaType::Void, 0x80000000 | 17, 0x80000000 | 13,   18,   14,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,

       0        // eod
};

void WorkerDoc::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WorkerDoc *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SigEsitoOpDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< DocOperation(*)>(_a[2]))); break;
        case 1: _t->SigEsitoCreaDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< CRDT(*)>(_a[2]))); break;
        case 2: _t->SigEsitoApriDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< CRDT(*)>(_a[2]))); break;
        case 3: _t->SigNessunClientStaEditando((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->workerDocCreaDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< WorkerSocket*(*)>(_a[2]))); break;
        case 5: _t->workerDocPrimaAperturaDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< WorkerSocket*(*)>(_a[2]))); break;
        case 6: _t->workerDocNsimaAperturaDoc((*reinterpret_cast< QUtente(*)>(_a[1])),(*reinterpret_cast< WorkerSocket*(*)>(_a[2]))); break;
        case 7: _t->opDoc((*reinterpret_cast< DocOperation(*)>(_a[1]))); break;
        case 8: _t->unClientHaChiusoIlDoc(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DocOperation >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CRDT >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CRDT >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DocOperation >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WorkerDoc::*)(QString , DocOperation );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerDoc::SigEsitoOpDoc)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WorkerDoc::*)(QString , CRDT );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerDoc::SigEsitoCreaDoc)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WorkerDoc::*)(QString , CRDT );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerDoc::SigEsitoApriDoc)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WorkerDoc::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerDoc::SigNessunClientStaEditando)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WorkerDoc::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_WorkerDoc.data,
    qt_meta_data_WorkerDoc,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WorkerDoc::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkerDoc::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WorkerDoc.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkerDoc::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void WorkerDoc::SigEsitoOpDoc(QString _t1, DocOperation _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WorkerDoc::SigEsitoCreaDoc(QString _t1, CRDT _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WorkerDoc::SigEsitoApriDoc(QString _t1, CRDT _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WorkerDoc::SigNessunClientStaEditando(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
