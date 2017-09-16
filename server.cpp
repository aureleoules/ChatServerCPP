#include "Server.h"

Server::Server() {
    serverState = new QLabel;
    quitButton = new QPushButton(tr("Quit"));
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(serverState);
    layout->addWidget(quitButton);
    this->setLayout(layout);

    setWindowTitle(tr("ChatServer"));

    server = new QTcpServer(this);
    if(!server->listen(QHostAddress::Any, 1337)) {
        serverState->setText(tr("Server couldn't start: ") + server->errorString());
    } else {
        serverState->setText(tr("Server started on port ") + QString::number(server->serverPort()));
        connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    }
    messageLength = 0;
}

void Server::newConnection() {
    sendToAll(tr("<em>A user is connected!</em>"));

    QTcpSocket* newUser = server->nextPendingConnection();
    users << newUser;

    connect(newUser, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(newUser, SIGNAL(disconnected()), this, SLOT(logoutUser()));
}

void Server::dataReceived() {
    //packet received
    //Whose packet is it?

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if(socket == 0) { //can't find sender
        return;
    }
    //otherwise lets continue
    //Getting message
    QDataStream in(socket);
    if(messageLength == 0) {
        if(socket->bytesAvailable() < (int)sizeof(quint16)) {
            return;
        }
        in >> messageLength;
    }
    //did we get all the bytes?
    if(socket->bytesAvailable() < messageLength) {
        return;  //no
    }
    //yes
    QString message;
    in >> message;

    sendToAll(message);
    messageLength = 0;
}

void Server::logoutUser() {
    sendToAll(tr("<em>User just logged out.</em>"));
    //which client wants to leave
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if(socket == 0) {
        return; //i dunno m8, let's stop there
    }
    users.removeOne(socket);
    socket->deleteLater(); //makes qt not lag; once the slot is done executing, will remove socket
}

void Server::sendToAll(const QString &message) {
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    for(int i = 0; i < users.size(); ++i) {
        users[i]->write(packet);
    }
}
