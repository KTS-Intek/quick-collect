#ifndef DEFSERIALDIALOG_H
#define DEFSERIALDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class DefSerialDialog;
}

class DefSerialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DefSerialDialog(QStringList list, QStringList listT, QWidget *parent = 0);
    ~DefSerialDialog();

signals:
    void setThisPortAsDefault(QString);

private slots:
    void on_buttonBox_accepted();


private:
    Ui::DefSerialDialog *ui;
};

#endif // DEFSERIALDIALOG_H
