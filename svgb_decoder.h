#ifndef UNSVGB_H
#define UNSVGB_H

#include <QThread>
#include <QStringList>
#include <QDataStream>
#include <QTextStream>

#define SVGB_EXIT_OK            0
#define SVGB_EXIT_USAGE         1
#define SVGB_EXIT_ERROR         2
#define SVGB_EXIT_UNSUPPORTED   3
#define APP_VERSION "1.4.0"



typedef struct _SvgFill
{
    QString name;
    qint8 value[4];
} SvgFill;

typedef struct _SvgPathSegType
{
    char command;       /* command character */
    int params;         /* number of parameters */
} SvgPathSegType;

typedef struct _SvgElementAttr
{
    QString name;
    QString value;
} SvgElementAttr;

typedef struct _SvgAttr SvgAttr;
typedef struct _SvgElement SvgElement;

struct _SvgElement
{
    QString name;                   /* element name */
    qint16 code;                    /* element code */
    const SvgElementAttr * attr;    /* additional attributes */
    int nattr;                      /* number of additional attributes */
};

typedef bool (*SvgAttrDecodeCB) (const SvgAttr * attr,
                                 const SvgElement * elem,
                                 QDataStream * in,
                                 QTextStream * out);
struct _SvgAttr
{
    QString name;
    SvgAttrDecodeCB decode;
};

typedef enum _SvgDecodeElemStatus
{
    SvgDecodeError,
    SvgDecodeElemStarted,
    SvgDecodeElemFinished
} SvgDecodeElemStatus;

typedef struct _SvgBlock
{
    const SvgElement * element;
} SvgBlock;

#define SvgAttrDecodePrototype(f) static bool f(const SvgAttr * attr, const SvgElement * elem, QDataStream * in, QTextStream * out);

class Decoder : public QThread
{
    Q_OBJECT
public:
    Decoder(const QStringList& files,QObject* parent=0);
    void run();
    QStringList errorFileList;
signals:
    void IncProgressBar(int val);
    void FileProcessed(const QString& result);
    void Finish();
private:
    QStringList fileList;
    int fileCount;
    int incBarAfter;
};

#endif // UNSVGB_H
