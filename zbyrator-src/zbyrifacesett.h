#ifndef ZBYRIFACESETT_H
#define ZBYRIFACESETT_H

#include "src/shared/matildaconfwidget.h"
#include <QComboBox>

namespace Ui {
class ZbyrIfaceSett;
}

class ZbyrIfaceSett : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit ZbyrIfaceSett(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~ZbyrIfaceSett();

    void setCbxIndx(QComboBox *cbx, const QString &text, const QString &defValue);

    QVariantHash getPageSett(bool &ok, QString &mess, const bool &is4copy);

signals:
    void sendMeIfaceSett();

    void setNewSettings(QVariantHash h);

    void closeThisPage();

    void sendMeTheTcpHistory(QLineEdit *le);

    void sendMeTheM2mHistory(QLineEdit *le);


    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );

public slots:
    void initPage();

    void setPageSett(QVariantHash h);

    void setTcpClientCompliter(QStringList tcpServers);

private slots:
    void on_tbUpdateSerial_clicked();

    void onRbClicked();

    void on_pbRestore_clicked();

    void on_pbSave_clicked();

    void on_cbxAutoUartPort_clicked(bool checked);

    void on_tbUpdateDefSerial_clicked();


    void on_pbM2mEdit_clicked();

    void on_pbM2mAdd_clicked();


    void on_pbTcpHistory_clicked();

    void on_pbM2mHistory_clicked();

private:
    Ui::ZbyrIfaceSett *ui;

    QStringList lastUartList;


};

#endif // ZBYRIFACESETT_H
