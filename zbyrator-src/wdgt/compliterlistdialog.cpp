#include "compliterlistdialog.h"
#include "ui_compliterlistdialog.h"

#include <QMenu>
#include <QKeySequence>
#include <QItemSelectionModel>
#include <QClipboard>
#include <QMessageBox>

#include "src/matilda/guihelper.h"

//==============================================================================================
CompliterListDialog::CompliterListDialog(QStringList listModel, int tag, QStringList listPath2ico, int colCount, const QString separ, const bool hideRegister, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompliterListDialog)
{
    ui->setupUi(this);

    GuiHelper *guiHelper = new GuiHelper(this);
    ui->lineEdit->setClearButtonEnabled(true);
    ui->toolButton->hide();
    lastSepar = separ;
//    /*QStringListModel **/model = new QStringListModel(listModel, ui->listView);
//    /*QSortFilterProxyModel **/model_proxy = new QSortFilterProxyModel(ui->listView);
    hasIco = !listPath2ico.isEmpty();
    model = new QStandardItemModel(ui->tableView);
    model_proxy = new MySortFilterProxyModel(ui->tableView);

    model_proxy->setSourceModel(model);
    ui->tableView->setModel(model_proxy);

    model_proxy->setDynamicSortFilter(true);
    model_proxy->setFilterModeFromTo(0, 2);

    for(int i = 0, iMax = listModel.size(), iMax2 = listPath2ico.size(); i < iMax; i++){
        QList<QStandardItem*> li;
        int cMax = colCount;
        if(hasIco){
            li.append(new QStandardItem(QIcon( (i < iMax2) ? listPath2ico.at(i) : ""), ""));
            cMax--;
        }

        const QStringList l = listModel.at(i).split(separ);
        for(int col = 0, jMax = l.size() ; col < cMax; col++)
            li.append( new QStandardItem( (col < jMax) ? l.at(col) : QString()));

        model->appendRow(li);
    }

    guiHelper->resizeTv2content(ui->tableView);

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), model_proxy, SLOT(setNewFileterStr(QString)) );

    if(ui->checkBox->isChecked())
        model_proxy->setFilterCaseSensitivity(Qt::CaseSensitive);
    else
        model_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    keyTake = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this);
    keySelectAll = new QShortcut(QKeySequence(QKeySequence::SelectAll), this);
    keyCopy = new QShortcut(QKeySequence(QKeySequence::Copy), this);
    keyDelete = new QShortcut(QKeySequence(QKeySequence::Delete), this);

    ui->checkBox->setHidden(hideRegister);


    connect(keyTake, SIGNAL(activated()), SLOT(onActTakeCurrente()) );
    connect(keySelectAll, SIGNAL(activated()), SLOT(onActSelectAll()) );
    connect(keyCopy, SIGNAL(activated()), SLOT(onActCopyToClipboard()) );
    connect(keyDelete, SIGNAL(activated()), SLOT(onActDelete()) );

    lastList = listModel;

    listChanged = false;
    currMode = tag;

}
//==============================================================================================
CompliterListDialog::~CompliterListDialog()
{
    delete ui;
}
//---------------------------------------------------------------------------------------------
QString CompliterListDialog::getTableLine(const int &tableRow)
{
    return getTableLineReal(model_proxy->mapToSource(model_proxy->index(tableRow,0)).row());
}
//---------------------------------------------------------------------------------------------
QString CompliterListDialog::getTableLineReal(const int &row)
{
    if(row < 0)
        return "";
    int c = hasIco ? 1 : 0;
    QStringList l;
    for(int cmax = model->columnCount(); c < cmax; c++)
        l.append(model->item(row, c)->text());
    return l.join(lastSepar);
}
//---------------------------------------------------------------------------------------------
QString CompliterListDialog::getTableLineCurrentIndx()
{
    return getTableLine(ui->tableView->currentIndex().row());
}
//==============================================================================================
void CompliterListDialog::on_buttonBox_accepted()
{
    if(ui->tableView->currentIndex().row() < 0)
        return;
    //            commandList.append(mProxyModel->data(mProxyModel->index(index.row(), 2), Qt::DisplayRole).toString());

    emit selectedString(getTableLineCurrentIndx());

    if(listChanged)
        emit onListChanged(lastList, currMode);
}

