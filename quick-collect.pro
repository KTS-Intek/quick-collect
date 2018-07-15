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

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        qcmainwindow.cpp \
    src/matilda/settloader.cpp \
    main-pgs/langdialog.cpp \
    src/shared/guisett4all.cpp \
    src/shared/globalhelper.cpp \
    src/matilda/guihelper.cpp \
    src/shared/mysortfilterproxymodel.cpp \
    src/meter/zbyratorfilehelper.cpp \
    src/matilda/lastdevinfo.cpp \
    src/geo/coordinateobject.cpp \
    src/widgets/tableheaders.cpp \
    main-pgs/duplicateswdiget.cpp \
    src/shared/referencewidgetclass.cpp \
    src/matilda/matildacachehelper.cpp \
    main-pgs/filterwidgetform.cpp \
    src/matilda/showmesshelper.cpp \
    src/matilda/matildaprotocolhelper.cpp \
    src/printer/myprintpreviewdlg.cpp \
    src/shared/matildaconfwidget.cpp \
    src/meter/meterpluginloader.cpp \
    matilda-bbb-src/shared/pathsresolver.cpp \
    matilda-bbb-src/shared/runprocess.cpp \
    src/meter/meteroperations.cpp \
    src/shared/compresshelper.cpp \
    matilda-bbb-src/shared/macaddresshelper.cpp \
    src/shared/sharedmemoprotocolhelper.cpp \
    src/meter/numberconvertation.cpp \
    matilda-bbb-src/shared/ifacemanagerhelper.cpp \
    src/shared/networkconverthelper.cpp \
    src/shared/sharedmemohelper.cpp \
    zbyrator-src/createtoolbar.cpp \
    src/shared/stackwidgethelper.cpp \
    dataconcetrator-pgs/meterlistwdgt.cpp \
    map-pgs/mapwidget.cpp \
    dataconcetrator-pgs/import-from-file/comboboxdelegate.cpp \
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
    src/matilda/dbchartview.cpp \
    src/matilda/dbzoomchart.cpp \
    src/matilda/dbzoomchartview.cpp \
    template-pgs/selectdtwdgt.cpp \
    src/matilda/balancemanager.cpp \
    src/firefly/callout.cpp \
    src/meter/meterstatehelper.cpp \
    src/matilda/balancehelper.cpp \
    template-pgs/smplptewdgt.cpp \
    template-pgs/searchwdgt.cpp \
    zbyrator-src/ktsconnectwdgt.cpp \
    zbyrator-src/startexchange.cpp \
    src/shared/treeitem.cpp \
    src/shared/treemodel.cpp \
    zbyrator-src/relaywdgt.cpp \
    zbyrator-src/metersdatetime.cpp \
    zbyrator-src/setmeteraddress.cpp \
    zbyrator-src/checkconnectiontoolwdgt.cpp \
    zbyrator-src/zbyratorservice.cpp \
    zbyrator-src/startpagepoll.cpp \
    zbyrator-src/zbyrifacesett.cpp \
    dataconcetrator-pgs/zbyratortasks.cpp \
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
    src/zbyrator-v2/waterlastschedule.cpp \
    src/zbyrator-v2/zbyratordatatypehelper.cpp \
    src/zbyrator-v2/zbyratorstatistichelper.cpp \
    src/zbyrator-v2/zbyratortasksharedmemo.cpp \
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
    src/shared/mytaskhelper.cpp \
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
    zbyrator-src/zbyrmeterlistmedium.cpp


