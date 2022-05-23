#ifndef SELECTMETERS4POLLTOPWDGT_H
#define SELECTMETERS4POLLTOPWDGT_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>

namespace Ui {
class SelectMeters4pollTopWdgt;
}

class SelectMeters4pollTopWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit SelectMeters4pollTopWdgt(QWidget *parent = nullptr);
    ~SelectMeters4pollTopWdgt();

    QPushButton *getPbCancel();

    QPushButton *getPbPollAll();

    QPushButton *getPbSelected();

    QPushButton *getPbCheckedOn();

    QCheckBox *getCbxIgnoreExistingData();

    QLabel *getLblDatabase();

    QComboBox *getCbxGroups();

signals:


private:
    Ui::SelectMeters4pollTopWdgt *ui;
};

#endif // SELECTMETERS4POLLTOPWDGT_H
