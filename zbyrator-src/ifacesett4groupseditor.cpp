#include "ifacesett4groupseditor.h"
#include "ui_ifacesett4groupseditor.h"

///[!] widgets-shared
#include "zbyrator-src/zbyrifacesett.h"


IfaceSett4groupsEditor::IfaceSett4groupsEditor(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClass(gHelper, parent),
    ui(new Ui::IfaceSett4groupsEditor)
{
    ui->setupUi(this);
}

IfaceSett4groupsEditor::~IfaceSett4groupsEditor()
{
    delete ui;
}

void IfaceSett4groupsEditor::initPage()
{
    clearPage();
    createZbyrIfaceEditor();
}

void IfaceSett4groupsEditor::clearPage()
{
//    ui->rbGlobal->animateClick();
    ui->rbCustom->setChecked(false);
//    ui->rbGlobal->setChecked(true);
    if(!ui->rbGlobal->isChecked())
        ui->rbGlobal->animateClick();

}

void IfaceSett4groupsEditor::setPageSett(const QVariantHash &h)
{
    lPageSett.groupni = h.value("groupni").toString();
    lPageSett.groupsett = h.value("sett").toHash();

    if(lPageSett.groupsett.isEmpty()){
        if(!ui->rbGlobal->isChecked())
        ui->rbGlobal->animateClick();
    }else{
        if(!ui->rbCustom->isChecked())
        ui->rbCustom->animateClick();
    }

    if(lPageSett.isIfaceEditorReady)
        emit setIfaceSettOneGroup(lPageSett.groupsett);

}

void IfaceSett4groupsEditor::setNewSettingsOneGroup(QVariantHash sett)
{

    emit setIfaceSettOneGroup2parent(lPageSett.groupni, sett);

}

void IfaceSett4groupsEditor::onIfaceEditorReady()
{
    lPageSett.isIfaceEditorReady = true;

    emit setIfaceSettOneGroup(lPageSett.groupsett);
}

void IfaceSett4groupsEditor::on_rbGlobal_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void IfaceSett4groupsEditor::on_rbCustom_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void IfaceSett4groupsEditor::createZbyrIfaceEditor()
{
    ZbyrIfaceSett * w = new ZbyrIfaceSett(tr("One group settings"), gHelper,  this);

//    connect(metersListMedium, SIGNAL(setIfaceSett(QVariantHash)), w, SLOT(setPageSett(QVariantHash)) );
    connect(this, SIGNAL(setIfaceSettOneGroup(QVariantHash)), w, SLOT(setPageSett(QVariantHash)));

    connect(w, SIGNAL(setNewSettings(QVariantHash)), this, SLOT(setNewSettingsOneGroup(QVariantHash)));
    connect(w, SIGNAL(sendMeIfaceSett()), this, SLOT(onIfaceEditorReady()) );


    connect(w, SIGNAL(sendMeTheTcpHistory(QLineEdit*))      , this, SIGNAL(sendMeTheTcpHistory(QLineEdit*)));
    connect(w, SIGNAL(sendMeTheM2mHistory(QLineEdit*))      , this, SIGNAL(sendMeTheM2mHistory(QLineEdit*)));

    connect(w, SIGNAL(openEditMacProfileWdgt(bool,QLineEdit*)), this, SIGNAL(openEditMacProfileWdgt(bool,QLineEdit*)));

    connect(w, &ZbyrIfaceSett::closeThisPage, this, &IfaceSett4groupsEditor::hideEditor);
    ui->stackedWidget->addWidget(w);
    ui->stackedWidget->setCurrentIndex(0);
//    return w;
}

void IfaceSett4groupsEditor::on_pbOk_clicked()
{
    setNewSettingsOneGroup(QVariantHash());
    //use global
}

void IfaceSett4groupsEditor::on_pushButton_clicked()
{
    emit hideEditor();
}
