#ifndef ADDEDITMACFORM_H
#define ADDEDITMACFORM_H

#include <QWidget>
#include <zbyrator-src/src/ifacesettloader.h>

namespace Ui {
class AddEditMacForm;
}

class AddEditMacForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddEditMacForm(IfaceSettLoader *connWdgt, const QString &lastLblTxt, QWidget *parent = 0);
    ~AddEditMacForm();
    IfaceSettLoader *ifaceLoader;


signals:
    void setLblTxt(QString lblTxt);

    void showMess(QString mess);

    void openScanWdgt(int openMode, IfaceSettLoader *connWdgt);

public slots:
    void preapryPage(QString lastLblTxt);

    void setThisMacAndObjName(QString mac, QString id);

    void setThisObjNameIpPort(QString name, QString ip, quint16 port);

    void connect2ifaceLoader();

    void updateSettFromIfaceLoader();

    void onTheseLoginPasswd(QString login, QString passwd);

    void onTheseM2mParams(QString ip, int port);

private slots:
    void on_cbxUseCustomService_toggled(bool checked);

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_checkBox_clicked(bool checked);

    void on_tbScan0_clicked();

    void on_tbScan1_clicked();

private:
    bool pageSettFromHash(const QVariantHash &oneObj);

    QVariantHash pageSett2hash(QString &lastError);

    bool messHshIsValid(QByteArray readArr);

    Ui::AddEditMacForm *ui;
    QString startProfileId;
};

#endif // ADDEDITMACFORM_H
