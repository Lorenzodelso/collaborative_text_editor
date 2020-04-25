//
// Created by Edoardo Dazzara on 25/04/2020.
//


#include <QApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
    QApplication* serverApp= new QApplication(argc, argv);
    Server* server= new Server();
    qint32 ris = serverApp->exec();
}
