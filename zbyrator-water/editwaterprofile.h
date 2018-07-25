#ifndef EDITWATERPROFILE_H
#define EDITWATERPROFILE_H

#include "src/matilda-conf-wdgt/confpopupwdgt.h"

namespace Ui {
class EditWaterProfile;
}

class EditWaterProfile : public ConfPopupWdgt
{
    Q_OBJECT

public:
    explicit EditWaterProfile(QWidget *parent = 0);
    ~EditWaterProfile();


signals:
    void onSaveProfile(QString name, QVariantHash profile);

    void setProfile(const QVariantHash &profile);

    void setProfileName(QString name);

private slots:

    void on_pbDefault_clicked();

    void on_pbSave_clicked();

public slots:
    void setProfileSlot(const QVariantHash &profile);


    void onNewProfile(const QVariantHash &profile);

private:
    Ui::EditWaterProfile *ui;

};

#endif // EDITWATERPROFILE_H
