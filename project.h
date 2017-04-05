#ifndef PROJECT_H
#define PROJECT_H

#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <QListWidget>

using namespace cv;

#include "tools.h"
#include "labs/lab1.h"
#include "labs/lab2.h"
#include "labs/lab3.h"
#include "labs/lab4.h"
#include "labs/lab5.h"
#include "labs/lab6.h"

#define MAX_PATH 256

class Task {
public:
    Task(QString name, void (*operation)());

    void execute();

private:
    QString name;
    void (*operation)();

};

class Lab {

public:
    Lab(QString name);

    Lab* addTask(Task task);
    QString getName();

private:
    QString name;
    QList<Task> tasks;

};

class LabApp : public QWidget {

public:
    LabApp(QList<Lab*> labs);

private:
    QLayout *main_layout;
    QList<Lab*> labs;
    QListWidget *lab_list, *task_list, *file_list;

};

LabApp* createLabApp();

#endif//PROJECT_H
