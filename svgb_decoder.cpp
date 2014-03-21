#include "svgb_decoder.h"

#include <QFile>
#include <QDir>
class QObject;

static const qint32 SvgFileStart = 0x03FA56CC;
static const qint32 SvgFileStart2 = 0x03FA56CD;
static const qint32 SvgFileStart3 = 0x03FA56CE;
static const qint32 SvgFileStart4 = 0x03FA56CF;
static const char  SvgFileEnd = 0xFF;
static const qint8  SvgAtrEnd[2] = {0xE8, 0x03};
static const char  SvgElementEnd = 0xFE;
static const char  SvgCData = 0xFD;
static const qint8  SvgRootElement = 0;
int is_new=0;



static const SvgFill SVGB_FILL[] =
{
    { "none",           {0xFF, 0xFF, 0xFF, 0x01}},
    { "currentColor",   {0xFF, 0xFF, 0xFF, 0x02}}
};

static const SvgPathSegType SVG_PATH_SEG [] =
{
    {'M', 2},   /* moveto */
    {'L', 2},   /* lineto */
    {'Q', 4},   /* quadratic curve */
    {'C', 6},   /* cubic curve */
    {'z', 0}    /* closepath */
};


static const QString SVG_START = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\xD\xA"
                                 "<!-- Decoded by svgb2svg converter version "APP_VERSION" -->\xD\xA"
                                 "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" "
                                 "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\xD\xA";



SvgAttrDecodePrototype(SVGB_DecodeFail);
SvgAttrDecodePrototype(SVGB_DecodeSize);
SvgAttrDecodePrototype(SVGB_DecodeVisibility);
SvgAttrDecodePrototype(SVGB_DecodeFloat);
SvgAttrDecodePrototype(SVGB_DecodeOpacity);
SvgAttrDecodePrototype(SVGB_DecodeRect);
SvgAttrDecodePrototype(SVGB_DecodeString);
SvgAttrDecodePrototype(SVGB_DecodeGradUnits);
SvgAttrDecodePrototype(SVGB_DecodeDisplay);
SvgAttrDecodePrototype(SVGB_DecodeSpreadMethod);
SvgAttrDecodePrototype(SVGB_DecodeFontStyle);
SvgAttrDecodePrototype(SVGB_DecodeFontWeight);
SvgAttrDecodePrototype(SVGB_DecodeTextAnchor);
SvgAttrDecodePrototype(SVGB_DecodeTextDecor);
SvgAttrDecodePrototype(SVGB_DecodeTrans);
SvgAttrDecodePrototype(SVGB_DecodeGradTrans);
SvgAttrDecodePrototype(SVGB_DecodePoints);
SvgAttrDecodePrototype(SVGB_DecodePath);
SvgAttrDecodePrototype(SVGB_DecodeFill);
SvgAttrDecodePrototype(SVGB_DecodeCssColor);
SvgAttrDecodePrototype(SVGB_DecodeHref);
SvgAttrDecodePrototype(SVGB_DecodeLHref);
SvgAttrDecodePrototype(SVGB_DecodeDasharray);
SvgAttrDecodePrototype(SVGB_DecodeUn);



