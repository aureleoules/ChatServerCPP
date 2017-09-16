#ifndef SERVER_H
#define SERVER_H

#include <QtWidgets>
#include <QtNetwork>

class Server : public QWidget {

    Q_OBJECT

    public:
        Server();
        void sendToAll(const QString &message);

    private slots:
        void newConnection();
        void dataReceived();
        void logoutUser();

    private:
        QLabel *serverState;
        QPushButton *quitButton;
        QTcpServer *server;
        QList<QTcpSocket *> users;
        quint16 messageLength;

};

#endif // SERVER_H
