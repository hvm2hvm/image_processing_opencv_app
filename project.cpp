#include <QtGui/QHBoxLayout>
#include "project.h"

Task::Task(QString name, void (*operation)()) {
    this->name = name;
    this->operation = operation;
}

void Task::execute() {
    this->operation();
}

Lab::Lab(QString name) {
    this->name = name;
    this->tasks = QList<Task>();
}

Lab* Lab::addTask(Task task) {
    this->tasks.push_back(task);
    return this;
}

QString Lab::getName() {
    return this->name;
}

LabApp::LabApp(QList<Lab*> labs) {
    this->labs = labs;

    main_layout = new QHBoxLayout(this);
    this->setLayout(main_layout);

    this->lab_list = new QListWidget(this);
    this->task_list = new QListWidget(this);
    this->file_list = new QListWidget(this);

    this->main_layout->addWidget(this->lab_list);
    this->main_layout->addWidget(this->task_list);
    this->main_layout->addWidget(this->file_list);

    this->lab_list->clear();
    for (auto i = this->labs.begin(); i != this->labs.end(); ++i) {
        printf("name: %s\n", (*i)->getName().toAscii().data());
        this->lab_list->addItem("hello");
    }
};

QList<Lab*> initialize_labs() {
    QList<Lab*> result;
    result.push_back(Lab("lab 6")
         .addTask(Task("contour points", &lab6_contour_points))
    );

    return result;
}

LabApp* createLabApp() {
    return new LabApp(initialize_labs());
}
