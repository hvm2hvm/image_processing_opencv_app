#include <QHBoxLayout>
#include "project.h"

Task::Task(QString name, void (*operation)(char *filePath)) {
    this->name = name;
    this->operation = operation;
}

void Task::execute(QString filePath) {
    this->operation(filePath.toUtf8().data());
}

void addFilesFromDir(QList<File*> &files, QString directory) {
    QDir dir(QString("../../Images/") + directory);
    QFileInfoList filesList = dir.entryInfoList(QDir::Files);
    for (auto file = filesList.begin(); file != filesList.end(); ++file) {
        files.push_back(new File(
                file->absoluteFilePath(), file->fileName()
        ));
    }
}

Task* Task::addFilesDir(QString directory) {
    addFilesFromDir(this->files, directory);
    return this;
}

Lab::Lab(QString name) {
    this->name = name;
    this->tasks = QList<Task*>();
    this->files = QList<File*>();
}

Lab* Lab::addTask(Task *task) {
    this->tasks.push_back(task);
    return this;
}

Lab* Lab::addFilesDir(QString directory) {
    addFilesFromDir(this->files, directory);
    return this;
}

QString Lab::getName() {
    return this->name;
}

LabApp::LabApp(QList<Lab*> labs) {
    this->labs = labs;

    main_layout = new QVBoxLayout(this);
    lists_layout = new QHBoxLayout();
    this->setLayout(main_layout);
    this->setMinimumHeight(500);

    this->lab_list = new QListWidget(this);
    this->task_list = new QListWidget(this);
    this->file_list = new QListWidget(this);
    this->status_label = new QLabel(this);

    this->lab_list->setSelectionMode(QAbstractItemView::SingleSelection);
    this->task_list->setSelectionMode(QAbstractItemView::SingleSelection);
    this->file_list->setSelectionMode(QAbstractItemView::SingleSelection);

    this->main_layout->addItem(this->lists_layout);
    this->main_layout->addWidget(this->status_label);

    this->lists_layout->addWidget(this->lab_list);
    this->lists_layout->addWidget(this->task_list);
    this->lists_layout->addWidget(this->file_list);

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
    int labIndex = this->lab_list->currentRow();
    QList<Task*> tasks = this->labs.at(labIndex)->getTasks();

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

    QList<File*> files = getFiles(labIndex, taskIndex);
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
    QList<File*> files = getFiles(labIndex, taskIndex);
    int fileIndex = this->file_list->currentRow();
    if (fileIndex < 0) {
        return;
    }
    Task *task = tasks.at(taskIndex);
    File *file = files.at(fileIndex);

    this->execute_task(task, file);
}

QList<File*> LabApp::getFiles(int labIndex, int taskIndex) {
    QList<File*> lab_files = this->labs.at(labIndex)->getFiles();
    QList<Task*> tasks = this->labs.at(labIndex)->getTasks();
    QList<File*> task_files = tasks.at(taskIndex)->getFiles();
    return lab_files + task_files;
}

void LabApp::execute_task(Task *task, File *file) {
    cvDestroyAllWindows();

    char buffer[64];
    int64 tick_start = getTickCount();

    task->execute(file->getFilePath());

    int64 tick_end = getTickCount();
    snprintf(buffer, 64, "Operation executed in %7.4fs\n", (tick_end - tick_start) / getTickFrequency());

    this->status_label->setText(buffer);
}

QList<Lab*> initialize_labs() {
    QList<Lab*> result;
    result.push_back((new Lab("Lab 5"))
         ->addTask(new Task("Breadth First Search", &lab5_bfs))
         ->addTask(new Task("Multipass", &lab5_multipass))
         ->addFilesDir("lab5")
    );
    result.push_back((new Lab("Lab 6"))
         ->addTask(new Task("Contour points", &lab6_contour_points))
         ->addTask(new Task("Chain code", &lab6_chain_code))
         ->addTask(new Task("Chain code derivation", &lab6_chain_deriv))
         ->addTask(new Task("Object reconstruction", &lab6_chain_reconstruction))
         ->addFilesDir("lab6")
    );
    result.push_back((new Lab("Lab 7"))
         ->addTask((new Task("Dilation", &lab7_dilation))
                 ->addFilesDir("lab7/1_Dilate"))
         ->addTask((new Task("Erosion", &lab7_erosion))
                 ->addFilesDir("lab7/2_Erode"))
         ->addTask((new Task("Opening", &lab7_open))
                 ->addFilesDir("lab7/3_Open"))
         ->addTask((new Task("Closing", &lab7_close))
                 ->addFilesDir("lab7/4_Close"))
         ->addTask((new Task("Boundary extraction", &lab7_boundary))
                 ->addFilesDir("lab7/5_BoundaryExtraction"))
         ->addTask((new Task("Region filling", &lab7_region_filling))
                 ->addFilesDir("lab7/6_RegionFilling"))
    );
    result.push_back((new Lab("Lab 8"))
         ->addTask(new Task("Compute histogram", &lab8_histogram))
         ->addTask(new Task("Segment image", &lab8_threshold))
         ->addTask(new Task("Histogram scaling", &lab8_histogram_scaling))
         ->addTask(new Task("Histogram sliding", &lab8_histogram_sliding))
         ->addTask(new Task("Gamma correction", &lab8_gamma_correction))
         ->addTask(new Task("Histogram equalization", &lab8_histogram_equalization))
         ->addFilesDir("lab8")
    );
    result.push_back((new Lab("Lab 9"))
         ->addTask(new Task("Custom convolution filter", &lab9_convolution_custom))
         ->addTask(new Task("Mean filter", &lab9_mean_filter))
         ->addTask(new Task("Gaussian filter", &lab9_gaussian_filter))
         ->addTask(new Task("Laplace 4 neighbors", &lab9_laplace_4neighbors))
         ->addTask(new Task("Laplace 8 neighbors", &lab9_laplace_8neighbors))
         ->addTask(new Task("Laplace 4 neighbors and low pass",
                            &lab9_laplace_4neighbors_lowpass))
         ->addTask(new Task("Laplace 8 neighbors and low pass",
                            &lab9_laplace_8neighbors_lowpass))
         ->addTask(new Task("Fourier identity transform", &lab9_fourier_transform))
         ->addTask(new Task("Fourier parameters", &lab9_fourier_parameters))
         ->addTask(new Task("Ideal low/high pass", &lab9_ideal_lowhighpass))
         ->addTask(new Task("Ideal gaussian low/high pass", &lab9_ideal_gaussian_lowhighpass))
         ->addFilesDir("lab9")
    );
    result.push_back((new Lab("Lab 10"))
         ->addTask(new Task("Median filter", &lab10_median_filter))
         ->addTask(new Task("Gaussian filter", &lab10_gaussian_filter))
         ->addTask(new Task("Gaussian 1d filters", &lab10_gaussian_1d_filter))
         ->addFilesDir("lab10")
    );
    result.push_back((new Lab("Lab 11"))
         ->addTask(new Task("Prewitt components", &lab11_gradient_prewitt))
         ->addTask(new Task("Sobel components", &lab11_gradient_sobel))
         ->addTask(new Task("Gradient magnitude", &lab11_gradient_magnitude))
         ->addTask(new Task("Gradient thresholding", &lab11_gradient_thresholding))
         ->addTask(new Task("Canny edge detection", &lab11_canny_edge_detection))
         ->addFilesDir(".")
         ->addFilesDir("lab10")
    );

    return result;
}

LabApp* createLabApp() {
    return new LabApp(initialize_labs());
}
