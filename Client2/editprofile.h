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

signals:
    void SigModificaProfiloUtente(QUtente userNew);

private slots:
    void savePressed();
    void discardPressed();
    void selectImagePressed();
    void changedNick(const QString &);
    void imageHovered();
    void imageUnhovered();

   /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    *
    * nota:
    * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
    * il costruttore senza paramteri
    * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
    *
    * */
    void esitoModificaProfiloUtente(QString esito/*esito*/, QUtente userNew);

private:
    const QString rsrc = ":/images/win";
    QLabel *nickname;
    QLabel *username;
    ClickableLabel *userPic;
    QPixmap *profilePic;
    QLineEdit *usernameEdit;
    QLineEdit *nickEdit;
    QPushButton *save;
    QPushButton *discard;
    QUtente *user;
    WorkerSocketClient* wscP;
    QUtente *utente;

};

#endif // EDITPROFILE_H
