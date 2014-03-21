TEMPLATE = app
TARGET = SVGB2SVG
QT += svg

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
    MMP_RULES += "DEBUGGABLE"
    TARGET.UID3 = 0xE108736F
    HEADERS +=
    #ICON = Icon.svg
    vendorinfo = "%{\"huellif\"}" ":\"huellif\""
    my_deployment.pkg_prerules = vendorinfo
    DEPLOYMENT += my_deployment
    DEPLOYMENT.display_name += SVGB2SVG
    VERSION = 1.6.0
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

MMP_RULES += "OPTION gcce -O3"
MMP_RULES += "OPTION gcce -march=armv6"
MMP_RULES += "OPTION gcce -mfpu=vfp"
MMP_RULES += "OPTION gcce -mfloat-abi=softfp"
MMP_RULES += "OPTION gcce -marm"
MMP_RULES += "OPTION gcce -fpermissive"
#MMP_RULES += "OPTION gcce -w"
MMP_RULES += "OPTION gcce -ffast-math"



