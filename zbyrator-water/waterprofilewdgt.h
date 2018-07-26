#ifndef WATERPROFILEWDGT_H
#define WATERPROFILEWDGT_H

#include <QWidget>
#include <QVariantHash>

namespace Ui {
class WaterProfileWdgt;
}

class WaterProfileWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit WaterProfileWdgt(const bool &enableEditor, QWidget *parent = 0);
    ~WaterProfileWdgt();

    static QStringList getDowNames();

    static QVariantHash getDefault();


    QVariantHash getProfile();

signals:

    void onTableChanged();

    void killCbxDelegate();

    void onSaveProfile(QString name, QVariantHash profile);

    void onNewProfile(QVariantHash profile);


public slots:
    void setProfile(const QVariantHash &profile);

    void setDefaultValues();

    void saveProfile(QString name);

    void updatePteText();

    void updatePteWidth();




private:
    Ui::WaterProfileWdgt *ui;

    struct LastProfileSett
    {
        QStringList dowNames;
        QString anyLocalTxt;
        bool enableEditor;
        LastProfileSett() {}
    } lProfile;
};

#endif // WATERPROFILEWDGT_H
