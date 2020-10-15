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
#include "ClickableLabel.h"
#include "QUtente.h"
#include "WorkerSocketClient.h"
#include "RecentDocsDialogs.h"
#include "WorkerSocketClient.h"
#include <QPixmap>

class QLabel;
class QLineEdit;
class QPushButton;

class NewProfileDialog:public QDialog
{
    Q_OBJECT
public:
    NewProfileDialog(QWidget *parent, WorkerSocketClient* wscP);
    bool checkString (QString arg);

signals:
    void SigRegistrazione(const QUtente user);

public slots:
    void registerPressed();
    void abortPressed();
    void comparePasswords();
    void imageHovered();
    void imageUnhovered();
    void selectImagePressed();
    void userWhitespaces();
    void esitoRegistrazione(QString esito/*esito*/);


private:
    const QString rsrc = ":/images/win";
    WorkerSocketClient *wscP;
    QUtente *utente;
    QLabel *username;
    QLabel *password;
    QLabel *repeatPassword;
    ClickableLabel *userPic;
    QPixmap *profilePic;
    QPushButton *registerUser;
    QPushButton *abort;
    QLineEdit *userEdit;
    QLineEdit *passEdit;
    QLineEdit *repPassEdit;
    QLabel *userErr;
    QLabel *passErr;
    RecentDocsDialogs *docsDialog;
    int userFlag = 0, passFlag = 0;
};

#endif // NEWPROFILEDIALOG_H
