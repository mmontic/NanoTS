#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QDateTime>

#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void insertTimeSheet ();

public slots:
    void startTSTimer();
    void synchronizeData();
    void displayTime();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;
    QTimer *myTimer;
    QDateTime counterStart;
    QString employeeID;
    bool timeRunning;
};

#endif // MAINWINDOW_H
