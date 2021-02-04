#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exporter.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowTitle(tr("Export GPFS"));

    this->resize(400,150);

    QWidget *widget = new QWidget();
    this->setCentralWidget(widget);

    QGridLayout * layout = new QGridLayout;

    QLabel * okinaFileLabel = new QLabel(tr("Fichier CSV OKINA :"));
    okinaFileLineEdit = new QLineEdit(okinaFile);
    connect(okinaFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setOkinaFile(QString)));
    QPushButton * chooseFileButton = new QPushButton(tr("Parcourir..."));
    connect(chooseFileButton, SIGNAL(clicked()), this, SLOT(selectOkinaFile()));
    layout->addWidget(okinaFileLabel, 0, 0);
    layout->addWidget(okinaFileLineEdit, 0, 1);
    layout->addWidget(chooseFileButton, 0, 2);

    QLabel * folderPathLabel = new QLabel(tr("Dossier export GPFS :"));
    folderPathLineEdit = new QLineEdit(folderPath);
    connect(folderPathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setFolderPath(QString)));
    QPushButton * chooseFolderButton = new QPushButton(tr("Parcourir..."));
    connect(chooseFolderButton, SIGNAL(clicked()), this, SLOT(selectFolderPath()));
    layout->addWidget(folderPathLabel, 1, 0);
    layout->addWidget(folderPathLineEdit, 1, 1);
    layout->addWidget(chooseFolderButton, 1, 2);

    QPushButton * exportButton = new QPushButton(tr("Export GPFS"));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportGPFS()));
    layout->addWidget(exportButton, 2, 2);

    widget->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectOkinaFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choisir fichier CSV OKINA"),
                                                 "C:\\",
                                                 tr("Fichier CSV (*.csv)"));
    okinaFile = fileName;
    okinaFileLineEdit->setText(okinaFile);
}

void MainWindow::selectFolderPath()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Choisir dossier export"),
                                                 "C:\\",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    folderPath = folderName;
    folderPathLineEdit->setText(folderPath);
}

void MainWindow::exportGPFS()
{
    Exporter exporter(okinaFile, folderPath);

    try {
        exporter.doExport();

        QMessageBox msgBox;
        msgBox.setText("Export terminé");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();

        close();

        return;
    } catch (QString errorMessage) {
        QMessageBox msgBox;
        msgBox.setText("Erreur lors de l'export");
        msgBox.setDetailedText(errorMessage);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

}