static const SvgAttr SVGB_ATTRS [] =
{

    {"fill", SVGB_DecodeFill },                     /* 0    00 */
    {"stroke", SVGB_DecodeCssColor },               /* 1    01 */
    {"stroke-width", SVGB_DecodeFloat },            /* 2    02 */
    {"visibility", SVGB_DecodeVisibility },         /* 3    03 */
    {"font-family", SVGB_DecodeString },            /* 4    04 */
    {"font-size", SVGB_DecodeFloat },               /* 5    05 */
    {"font-style", SVGB_DecodeFontStyle },          /* 6    06 */
    {"font-weight", SVGB_DecodeFontWeight },        /* 7    07 */
    {"stroke-dasharray", SVGB_DecodeDasharray },    /* 8    08 */
    {"display", SVGB_DecodeDisplay },               /* 9    09 */
    {"fill-rule", SVGB_DecodeString },              /* 10   0A */
    {"stroke-linecap", SVGB_DecodeString },         /* 11   0B */
    {"stroke-linejoin", SVGB_DecodeString },        /* 12   0C */
    {"stroke-dashoffset", SVGB_DecodeFloat },       /* 13   0D */
    {"stroke-miterlimit", SVGB_DecodeFloat },       /* 14   0E */
    {"color", SVGB_DecodeCssColor },                /* 15   0F */
    {"text-anchor", SVGB_DecodeTextAnchor },        /* 16   10 */
    {"text-decoration", SVGB_DecodeTextDecor },     /* 17   11 */
    {"color-interpolation", SVGB_DecodeFail },      /* 18   12 */
    {"color-rendering", SVGB_DecodeFail },          /* 19   13 */
    {"letterSpacing", SVGB_DecodeFail },            /* 20   14 */
    {"word-spacing", SVGB_DecodeFail },             /* 21   15 */
    {"opacity", SVGB_DecodeOpacity },               /* 22   16 */
    {"stroke-opacity", SVGB_DecodeFloat },          /* 23   17 */
    {"fill-opacity", SVGB_DecodeOpacity },   /* 24   18 */
    //{"opacity", SVGB_DecodeOpacity },               /* 24   18 */

    //{"font", SVGB_DecodeFail },                   /* 25   19 */
    {"textLength", SVGB_DecodeFail },               /* 25   19 */

    {"width", SVGB_DecodeSize },                    /* 26   1A */
    {"height", SVGB_DecodeSize },                   /* 27   1B */
    {"r", SVGB_DecodeFloat },                       /* 28   1C */
    {"rx", SVGB_DecodeFloat },                      /* 29   1D */

    {"ry", SVGB_DecodeFloat },                      /* 30   1E */
    {"horizAdvX", SVGB_DecodeFail },                /* 31   1F  */
    {"horizOriginX", SVGB_DecodeFail },             /* 32   20 */
    {"horizOriginY", SVGB_DecodeFail },             /* 33   21 */
    {"ascent", SVGB_DecodeFail },                   /* 34   22 */
    {"descent", SVGB_DecodeFail },                  /* 35   23 */
    {"alphabetic", SVGB_DecodeFail },               /* 36   24 */
    {"underlinePosition", SVGB_DecodeFail },        /* 37   25 */
    {"underlineThickness", SVGB_DecodeFail },       /* 38   26 */
    {"overlinePosition", SVGB_DecodeFail },         /* 39   27 */

    {"overlineThickness", SVGB_DecodeFail },        /* 40   28 */
    {"strikethroughPosition", SVGB_DecodeFail },    /* 41   29 */
    {"strikethroughThickness", SVGB_DecodeFail },   /* 42   2A */
    {"unitsPerEm", SVGB_DecodeFail },               /* 43   2B */
    {"wordSpacing", SVGB_DecodeFail },              /* 44   2C */
    {"letterSpacing", SVGB_DecodeFail },            /* 45   2D */
    {"cx", SVGB_DecodeFloat },                      /* 46   2E */
    {"cy", SVGB_DecodeFloat },                      /* 47   2F */
    {"y", SVGB_DecodeFloat },                       /* 48   30 */
    {"x", SVGB_DecodeFloat },                       /* 49   31 */

    {"y1", SVGB_DecodeFloat },                      /* 50   32 */
    {"y2", SVGB_DecodeFloat },                      /* 51   33 */
    {"x1", SVGB_DecodeFloat },                      /* 52   34 */
    {"x2", SVGB_DecodeFloat },                      /* 53   35 */
    {"k", SVGB_DecodeFail },                        /* 54   36 */
    {"g1", SVGB_DecodeFail },                       /* 55   37 */
    {"g2", SVGB_DecodeFail },                       /* 56   38 */
    {"u1", SVGB_DecodeFail },                       /* 57   39 */
    {"u2", SVGB_DecodeFail },                       /* 58   3A */
    {"unicode", SVGB_DecodeFail },                  /* 59   3B */

    {"glyphName", SVGB_DecodeFail },                /* 60   3C */
    {"lang", SVGB_DecodeFail },                     /* 61   3D */
    {"textDecoration", SVGB_DecodeFail },           /* 62   3E */
    {"textAnchor", SVGB_DecodeFail },               /* 63   3F */
    {"rotate", SVGB_DecodeFail },                   /* 64   40 */
    {"cdata", SVGB_DecodeFail },                    /* 65   41 */
    {"transform", SVGB_DecodeTrans },               /* 66   42 */
    {"style", SVGB_DecodeFail },                    /* 67   43 */
    {"fill", SVGB_DecodeFail },                     /* 68   44 */
    {"stroke", SVGB_DecodeFail },                   /* 69   45 */

    {"color", SVGB_DecodeFail },                    /* 70   46 */
    {"from", SVGB_DecodeFail },                     /* 71   47 */
    {"to", SVGB_DecodeFail },                       /* 72   48 */
    {"by", SVGB_DecodeFail },                       /* 73   49 */
    {"attributeName", SVGB_DecodeUn },              /* 74   4A */
    {"pathLength", SVGB_DecodeFail },               /* 75   4B */
    {"version", SVGB_DecodeFloat },		    /* 76   4C */
    {"strokeWidth", SVGB_DecodeFail },              /* 77   4D */
    {"points", SVGB_DecodePoints },                 /* 78   4E */
    {"d", SVGB_DecodePath },                        /* 79   4F */
    {"type", SVGB_DecodeFail },                     /* 80   50 */

    {"stop-color", SVGB_DecodeCssColor },           /* 81 */
    {"fx", SVGB_DecodeFloat },                      /* 82 */
    {"fy", SVGB_DecodeFloat },                      /* 83 */
    {"offset", SVGB_DecodeFloat },                  /* 84 */
    {"spreadMethods", SVGB_DecodeSpreadMethod },    /* 85 */
    {"gradientUnits", SVGB_DecodeGradUnits },       /* 86 */
    {"stopOpacity", SVGB_DecodeFloat },             /* 87 */
    {"viewBox", SVGB_DecodeRect },                  /* 88 */
    {"baseProfile", SVGB_DecodeString },            /* 89 */

    {"zoomAndPan", SVGB_DecodeUn },		    /* 90 */
    {"preserveAspectRatio", SVGB_DecodeString },    /* 91 */
    {"id", SVGB_DecodeString },                     /* 92 */
    {"xml:base", SVGB_DecodeString },               /* 93 */
    {"xml:lang", SVGB_DecodeString },               /* 94 */
    {"xml:space", SVGB_DecodeString },              /* 95 */
    {"requiredExtensions", SVGB_DecodeFail },       /* 96 */
    {"requiredFeatures", SVGB_DecodeFail },         /* 97 */
    {"systemLanguage", SVGB_DecodeFail },           /* 98 */
    {"dx", SVGB_DecodeFail },                       /* 99 */

    {"dy", SVGB_DecodeFail },                       /* 100 */
    {"media", SVGB_DecodeFail },                    /* 101 */
    {"title", SVGB_DecodeFail },                    /* 102 */
    {"xlink:actuate", SVGB_DecodeString },          /* 103 */
    {"xlink:arcrole", SVGB_DecodeFail },            /* 104 */
    {"xlink:role", SVGB_DecodeFail },               /* 105 */
    {"xlink:show", SVGB_DecodeString },             /* 106 */
    {"xlink:title", SVGB_DecodeFail },              /* 107 */
    {"xlink:type", SVGB_DecodeString },             /* 108 */
    {"xlink:href", SVGB_DecodeHref },               /* 109 */

    {"begin", SVGB_DecodeFail },                    /* 110 */
    {"dur", SVGB_DecodeFail },                      /* 111 */
    {"repeatCount", SVGB_DecodeFail },              /* 112 */
    {"repeatDur", SVGB_DecodeFail },                /* 113 */
    {"end", SVGB_DecodeFail },                      /* 114 */
    {"restart", SVGB_DecodeFail },                  /* 115 */
    {"accumulate", SVGB_DecodeFail },               /* 116 */
    {"additive", SVGB_DecodeFail },                 /* 117 */
    {"keySplines", SVGB_DecodeFail },               /* 118 */
    {"keyTimes", SVGB_DecodeFail },                 /* 119 */
    {"calcMode", SVGB_DecodeFail },                 /* 120 */
    {"path", SVGB_DecodeFail },                     /* 121 */
    {"animateMotion", SVGB_DecodeFail },            /* 122 */
    {"gradientTransform", SVGB_DecodeGradTrans },   /* 123 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 124 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 125 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 126 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 127 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 128 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 129 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 130 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 131 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 132 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 133 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 134 */
    {"UNCNOUN", SVGB_DecodeFail },                  /* 135 */
    {"xlink:href", SVGB_DecodeLHref }               /* 136 */
};

