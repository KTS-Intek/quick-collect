#ifndef COMPLITERLISTDIALOG_H
#define COMPLITERLISTDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QShortcut>
#include <QStandardItemModel>

#include "gui-src/mysortfilterproxymodel.h"

namespace Ui {
class CompliterListDialog;
}

class CompliterListDialog : public QDialog
{
    Q_OBJECT


public:
    explicit CompliterListDialog(QStringList listModel, int tag, QStringList listPath2ico, int colCount, const QString separ = "\t", const bool hideRegister = false, QWidget *parent = 0);
    ~CompliterListDialog();

    QString getTableLine(const int &tableRow);
    QString getTableLineReal(const int &row);

    QString getTableLineCurrentIndx();

public slots:
    void onActTakeCurrente();

    void onActSelectAll();

    void onActCopyToClipboard();

    void onActDelete();

private slots:
    void on_buttonBox_accepted();


    void on_checkBox_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_customContextMenuRequested(const QPoint &pos);

signals:
    void selectedString(QString);
    void onListChanged(QStringList,int);

private:
    Ui::CompliterListDialog *ui;

    QStringList lastList;

    bool listChanged;

    int currMode ;

    QShortcut *keyTake;
    QShortcut *keySelectAll;
    QShortcut *keyCopy;
    QShortcut *keyDelete;

     MySortFilterProxyModel *model_proxy;
     QStandardItemModel *model;
     bool hasIco;
     QString lastSepar;
};

#endif // COMPLITERLISTDIALOG_H
