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




class recentDocsDialogs : public QWidget
{
    Q_OBJECT
public:
    recentDocsDialogs(QWidget *parent);

signals:
    void SigDisconnessioneDalServer();

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
    class editProfile *edit;


};

#endif // RECENTDOCSDIALOGS_H
