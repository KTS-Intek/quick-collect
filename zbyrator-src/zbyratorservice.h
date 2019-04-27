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

signals:
    void lockButtons(bool disable);


    void command4dev(quint16 command, QString args);//pollCode args
    void command4dev(quint16 command, QVariantMap args);//pollCode args


public slots:
    void initPage();

private slots:
    void on_pbOpenDa_clicked();

    void on_pbResetNetwork_clicked();

    void startPollCode(const quint8 &pollCode);

private:
    Ui::ZbyratorService *ui;

    QVariantMap tryStartTheOperation(int pollCode);

};

#endif // ZBYRATORSERVICE_H