HEADERS += \
        qcmainwindow.h \
    src/matilda/settloader.h \
    main-pgs/langdialog.h \
    src/shared/guisett4all.h \
    src/shared/globalhelper.h \
    src/matilda/guihelper.h \
    src/shared/mysortfilterproxymodel.h \
    src/meter/zbyratorfilehelper.h \
    src/matilda/lastdevinfo.h \
    src/geo/coordinateobject.h \
    src/printer/qrcodegeneratortypes.h \
    src/matilda/classmanagertypes.h \
    src/widgets/tableheaders.h \
    main-pgs/duplicateswdiget.h \
    src/shared/referencewidgetclass.h \
    src/matilda/matildacachehelper.h \
    main-pgs/filterwidgetform.h \
    src/matilda/showmesshelper.h \
    src/matilda/matildaprotocolhelper.h \
    src/printer/myprintpreviewdlg.h \
    src/shared/matildaconfwidget.h \
    src/meter/meterpluginloader.h \
    matilda-bbb-src/shared/pathsresolver.h \
    matilda-bbb-src/shared/runprocess.h \
    src/meter/meteroperations.h \
    src/shared/compresshelper.h \
    matilda-bbb-src/shared/macaddresshelper.h \
    src/shared/sharedmemoprotocolhelper.h \
    src/meter/numberconvertation.h \
    matilda-bbb-src/shared/ifacemanagerhelper.h \
    src/shared/networkconverthelper.h \
    src/shared/sharedmemohelper.h \
    zbyrator-src/createtoolbar.h \
    src/shared/stackwidgethelper.h \
    dataconcetrator-pgs/meterlistwdgt.h \
    map-pgs/mapwidget.h \
    dataconcetrator-pgs/import-from-file/comboboxdelegate.h \
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
    src/meter/definedmetertypes.h \
    src/meter/meterpluginsloadhelper.h \
    dataconcetrator-pgs/databasewdgt.h \
    dataconcetrator-pgs/dbdataform.h \
    dataconcetrator-pgs/dbdataformcalcconsumption.h \
    dataconcetrator-pgs/dbdataformdefy.h \
    dataconcetrator-pgs/dbdatafromsmpl.h \
    dataconcetrator-pgs/meterjournalform.h \
    template-pgs/chartcontrols.h \
    src/matilda/dbchartview.h \
    src/matilda/dbzoomchart.h \
    src/matilda/dbzoomchartview.h \
    template-pgs/selectdtwdgt.h \
    src/matilda/balancemanager.h \
    src/firefly/callout.h \
    src/meter/meterstatehelper.h \
    src/matilda/balancehelper.h \
    template-pgs/smplptewdgt.h \
    template-pgs/searchwdgt.h \
    zbyrator-src/ktsconnectwdgt.h \
    zbyrator-src/startexchange.h \
    src/shared/treeitem.h \
    src/shared/treemodel.h \
    zbyrator-src/relaywdgt.h \
    zbyrator-src/metersdatetime.h \
    zbyrator-src/setmeteraddress.h \
    zbyrator-src/checkconnectiontoolwdgt.h \
    zbyrator-src/zbyratorservice.h \
    zbyrator-src/startpagepoll.h \
    zbyrator-src/zbyrifacesett.h \
    dataconcetrator-pgs/zbyratortasks.h \
    info-pgs/statisticofexchangewdgt.h \
    zbyrator-src/zbyratorprocessmanager.h \
    src/zbyrator-v2/addpolldata2dbobject.h \
    src/zbyrator-v2/dataprocessinghelper.h \
    src/zbyrator-v2/metermanager.h \
    src/zbyrator-v2/metermanagerconstants.h \
    src/zbyrator-v2/meterscheduler.h \
    src/zbyrator-v2/meterschedulerhelper.h \
    src/zbyrator-v2/myucmmeters.h \
    src/zbyrator-v2/quickpollhelper.h \
    src/zbyrator-v2/thelordofmeters.h \
    src/zbyrator-v2/thelordofmetershelper.h \
    src/zbyrator-v2/waterlastschedule.h \
    src/zbyrator-v2/zbyratordatatypehelper.h \
    src/zbyrator-v2/zbyratorstatistichelper.h \
    src/zbyrator-v2/zbyratortasksharedmemo.h \
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
    src/shared/mytaskhelper.h \
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
    zbyrator-src/zbyrmeterlistmedium.h

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
    dataconcetrator-pgs/zbyratortasks.ui \
    info-pgs/statisticofexchangewdgt.ui

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