//==============================================================================================
void CompliterListDialog::onActTakeCurrente()
{
    on_tableView_doubleClicked(ui->tableView->currentIndex());
}
//==============================================================================================
void CompliterListDialog::onActSelectAll()
{
//    QItemSelectionModel *selModel = new QItemSelectionModel(this);
//            ui->tableView->selectionModel()->select(selRow, QItemSelectionModel::Select | QItemSelectionModel::Rows);

    for(int row = 0, rowMax = ui->tableView->model()->rowCount(); row < rowMax; row++){

        QModelIndex myIndexLeft = ui->tableView->model()->index(row,0); //myList.at(row);

        QItemSelection selRow( myIndexLeft, myIndexLeft);
        ui->tableView->selectionModel()->select(selRow, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}
//==============================================================================================
void CompliterListDialog::onActCopyToClipboard()
{
   const QString s = getTableLineCurrentIndx();
    if(s.isEmpty())
        return;

    //copy to clipboard

    QApplication::clipboard()->setText(s);
}
//==============================================================================================
void CompliterListDialog::onActDelete()
{
    const QModelIndex index = ui->tableView->currentIndex();

    if(index.row() < 0){
        return;
    }

    const QModelIndexList myList = ui->tableView->selectionModel()->selectedRows();

    if(QMessageBox::question(this, window()->windowTitle(), tr("Delete %1 items?").arg(myList.size()), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
        return;

    QList<int> rowList;

    for(int i = 0, iMax = myList.size(); i < iMax; i++){
//        model_proxy->removeRow(myList.at(i).row());
        rowList.append(myList.at(i).row());
    }

    std::sort(rowList.begin(), rowList.end());
    for(int i = rowList.size() - 1; i >= 0; i--){
        model_proxy->removeRow(rowList.at(i));
    }

    lastList.clear();
    listChanged = true;
    for(int row = 0, rowMax = model->rowCount(); row < rowMax; row++ ){
        lastList.append(getTableLineReal(row));
//        lastList.append(ui->listView->model()->data(ui->listView->model()->index(row, 0) , Qt::DisplayRole).toString());
    }
    ui->tableView->clearSelection();
}
//==============================================================================================

void CompliterListDialog::on_checkBox_clicked()
{
        model_proxy->setFilterCaseSensitivity(ui->checkBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
}
//==============================================================================================

void CompliterListDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(index.row() < 0){
        accept();
        return;
    }
    //            commandList.append(mProxyModel->data(mProxyModel->index(index.row(), 2), Qt::DisplayRole).toString());

    emit selectedString(getTableLineCurrentIndx());
    if(listChanged)
        emit onListChanged(lastList, currMode);
    this->accept();
}

void CompliterListDialog::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(ui->tableView);
    QAction *actSelect = new QAction(tr("Choose"), menu);

    QAction *actSelectAll = new QAction(tr("Select all"), menu);
    QAction *actCopyToClipBoard = new QAction(tr("Copy"), menu);
    QAction *actDelete = new QAction(tr("Delete"), menu);
//:/katynko/svg/edit-delete.svg
    actDelete->setIcon(QIcon(":/katynko/svg/edit-delete.svg"));

    actSelect->setIcon(QIcon(":/katynko/svg/edit-select.svg"));
    actSelectAll->setIcon(QIcon(":/katynko/svg/edit-select-all.svg"));
    actCopyToClipBoard->setIcon(QIcon(":/katynko/svg/edit-copy.svg"));


    int rowCount = ui->tableView->model()->rowCount();

    if(rowCount < 1){
        actSelect->setEnabled(false);
        actSelectAll->setEnabled(false);
        actCopyToClipBoard->setEnabled(false);
        actDelete->setEnabled(false);
    }

    connect(actSelect, SIGNAL(triggered()), SLOT(onActTakeCurrente()) );
    connect(actSelectAll, SIGNAL(triggered()), SLOT(onActSelectAll()) );
    connect(actCopyToClipBoard, SIGNAL(triggered()), SLOT(onActCopyToClipboard()) );
    connect(actDelete, SIGNAL(triggered()), SLOT(onActDelete()) );

    menu->addAction(actSelect );
    menu->addAction(actSelectAll);
    menu->addAction(actCopyToClipBoard);
    menu->addAction(actDelete);

    actSelect->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    actSelectAll->setShortcut(QKeySequence(QKeySequence::SelectAll));
    actCopyToClipBoard->setShortcut(QKeySequence(QKeySequence::Copy));
    actDelete->setShortcut(QKeySequence(QKeySequence::Delete));
//    actClear->setShortcutContext(Qt::ApplicationShortcut);


    menu->exec(ui->tableView->mapToGlobal(pos));

    menu->deleteLater();
}
