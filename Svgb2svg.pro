TEMPLATE = app
TARGET = SVGB2SVG
CONFIG += qtestlib
QT += core \
    gui\
    svg\

SOURCES += \
    svgb_decoder.cpp \
    mainwindow.cpp \
    main.cpp \
    previewdialog.cpp

HEADERS += \
    svgb_decoder.h \
    mainwindow.h \
    previewdialog.h

FORMS += mainwindow.ui \
    previewdialog.ui
RESOURCES +=
symbian {
    #MMP_RULES += "DEBUGGABLE"
    TARGET.UID3 = 0xE108736F
    HEADERS +=
    ICON = Icon.svg
    vendorinfo = "%{\"huellif\"}" ":\"huellif\""
    my_deployment.pkg_prerules = vendorinfo
    DEPLOYMENT += my_deployment
    DEPLOYMENT.display_name += SVGB2SVG
    VERSION = 1.4.0
    TARGET.EPOCSTACKSIZE = 0x10000 \
    // \
    20kb
    TARGET.EPOCHEAPSIZE = 0x20000 \
    0x2000000 \
    // \
    Min \
    128kb, \
    Max \
    16Mb
}




