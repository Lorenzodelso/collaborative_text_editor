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

private slots:
    void loginClicked();
    void cancelClicked();
    void enableLoginButton();
    void registerClicked();
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
