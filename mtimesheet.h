#ifndef MTIMESHEET_H
#define MTIMESHEET_H

#include <QString>
#include <QDateTime>

class MTimeSheet
{
public:
    MTimeSheet();

    QString tsID;
    QDateTime tsStart;
    QDateTime tsStop;
    QString tsComment;
    QString tsProjectID;
    QString tsEmployeeID;

    bool loadRecord(QString aID);

    bool insertRecord();
    bool updateRecord();
    bool deleteRecord();
};

#endif // MTIMESHEET_H