#define SvgElementSVG 0

static const SvgElementAttr SvgAttrSVG [] =
{
    {"xmlns", "http://www.w3.org/2000/svg"}
};

#define COUNT(array) ((int)(sizeof(array)/sizeof(array[0])))

static const SvgElement SVGB_ELEMS [] =
{
    {"svg", SvgElementSVG, SvgAttrSVG, COUNT(SvgAttrSVG)},  /* 0 */
    {"altglyph", 1, NULL, 0},                               /* 1 */
    {"altglyphdef", 2, NULL, 0},                            /* 2 */
    {"defs", 3, NULL, 0},                                   /* 3 */
    {"desc", 4, NULL, 0},                                   /* 4 */

    {"foreignObject", 5, NULL, 0},                          /* 5 */
    {"metadata", 6, NULL, 0},                               /* 6 */
    {"title", 7, NULL, 0},                                  /* 7 */
    {"fontfacename", 8, NULL, 0},                           /* 8 */
    {"fontfacesrc", 9, NULL, 0},                            /* 9 */

    {"fontfaceuri", 10, NULL, 0},                           /* 10 */
    {"g", 11, NULL, 0},                                     /* 11 */
    {"glyphref", 12, NULL, 0},                              /* 12 */
    {"vkern", 13, NULL, 0},                                 /* 13 */
    {"script", 14, NULL, 0},                                /* 14 */

    {"switch", 15, NULL, 0},                                /* 15 */
    {"view", 16, NULL, 0},                                  /* 16 */
    {"hkern", 17, NULL, 0},                                 /* 17 */
    {"a", 18, NULL, 0},                                     /* 18 */
    {"font", 19, NULL, 0},                                  /* 19 */

    {"fontface", 20, NULL, 0},                              /* 20 */
    {"glyph", 21, NULL, 0},                                 /* 21 */
    {"image", 22, NULL, 0},                                 /* 22 */
    {"missingglyph", 23, NULL, 0},                          /* 23 */
    {"style", 24, NULL, 0},                                 /* 24 */

    {"text", 25, NULL, 0},                                  /* 25 */
    {"use", 26, NULL, 0},                                   /* 26 */
    {"circle", 27, NULL, 0},                                /* 27 */
    {"ellipse", 28, NULL, 0},                               /* 28 */
    {"line", 29, NULL, 0},                                  /* 29 */

    {"path", 30, NULL, 0},                                  /* 30 */
    {"polygon", 31, NULL, 0},                               /* 31 */
    {"polyline", 32, NULL, 0},                              /* 32 */
    {"rect", 33, NULL, 0},                                  /* 33 */
    {"animate", 34, NULL, 0},                               /* 34 */

    {"animateColor", 35, NULL, 0},                          /* 35 */
    {"animateMotion", 36, NULL, 0},                         /* 36 */
    {"animateTransform", 37, NULL, 0},                      /* 37 */
    {"set", 38, NULL, 0},                                   /* 38 */
    {"mpath", 39, NULL, 0},                                 /* 39 */

    {"linearGradient", 40, NULL, 0},                        /* 40 */
    {"radialGradient", 41, NULL, 0},                        /* 41 */
    {"stop", 42, NULL, 0}                                   /* 42 */
};



int WrapLevel = 0;

QString pname;
QString error;
/** ---- **/

void WRAP_Indent(int n)
{
    WrapLevel += n;
}

/**
 * Adds element on the top of the stack.
 * Returns False if memory allocation failed
 */
bool STACK_Push(QStringList* s, const SvgElement * e)
{
    s->insert(0, e->name);
    return true;
}

/**
 * Removes element from the top of the stack
 * and returns this value.
 */
QString STACK_Pop(QStringList* s)
{
    QString tmp = s->at(0);
    s->removeFirst();
    return tmp;
}

