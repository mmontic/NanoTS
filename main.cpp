#include <QApplication>
#include <QMessageBox>
#include <QtDebug>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>

#include "mainwindow.h"
#include "main.h"

/***********************************************************************************************
* Check if a file exists
************************************************************************************************
* Params : Path, contain the full path including the filename
************************************************************************************************
* Return : indicate if the file exist or not
************************************************************************************************/

bool fileExists(QString path)
{
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

/***********************************************************************************************
* Build the tables in the local database (SQLITE)
************************************************************************************************
* Params : None
************************************************************************************************
* Return : void
************************************************************************************************/

void buildLocalTables()
{
    QSqlQuery build;
    bool ok;

//
//  Create Projects table
//
    ok = build.exec( QString("CREATE TABLE IF NOT EXISTS Projects ("
                              "ProjectID VARCHAR(38) NOT NULL PRIMARY KEY,"
                              "ProjectName VARCHAR(45) NOT NULL,"
                              "ProjectPriority VARCHAR(10) NOT NULL,"
                              "ProjectStatus VARCHAR(10) NOT NULL,"
                              "ProjectType VARCHAR(10) NOT NULL)"));
    if (!ok)
    {
        qDebug() << "Unable to create table 'Projects' " << build.lastError().text();
        return;
    }
    else
        qDebug() << "Table Projects ok";

    ok = build.exec( QString("CREATE UNIQUE INDEX ProjectName_Unique ON Projects (ProjectName)"));
    if (!ok)
    {
        qDebug() << "Unable to create unique key for table 'Projects' " << build.lastError().text();
        return;
    }
    else
        qDebug() << "Indexes on Projects ok";

//
//  Create Calendars table
//
    ok = build.exec( QString("CREATE TABLE IF NOT EXISTS Calendars ("
                            "CalendarID VARCHAR(38) NOT NULL PRIMARY KEY,"
                            "CalendarName VARCHAR(45) NOT NULL)"));
    if (!ok)
    {
        qDebug() << "Unable to create table 'Calendars' " << build.lastError().text();
        return;
    }
    else
        qDebug() << "Table Calendars ok";

    ok = build.exec( QString("CREATE UNIQUE INDEX CalendarName_Unique ON Calendars (CalendarName)"));
    if (!ok)
    {
        qDebug() << "Unable to create unique key for table 'Calendars' " << build.lastError().text();
        return;
    }
    else
        qDebug() << "Indexes on Calendars ok";

//
//  Create Employees table
//
    ok = build.exec( QString("CREATE TABLE IF NOT EXISTS Employees ("
                             "EmployeeID VARCHAR(38) NOT NULL PRIMARY KEY,"
                             "EmployeeName VARCHAR(45) NOT NULL,"
                             "Calendars_CalendarID VARCHAR(38) NOT NULL)"));
    if (!ok)
    {
        qDebug() << "Unable to create table 'Employees' " << build.lastError().text();
        return;
    }
    else
        qDebug() << "Table Employees ok";

    ok = build.exec( QString("CREATE UNIQUE INDEX EmployeeName_Unique ON Employees (EmployeeName)"));
    if (!ok)
    {
        qDebug() << "Unable to create unique key for table 'Employees' " << build.lastError().text();
        return;
    }
    else
        qDebug() << "Indexes on Employees ok";

//
//  Create CalendarDetails table
//
    ok = build.exec( QString("CREATE TABLE IF NOT EXISTS CalendarDetails ("
                             "CalendarDetailID VARCHAR(38) NOT NULL,"
                             "CalendarDetailDate DATETIME NOT NULL,"
                             "CalendarDetailType VARCHAR(10) NOT NULL,"
                             "Calendars_CalendarID VARCHAR(38) NOT NULL, PRIMARY KEY (CalendarDetailID, Calendars_CalendarID))"));
    if (!ok)
    {
        qDebug() << "Unable to create table 'CalendarDetails' " << build.lastError().text();
        return;
    }
    else
        qDebug() << "Table CalendarDetails ok";


//
//  Create CalendarDetails table
//
    ok = build.exec( QString("CREATE TABLE IF NOT EXISTS TimeSheets ("
                             "TimeSheetID VARCHAR(38) NOT NULL,"
                             "TimeSheetStart DATETIME NOT NULL,"
                             "TimeSheetStop DATETIME NOT NULL,"
                             "TimeSheetComment VARCHAR(80) NOT NULL,"
                             "Projects_ProjectID VARCHAR(38) NOT NULL,"
                             "Employees_EmployeeID VARCHAR(38) NOT NULL,"
                             "PRIMARY KEY (TimeSheetID, Projects_ProjectID, Employees_EmployeeID))"));
    if (!ok)
    {
        qDebug() << "Unable to create table 'TimeSheets' " << build.lastError().text();
        return;
    }
    else
        qDebug() << "Table TimeSheets ok";
}

/***********************************************************************************************
* Synchronize the local and central databases
************************************************************************************************
* Params : pointers to local and central database objects
************************************************************************************************
* Return : void
************************************************************************************************/

void synchronizeDatabase(QSqlDatabase *local, QSqlDatabase *central)
{
    // Import missing records for Employees, Calendars and Projects from Central database
    // Export not yet transfered TimeSheets to Central database

    bool ok;
    QString projectID;
    QString projectName;
    QString projectPriority;
    QString projectStatus;
    QString projectType;

    QSqlQuery findQry("",*local);
    QSqlQuery insQry("",*local);

    QSqlQuery findAdm("",*central);
    QSqlQuery insAdm("",*central);

    qDebug() << "Synchronize database.";

//
//  Projects
//
    qDebug() << "  > Projects";

    QSqlQuery qryProjects("SELECT ProjectID, ProjectName, ProjectPriority, ProjectStatus, ProjectType FROM Projects",*central);
    while (qryProjects.next())
    {
        projectID = qryProjects.value(0).toString();
        projectName = qryProjects.value(1).toString();
        projectPriority = qryProjects.value(2).toString();
        projectStatus = qryProjects.value(3).toString();
        projectType = qryProjects.value(4).toString();

        ok = findQry.exec("SELECT * FROM Projects WHERE ProjectID='" + projectID + "'");
        if (ok && !findQry.next())
        {
            if (insQry.prepare("INSERT INTO Projects (ProjectID, ProjectName, ProjectPriority, ProjectStatus, ProjectType) "
                           "VALUES (:ProjectID, :ProjectName, :ProjectPriority, :ProjectStatus, :ProjectType);"))
            {
                insQry.bindValue(":ProjectID", projectID);
                insQry.bindValue(":ProjectName", projectName);
                insQry.bindValue(":ProjectPriority", projectPriority);
                insQry.bindValue(":ProjectStatus", projectStatus);
                insQry.bindValue(":ProjectType", projectType);
                insQry.exec();
                if (!insQry.isActive())
                    qDebug() << "Project synchro failure " << insQry.lastError();
            }
        }
    }

//
//  Calendars
//
    qDebug() << "  > Calendars";

    QSqlQuery qryCalendarHead("SELECT CalendarID, CalendarName FROM Calendars",*central);
    while (qryCalendarHead.next())
    {
        QString calendarID = qryCalendarHead.value(0).toString();
        QString calendarName = qryCalendarHead.value(1).toString();

        ok = findQry.exec("SELECT * FROM Calendars WHERE CalendarID='" + calendarID + "'");
        if (ok && !findQry.next())
        {
            if (insQry.prepare("INSERT INTO Calendars (CalendarID, CalendarName) VALUES (:CalendarID, :CalendarName);"))
            {
                insQry.bindValue(":CalendarID", calendarID);
                insQry.bindValue(":CalendarName", calendarName);
                insQry.exec();
                if (!insQry.isActive())
                    qDebug() << "Calendar synchro failure " << insQry.lastError();

            }
        }
    }

    qDebug() << "  > Calendar lines";

    QSqlQuery qryCalendarLine("SELECT CalendarDetailID, CalendarDetailDate, CalendarDetailType, Calendars_CalendarID FROM CalendarDetails",*central);
    while (qryCalendarLine.next())
    {
        QString calendarDetailID = qryCalendarLine.value(0).toString();
        QDateTime calendarDetailDate = qryCalendarLine.value(1).toDateTime();
        QString calendarDetailType = qryCalendarLine.value(2).toString();
        QString calendarID = qryCalendarLine.value(3).toString();

        ok = findQry.exec("SELECT * FROM CalendarDetails WHERE CalendarDetailID='" + calendarDetailID + "'");
        if (ok && !findQry.next())
        {
            if (insQry.prepare("INSERT INTO CalendarDetails (CalendarDetailID, CalendarDetailDate, CalendarDetailType, Calendars_CalendarID) "
                               "VALUES (:CalendarDetailID, :CalendarDetailDate, :CalendarDetailType, :CalendarID);"))
            {
                insQry.bindValue(":CalendarDetailID", calendarDetailID);
                insQry.bindValue(":CalendarDetailDate", calendarDetailDate);
                insQry.bindValue(":CalendarDetailType", calendarDetailType);
                insQry.bindValue(":CalendarID", calendarID);
                insQry.exec();
                if (!insQry.isActive())
                    qDebug() << "Calendar detail synchro failure " << insQry.lastError();

            }
        }
    }

//
//  Employees
//
    qDebug() << "  > Employees";

    QSqlQuery qryEmployee("SELECT EmployeeID, EmployeeName, Calendars_CalendarID FROM Employees",*central);
    while (qryEmployee.next())
    {
        QString employeeID = qryEmployee.value(0).toString();
        QString employeeName = qryEmployee.value(1).toString();
        QString calendarID = qryEmployee.value(2).toString();

        ok = findQry.exec("SELECT EmployeeID FROM Employees WHERE EmployeeID='" + employeeID + "'");
        if (ok && !findQry.next())
        {
            if (insQry.prepare("INSERT INTO Employees (EmployeeID, EmployeeName, Calendars_CalendarID) VALUES (:EmployeeID, :EmployeeName, :CalendarID);"))
            {
                insQry.bindValue(":EmployeeID", employeeID);
                insQry.bindValue(":ProjectName", employeeName);
                insQry.bindValue(":CalendarID", calendarID);
                insQry.exec();
                if (!insQry.isActive())
                    qDebug() << "Employee synchro failure " << insQry.lastError();
            }
        }
    }

//
//  Export time sheets
//
    qDebug() << "  > Timesheets";

    QSqlQuery qryTS("SELECT TimeSheetID, TimeSheetStart, TimeSheetStop, TimeSheetComment, Projects_ProjectID, Employees_EmployeeID FROM TimeSheets",*local);
    while (qryTS.next())
    {
        QString tsID = qryTS.value(0).toString();
        QDateTime tsStart = qryTS.value(1).toDateTime();
        QDateTime tsStop = qryTS.value(2).toDateTime();
        QString tsComment = qryTS.value(3).toString();
        QString tsProjectID = qryTS.value(4).toString();
        QString tsEmployeeID = qryTS.value(5).toString();

        ok = findAdm.exec("SELECT TimeSheetID FROM TimeSheets WHERE TimeSheetID='" + tsID + "'");
        if (ok && !findAdm.next())
        {
            if (insAdm.prepare("INSERT INTO TimeSheets(TimeSheetID, TimeSheetStart, TimeSheetStop, TimeSheetComment, Projects_ProjectID, Employees_EmployeeID)"
                              " VALUES (:TimeSheetID, :TimeSheetStart, :TimeSheetStop, :TimeSheetComment, :ProjectID, :EmployeeID);"))
            {
                insAdm.bindValue(":TimeSheetID", tsID);
                insAdm.bindValue(":TimeSheetStart", tsStart);
                insAdm.bindValue(":TimeSheetStop", tsStop);
                insAdm.bindValue(":TimeSheetComment", tsComment);
                insAdm.bindValue(":ProjectID", tsProjectID);
                insAdm.bindValue(":EmployeeID", tsEmployeeID);
                insAdm.exec();
                if (!insAdm.isActive())
                    qDebug() << "Timesheet synchro failure " << insQry.lastError();
            }
        }
    }
}

/***********************************************************************************************
* Open the local (SQLITE) database on the local machine
************************************************************************************************
* Params : None
************************************************************************************************
* Return : void
************************************************************************************************/

void openLocalDatabase(QApplication *a)
{
    QString path = a->applicationDirPath() + "/nanoTS.db";

//
//  Check if local database file exist
//
    if (!fileExists(path))
    {
//
//  Create the database file
//
        dbLocal = QSqlDatabase::addDatabase("QSQLITE");
        dbLocal.setDatabaseName(path);
        if (!dbLocal.open())
            qDebug() << "Unable to open database";
        else
        {
//
//  Build the table structure inside
//
            buildLocalTables();
            qDebug() << "Database created";
        }
    }
    else
    {
//
//  If the file exist, open the database
//
        QSqlDatabase dbLocal = QSqlDatabase::addDatabase("QSQLITE");
        dbLocal.setDatabaseName(path);
        if (!dbLocal.open())
            qDebug() << "Database already exists";
    }
}

/***********************************************************************************************
* Open the central (MYSQL) database on the server
************************************************************************************************
* Params : None
************************************************************************************************
* Return : indicate if the central database is open (found) or not
************************************************************************************************/

bool openCentralDatabase()
{
//
//  Open the central database
//
    dbCentral = QSqlDatabase::addDatabase("QMYSQL","ADMIN");
    dbCentral.setHostName("localhost");
    dbCentral.setDatabaseName("nanoTSAdmin");
    dbCentral.setUserName("root");

    return dbCentral.open();
}

/***********************************************************************************************
* Main entry point
************************************************************************************************
* Params : argc, count of parameters
*          argv, parameter list
************************************************************************************************
* Return : error code
************************************************************************************************/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    openLocalDatabase(&a);

    if (openCentralDatabase())
        synchronizeDatabase(&dbLocal, &dbCentral);

    MainWindow w;

    w.show();
    return a.exec();
}
