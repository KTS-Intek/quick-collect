#ifndef ZBYRATOROPTIONS_H
#define ZBYRATOROPTIONS_H

#include "gui-src/referencewidgetclass.h"

namespace Ui {
class ZbyratorOptions;
}

class ZbyratorOptions : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit ZbyratorOptions(GuiHelper *gHelper, QWidget *parent = 0);
    ~ZbyratorOptions();


signals:
    void reloadSettings2ucEmulator();

    void stopDaServer();
    void startDaServer(qint8,quint16); //mode , port

    void command2extension(quint16 extName, quint16 command, QVariant data);

    void command2extensionBBB( quint16 extName, quint16 extCommand, QVariant data);//extId command data

public slots:
    void initPage();

private slots:
    void onLvOptions_clicked(const QModelIndex &index);

//    void on_stackedWidget_currentChanged(int arg1);


private:
    void addItems2model();

    void addThisWdgt2stack(const int &row, const QString &pageName, const QString &realPageName);

    MatildaConfWidget *createPollWdgt();

    MatildaConfWidget *createOptionsWdgt();


    Ui::ZbyratorOptions *ui;

};

#endif // ZBYRATOROPTIONS_H