/**
 * Writes <tag to the output stream
 */
bool XML_StartTag(QTextStream*  out, QString tag)
{
    for (int i=0;i<WrapLevel;i++)
        *out << " ";

    *out << "<" << tag;
    return true;
}

bool XML_CloseTag(QTextStream*  out, QString tag, bool eol)
{
    WRAP_Indent(-1);

    for (int i=0;i<WrapLevel;i++)
        *out << " ";

    *out << "</" << tag << (eol ? ">\xD\xA" : ">");
    return true;
}

/**
 * Writes an attribute to the output stream. Does not escape non-printable
 * characters.
 */
bool XML_WriteAttrNoEsc(QTextStream* out, QString attr, QString value)
{
    *out << " " << attr << "=\"" << value << "\"";
    return true;
}

/**
 * Formats a single precision floating point number
 */
static QString SVGB_FormatFloat(QString* buf, float val, bool append)
{
    if (!append) buf->clear();

    QString tmp;
    tmp.sprintf("%.4f",(double)val);

    if (tmp.endsWith(".0000"))
        tmp.truncate(tmp.indexOf(".0000"));
    else
        if (tmp.endsWith("000"))
            tmp.truncate(tmp.lastIndexOf("000"));
    else
        if (tmp.endsWith("00"))
            tmp.truncate(tmp.lastIndexOf("00"));
    else
        if (tmp.endsWith("00"))
            tmp.truncate(tmp.lastIndexOf("00"));
    else
        if (tmp.endsWith("0"))
            tmp.truncate(tmp.lastIndexOf("0"));

    buf->append(tmp);

    return *buf;
}

bool ReadBytes(QDataStream* in, char* ch, int count )
{
    bool ok = false;

    for(int i=0;i<count;i++)
    {
        in->readRawData(&ch[i],1);
    }

    ok = true;

    return ok;
}

bool FILE_ReadAll(QDataStream* in, char* ch, int count )
{
    return ReadBytes(in, ch, count);
}

bool FILE_ReadF32L(QDataStream* in, float* data)
{
    ReadBytes(in,(char*)data,4);
    return true;
}

bool FILE_ReadI16L(QDataStream* in, qint16* data)
{
    ReadBytes(in,(char*)data,2);

    return true;
}

bool FILE_ReadI32L(QDataStream* in, qint32* data)
{
    ReadBytes(in,(char*)data,4);

    return true;
}

bool ReadFloat32(QDataStream* in, float* data)
{
    if(is_new==1 || is_new==2)
    {
        quint16 real;
        qint16 integ;

        *in >> real;
        *in >> integ;
        *data=integ+((float)real/0xFFFF);
        return true;
    }
    else
    {
        if (FILE_ReadF32L(in, data))
            return true;
    }

    return false;
}


QString SVGB_ReadString(QDataStream * in)
{
    char len;
    in->readRawData(&len,1);
    QByteArray str;
    str = in->device()->read(len);
    str.replace('\0',QString().null);
    return str;
}

QString SVGB_ReadLString(QDataStream * in)
{
    int len;
    *in >> len;
    QByteArray str;
    str = in->device()->read(len);
    str.replace('\0',QString().null);
    return str;
}


/**
 * Writes a single precision floating point attribute to the output stream.
 * Almost like XML_WriteFloatAttr, but with less precision.
 */
static bool SVGB_WriteFloatAttr(QTextStream* out, QString attr, float val, bool percent)
{
    QString text;
    text = SVGB_FormatFloat(&text, val, false);

    *out << " " << attr << "=\"" << text;
    if (percent)  *out << "%";
    *out <<  "\"";
    return true;
}

static bool SVGB_ReadWriteFloatAttr(const SvgAttr * attr, const SvgElement * elem,
                                    QDataStream * in, QTextStream * out)
{

    float value;
    if (ReadFloat32(in, &value))
    {
        SVGB_WriteFloatAttr(out, attr->name, value, false);
        return true;
    }

    return false;
}

/**
 * Placeholder for attributes that are not implemented
 */
static bool SVGB_DecodeDasharray(const SvgAttr * attr, const SvgElement * elem,
                                 QDataStream * in, QTextStream * out)
{

    char len;
    QString buf;
    in->readRawData(&len,1);
    for (int i=0; i<len; i++) {
        float value;
        ReadFloat32(in, &value);
        if (i > 0) buf.append(',');
        SVGB_FormatFloat(&buf, value, true);
    }
    XML_WriteAttrNoEsc(out, attr->name, buf);
    return true;
}

/**
 * Placeholder for attributes that are not implemented
 */
static bool SVGB_DecodeFail(const SvgAttr * attr, const SvgElement * elem,
                            QDataStream * in, QTextStream * out)
{
    error.append(QObject::tr("Element \"%1\" is not implemented (offset 0x%2)<br>").arg(attr->name).arg(in->device()->pos()-2,8,16,QChar( '0' )));
    return false;
}


static bool SVGB_DecodeUn(const SvgAttr * attr, const SvgElement * elem,
                          QDataStream * in, QTextStream * out)
{
    char i;
    in->readRawData(&i,1);
    if(i==1 && !(attr->name.compare(QObject::tr("stroke-dasharray")))) in->skipRawData(4);

    return true;
}

static bool SVGB_DecodeHref(const SvgAttr * attr, const SvgElement * elem,
                            QDataStream * in, QTextStream * out)
{



    QString str = SVGB_ReadString(in);
    if (!str.isNull()) {
        XML_WriteAttrNoEsc(out, attr->name, str);

        // Next comes another string, usually the same as the first one
        char len;
        in->readRawData(&len,-1);

        if (len ==0) {

            return true;
        } else if(len >0) {
            in->skipRawData(len);
            return true;
        }
        return false;
    }
    return false;
}

