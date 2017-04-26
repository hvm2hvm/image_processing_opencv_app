#include "project.h"
#include <QFileDialog>

unsigned char bounded(int value) {
    if (value > 255) {
        return 255;
    }
    else if (value < 0) {
        return 0;
    }
    else {
        return value;
    }
}

bool openFileDlg(char *filename) {
    QString result = QFileDialog::getOpenFileName(
            NULL, "Select image", "/home/voicu/work/017_facultate/IP/");

    if (!result.isEmpty()) {
        strcpy(filename, result.toUtf8().data());
        return true;
    } else {
        return false;
    }
}

char *open_file(QString labImages) {
    QString location = QString("/home/voicu/work/017_facultate/IP/Images/") + labImages;
    QString result = QFileDialog::getOpenFileName(
            NULL, "Select image", location);
    return result.toUtf8().data();
}
