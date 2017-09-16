#include <QApplication>
#include "Server.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Server window;
    window.show();

    return app.exec();
}
