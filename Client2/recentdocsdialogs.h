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
#include "TextEdit.h"
#include "EditProfile.h"
#include "WorkerSocketClient.h"
#include "QUtente.h"


class RecentDocsDialogs : public QWidget
{
    Q_OBJECT

public:
    RecentDocsDialogs(QWidget *parent, WorkerSocketClient* wscP,quint32 siteId, QUtente utente, QList<QString> docList);

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
    void esitoCreaDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);
    void esitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);
    void esitoChiudiDoc(QString esito/*esito*/);

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
    quint16 siteId;
    WorkerSocketClient* wscP;
    class EditProfile *edit;
    QUtente utente;
    QList<QString> docList;
    QString fileName;
};

#endif // RECENTDOCSDIALOGS_H
