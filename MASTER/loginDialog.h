#ifndef LOGINDIAG_H
#define LOGINDIAG_H

#include <QDialog>
#include <QVBoxLayout>
#include "newprofiledialog.h"
#include "recentdocsdialogs.h"
#include "WorkerSocketClient.h"

class QLabel;
class QLineEdit;
class QPushButton;

class loginDialog : public QWidget{
    Q_OBJECT
public:
    loginDialog(QWidget *parent = 0, WorkerSocketClient* wscP = 0);

public slots:

    /*emette il segnale SigConnessioneAlServer*/
    void attivaSocket();

    /*emette il segnale SigDisconnessione al server*/
    void disattivaSocket();


private slots:
    void loginClicked();
    void cancelClicked();
    void enableLoginButton();
    void registerClicked();

    /*
    * ritorna il parametro ricevuto
    * */
    QString esitoConnessioneAlServer(QString esito/*esito*/);

    /*
     * fa la cosa opportuna sulla base dell'esito ricevuto
     *
     * nota:
     * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
     * il costruttore senza paramteri
     * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
     *
     * */
   void esitoLogin(QString esito/*esito*/, QUtente user, QList<QString> nomiFilesEditati);


signals:
    void loginSuccessful();

    void SigLogin(QUtente user);

    void SigConnessioneAlServer();

    void SigDisconnessioneDalServer();




private:
    QLabel *username;
    QLabel *password;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QPushButton *newProfileButton;
    newProfileDialog *newProfile;
    WorkerSocketClient *wscP;
    recentDocsDialogs *recDoc;

};

#endif //LOGINDIAG_H
