#ifndef RECENTDOCSDIALOGS_H
#define RECENTDOCSDIALOGS_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDialogButtonBox>
#include <QApplication>
#include <QDesktopWidget>
#include "textedit.h"
#include "editprofile.h"
#include "WorkerSocketClient.h"




class recentDocsDialogs : public QWidget
{
    Q_OBJECT
public:
    recentDocsDialogs(QWidget *parent, WorkerSocketClient* wscP);

signals:
    void SigDisconnessioneDalServer();

    void SigCreaDoc(QString nomeFile);

    void SigApriDoc(QString nomeFile);



private slots:
    void newFilePressed();
    void openUrlPressed();
    void abortPressed();
    void openPressed();
    void urlChanged(const QString &url);
    void newFileChanged(const QString &newFileName);
    void listItemSelected();
    void launchEditProfile();
    void updateRecDocs();

    /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    *
    * nota:
    * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
    * il costruttore senza paramteri
    * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
    *
    * */
    void esitoCreaDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);

    /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    *
    * nota:
    * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
    * il costruttore senza paramteri
    * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
    *
    * */
    void esitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);




private:
    QListWidget *recentDocs;
    QPushButton *create;
    QPushButton *open;
    QPushButton *abort;
    QPushButton *openUrl;
    QPushButton *editProfile;
    QLineEdit *URL;
    QLineEdit *newFileName;
    TextEdit *mw;
    WorkerSocketClient* wscPointer;
    class editProfile *edit;


};

#endif // RECENTDOCSDIALOGS_H
