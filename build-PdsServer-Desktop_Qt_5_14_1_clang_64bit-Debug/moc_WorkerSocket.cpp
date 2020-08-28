/****************************************************************************
** Meta object code from reading C++ file 'WorkerSocket.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../Server2/WorkerSocket.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkerSocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WorkerSocket_t {
    QByteArrayData data[45];
    char stringdata0[734];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WorkerSocket_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WorkerSocket_t qt_meta_stringdata_WorkerSocket = {
    {
QT_MOC_LITERAL(0, 0, 12), // "WorkerSocket"
QT_MOC_LITERAL(1, 13, 10), // "SigApriDoc"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 8), // "nomeFile"
QT_MOC_LITERAL(4, 34, 13), // "WorkerSocket*"
QT_MOC_LITERAL(5, 48, 3), // "wsP"
QT_MOC_LITERAL(6, 52, 7), // "QUtente"
QT_MOC_LITERAL(7, 60, 4), // "user"
QT_MOC_LITERAL(8, 65, 10), // "SigCreaDoc"
QT_MOC_LITERAL(9, 76, 8), // "SigLogin"
QT_MOC_LITERAL(10, 85, 16), // "SigRegistrazione"
QT_MOC_LITERAL(11, 102, 24), // "SigModificaProfiloUtente"
QT_MOC_LITERAL(12, 127, 7), // "userOld"
QT_MOC_LITERAL(13, 135, 7), // "userNew"
QT_MOC_LITERAL(14, 143, 38), // "SigChiusuraConnessioneDaParte..."
QT_MOC_LITERAL(15, 182, 38), // "SigChiusuraConnessioneDaParte..."
QT_MOC_LITERAL(16, 221, 36), // "SigChiusuraDocumentoDaParteDe..."
QT_MOC_LITERAL(17, 258, 8), // "SigOpDoc"
QT_MOC_LITERAL(18, 267, 12), // "DocOperation"
QT_MOC_LITERAL(19, 280, 9), // "operation"
QT_MOC_LITERAL(20, 290, 22), // "SigOpChiHaInseritoCosa"
QT_MOC_LITERAL(21, 313, 20), // "WorkerSocketAttivati"
QT_MOC_LITERAL(22, 334, 8), // "quintptr"
QT_MOC_LITERAL(23, 343, 16), // "socketDescriptor"
QT_MOC_LITERAL(24, 360, 11), // "leggiMsgApp"
QT_MOC_LITERAL(25, 372, 18), // "rispondiEsitoLogin"
QT_MOC_LITERAL(26, 391, 14), // "QList<QString>"
QT_MOC_LITERAL(27, 406, 16), // "nomiFilesEditati"
QT_MOC_LITERAL(28, 423, 20), // "rispondiEsitoApriDoc"
QT_MOC_LITERAL(29, 444, 5), // "esito"
QT_MOC_LITERAL(30, 450, 4), // "CRDT"
QT_MOC_LITERAL(31, 455, 3), // "doc"
QT_MOC_LITERAL(32, 459, 20), // "rispondiEsitoCreaDoc"
QT_MOC_LITERAL(33, 480, 26), // "rispondiEsitoRegistrazione"
QT_MOC_LITERAL(34, 507, 7), // "nomeImg"
QT_MOC_LITERAL(35, 515, 18), // "rispondiEsitoOpDoc"
QT_MOC_LITERAL(36, 534, 34), // "rispondiEsitoModificaProfiloU..."
QT_MOC_LITERAL(37, 569, 18), // "immagineModificata"
QT_MOC_LITERAL(38, 588, 46), // "rispondiEsitoChiusuraDocument..."
QT_MOC_LITERAL(39, 635, 23), // "questoUserHaApertoIlDoc"
QT_MOC_LITERAL(40, 659, 5), // "QUser"
QT_MOC_LITERAL(41, 665, 23), // "questoUserHaChiusoIlDoc"
QT_MOC_LITERAL(42, 689, 25), // "rispondiChiHaInseritoCosa"
QT_MOC_LITERAL(43, 715, 12), // "QList<QUser>"
QT_MOC_LITERAL(44, 728, 5) // "users"

    },
    "WorkerSocket\0SigApriDoc\0\0nomeFile\0"
    "WorkerSocket*\0wsP\0QUtente\0user\0"
    "SigCreaDoc\0SigLogin\0SigRegistrazione\0"
    "SigModificaProfiloUtente\0userOld\0"
    "userNew\0SigChiusuraConnessioneDaParteDelClient\0"
    "SigChiusuraConnessioneDaParteDelServer\0"
    "SigChiusuraDocumentoDaParteDelClient\0"
    "SigOpDoc\0DocOperation\0operation\0"
    "SigOpChiHaInseritoCosa\0WorkerSocketAttivati\0"
    "quintptr\0socketDescriptor\0leggiMsgApp\0"
    "rispondiEsitoLogin\0QList<QString>\0"
    "nomiFilesEditati\0rispondiEsitoApriDoc\0"
    "esito\0CRDT\0doc\0rispondiEsitoCreaDoc\0"
    "rispondiEsitoRegistrazione\0nomeImg\0"
    "rispondiEsitoOpDoc\0"
    "rispondiEsitoModificaProfiloUtente\0"
    "immagineModificata\0"
    "rispondiEsitoChiusuraDocumentoDaParteDelClient\0"
    "questoUserHaApertoIlDoc\0QUser\0"
    "questoUserHaChiusoIlDoc\0"
    "rispondiChiHaInseritoCosa\0QList<QUser>\0"
    "users"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WorkerSocket[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,  124,    2, 0x06 /* Public */,
       8,    3,  131,    2, 0x06 /* Public */,
       9,    2,  138,    2, 0x06 /* Public */,
      10,    2,  143,    2, 0x06 /* Public */,
      11,    3,  148,    2, 0x06 /* Public */,
      14,    2,  155,    2, 0x06 /* Public */,
      15,    1,  160,    2, 0x06 /* Public */,
      16,    2,  163,    2, 0x06 /* Public */,
      17,    1,  168,    2, 0x06 /* Public */,
      20,    2,  171,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      21,    1,  176,    2, 0x0a /* Public */,
      24,    0,  179,    2, 0x0a /* Public */,
      25,    2,  180,    2, 0x0a /* Public */,
      28,    2,  185,    2, 0x0a /* Public */,
      32,    2,  190,    2, 0x0a /* Public */,
      33,    2,  195,    2, 0x0a /* Public */,
      35,    2,  200,    2, 0x0a /* Public */,
      36,    2,  205,    2, 0x0a /* Public */,
      38,    1,  210,    2, 0x0a /* Public */,
      39,    1,  213,    2, 0x0a /* Public */,
      41,    1,  216,    2, 0x0a /* Public */,
      42,    1,  219,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4, 0x80000000 | 6,    3,    5,    7,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4, 0x80000000 | 6,    3,    5,    7,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 6,    5,    7,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 6,    5,    7,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 6, 0x80000000 | 6,    5,   12,   13,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 6,    5,    7,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 6,    5,    7,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 6,    5,    7,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 26,    7,   27,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 30,   29,   31,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 30,   29,   31,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   29,   34,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 18,   29,   19,
    QMetaType::Void, 0x80000000 | 6, QMetaType::Bool,   13,   37,
    QMetaType::Void, QMetaType::QString,   29,
    QMetaType::Void, 0x80000000 | 40,    7,
    QMetaType::Void, 0x80000000 | 40,    7,
    QMetaType::Void, 0x80000000 | 43,   44,

       0        // eod
};

void WorkerSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WorkerSocket *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SigApriDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< WorkerSocket*(*)>(_a[2])),(*reinterpret_cast< QUtente(*)>(_a[3]))); break;
        case 1: _t->SigCreaDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< WorkerSocket*(*)>(_a[2])),(*reinterpret_cast< QUtente(*)>(_a[3]))); break;
        case 2: _t->SigLogin((*reinterpret_cast< WorkerSocket*(*)>(_a[1])),(*reinterpret_cast< QUtente(*)>(_a[2]))); break;
        case 3: _t->SigRegistrazione((*reinterpret_cast< WorkerSocket*(*)>(_a[1])),(*reinterpret_cast< QUtente(*)>(_a[2]))); break;
        case 4: _t->SigModificaProfiloUtente((*reinterpret_cast< WorkerSocket*(*)>(_a[1])),(*reinterpret_cast< QUtente(*)>(_a[2])),(*reinterpret_cast< QUtente(*)>(_a[3]))); break;
        case 5: _t->SigChiusuraConnessioneDaParteDelClient((*reinterpret_cast< WorkerSocket*(*)>(_a[1])),(*reinterpret_cast< QUtente(*)>(_a[2]))); break;
        case 6: _t->SigChiusuraConnessioneDaParteDelServer((*reinterpret_cast< WorkerSocket*(*)>(_a[1]))); break;
        case 7: _t->SigChiusuraDocumentoDaParteDelClient((*reinterpret_cast< WorkerSocket*(*)>(_a[1])),(*reinterpret_cast< QUtente(*)>(_a[2]))); break;
        case 8: _t->SigOpDoc((*reinterpret_cast< DocOperation(*)>(_a[1]))); break;
        case 9: _t->SigOpChiHaInseritoCosa((*reinterpret_cast< WorkerSocket*(*)>(_a[1])),(*reinterpret_cast< QUtente(*)>(_a[2]))); break;
        case 10: _t->WorkerSocketAttivati((*reinterpret_cast< quintptr(*)>(_a[1]))); break;
        case 11: _t->leggiMsgApp(); break;
        case 12: _t->rispondiEsitoLogin((*reinterpret_cast< QUtente(*)>(_a[1])),(*reinterpret_cast< QList<QString>(*)>(_a[2]))); break;
        case 13: _t->rispondiEsitoApriDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< CRDT(*)>(_a[2]))); break;
        case 14: _t->rispondiEsitoCreaDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< CRDT(*)>(_a[2]))); break;
        case 15: _t->rispondiEsitoRegistrazione((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 16: _t->rispondiEsitoOpDoc((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< DocOperation(*)>(_a[2]))); break;
        case 17: _t->rispondiEsitoModificaProfiloUtente((*reinterpret_cast< QUtente(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 18: _t->rispondiEsitoChiusuraDocumentoDaParteDelClient((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 19: _t->questoUserHaApertoIlDoc((*reinterpret_cast< QUser(*)>(_a[1]))); break;
        case 20: _t->questoUserHaChiusoIlDoc((*reinterpret_cast< QUser(*)>(_a[1]))); break;
        case 21: _t->rispondiChiHaInseritoCosa((*reinterpret_cast< QList<QUser>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DocOperation >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WorkerSocket* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QString> >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            }
            break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CRDT >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CRDT >(); break;
            }
            break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DocOperation >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUtente >(); break;
            }
            break;
        case 19:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUser >(); break;
            }
            break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QUser >(); break;
            }
            break;
        case 21:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QUser> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WorkerSocket::*)(QString , WorkerSocket * , QUtente );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigApriDoc)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(QString , WorkerSocket * , QUtente );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigCreaDoc)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(WorkerSocket * , QUtente );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigLogin)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(WorkerSocket * , QUtente );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigRegistrazione)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(WorkerSocket * , QUtente , QUtente );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigModificaProfiloUtente)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(WorkerSocket * , QUtente );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigChiusuraConnessioneDaParteDelClient)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(WorkerSocket * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigChiusuraConnessioneDaParteDelServer)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(WorkerSocket * , QUtente );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigChiusuraDocumentoDaParteDelClient)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(DocOperation );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigOpDoc)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (WorkerSocket::*)(WorkerSocket * , QUtente );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WorkerSocket::SigOpChiHaInseritoCosa)) {
                *result = 9;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WorkerSocket::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_WorkerSocket.data,
    qt_meta_data_WorkerSocket,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WorkerSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkerSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WorkerSocket.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkerSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void WorkerSocket::SigApriDoc(QString _t1, WorkerSocket * _t2, QUtente _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WorkerSocket::SigCreaDoc(QString _t1, WorkerSocket * _t2, QUtente _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WorkerSocket::SigLogin(WorkerSocket * _t1, QUtente _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WorkerSocket::SigRegistrazione(WorkerSocket * _t1, QUtente _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WorkerSocket::SigModificaProfiloUtente(WorkerSocket * _t1, QUtente _t2, QUtente _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void WorkerSocket::SigChiusuraConnessioneDaParteDelClient(WorkerSocket * _t1, QUtente _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void WorkerSocket::SigChiusuraConnessioneDaParteDelServer(WorkerSocket * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void WorkerSocket::SigChiusuraDocumentoDaParteDelClient(WorkerSocket * _t1, QUtente _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void WorkerSocket::SigOpDoc(DocOperation _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void WorkerSocket::SigOpChiHaInseritoCosa(WorkerSocket * _t1, QUtente _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
