#include <QSqlQuery>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QUuid>
#include <QDebug>
#include <QSqlError>

#include "mtimesheet.h"


MTimeSheet::MTimeSheet()
{

}

bool MTimeSheet::loadRecord(QString aID)
{
    bool result;

    result = false;
    QSqlQuery qry("SELECT * FROM TimeSheets WHERE TimeSheetID='" + aID + "'");
    if (qry.isActive())
    {
        tsID =          qry.value(0).toString();
        tsStart =       qry.value(1).toDateTime();
        tsStop =        qry.value(2).toDateTime();
        tsComment =     qry.value(3).toString();
        tsProjectID =   qry.value(4).toString();
        tsEmployeeID =  qry.value(5).toString();

        result = true;
    }

    return result;
}

bool MTimeSheet::insertRecord()
{
    bool result;
    QSqlQuery query;

    result = false;
    if (query.prepare("INSERT INTO TimeSheets (TimeSheetID, TimeSheetStart, TimeSheetStop, TimeSheetComment, Projects_ProjectID, Employees_EmployeeID)"
                      " VALUES (:TimeSheetID, :TimeSheetStart, :TimeSheetStop, :TimeSheetComment, :ProjectID, :EmployeeID);"))
    {
        tsID = QUuid::createUuid().toString();

        query.bindValue(":TimeSheetID", tsID);
        query.bindValue(":TimeSheetStart", tsStart);
        query.bindValue(":TimeSheetStop", tsStop);
        query.bindValue(":TimeSheetComment", tsComment);
        query.bindValue(":ProjectID", tsProjectID);
        query.bindValue(":EmployeeID", tsEmployeeID);

        query.exec();
        if (!query.isActive())
            qDebug() << "TimeSheet insert failure " << query.lastError();
        else
            result = true;
    }

    return result;
}

bool MTimeSheet::updateRecord()
{
    bool result;
    QSqlQuery query;

    result = false;
    if (query.prepare("UPDATE TimeSheets SET TimeSheetStart=:TimeSheetStart, TimeSheetStop=:TimeSheetStop, TimeSheetComment=:TimeSheetComment, "
                      "Projects_ProjectID=:ProjectID, Employees_EmployeeID=:EmployeeID WHERE TimeSheetID=:TimeSheetID"))
    {
        query.bindValue(":TimeSheetID", tsID);
        query.bindValue(":TimeSheetStart", tsStart);
        query.bindValue(":TimeSheetStop", tsStop);
        query.bindValue(":TimeSheetComment", tsComment);
        query.bindValue(":ProjectID", tsProjectID);
        query.bindValue(":EmployeeID", tsEmployeeID);

        query.exec();
        if (!query.isActive())
            qDebug() << "TimeSheet uupdate failure " << query.lastError();
        else
            result = true;
    }

    return result;
}

bool MTimeSheet::deleteRecord()
{
    bool result;
    QSqlQuery query;

    result = false;
    if (query.prepare("DELETE FROM TimeSheets WHERE TimeSheetID=:TimeSheetID"))
    {
        query.bindValue(":TimeSheetID", tsID);
        query.exec();
        if (!query.isActive())
            qDebug() << "TimeSheet delete failure " << query.lastError();
        else
            result = true;
    }

    return result;
}