static bool SVGB_DecodeLHref(const SvgAttr * attr, const SvgElement * elem,
                             QDataStream * in, QTextStream * out)
{



    QString str = SVGB_ReadLString(in);
    if (!str.isNull()) {
        XML_WriteAttrNoEsc(out, attr->name, str);

        // Next comes another string, usually the same as the first one
        char len;
        in->readRawData(&len,-1);

        if (len ==0) {

            return true;
        } else if(len >0) {
            in->skipRawData(len);
            return true;
        }
        return false;
    }
    return false;
}

/**
 * XX           - flag (0 == color, 1 == URL)
 * followed by either 32-bit color or URL string
 */
static bool SVGB_DecodeFill(const SvgAttr * attr, const SvgElement * elem,
                            QDataStream * in, QTextStream * out)
{
    qint8 flag;
    in->readRawData((char *)&flag,1);
    if (flag >= 0)
    {
        char color[4];

        switch (flag)
        {
        case 0:
            if (ReadBytes(in,color,4))
            {
                int i;
                const SvgFill * fill = NULL;
                for (i=0; i<COUNT(SVGB_FILL); i++) {
                    if (!memcmp(SVGB_FILL[i].value,color,sizeof(color))) {
                        fill = SVGB_FILL + i;
                        break;
                    }
                }
                if (fill) {
                    XML_WriteAttrNoEsc(out, attr->name, fill->name);
                } else {
                    QString buf;

                    QString color0 = QString().sprintf("%02X",color[0]).right(2);
                    QString color1 = QString().sprintf("%02X",color[1]).right(2);
                    QString color2 = QString().sprintf("%02X",color[2]).right(2);

                    if(is_new==0 || is_new==1)
                        buf = QString("#%1%2%3").arg(color0).arg(color1).arg(color2);
                    else
                        buf = QString("#%1%2%3").arg(color2).arg(color1).arg(color0);

                    XML_WriteAttrNoEsc(out, attr->name, buf);
                }
                return true;
            }


            break;

        case 1:
            {
                char len;
                in->readRawData(&len,1);

                QByteArray str;
                str = in->device()->read(len);
                str.replace('\0',QString().null);

                XML_WriteAttrNoEsc(out, attr->name, "url(#"+str+")");

                return true;
            }
        default:
            error.append(QObject::tr("Unsupported fill flag %1<br>").arg((unsigned int)flag));
            return false;
        }
    }

    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
    return false;
}

/**
 * XX XX XX XX  - Color
 */
static bool SVGB_DecodeCssColor(const SvgAttr * attr, const SvgElement * elem,
                                QDataStream * in, QTextStream * out)
{
    char color[4];
    if (ReadBytes(in,color,4))
    {
        QString buf;

        QString color0 = QString().sprintf("%02X",color[0]).right(2);
        QString color1 = QString().sprintf("%02X",color[1]).right(2);
        QString color2 = QString().sprintf("%02X",color[2]).right(2);

        if(is_new==0 || is_new==1)
            buf = QString("#%1%2%3").arg(color0).arg(color1).arg(color2);
        else
            buf = QString("#%1%2%3").arg(color2).arg(color1).arg(color0);


        XML_WriteAttrNoEsc(out, attr->name, buf);
        return true;
    }
    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
    return false;
}

/**
 * XX  - enum value
 */
static bool SVGB_DecodeEnum(const SvgAttr * attr,
                            QDataStream * in, QTextStream * out,
                            const QString values[], int n)
{
    qint8 flag;
    in->readRawData((char *)&flag,1);

    if (flag >= 0) {
        if (flag < n) {
            XML_WriteAttrNoEsc(out, attr->name, values[flag]);
        } else {
            error.append(QObject::tr("<i>Unexpected value %1 for %2<i><br>").arg(flag).arg(attr->name));
        }
        return true;
    }
    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
    return false;
}

static bool SVGB_DecodeSpreadMethod(const SvgAttr * attr, const SvgElement * elem,
                                    QDataStream * in, QTextStream * out)
{
    static const QString values[] = {"pad", "reflect","repeat"};
    return SVGB_DecodeEnum(attr, in, out, values, COUNT(values));
}

static bool SVGB_DecodeGradUnits(const SvgAttr * attr, const SvgElement * elem,
                                 QDataStream * in, QTextStream * out)
{
    static const QString values[] = {"userSpaceOnUse", "objectBoundingBox"};
    return SVGB_DecodeEnum(attr, in, out, values, COUNT(values));
}

/**
 * XX XX XX XX - 32-bit enum value
 */
static bool SVGB_DecodeCssEnum(const SvgAttr * attr,
                               QDataStream * in, QTextStream * out,
                               const QString values[], qint32 n, QString defval)
{


    qint32 i;
    if (FILE_ReadI32L(in, &i)) {
        if (i < n) {
            XML_WriteAttrNoEsc(out, attr->name, values[i]);
        } else if (!defval.isEmpty()) {
            XML_WriteAttrNoEsc(out, attr->name, defval);
        } else {
            error.append(QObject::tr("<i>Unexpected value %1 for %2<i><br>").arg(i).arg(attr->name));
        }
        return true;
    }

    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
    return false;
}

static bool SVGB_DecodeDisplay(const SvgAttr * attr, const SvgElement * elem,
                               QDataStream * in, QTextStream * out)
{
    static const QString values[] = {"inline", NULL, NULL, NULL, NULL, NULL,
                                     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "none"};
    return SVGB_DecodeCssEnum(attr, in, out, values, COUNT(values),"inherit");
}

