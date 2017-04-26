#include <QHBoxLayout>
#include "project.h"

Task::Task(QString name, void (*operation)(char *filePath)) {
    this->name = name;
    this->operation = operation;
}

void Task::execute(QString filePath) {
    this->operation(filePath.toUtf8().data());
}

Task* Task::addFilesDir(QString directory) {
    QDir dir(directory);
    QFileInfoList filesList = dir.entryInfoList(QDir::Files);
    for (auto file = filesList.begin(); file != filesList.end(); ++file) {
        this->files.push_back(new File(
                file->absoluteFilePath(), file->fileName()
        ));
    }
    return this;
}

Lab::Lab(QString name) {
    this->name = name;
    this->tasks = QList<Task*>();
}

Lab* Lab::addTask(Task *task) {
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

    this->lab_list->setSelectionMode(QAbstractItemView::SingleSelection);
    this->task_list->setSelectionMode(QAbstractItemView::SingleSelection);
    this->file_list->setSelectionMode(QAbstractItemView::SingleSelection);

    this->main_layout->addWidget(this->lab_list);
    this->main_layout->addWidget(this->task_list);
    this->main_layout->addWidget(this->file_list);

    this->lab_list->clear();
    for (auto i = this->labs.begin(); i != this->labs.end(); ++i) {
        this->lab_list->addItem((*i)->getName());
    }

    connect(this->lab_list, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(lab_selected(QListWidgetItem*, QListWidgetItem*)));
    connect(this->task_list, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(task_selected(QListWidgetItem*, QListWidgetItem*)));
    connect(this->file_list, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(file_selected(QListWidgetItem*, QListWidgetItem*)));

    this->lab_list->setCurrentRow(this->lab_list->count()-1);
};

void LabApp::lab_selected(QListWidgetItem *current, QListWidgetItem *previous) {
    int index = this->lab_list->currentRow();
    QList<Task*> tasks = this->labs.at(index)->getTasks();

    this->task_list->clear();
    for (auto task = tasks.begin(); task != tasks.end(); ++task) {
        this->task_list->addItem((*task)->getName());
    }

    this->task_list->setCurrentRow(this->task_list->count()-1);
}

void LabApp::task_selected(QListWidgetItem *current, QListWidgetItem *previous) {
    int labIndex = this->lab_list->currentRow();
    QList<Task*> tasks = this->labs.at(labIndex)->getTasks();

    this->file_list->clear();
    int taskIndex = this->task_list->currentRow();
    if (taskIndex < 0) {
        return;
    }
    QList<File*> files = tasks.at(taskIndex)->getFiles();
    for (auto file = files.begin(); file != files.end(); ++file) {
        this->file_list->addItem((*file)->getFileName());
    }
    this->file_list->setCurrentRow(0);
}

void LabApp::file_selected(QListWidgetItem *current, QListWidgetItem *previous) {
    int labIndex = this->lab_list->currentRow();
    QList<Task*> tasks = this->labs.at(labIndex)->getTasks();
    int taskIndex = this->task_list->currentRow();
    if (taskIndex < 0) {
        return;
    }
    QList<File*> files = tasks.at(taskIndex)->getFiles();
    int fileIndex = this->file_list->currentRow();
    if (fileIndex < 0) {
        return;
    }
    Task *task = tasks.at(taskIndex);
    File *file = files.at(fileIndex);

    task->execute(file->getFilePath());
}

QList<Lab*> initialize_labs() {
    QList<Lab*> result;
    result.push_back((new Lab("lab 6"))
         ->addTask((new Task("contour points", &lab6_contour_points))
                   ->addFilesDir("../../Images/lab6"))
    );
    result.push_back((new Lab("lab 8"))
         ->addTask((new Task("compute histogram", &lab8_histogram))
                   ->addFilesDir("../../Images/lab8"))
    );

    return result;
}

LabApp* createLabApp() {
    return new LabApp(initialize_labs());
}
