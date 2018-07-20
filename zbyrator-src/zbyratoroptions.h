#ifndef ZBYRATOROPTIONS_H
#define ZBYRATOROPTIONS_H

#include "src/shared/referencewidgetclass.h"

namespace Ui {
class ZbyratorOptions;
}

class ZbyratorOptions : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit ZbyratorOptions(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~ZbyratorOptions();


public slots:
    void initPage();

private slots:
    void onLvOptions_clicked(const QModelIndex &index);

    void on_stackedWidget_currentChanged(int arg1);

private:
    void addItems2model();

    void addThisWdgt2stack(const int &row, const QString &pageName, const QString &realPageName);

    MatildaConfWidget *createPollWdgt();



    Ui::ZbyratorOptions *ui;

};

#endif // ZBYRATOROPTIONS_H
