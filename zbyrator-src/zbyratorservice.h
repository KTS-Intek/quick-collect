#ifndef ZBYRATORSERVICE_H
#define ZBYRATORSERVICE_H

#include "gui-src/matildaconfwidget.h"

namespace Ui {
class ZbyratorService;
}

class ZbyratorService : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit ZbyratorService(GuiHelper *gHelper, QWidget *parent = 0);
    ~ZbyratorService();

    static QStringList getWriteCommands(const bool &enableapi);


signals:
    void lockButtons(bool disable);


    void command4dev(quint16 command, QString args);//pollCode args
    void command4dev(quint16 command, QVariantMap args);//pollCode args


public slots:
    void initPage();

private slots:

    void on_pbResetNetwork_clicked();

    void startPollCode(const quint8 &pollCode);


    void on_pbModemReset_clicked();

    void on_pbEnableApi_clicked();

    void on_pbDisableApi_clicked();

private:
    Ui::ZbyratorService *ui;
    bool lastWasEnableApi;

    QVariantMap tryStartTheOperation(int pollCode);

};

#endif // ZBYRATORSERVICE_H
