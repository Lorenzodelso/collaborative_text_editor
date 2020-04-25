#ifndef NEWPROFILEDIALOG_H
#define NEWPROFILEDIALOG_H

#include <QObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QtWidgets>
#include <QFormLayout>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include "clickablelabel.h"
#include "QUtente.h"
#include "WorkerSocketClient.h"
#include "recentdocsdialogs.h"
#include "WorkerSocketClient.h"
#include <QPixmap>

class QLabel;
class QLineEdit;
class QPushButton;

class newProfileDialog:public QDialog
{
    Q_OBJECT
public:
    newProfileDialog(QWidget *parent, WorkerSocketClient* wscP);

signals:
    void SigRegistrazione(const QUtente user);

public slots:
    void registerPressed();
    void abortPressed();
    void comparePasswords();
    void imageHovered();
    void imageUnhovered();
    void selectImagePressed();

   /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    *
    * nota:
    * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
    * il costruttore senza paramteri
    * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
    *
    * */
    void esitoRegistrazione(QString esito/*esito*/);


private:
    const QString rsrc = ":/images/win";

    WorkerSocketClient *wscP;
    QUtente *utente;
    QLabel *username;
    QLabel *password;
    QLabel *repeatPassword;
    QLabel *nickname;
    ClickableLabel *userPic;
    QPixmap *profilePic;
    QPushButton *registerUser;
    QPushButton *abort;
    QLineEdit *userEdit;
    QLineEdit *passEdit;
    QLineEdit *repPassEdit;
    QLineEdit *nickEdit;
    QLabel *err;
    recentDocsDialogs *docsDialog;
};

#endif // NEWPROFILEDIALOG_H
