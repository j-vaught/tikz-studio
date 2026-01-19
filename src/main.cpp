#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("TikZ Editor");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("UofSC");

    MainWindow window;
    window.show();

    return app.exec();
}
