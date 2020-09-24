#ifndef SERVERSELECTION_H
#define SERVERSELECTION_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include "WorkerSocketClient.h"
#include "loginDialog.h"
#include <QVBoxLayout>
#include <regex>

class ServerSelection : public QWidget
{
    Q_OBJECT
public:
    ServerSelection(QWidget *parent, WorkerSocketClient *wscP);


    LoginDialog *ld;


signals:
    void SigConnessioneAlServer(QString ipAddr);

public slots:
    void attivaSocket();

    void activateConnect();
    void closePressed();
    void esitoConnessioneAlServer(QString esito);
private:
    QPushButton *connectButton;
    WorkerSocketClient *wscP;
    QPushButton *closeButton;
    QLineEdit *address;
    QLabel *addressLabel;
    std::regex ipv4 = std::regex("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
};

#endif // SERVERSELECTION_H
