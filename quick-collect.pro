#-------------------------------------------------
#
# Project created by QtCreator 2018-07-14T10:52:43
#
#-------------------------------------------------

QT       += core gui network printsupport xml svg charts qml quick positioning location sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#unix:documentation.extra = create_docs; mv master.doc toc.doc

TARGET = quick-collect
TEMPLATE = app
VERSION = 3


android:{
QT += androidextras
#SOURCES += snif-android-src/qtandroidftdi.cpp
#HEADERS += snif-android-src/qtandroidftdi.h
#DEFINES += ADDANDROIDFTDIUART=1
}



# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APPLCTN_NAME=\\\"quick-collect\\\"
DEFINES += IS_ZBYRATORWDGT
DEFINES += IS_ZBYRATOR
#deprecated, it is better to use HASGUI4USR
DEFINES += VERSION_4_PC

DEFINES += DISABLE_METERSCHEDULER
DEFINES += HASSERIALLIB
DEFINES += ENABLE_EXTSUPPORT_OF_IFACES
DEFINES += DISABLE_UART_PRIORITY
DEFINES += HASGUI4USR
DEFINES += HASSETTLOADEREMUL
DEFINES += DONOTINCLUDEFIREFLY

# it reduces matilda-bbb-core functionality
DEFINES += DISABLE_WATCHDOG_TIMER
DEFINES += DISABLE_PROC_MANAGER

#appversion.cpp
VERSION = 0.2.1

DEFINES += APPLCTN_NAME=\\\"quick-collect\\\"
DEFINES += "MYAPPNAME=\"\\\"Quick Collect\\\"\""
DEFINES += "MYAPPOWNER=\"\\\"KTS Intek Ltd\\\"\""
DEFINES += "MYAPPOWNERSITE=\"\\\"http://kts-intek.com.ua\\\"\""

#defines that disable some modules, that I don't want to use
DEFINES += DISABLEVPNMANAGER
DEFINES += DISABLECOMMANDONUPDATE
DEFINES += DISABLE_RUN_PROCESS


# I need some functionality
DEFINES += IS_USESTARTDEV


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../../Matilda-units/device-poll/zbyrator-base/zbyrator-base.pri)

include(../../Matilda-units/meter-plugin-shared/plugin-types/plugin-types.pri)


include(../../Matilda-units/matilda-bbb/matilda-bbb-settings/matilda-bbb-settings.pri)

include(../../Matilda-units/matilda-bbb/matilda-bbb-serverside-shared/matilda-bbb-serverside-shared.pri)

include(../../Matilda-units/matilda-bbb/matilda-bbb-m2m-server/matilda-bbb-m2m-server.pri)


include(../../Matilda-units/database/sqliteserver/sqliteserver.pri)

include(../../Matilda-units/database/sqliteshared/sqliteshared.pri)

#include(../sqliteshared/sqliteshared.pri)

include(../../Matilda-units/ifaces/ifaces/ifaces.pri)
#include(../../Matilda-units/tasks/tasks-types/tasks-types.pri)

include(../../Matilda-units/matilda-base/MatildaIO/MatildaIO.pri)

include(../../Matilda-units/matilda-base/type-converter/type-converter.pri)


include(../../Matilda-units/debugtools/debug-internal-unit/debug-internal-unit.pri)


include(../../Matilda-units/gui/image-and-font/image-and-font.pri)

include(../../Matilda-units/charts/widget-charts/widget-charts.pri)

include(../../Matilda-units/printer/printer-widgets/printer-widgets.pri)


include(../../Matilda-units/qtwidgetonly/widgets-shared/widgets-shared.pri)

include(../../Matilda-units/qtwidgetonly/widgets-meters/widgets-meters.pri)

include(../../Matilda-units/qtwidgetonly/matilda-conf-shared-widgets/matilda-conf-shared-widgets.pri)




include(../../Matilda-units/maps/map-widget/map-widget.pri)

include(../../Matilda-units/firefly/firefly-shared/firefly-shared.pri)

include(../../Matilda-units/gui/quick-collect-gui-core/quick-collect-gui-core.pri)

include(../../Matilda-units/matilda-bbb/matilda-bbb-core/matilda-bbb-core.pri)

include(../../Matilda-units/ipc/localsockets/localsockets.pri)

include(../../Matilda-units/gui/guisett-shared-ucon/guisett-shared-ucon.pri)

include(../../Matilda-units/qtwidgetonly/matilda-conf-widgets/matilda-conf-widgets.pri)

include(../../Matilda-units/qtwidgetonly/prepaid-shared/prepaid-shared.pri)

