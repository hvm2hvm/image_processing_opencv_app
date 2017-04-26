#ifndef TOOLS_H
#define TOOLS_H

#include "project.h"

#include <QFileDialog>

unsigned char bounded(int value);

bool openFileDlg(char *filename);
char *open_file(QString labImages);

#endif//TOOLS_H