static bool SVGB_DecodeVisibility(const SvgAttr * attr, const SvgElement * elem,
                                  QDataStream * in, QTextStream * out)
{
    static const QString values[] = {"visible", "hidden"};
    return SVGB_DecodeCssEnum(attr, in, out, values, COUNT(values), NULL);
}

static bool SVGB_DecodeFontStyle(const SvgAttr * attr, const SvgElement * elem,
                                 QDataStream * in, QTextStream * out)
{
    static const QString values[] = {"normal", "italic", "oblique", "inherit"};
    return SVGB_DecodeCssEnum(attr, in, out, values, COUNT(values), NULL);
}

static bool SVGB_DecodeFontWeight(const SvgAttr * attr, const SvgElement * elem,
                                  QDataStream * in, QTextStream * out)
{
    static const QString values[] = {"normal", "bold", "bolder", "lighter",
                                     "100", "200", "300", "400", "500", "600", "700", "800", "900",
                                     "inherit"};
    return SVGB_DecodeCssEnum(attr, in, out, values, COUNT(values), NULL);
}

static bool SVGB_DecodeTextAnchor(const SvgAttr * attr, const SvgElement * elem,
                                  QDataStream * in, QTextStream * out)
{
    static const QString values[] = {"start", "middle", "end"};

    return SVGB_DecodeCssEnum(attr, in, out, values, COUNT(values), NULL);
}

static bool SVGB_DecodeTextDecor(const SvgAttr * attr, const SvgElement * elem,
                                 QDataStream * in, QTextStream * out)
{
    static const QString values[]={"none","underline","overline","line-through"};
    return SVGB_DecodeCssEnum(attr, in, out, values, COUNT(values), NULL);
}

/**
 * XX  - size in bytes
 * XXXX XXXX - UCS2 string
 */
static bool SVGB_DecodeString(const SvgAttr * attr, const SvgElement * elem,
                              QDataStream * in, QTextStream * out)
{
    char len;
    in->readRawData(&len,1);

    QByteArray str;
    str = in->device()->read(len);
    str.replace('\0',QString().null);

    XML_WriteAttrNoEsc(out, attr->name, str);

    return true;
}

/**
 * XX XX            - number of segments
 * XX * n           - segment types
 * XX XX            - number of 32 bit floating point values following
 * XX XX XX XX * n  - 32 bit floating point values
 */
static bool SVGB_DecodePath(const SvgAttr * attr, const SvgElement * elem,
                            QDataStream * in, QTextStream * out)
{
    qint16 nseg;
    if (FILE_ReadI16L(in, &nseg)) {
        char *seg=new char[nseg*sizeof(nseg)];

        if (seg) {
            if (FILE_ReadAll(in, seg, nseg)) {
                qint16 nval;
                if (FILE_ReadI16L(in, &nval)) {
                    quint16 iseg, ival = 0;
                    QString buf;

                    for (iseg=0; iseg<nseg; iseg++) {
                        if (seg[iseg] < COUNT(SVG_PATH_SEG)) {
                            int i;
                            const SvgPathSegType * s = SVG_PATH_SEG+seg[iseg];
                            buf.append(s->command);
                            for(i=0; i<s->params; i++, ival++) {
                                float val;
                                if (ival >= nval) {
                                    return false;
                                }
                                if (!ReadFloat32(in, &val)) {
                                    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
                                    return false;
                                }
                                if (i > 0 && val >= 0) {
                                    buf.append(',');
                                }
                                SVGB_FormatFloat(&buf, val, true);
                            }
                        } else {
                            return false;
                        }
                    }
                    XML_WriteAttrNoEsc(out, attr->name,buf);
                    return true;
                }
            }
        } else {
            return false;
        }
    }
    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
    return false;
}

/**
 * XX XX            - number of bytes following (n)
 * XX * n           - bytes (segment types?)
 * XX XX            - number of 32 bit floating point values following
 * XX XX XX XX * n  - 32 bit floating point values, 2 for each point
 */
static bool SVGB_DecodePoints(const SvgAttr * attr, const SvgElement * elem,
                              QDataStream * in, QTextStream * out)
{
    qint16 nflags;
    if (FILE_ReadI16L(in, &nflags)) {
        char *flags=new char[nflags*sizeof(nflags)];
        if (flags) {
            if (FILE_ReadAll(in, flags, nflags)) {
                qint16 nval;
                if (FILE_ReadI16L(in, &nval)) {
                    qint16 i;
                    QString buf;
                    for (i=0; i<nval/2; i++) {
                        float x, y;
                        if (!ReadFloat32(in, &x) || !ReadFloat32(in, &y)) {
                            error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
                            return false;
                        }
                        if (i > 0) buf.append(' ');
                        SVGB_FormatFloat(&buf, x, true);
                        buf.append(' ');
                        SVGB_FormatFloat(&buf, y, true);
                    }

                    XML_WriteAttrNoEsc(out, attr->name, buf);
                    return true;
                }
            }

        } else {
            return false;
        }
    }
    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
    return false;
}

/**
 * XX XX XX XX  - 32 bit floating point value (matrix[0][0])
 * XX XX XX XX  - 32 bit floating point value (matrix[0][1])
 * XX XX XX XX  - 32 bit floating point value (matrix[0][2])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][0])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][1])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][2])
 */