include(../../Matilda-units/matilda-bbb/matilda-bbb-clientside/matilda-bbb-clientside.pri)


SOURCES += \
        main.cpp \
        qcmainwindow.cpp \
    zbyrator-src/createtoolbar.cpp \
    zbyrator-src/ktsconnectwdgt.cpp \
    zbyrator-src/startexchange.cpp \
    zbyrator-src/relaywdgt.cpp \
    zbyrator-src/metersdatetime.cpp \
    zbyrator-src/setmeteraddress.cpp \
    zbyrator-src/checkconnectiontoolwdgt.cpp \
    zbyrator-src/zbyratorservice.cpp \
    zbyrator-src/startpagepoll.cpp \
    zbyrator-src/zbyrmeterlistmedium.cpp \
    zbyrator-src/src/zbyrtableheaders.cpp \
    zbyrator-src/src/zbyratordatacalculation.cpp \
    zbyrator-src/zbyratoroptions.cpp \
    zbyrator-src/options/databasesettings.cpp \
    zbyrator-src/options/ktsconnectmode.cpp \
    zbyrator-src/options/ucemulator.cpp \
    zbyrator-src/options-src/pollwdgtmom.cpp \
    zbyrator-src/selectmeters4poll.cpp \
    zbyrator-src/src/getreadymetersdata.cpp \
    zbyrator-src/src/zbyratortasksmedium.cpp \
    zbyrator-src/src/zbyratordatabasemedium.cpp \
    zbyrator-src/src/zbyratorucmedium.cpp \
    zbyrator-src/startpagepollv2.cpp \
    zbyrator-src/waterstartpollsleepdlg.cpp \
    zbyrator-src/src/startpollsetmetermodeldialog.cpp \
    zbyrator-src/ifacesett4groups.cpp \
    zbyrator-src/ifacesett4groupseditor.cpp


HEADERS += \
        qcmainwindow.h \
    zbyrator-src/createtoolbar.h \
    zbyrator-src/ktsconnectwdgt.h \
    zbyrator-src/startexchange.h \
    zbyrator-src/relaywdgt.h \
    zbyrator-src/metersdatetime.h \
    zbyrator-src/setmeteraddress.h \
    zbyrator-src/checkconnectiontoolwdgt.h \
    zbyrator-src/zbyratorservice.h \
    zbyrator-src/startpagepoll.h \
    zbyrator-src/zbyrmeterlistmedium.h \
    zbyrator-src/src/zbyrtableheaders.h \
    zbyrator-src/src/zbyratordatacalculation.h \
    zbyrator-src/zbyratoroptions.h \
    zbyrator-src/options/databasesettings.h \
    zbyrator-src/options/ktsconnectmode.h \
    zbyrator-src/options/ucemulator.h \
    zbyrator-src/options-src/pollwdgtmom.h \
    zbyrator-src/selectmeters4poll.h \
    zbyrator-src/src/getreadymetersdata.h \
    zbyrator-src/src/zbyratortasksmedium.h \
    zbyrator-src/src/zbyratordatabasemedium.h \
    zbyrator-src/src/zbyratorucmedium.h \
    zbyrator-src/startpagepollv2.h \
    zbyrator-src/waterstartpollsleepdlg.h \
    zbyrator-src/src/startpollsetmetermodeldialog.h \
    zbyrator-src/ifacesett4groups.h \
    zbyrator-src/ifacesett4groupseditor.h


FORMS += \
        qcmainwindow.ui \
    zbyrator-src/ktsconnectwdgt.ui \
    zbyrator-src/startexchange.ui \
    zbyrator-src/relaywdgt.ui \
    zbyrator-src/metersdatetime.ui \
    zbyrator-src/setmeteraddress.ui \
    zbyrator-src/checkconnectiontoolwdgt.ui \
    zbyrator-src/zbyratorservice.ui \
    zbyrator-src/startpagepoll.ui \
    zbyrator-src/zbyratoroptions.ui \
    zbyrator-src/options/databasesettings.ui \
    zbyrator-src/options/ucemulator.ui \
    zbyrator-src/options/ktsconnectmode.ui \
    zbyrator-src/selectmeters4poll.ui \
    zbyrator-src/waterstartpollsleepdlg.ui \
    zbyrator-src/src/startpollsetmetermodeldialog.ui \
    zbyrator-src/ifacesett4groupseditor.ui


RESOURCES +=


RC_FILE += file.rc
OTHER_FILES += \
    file.rc

TRANSLATIONS = lang_uk.ts\
        lang_en.ts\
        lang_ru.ts
