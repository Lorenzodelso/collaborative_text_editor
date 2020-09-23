#ifndef LOGINDIAG_H
#define LOGINDIAG_H

#include <QDialog>
#include <QVBoxLayout>
#include "NewProfileDialog.h"
#include "RecentDocsDialogs.h"
#include "WorkerSocketClient.h"

class QLabel;
class QLineEdit;
class QPushButton;

class LoginDialog : public QWidget{
    Q_OBJECT
public:
    LoginDialog(QWidget *parent = 0, WorkerSocketClient* wscP = 0);

public slots:



private slots:
    void loginClicked();
    void cancelClicked();
    void enableLoginButton();
    void registerClicked();


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



private:
    QLabel *username;
    QLabel *password;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QPushButton *newProfileButton;
    NewProfileDialog *newProfile;
    WorkerSocketClient *wscP;
    RecentDocsDialogs *recDoc;

};

#endif //LOGINDIAG_H
