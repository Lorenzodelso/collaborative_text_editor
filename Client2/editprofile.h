#ifndef EDITPROFILE_H
#define EDITPROFILE_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFileInfo>
#include <QFile>
#include <QHoverEvent>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>

#include "ClickableLabel.h"
#include "WorkerSocketClient.h"
#include "QUtente.h"

class EditProfile:public QWidget
{
    Q_OBJECT
public:
    EditProfile(QWidget *parent, WorkerSocketClient* wscP, QUtente* utente);
    bool checkString (QString arg);


signals:
    void SigModificaProfiloUtente(QUtente userNew);

private slots:
    void savePressed();
    void discardPressed();
    void selectImagePressed();
    void imageHovered();
    void comparePasswords();
    void userWhitespaces();
    void imageUnhovered();
    void esitoModificaProfiloUtente(QString esito, QUtente userNew);

private:
    const QString rsrc = ":/images/win";
    QLabel *username;
    QLabel *newPass;
    QLabel *newRepPass;
    ClickableLabel *userPic;
    QPixmap *profilePic;
    QLineEdit *usernameEdit;
    QLineEdit *newPassEdit;
    QLineEdit *newRepPassEdit;
    QPushButton *save;
    QPushButton *discard;
    WorkerSocketClient* wscP;
    QUtente *utenteLocale;
    QLabel *userErr;
    QLabel *passErr;
    QUtente *recDocsUtente;
    int userFlag = 1, passFlag = 1;
};

#endif // EDITPROFILE_H
