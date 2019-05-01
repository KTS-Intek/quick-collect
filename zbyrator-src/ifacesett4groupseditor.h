#ifndef IFACESETT4GROUPSEDITOR_H
#define IFACESETT4GROUPSEDITOR_H

///[!] widgets-shared
#include "gui-src/referencewidgetclass.h"



namespace Ui {
class IfaceSett4groupsEditor;
}

class IfaceSett4groupsEditor : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit IfaceSett4groupsEditor(GuiHelper *gHelper, QWidget *parent = nullptr);
    ~IfaceSett4groupsEditor();

signals:
    void setIfaceSettOneGroup(QVariantHash sett);


    void setIfaceSettOneGroup2parent(QString groupni, QVariantHash sett);



    void sendMeTheTcpHistory(QLineEdit *le);

    void sendMeTheM2mHistory(QLineEdit *le);

    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );


    void hideEditor();

public slots:
    void initPage();

    void clearPage();

    void setPageSett(const QVariantHash &h);


    void setNewSettingsOneGroup(QVariantHash sett);

    void onIfaceEditorReady();

private slots:
    void on_rbGlobal_clicked();

    void on_rbCustom_clicked();

    void on_pbOk_clicked();

    void on_pushButton_clicked();

private:
    Ui::IfaceSett4groupsEditor *ui;

    void createZbyrIfaceEditor();

    struct MyLastSett
    {
        QString groupni;
        QVariantHash groupsett;
        bool isIfaceEditorReady;
        MyLastSett() : isIfaceEditorReady(false){}
    } lPageSett;
};

#endif // IFACESETT4GROUPSEDITOR_H
