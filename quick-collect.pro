#-------------------------------------------------
#
# Project created by QtCreator 2018-07-14T10:52:43
#
#-------------------------------------------------

QT       += core gui network printsupport xml svg charts qml quick positioning location sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = quick-collect
TEMPLATE = app

android:{
QT += androidextras
#SOURCES += snif-android-src/qtandroidftdi.cpp
#HEADERS += snif-android-src/qtandroidftdi.h
#DEFINES += ADDANDROIDFTDIUART=1
}


win32 {
QT += axcontainer
DEFINES += BUILDTIME=\\\"$$system('echo %time%')\\\"
DEFINES += BUILDDATE=\\\"$$system('echo %date%')\\\"
} else {
DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M:%S')\\\"
DEFINES += BUILDDATE=\\\"$$system(date '+%Y/%m/%d')\\\"
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APPLCTN_NAME=\\\"quick-collect\\\"
DEFINES += IS_ZBYRATORWDGT
DEFINES += IS_ZBYRATOR
DEFINES += VERSION_4_PC
DEFINES += DISABLE_METERSCHEDULER
DEFINES += HASSERIALLIB
DEFINES += ENABLE_EXTSUPPORT_OF_IFACES
DEFINES += DISABLE_UART_PRIORITY


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        qcmainwindow.cpp \
    main-pgs/langdialog.cpp \
    src/meter/zbyratorfilehelper.cpp \
    src/matilda/lastdevinfo.cpp \
    src/geo/coordinateobject.cpp \
    src/widgets/tableheaders.cpp \
    main-pgs/duplicateswdiget.cpp \
    main-pgs/filterwidgetform.cpp \
    src/matilda/matildaprotocolhelper.cpp \
    src/printer/myprintpreviewdlg.cpp \
    src/meter/meterpluginloader.cpp \
    matilda-bbb-src/shared/pathsresolver.cpp \
    matilda-bbb-src/shared/runprocess.cpp \
    src/meter/meteroperations.cpp \
    src/shared/compresshelper.cpp \
    matilda-bbb-src/shared/macaddresshelper.cpp \
    src/shared/sharedmemoprotocolhelper.cpp \
    src/meter/numberconvertation.cpp \
    matilda-bbb-src/shared/ifacemanagerhelper.cpp \
    src/shared/sharedmemohelper.cpp \
    zbyrator-src/createtoolbar.cpp \
    dataconcetrator-pgs/meterlistwdgt.cpp \
    map-pgs/mapwidget.cpp \
    dataconcetrator-pgs/import-from-file/finalcheckwdgt.cpp \
    dataconcetrator-pgs/import-from-file/importfromfilecolumnwdgt.cpp \
    dataconcetrator-pgs/import-from-file/importfromfilemodel2modelwdgt.cpp \
    dataconcetrator-pgs/import-from-file/importfromfilesheetwdgt.cpp \
    dataconcetrator-pgs/import-from-file/importfromfilewdgt.cpp \
    dataconcetrator-pgs/import-from-file/importmemowdgt.cpp \
    dataconcetrator-pgs/import-from-file/readmatildabffile.cpp \
    dataconcetrator-pgs/import-from-file/readmeterlistfile.cpp \
    dataconcetrator-pgs/import-from-file/readodsfiles.cpp \
    dataconcetrator-pgs/import-from-file/readtextfiles.cpp \
    dataconcetrator-pgs/import-from-file/readxlsxfiles.cpp \
    dataconcetrator-pgs/import-from-file/selectcodecandseparwdgt.cpp \
    qml-geo/qml2cpp.cpp \
    qml-geo/qmlitemmodel.cpp \
    qml-geo/settloaderqmlgeo.cpp \
    dataconcetrator-pgs/addeditmeterwdgt.cpp \
    dataconcetrator-pgs/editenergywidget.cpp \
    src/matilda-conf-wdgt/confpopupwdgt.cpp \
    src/matilda-conf-wdgt/movetoolbutton.cpp \
    src/matilda-conf-wdgt/pushbuttonv2.cpp \
    src/meter/meternigenerator.cpp \
    src/meter/meterpluginmanager.cpp \
    src/meter/meterpluginsloadhelper.cpp \
    dataconcetrator-pgs/databasewdgt.cpp \
    dataconcetrator-pgs/dbdataform.cpp \
    dataconcetrator-pgs/dbdataformcalcconsumption.cpp \
    dataconcetrator-pgs/dbdatafromsmpl.cpp \
    dataconcetrator-pgs/meterjournalform.cpp \
    template-pgs/chartcontrols.cpp \
    template-pgs/selectdtwdgt.cpp \
    src/matilda/balancemanager.cpp \
    src/firefly/callout.cpp \
    src/meter/meterstatehelper.cpp \
    src/matilda/balancehelper.cpp \
    template-pgs/smplptewdgt.cpp \
    template-pgs/searchwdgt.cpp \
    zbyrator-src/ktsconnectwdgt.cpp \
    zbyrator-src/startexchange.cpp \
    zbyrator-src/relaywdgt.cpp \
    zbyrator-src/metersdatetime.cpp \
    zbyrator-src/setmeteraddress.cpp \
    zbyrator-src/checkconnectiontoolwdgt.cpp \
    zbyrator-src/zbyratorservice.cpp \
    zbyrator-src/startpagepoll.cpp \
    zbyrator-src/zbyrifacesett.cpp \
    info-pgs/zbyratortasks.cpp \
    info-pgs/statisticofexchangewdgt.cpp \
    zbyrator-src/zbyratorprocessmanager.cpp \
    src/zbyrator-v2/addpolldata2dbobject.cpp \
    src/zbyrator-v2/dataprocessinghelper.cpp \
    src/zbyrator-v2/metermanager.cpp \
    src/zbyrator-v2/meterscheduler.cpp \
    src/zbyrator-v2/meterschedulerhelper.cpp \
    src/zbyrator-v2/myucmmeters.cpp \
    src/zbyrator-v2/quickpollhelper.cpp \
    src/zbyrator-v2/thelordofmeters.cpp \
    src/zbyrator-v2/thelordofmetershelper.cpp \
    src/zbyrator-v2/zbyratordatatypehelper.cpp \
    src/zbyrator-v2/zbyratorstatistichelper.cpp \
    src/task/zbyratortasksharedmemo.cpp \
    src/meter/zbyratorfilesetthelper.cpp \
    matilda-bbb-src/database/sqlitelocalclient.cpp \
    matilda-bbb-src/shared/matildadatatypehelper.cpp \
    sqlite-medium-src/dbfirstquery.cpp \
    sqlite-medium-src/sqlitemediumlocalserver.cpp \
    sqlite-medium-src/sqlitemediumlocalsocket.cpp \
    src/matilda/settloader4matildadefaults.cpp \
    src/matilda/serialporthelper.cpp \
    zbyrator-src/settloader4matildaemulator.cpp \
    zbyrator-src/platformdependclass.cpp \
    src/meter/add2dbhelper.cpp \
    src/meter/zbyratorhelper.cpp \
    src/meter/pollenergyhelper.cpp \
    src/task/mytaskhelper.cpp \
    src/shared/smartscheduletimer.cpp \
    src/meter/defparams4zbyrator.cpp \
    matilda-bbb-src/shared/argsdecodehelper.cpp \
    src/meter/meterpluginhelper.cpp \
    src/emb/conf2modem.cpp \
    src/emb/conf2modemhelper.cpp \
    src/shared/sharedmemowriter.cpp \
    src/shared/readwriteiodevice.cpp \
    matilda-bbb-src/database/sharedatabase.cpp \
    matilda-bbb-src/database/matildasqlhelper.cpp \
    matilda-bbb-src/database/sqlitemediumerr.cpp \
    matilda-bbb-src/shared/readjsonhelper.cpp \
    zbyrator-src/zbyrmeterlistmedium.cpp \
    zbyrator-src/src/zbyrtableheaders.cpp \
    src/widgets/selectionchecker.cpp \
    zbyrator-src/wdgt/defserialdialog.cpp \
    zbyrator-src/wdgt/compliterlistdialog.cpp \
    zbyrator-src/wdgt/addeditmacform.cpp \
    zbyrator-src/src/ifacesettloader.cpp \
    main-pgs/custommessagewidget.cpp \
    main-pgs/scanipwidget.cpp \
    src/matilda/classmanagerudpscanner.cpp \
    src/matilda/udpscanner.cpp \
    src/matilda/udpscannertcpsocket.cpp \
    src/shared/networkconverthelper.cpp \
    src/m2m-service/svahaserviceconnector.cpp \
    src/m2m-service/svahasocket.cpp \
    src/emb/checkcurrport.cpp \
    src/shared/ifacehelper.cpp \
    zbyrator-src/src/zbyratordatacalculation.cpp \
    src/matilda/classmanagerhelper.cpp \
    prepaid-pgs/ifaceindicationwdgt.cpp \
    appversion.cpp \
    zbyrator-src/zbyratoroptions.cpp \
    zbyrator-src/options/databasesettings.cpp \
    zbyrator-src/options/ktsconnectmode.cpp \
    zbyrator-src/options/ucemulator.cpp \
    dataconcetrator-pgs/pollwdgt.cpp \
    zbyrator-src/options-src/pollwdgtmom.cpp \
    main-pgs/optionswdgt.cpp \
    zbyrator-src/selectmeters4poll.cpp \
    zbyrator-src/src/getreadymetersdata.cpp \
    zbyrator-src/src/zbyratortasksmedium.cpp \
    matilda-bbb-src/decodematildaprotocolv5helper.cpp \
    zbyrator-src/src/startexchangehelper.cpp \
    zbyrator-src/src/zbyratordatabasemedium.cpp \
    src/matilda/classmanagerdatabasereader.cpp \
    zbyrator-src/src/zbyratorucmedium.cpp \
    matilda-bbb-src/database/ucondatabasereader.cpp \
    matilda-bbb-src/matildaprotocolinitstep.cpp \
    matilda-bbb-src/shared/ineedmoretimeobject.cpp \
    matilda-bbb-src/shared/matildaprotocolhelpersmpl.cpp \
    matilda-bbb-src/database/sqlitelocalclienthelper.cpp \
    zbyrator-water/watersleepscheduler.cpp \
    zbyrator-water/waterprofilewdgt.cpp \
    template-pgs/comboboxdelegate.cpp \
    zbyrator-water/editwaterprofile.cpp \
    src/matilda/matildadatakeys.cpp \
    src/shared/compressfilehelper.cpp \
    dataconcetrator-pgs/src/dbdatafromsmplhelper.cpp \
    dataconcetrator-pgs/dbdatafrommodelhelper.cpp \
    src/firefly/fireflyglobal.cpp \
    src/shared/sharedmemowritelater.cpp \
    dataconcetrator-pgs/src/getmetersnis.cpp \
    zbyrator-water/src/watersleepschedulesaver.cpp \
    zbyrator-water/waterscheduleeditorpro.cpp \
    src/zbyrator-v2/watermeterhelper.cpp \
    src/zbyrator-v2/activesleepprofiles.cpp \
    src/task/taskmanagerv3.cpp \
    src/zbyrator-v2/metersloader.cpp \
    src/matilda-conf/protocol5togui.cpp \
    zbyrator-water/watersleepscheduleradder.cpp \
    src/zbyrator-v2/activesleepprofileshelper.cpp \
    src/matilda-conf/securecopypast.cpp \
    src/matilda-conf/uconloginpashelper.cpp \
    src/task/taskmanagementsett.cpp \
    gui-src/chart/dbchartview.cpp \
    gui-src/chart/dbzoomchart.cpp \
    gui-src/chart/dbzoomchartview.cpp \
    gui-src/emailhelper.cpp \
    gui-src/escwidget.cpp \
    gui-src/globalhelper.cpp \
    gui-src/guihelper.cpp \
    gui-src/guisett4all.cpp \
    gui-src/listviewhelper.cpp \
    gui-src/matildacachehelper.cpp \
    gui-src/matildaconfwidget.cpp \
    gui-src/mtreemodelproxy.cpp \
    gui-src/mysortfilterproxymodel.cpp \
    gui-src/printerhelper.cpp \
    gui-src/printimagehelper.cpp \
    gui-src/qelidablelabel.cpp \
    gui-src/referencewidgetclass.cpp \
    gui-src/settloader.cpp \
    gui-src/showmesshelper4wdgt.cpp \
    gui-src/stackwidgethelper.cpp \
    gui-src/standarditemmodelhelper.cpp \
    gui-src/tableviewhelper.cpp \
    gui-src/treeitem.cpp \
    gui-src/treemodel.cpp \
    src/widgets/tablevalueinserter.cpp \
    template-pgs/integereditdlg.cpp \
    template-pgs/cbxeditdlg.cpp \
    src/widgets/ifacelabel.cpp


HEADERS += \
        qcmainwindow.h \
    main-pgs/langdialog.h \
    src/meter/zbyratorfilehelper.h \
    src/matilda/lastdevinfo.h \
    src/geo/coordinateobject.h \
    src/printer/qrcodegeneratortypes.h \
    src/matilda/classmanagertypes.h \
    src/widgets/tableheaders.h \
    main-pgs/duplicateswdiget.h \
    main-pgs/filterwidgetform.h \
    src/matilda/matildaprotocolhelper.h \
    src/printer/myprintpreviewdlg.h \
    src/meter/meterpluginloader.h \
    matilda-bbb-src/shared/pathsresolver.h \
    matilda-bbb-src/shared/runprocess.h \
    src/meter/meteroperations.h \
    src/shared/compresshelper.h \
    matilda-bbb-src/shared/macaddresshelper.h \
    src/shared/sharedmemoprotocolhelper.h \
    src/meter/numberconvertation.h \
    matilda-bbb-src/shared/ifacemanagerhelper.h \
    src/shared/sharedmemohelper.h \
    zbyrator-src/createtoolbar.h \
    dataconcetrator-pgs/meterlistwdgt.h \
    map-pgs/mapwidget.h \
    dataconcetrator-pgs/import-from-file/finalcheckwdgt.h \
    dataconcetrator-pgs/import-from-file/importfromfilecolumnwdgt.h \
    dataconcetrator-pgs/import-from-file/importfromfilemodel2modelwdgt.h \
    dataconcetrator-pgs/import-from-file/importfromfilesheetwdgt.h \
    dataconcetrator-pgs/import-from-file/importfromfilewdgt.h \
    dataconcetrator-pgs/import-from-file/importmemowdgt.h \
    dataconcetrator-pgs/import-from-file/readmatildabffile.h \
    dataconcetrator-pgs/import-from-file/readmeterlistfile.h \
    dataconcetrator-pgs/import-from-file/readodsfiles.h \
    dataconcetrator-pgs/import-from-file/readtextfiles.h \
    dataconcetrator-pgs/import-from-file/readxlsxfiles.h \
    dataconcetrator-pgs/import-from-file/selectcodecandseparwdgt.h \
    qml-geo/qml2cpp.h \
    qml-geo/qmlitemmodel.h \
    qml-geo/settloaderqmlgeo.h \
    dataconcetrator-pgs/addeditmeterwdgt.h \
    dataconcetrator-pgs/editenergywidget.h \
    src/matilda-conf-wdgt/confpopupwdgt.h \
    src/matilda-conf-wdgt/movetoolbutton.h \
    src/matilda-conf-wdgt/pushbuttonv2.h \
    src/meter/meternigenerator.h \
    src/meter/meterplugin.h \
    src/meter/meterpluginmanager.h \
    src/meter/meterpluginsloadhelper.h \
    dataconcetrator-pgs/databasewdgt.h \
    dataconcetrator-pgs/dbdataform.h \
    dataconcetrator-pgs/dbdataformcalcconsumption.h \
    dataconcetrator-pgs/dbdataformdefy.h \
    dataconcetrator-pgs/dbdatafromsmpl.h \
    dataconcetrator-pgs/meterjournalform.h \
    template-pgs/chartcontrols.h \
    template-pgs/selectdtwdgt.h \
    src/matilda/balancemanager.h \
    src/firefly/callout.h \
    src/meter/meterstatehelper.h \
    src/matilda/balancehelper.h \
    template-pgs/smplptewdgt.h \
    template-pgs/searchwdgt.h \
    zbyrator-src/ktsconnectwdgt.h \
    zbyrator-src/startexchange.h \
    zbyrator-src/relaywdgt.h \
    zbyrator-src/metersdatetime.h \
    zbyrator-src/setmeteraddress.h \
    zbyrator-src/checkconnectiontoolwdgt.h \
    zbyrator-src/zbyratorservice.h \
    zbyrator-src/startpagepoll.h \
    zbyrator-src/zbyrifacesett.h \
    info-pgs/zbyratortasks.h \
    info-pgs/statisticofexchangewdgt.h \
    zbyrator-src/zbyratorprocessmanager.h \
    src/zbyrator-v2/addpolldata2dbobject.h \
    src/zbyrator-v2/dataprocessinghelper.h \
    src/zbyrator-v2/metermanager.h \
    src/zbyrator-v2/meterscheduler.h \
    src/zbyrator-v2/meterschedulerhelper.h \
    src/zbyrator-v2/myucmmeters.h \
    src/zbyrator-v2/quickpollhelper.h \
    src/zbyrator-v2/thelordofmeters.h \
    src/zbyrator-v2/thelordofmetershelper.h \
    src/zbyrator-v2/zbyratordatatypehelper.h \
    src/zbyrator-v2/zbyratorstatistichelper.h \
    src/task/zbyratortasksharedmemo.h \
    src/zbyrator-v2/zbyratortypesv2.h \
    src/meter/zbyratorfilesetthelper.h \
    matilda-bbb-src/database/sqlitelocalclient.h \
    matilda-bbb-src/shared/matildadatatypehelper.h \
    sqlite-medium-src/dbfirstquery.h \
    sqlite-medium-src/sqlitemediumlocalserver.h \
    sqlite-medium-src/sqlitemediumlocalsocket.h \
    src/matilda/settloader4matildadefaults.h \
    src/matilda/settloader4matildakeys.h \
    src/matilda/serialporthelper.h \
    zbyrator-src/settloader4matildaemulator.h \
    zbyrator-src/platformdependclass.h \
    src/meter/add2dbhelper.h \
    src/meter/zbyratorhelper.h \
    src/meter/pollenergyhelper.h \
    src/task/mytaskhelper.h \
    src/shared/smartscheduletimer.h \
    src/meter/defparams4zbyrator.h \
    matilda-bbb-src/shared/argsdecodehelper.h \
    src/meter/meterpluginhelper.h \
    src/emb/conf2modem.h \
    src/emb/conf2modemhelper.h \
    src/emb/embeelimits.h \
    src/shared/sharedmemowriter.h \
    src/shared/readwriteiodevice.h \
    matilda-bbb-src/database/sharedatabase.h \
    matilda-bbb-src/database/matildasqlhelper.h \
    matilda-bbb-src/database/sqlitemediumerr.h \
    matilda-bbb-src/shared/readjsonhelper.h \
    zbyrator-src/zbyrmeterlistmedium.h \
    zbyrator-src/src/zbyrtableheaders.h \
    src/widgets/selectionchecker.h \
    zbyrator-src/wdgt/defserialdialog.h \
    zbyrator-src/wdgt/compliterlistdialog.h \
    zbyrator-src/wdgt/addeditmacform.h \
    zbyrator-src/src/ifacesettloader.h \
    main-pgs/custommessagewidget.h \
    main-pgs/scanipwidget.h \
    src/matilda/classmanagerudpscanner.h \
    src/matilda/udpscanner.h \
    src/matilda/udpscannertcpsocket.h \
    src/shared/networkconverthelper.h \
    src/m2m-service/svahaserviceconnector.h \
    src/m2m-service/svahasocket.h \
    src/emb/checkcurrport.h \
    src/shared/ifacehelper.h \
    zbyrator-src/src/zbyratordatacalculation.h \
    src/matilda-conf/classmanagersharedobjects.h \
    src/zbyrator-v2/myucmmeterstypes.h \
    src/matilda/classmanagerhelper.h \
    prepaid-pgs/ifaceindicationwdgt.h \
    appversion.h \
    zbyrator-src/zbyratoroptions.h \
    zbyrator-src/options/databasesettings.h \
    zbyrator-src/options/ktsconnectmode.h \
    zbyrator-src/options/ucemulator.h \
    dataconcetrator-pgs/pollwdgt.h \
    zbyrator-src/options-src/pollwdgtmom.h \
    main-pgs/optionswdgt.h \
    zbyrator-src/selectmeters4poll.h \
    zbyrator-src/src/getreadymetersdata.h \
    src/zbyrator-v2/meterschedulertypes.h \
    zbyrator-src/src/zbyratortasksmedium.h \
    matilda-bbb-src/decodematildaprotocolv5helper.h \
    zbyrator-src/src/startexchangehelper.h \
    src/meter/definedpollcodes.h \
    zbyrator-src/src/zbyratordatabasemedium.h \
    src/matilda/classmanagerdatabasereader.h \
    zbyrator-src/src/zbyratorucmedium.h \
    matilda-bbb-src/database/ucondatabasereader.h \
    matilda-bbb-src/matildaprotocolinitstep.h \
    matilda-bbb-src/shared/ineedmoretimeobject.h \
    matilda-bbb-src/shared/matildaprotocolhelpersmpl.h \
    matilda-bbb-src/database/sqlitelocalclienthelper.h \
    zbyrator-water/watersleepscheduler.h \
    zbyrator-water/waterprofilewdgt.h \
    template-pgs/comboboxdelegate.h \
    zbyrator-water/editwaterprofile.h \
    src/matilda/matildadatakeys.h \
    src/shared/compressfilehelper.h \
    src/matilda/mtypeconversion.h \
    dataconcetrator-pgs/src/dbdatafromsmplhelper.h \
    dataconcetrator-pgs/dbdatafrommodelhelper.h \
    src/firefly/fireflyglobal.h \
    src/shared/sharedmemowritelater.h \
    dataconcetrator-pgs/src/getmetersnis.h \
    zbyrator-water/src/watersleepschedulesaver.h \
    zbyrator-water/waterscheduleeditorpro.h \
    src/zbyrator-v2/watermeterhelper.h \
    src/zbyrator-v2/activesleepprofiles.h \
    src/task/taskmanagerv3.h \
    src/zbyrator-v2/metersloader.h \
    src/matilda-conf/protocol5togui.h \
    zbyrator-water/watersleepscheduleradder.h \
    src/zbyrator-v2/activesleepprofileshelper.h \
    src/matilda-conf/securecopypast.h \
    src/matilda-conf/uconloginpashelper.h \
    src/task/taskmanagementsett.h \
    gui-src/chart/dbchartview.h \
    gui-src/chart/dbzoomchart.h \
    gui-src/chart/dbzoomchartview.h \
    gui-src/emailhelper.h \
    gui-src/escwidget.h \
    gui-src/globalhelper.h \
    gui-src/guihelper.h \
    gui-src/guihelperdefines.h \
    gui-src/guisett4all.h \
    gui-src/listviewhelper.h \
    gui-src/matildacachehelper.h \
    gui-src/matildaconfwidget.h \
    gui-src/mtreemodelproxy.h \
    gui-src/mysortfilterproxymodel.h \
    gui-src/printerhelper.h \
    gui-src/printimagehelper.h \
    gui-src/qelidablelabel.h \
    gui-src/referencewidgetclass.h \
    gui-src/settloader.h \
    gui-src/settloaderdefy.h \
    gui-src/showmesshelper4wdgt.h \
    gui-src/showmesshelper4wdgtdef.h \
    gui-src/stackwidgethelper.h \
    gui-src/standarditemmodelhelper.h \
    gui-src/tableviewhelper.h \
    gui-src/treeitem.h \
    gui-src/treemodel.h \
    src/widgets/tablevalueinserter.h \
    template-pgs/integereditdlg.h \
    template-pgs/cbxeditdlg.h \
    src/widgets/ifacelabel.h \
    src/zbyrator-v2/zbyratorstatistichelperdefs.h

FORMS += \
        qcmainwindow.ui \
    main-pgs/langdialog.ui \
    main-pgs/duplicateswdiget.ui \
    main-pgs/filterwidgetform.ui \
    dataconcetrator-pgs/meterlistwdgt.ui \
    map-pgs/mapwidget.ui \
    dataconcetrator-pgs/import-from-file/finalcheckwdgt.ui \
    dataconcetrator-pgs/import-from-file/importfromfilecolumnwdgt.ui \
    dataconcetrator-pgs/import-from-file/importfromfilemodel2modelwdgt.ui \
    dataconcetrator-pgs/import-from-file/importfromfilesheetwdgt.ui \
    dataconcetrator-pgs/import-from-file/importfromfilewdgt.ui \
    dataconcetrator-pgs/import-from-file/importmemowdgt.ui \
    dataconcetrator-pgs/import-from-file/selectcodecandseparwdgt.ui \
    dataconcetrator-pgs/addeditmeterwdgt.ui \
    dataconcetrator-pgs/editenergywidget.ui \
    dataconcetrator-pgs/databasewdgt.ui \
    dataconcetrator-pgs/dbdataform.ui \
    dataconcetrator-pgs/dbdataformcalcconsumption.ui \
    dataconcetrator-pgs/dbdatafromsmpl.ui \
    dataconcetrator-pgs/meterjournalform.ui \
    template-pgs/chartcontrols.ui \
    template-pgs/selectdtwdgt.ui \
    template-pgs/smplptewdgt.ui \
    template-pgs/searchwdgt.ui \
    zbyrator-src/ktsconnectwdgt.ui \
    zbyrator-src/startexchange.ui \
    zbyrator-src/relaywdgt.ui \
    zbyrator-src/metersdatetime.ui \
    zbyrator-src/setmeteraddress.ui \
    zbyrator-src/checkconnectiontoolwdgt.ui \
    zbyrator-src/zbyratorservice.ui \
    zbyrator-src/startpagepoll.ui \
    zbyrator-src/zbyrifacesett.ui \
    info-pgs/zbyratortasks.ui \
    info-pgs/statisticofexchangewdgt.ui \
    zbyrator-src/wdgt/defserialdialog.ui \
    zbyrator-src/wdgt/compliterlistdialog.ui \
    zbyrator-src/wdgt/addeditmacform.ui \
    main-pgs/custommessagewidget.ui \
    main-pgs/scanipwidget.ui \
    prepaid-pgs/ifaceindicationwdgt.ui \
    zbyrator-src/zbyratoroptions.ui \
    zbyrator-src/options/databasesettings.ui \
    zbyrator-src/options/ktsconnectmode.ui \
    zbyrator-src/options/ucemulator.ui \
    dataconcetrator-pgs/pollwdgt.ui \
    main-pgs/optionswdgt.ui \
    zbyrator-src/selectmeters4poll.ui \
    zbyrator-water/watersleepscheduler.ui \
    zbyrator-water/waterprofilewdgt.ui \
    zbyrator-water/editwaterprofile.ui \
    dataconcetrator-pgs/src/getmetersnis.ui \
    zbyrator-water/waterscheduleeditorpro.ui \
    zbyrator-water/watersleepscheduleradder.ui \
    template-pgs/integereditdlg.ui \
    template-pgs/cbxeditdlg.ui

RESOURCES += \
    font.qrc \
    res.qrc \
    qml.qrc


RC_FILE += file.rc
OTHER_FILES += \
    file.rc

TRANSLATIONS = lang_uk.ts\
        lang_en.ts\
        lang_ru.ts
