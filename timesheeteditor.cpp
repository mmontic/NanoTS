#include "timesheeteditor.h"
#include "ui_timesheeteditor.h"

TimeSheetEditor::TimeSheetEditor(QWidget *parent) : QDialog(parent), ui(new Ui::TimeSheetEditor)
{
    ui->setupUi(this);
}

TimeSheetEditor::~TimeSheetEditor()
{
    delete ui;
}