static bool SVGB_DecodeMatrix(const SvgAttr * attr,
                              QDataStream * in, QTextStream * out,
                              float divisor)
{
    int i;
    QString buf;

    float matrix[6];
    static const int trans[6] = {0, 3, 1, 4, 2, 5};

    for (i=0; i<COUNT(matrix); i++) {
        if (ReadFloat32(in, matrix+i)) {
            if(!is_new) matrix[i] /= divisor;
        } else {
            error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
            return false;
        }
    }

    buf.append("matrix(");
    for (i=0; i<COUNT(matrix); i++) {
        if (i > 0) buf.append(' ');
        SVGB_FormatFloat(&buf, matrix[trans[i]], true);
    }
    buf.append(")");
    XML_WriteAttrNoEsc(out, attr->name, buf);

    return true;
}

/**
 * XX XX XX XX  - 32 bit floating point value (matrix[0][0])
 * XX XX XX XX  - 32 bit floating point value (matrix[0][1])
 * XX XX XX XX  - 32 bit floating point value (matrix[0][2])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][0])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][1])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][2])
 */
static bool SVGB_DecodeGradTrans(const SvgAttr * attr, const SvgElement * elem,
                                 QDataStream * in, QTextStream * out)
{
    return SVGB_DecodeMatrix(attr, in, out, 65535.0f);
}

/**
 * XX XX XX XX  - 32 bit floating point value (matrix[0][0])
 * XX XX XX XX  - 32 bit floating point value (matrix[0][1])
 * XX XX XX XX  - 32 bit floating point value (matrix[0][2])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][0])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][1])
 * XX XX XX XX  - 32 bit floating point value (matrix[1][2])
 * XX XX XX XX  - transform type?
 */
static bool SVGB_DecodeTrans(const SvgAttr * attr, const SvgElement * elem,
                             QDataStream * in, QTextStream * out)
{
    if (SVGB_DecodeMatrix(attr, in, out, 1.0f)) {
        qint32 type;
        FILE_ReadI32L(in,&type);
        return true;
    }

    return false;
}

/**
 * XX XX XX XX  - 32 bit floating point value (X)
 * XX XX XX XX  - 32 bit floating point value (Y)
 * XX XX XX XX  - 32 bit floating point value (Width)
 * XX XX XX XX  - 32 bit floating point value (Height)
 */
static bool SVGB_DecodeRect(const SvgAttr * attr, const SvgElement * elem,
                            QDataStream * in, QTextStream * out)
{
    int i;
    QString buf;

    for (i=0; i<4; i++)
    {
        float x;
        if (!ReadFloat32(in, &x))
        {
            error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
            //STRBUF_Destroy(&buf.sb);
            return false;
        }

        if (i > 0) buf.append(' ');
        SVGB_FormatFloat(&buf, x, true);
    }

    XML_WriteAttrNoEsc(out, attr->name, buf);

    return true;
}

/**
 * XX XX XX XX  - 32 bit floating point value
 */
static bool SVGB_DecodeFloat(const SvgAttr * attr, const SvgElement * elem,
                             QDataStream * in, QTextStream * out)
{
    if(SVGB_ReadWriteFloatAttr(attr,elem,in,out)) return true;
    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
    return false;
}

/**
 * Need special case for opacity attribute?
 */
static bool SVGB_DecodeOpacity(const SvgAttr * attr, const SvgElement * elem,
                               QDataStream * in, QTextStream * out)
{
    return SVGB_DecodeFloat(attr, elem, in, out);
}


/**
 * XX           - 1 for percent, 0 otherwise (root element only)
 * XX XX XX XX  - 32 bit floating point value
 */
static bool SVGB_DecodeSize(const SvgAttr * attr, const SvgElement * elem,
                            QDataStream * in, QTextStream * out)
{
    float value;
    char flag = 0;

    if (elem->code == SvgElementSVG)
    {
        in->readRawData(&flag,1);
    }

    if( ReadFloat32(in, &value) )
    {
        SVGB_WriteFloatAttr(out, attr->name, value, (flag != 0));
        return true;
    }


    if(SVGB_ReadWriteFloatAttr(attr,elem,in,out)) return true;


    error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
    return false;
}

/**
 * Decodes a single element.
 */
static SvgDecodeElemStatus SVGB_DecodeElement(const SvgElement * elem, QDataStream* in, QTextStream* out)
{
    int i;
    char endmark[2];
    /* Add implied attributes */
    XML_StartTag(out, elem->name);

    for (i=0; i<elem->nattr; i++)
    {
        XML_WriteAttrNoEsc(out, elem->attr[i].name, elem->attr[i].value);
    }

    /* Loop until we hit SvgAtrEnd mark */
    while (ReadBytes(in,endmark,2))
    {

        qint16 id = 0;
        if (!memcmp(endmark, SvgAtrEnd, sizeof(SvgAtrEnd)))
        {
            char next;
            if (in->readRawData(&next, 1))
            {
                if (next == SvgElementEnd)
                {
                    *out << "/>\xD\xA";
                    return SvgDecodeElemFinished;
                }
                else
                {
                    in->device()->seek(in->device()->pos()-1);
                    *out << ((next == SvgCData) ? ">" : ">\xD\xA");
                    WRAP_Indent(+1);
                    return SvgDecodeElemStarted;
                }

            }
            else
            {
                error.append(QObject::tr("Read error:(%1)<br>").arg(__LINE__));
                return SvgDecodeError;
            }
        }

        /* Handle next attribute */
        in->device()->seek(in->device()->pos()-sizeof(endmark));
        FILE_ReadI16L(in,&id);
        if (id < COUNT(SVGB_ATTRS))
        {
            const SvgAttr * a = SVGB_ATTRS + id;
            if (!a->decode(a, elem, in, out))
            {

                return SvgDecodeError;
            }

        }
        else
        {
            error.append(QObject::tr("Unexpected attribute %1 at offset 0x%2(%3)<br>").arg(id).
                         arg(in->device()->pos()-2,0,16,QChar('0')).arg(in->device()->pos()-2));
            break;
        }

    }
    error.append(QObject::tr("Unhandled error:(%1)").arg(__LINE__));
    return SvgDecodeError;
}

