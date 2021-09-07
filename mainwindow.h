#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void setOkinaFile(QString path){okinaFile = path;}
    void setFolderPath(QString path){folderPath = path;}
    void setSchoolYear(int index){schoolYearIndex = index;}
    void selectOkinaFile();
    void selectFolderPath();
    void exportGPFS();

private:
    Ui::MainWindow *ui;
    QString okinaFile;
    QString folderPath;
    int schoolYearIndex;
    QLineEdit * okinaFileLineEdit;
    QLineEdit * folderPathLineEdit;
};

#endif // MAINWINDOW_H
