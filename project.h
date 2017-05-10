#ifndef PROJECT_H
#define PROJECT_H

#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <QListWidget>
#include <QtWidgets/QLabel>

using namespace cv;

#include "tools.h"
#include "labs/lab1.h"
#include "labs/lab2.h"
#include "labs/lab3.h"
#include "labs/lab4.h"
#include "labs/lab5.h"
#include "labs/lab6.h"
#include "labs/lab7.h"
#include "labs/lab8.h"
#include "labs/lab9.h"
#include "labs/lab10.h"
#include "labs/lab11.h"

#define COLOR_CANVAS 255
#define COLOR_OBJECT 0

#define MAX_PATH 256

class File {
public:
    File(QString filePath, QString fileName) {
        this->filePath = filePath;
        this->fileName = fileName;
    }

    QString getFilePath() { return this->filePath; }
    QString getFileName() { return this->fileName; }

private:
    QString filePath;
    QString fileName;
};

class Task {
public:
    Task(QString name, void (*operation)(char *filePath));

    void execute(QString filePath);
    Task* addFilesDir(QString directory);
    QList<File*> getFiles() { return this->files; }
    QString getName() { return this->name; }

private:
    QString name;
    QList<File*> files;
    void (*operation)(char *filePath);

};

class Lab {

public:
    Lab(QString name);

    Lab* addTask(Task* task);
    Lab* addFilesDir(QString directory);
    QString getName();
    QList<Task*> getTasks() { return this->tasks; }
    QList<File*> getFiles() { return this->files; }

private:
    QString name;
    QList<Task*> tasks;
    QList<File*> files;

};

class LabApp : public QWidget {

    Q_OBJECT

public:
    LabApp(QList<Lab*> labs);

public slots:
    void lab_selected(QListWidgetItem *current, QListWidgetItem *previous);
    void task_selected(QListWidgetItem *current, QListWidgetItem *previous);
    void file_selected(QListWidgetItem *current, QListWidgetItem *previous);

private:
    QLayout *main_layout, *lists_layout;
    QList<Lab*> labs;
    QListWidget *lab_list, *task_list, *file_list;
    QLabel *status_label;

    QList<File*> getFiles(int labIndex, int taskIndex);
    void execute_task(Task *task, File *file);

};

LabApp* createLabApp();

#endif//PROJECT_H