/**
 * Decodes SVGB stream, writes XML to the output stream.
 */
static bool SVGB_Decode2(QString& inFileName, QString& outFileName)
{
    QFile inFile(inFileName);
    inFile.open(QIODevice::ReadOnly);

    qint32 start;
    QDataStream in(&inFile);
    in.setByteOrder(QDataStream::LittleEndian);
    in >> start;

    QFile outFile(outFileName);
    outFile.remove();
    outFile.open(QIODevice::WriteOnly);
    QTextStream out(&outFile);

    if (start == SvgFileStart  ||
        start == SvgFileStart2 ||
        start == SvgFileStart3 ||
        start == SvgFileStart4)
    {
        if(start == SvgFileStart2) is_new=1;
        if(start == SvgFileStart3) is_new=2;
        if(start == SvgFileStart4) is_new=3;

        out << SVG_START;

        char c;
        QStringList stack;

        //qint8 c;
        while ( in.readRawData(&c,1)!=-1 )
        {
            if (c == SvgFileEnd)
            {
                if ( in.readRawData(&c,1)!=-1 )
                {
                    inFile.seek(inFile.pos()-1);
                    error.append(QObject::tr("Trailing date ignored<br>"));
                }
                //        ASSERT(STACK_IsEmpty(stack));

                return true;
            }
            else
                if (c == SvgElementEnd)
                {
                WRAP_Indent(-1);
                QString elemName = STACK_Pop(&stack);
                //if (elem) {
                // XML_CloseTag(out, elem->name, true);
                XML_CloseTag(&out, elemName,true);
                //   } else {
                //Error("%s: parse error (element stack underflow)\n",   pname);
                //       return false;
                //   }

            }
            else
                if (c == SvgCData)
                {/*
                    char * str = SVGB_ReadString(in);
                    if (str) {
                        Bool wrap = WRAP_IsEnabled(out);
                        if (wrap) WRAP_Enable(out, False);
                        FILE_Puts(out, str);
                        if (wrap) WRAP_Enable(out, True);
                        MEM_Free(str);
                    } else {
                        return False;
                        */
                error.append(QObject::tr("CData ignored<br>"));
            }
            else
                if (c < COUNT(SVGB_ELEMS))
                {
                const SvgElement * elem = SVGB_ELEMS + c;
                SvgDecodeElemStatus s = SVGB_DecodeElement(elem, &in, &out);
                /*  ASSERT(elem->code == c); */
                switch (s)
                {
                case SvgDecodeElemStarted:{
                        WRAP_Indent(1);
                        STACK_Push(&stack, elem);
                    }
                case SvgDecodeElemFinished:
                    break;
                case SvgDecodeError:

                default:
                    // Error("unexpected element xxx at offset %1\n",in.device()->pos());
                    //error.append(QObject::tr("unexpected element %1(%3) at offset %2<br>").arg(elem->name).arg(in.device()->pos(),8,16,QChar( '0' )).arg(elem->code));
                    return false;
                }

            }
            else
            {
                error.append(QObject::tr("Unexpected element %1 at offset %2<br>").arg((unsigned int)c).arg(in.device()->pos(),8,16,QChar( '0' )));
                return false;
            }
        }

    }
    else
    {

        error.append(QObject::tr("This is not an S60 svgb file"));
        return false;
    }
    error.append(QObject::tr("Error reading input<br>"));
    return false;
}

/**
 * Decodes SVGB stream, writes XML to the output stream.
 */
bool SVGB_Decode(QString inFileName, QString outFileName,QString &reterr)
{
    bool ok;
    error.clear();
    ok = SVGB_Decode2(inFileName, outFileName);
    if (ok == false){
        QFile::remove(outFileName);
    }
    reterr.append(error);
    return ok;
}

Decoder::Decoder(const QStringList& files,QObject* parent):
        QThread(parent)
{
    fileList=files;
    fileCount=files.count();
    incBarAfter=(fileCount/100)+1;
    connect(this,SIGNAL(IncProgressBar(int)),parent,SLOT(ProgressBarIncNeed(int)));
    connect(this,SIGNAL(FileProcessed(const QString&)),parent,SLOT(FileProcessed(const QString&)));
    connect(this,SIGNAL(Finish()),parent,SLOT(FileProcessFinish()));
}

void Decoder::run()
{
        for (int i=0;i<fileCount;i++) {
            QString origFileName = fileList.at(i);
            origFileName.truncate(origFileName.lastIndexOf(".svgb"));
            QString convFileName = origFileName  + ".svg";
            origFileName =  fileList.at(i);
            QString eror;
            if((SVGB_Decode(origFileName, convFileName,eror)))
            {
                emit FileProcessed(tr("%1 <font color='green'><b>%2</b></font>").arg(origFileName).arg("OK!"));

            }
            else
            {
                emit FileProcessed(tr("%1 <font color='red'><b>%2</b>%3</font>").arg(origFileName).arg("ERROR! ").arg(eror));
                errorFileList.append(origFileName);
            }
            if(i%incBarAfter==0) emit IncProgressBar(i/incBarAfter);

        }
emit Finish();
}
