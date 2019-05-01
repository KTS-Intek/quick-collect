#ifndef IFACESETT4GROUPS_H
#define IFACESETT4GROUPS_H

#include "gui-src/wdgt/referencewidgetclassgui.h"

class IfaceSett4groups : public ReferenceWidgetClassGui
{
    Q_OBJECT
public:
    explicit IfaceSett4groups(GuiHelper *gHelper, QWidget *parent = nullptr);

    static QString getPrettyIfaceSettings(const QVariantHash &h);

    static QStringList getDefaultHeader();

signals:
    void sendMeTheTcpHistory(QLineEdit *le);

    void sendMeTheM2mHistory(QLineEdit *le);

    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );


    void startEditPageSett( QVariantHash h);


    void addIfaceSett2history(QVariantHash h);

public slots:
    void clearPage();

//    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);


    void onEditClick(int srcrow);



    void updateAllPowerCenters();

    void onEditorReady();


    void setIfaceSettOneGroup2parent(QString groupni, QVariantHash sett);

private slots:

    void initPage();


    void addThisMeters(MyListStringList &listRows, const QVariantList &l, const QVariantMap &connSettNi2sett);


private:
    QMap<QString,QString> memo2niMultiElectricity;
    QMap<QString,QString> memo2niMultiWater;

    void createEditor();



};

#endif // IFACESETT4GROUPS_H
