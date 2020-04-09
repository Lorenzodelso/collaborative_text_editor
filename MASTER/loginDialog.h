#ifndef LOGINDIAG_H
#define LOGINDIAG_H

#include <QDialog>
#include <QVBoxLayout>
#include "newprofiledialog.h"
#include "recentdocsdialogs.h"

class QLabel;
class QLineEdit;
class QPushButton;

class loginDialog : public QWidget{
    Q_OBJECT
public:
    loginDialog(QWidget *parent = 0);

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
    recentDocsDialogs *recDoc;

};

#endif //LOGINDIAG_H
