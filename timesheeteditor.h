#ifndef TIMESHEETEDITOR_H
#define TIMESHEETEDITOR_H

#include <QDialog>

namespace Ui {
class TimeSheetEditor;
}

class TimeSheetEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TimeSheetEditor(QWidget *parent = 0);
    ~TimeSheetEditor();

private:
    Ui::TimeSheetEditor *ui;
};

#endif // TIMESHEETEDITOR_H
