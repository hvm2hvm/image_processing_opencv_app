#include "project.h"
#include <QApplication>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    LabApp *labApp = createLabApp();
    labApp->show();

    app.exec();
    return 0;
}
