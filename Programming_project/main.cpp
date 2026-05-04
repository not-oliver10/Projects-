#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Library Management System");
    app.setOrganizationName("LibraryApp");

    MainWindow w;
    w.show();
    return app.exec();
}
