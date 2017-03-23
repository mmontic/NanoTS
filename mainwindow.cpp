#include <QMessageBox>
#include <QtDebug>

#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

#include <QTimer>
#include <QElapsedTimer>
#include <QDateTime>
#include <QComboBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "timesheeteditor.h"

//************************************************************************************************
// MainWindow creator
//************************************************************************************************
// Params :
// QWidget *parent      - Parent object of this window
//************************************************************************************************
// Return :
// void
//************************************************************************************************
// Initialize the database conenction and setup the internal timer
//************************************************************************************************

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//
//  Initialize the timer
//
    myTimer = new QTimer ();
    myTimer->setInterval(1000);
    connect (myTimer,SIGNAL (timeout()),this,SLOT(displayTime()));

//
//  Populate projectList combo box
//
    QVariant projectID;
    QString projectName;
    bool employeeFound = false;

    QSqlQuery qryProjects("SELECT * FROM Projects");
    while (qryProjects.next())
    {
        projectID = qryProjects.value(0).toString();
        projectName = qryProjects.value(1).toString();
        ui->projectList->addItem(projectName,projectID);
    }

    QSqlQuery qryEmployees("SELECT EmployeeName, EmployeeID FROM Employees WHERE EmployeeName='Max Monticelli'");
    while (qryEmployees.next())
    {
        if (qryEmployees.value(0).toString() == "Max Monticelli")
        {
            employeeFound = true;
            employeeID = qryEmployees.value(1).toString();
            break;
        }
    }

    if (!employeeFound)
        qDebug() << "Employee not found";
}

//************************************************************************************************
// MainWindow destructor
//************************************************************************************************
// Params :
// None
//************************************************************************************************
// Return :
// void
//************************************************************************************************
// Delete the ui object
//************************************************************************************************

MainWindow::~MainWindow()
{
    if (ui->startButton->text() == "Stop")
        insertTimeSheet ();

    delete ui;
}

//************************************************************************************************
// startTSTime slot
//************************************************************************************************
// Params :
// None
//************************************************************************************************
// Return :
// void
//************************************************************************************************
// Start/Stop the timer depoending on the startButton text, save the information to database
// when the stop button is pressed
//************************************************************************************************

void MainWindow::startTSTimer()
{
    if (ui->startButton->text() == "Start")
    {
        ui->startButton->setText("Stop");
        ui->timeSpent->setText("");

        timeRunning = true;
        myTimer->start();
        counterStart = QDateTime::currentDateTime();
    }
    else
    {
        ui->startButton->setText("Start");

        timeRunning = false;
        myTimer->stop();

        insertTimeSheet ();
    }
}

//************************************************************************************************
// synchronizeData slot
//************************************************************************************************
// Params :
// None
//************************************************************************************************
// Return :
// void
//************************************************************************************************
// Synchronize the data to and from the central database
//************************************************************************************************

void MainWindow::synchronizeData()
{
    qDebug() << "Synchronizing data";
}

//************************************************************************************************
// displayTime slot
//************************************************************************************************
// Params :
// None
//************************************************************************************************
// Return :
// void
//************************************************************************************************
// Display the elapsed time in the timeSpent label
//************************************************************************************************

void MainWindow::displayTime()
{
    ui->timeSpent->setText(QDateTime::fromTime_t(counterStart.secsTo(QDateTime::currentDateTime())).toUTC().toString("hh:mm:ss"));
}

//************************************************************************************************
// insertTimeSheet
//************************************************************************************************
// Params :
// None
//************************************************************************************************
// Return :
// void
//************************************************************************************************
// Insert e time-sheet into the database when stop button is pressed
//************************************************************************************************

void MainWindow::insertTimeSheet ()
{
    QSqlQuery query;

    if (query.prepare("INSERT INTO TimeSheets(TimeSheetID, TimeSheetStart, TimeSheetStop, TimeSheetComment, Projects_ProjectID, Employees_EmployeeID)"
                      " VALUES (:TimeSheetID, :TimeSheetStart, :TimeSheetStop, :TimeSheetComment, :ProjectID, :EmployeeID);"))
    {
        query.bindValue(":TimeSheetID", QUuid::createUuid().toString());
        query.bindValue(":TimeSheetStart", counterStart);
        query.bindValue(":TimeSheetStop", QDateTime::currentDateTime());
        query.bindValue(":TimeSheetComment", ui->comment->text());
        query.bindValue(":ProjectID", ui->projectList->itemData(ui->projectList->currentIndex()));
        query.bindValue(":EmployeeID", employeeID);
        query.exec();
        if (!query.isActive())
            qDebug() << "TimeSheet insert failure " << query.lastError();
    }
}
