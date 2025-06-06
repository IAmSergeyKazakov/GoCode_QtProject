// xlsxworkbook.h

#ifndef XLSXWORKBOOK_H
#define XLSXWORKBOOK_H

#include "xlsxabstractooxmlfile.h"
#include "xlsxabstractsheet.h"
#include "xlsxglobal.h"

#include <memory>

#include <QIODevice>
#include <QImage>
#include <QList>

QT_BEGIN_NAMESPACE_XLSX

class SharedStrings;
class Styles;
class Drawing;
class Document;
class Theme;
class Relationships;
class DocumentPrivate;
class MediaFile;
class Chart;
class Chartsheet;
class Worksheet;
class WorkbookPrivate;

class QXLSX_EXPORT Workbook : public AbstractOOXmlFile
{
    Q_DECLARE_PRIVATE(Workbook)
public:
    ~Workbook();

    int sheetCount() const;
    AbstractSheet *sheet(int index) const;

    AbstractSheet *addSheet(const QString &name           = QString(),
                            AbstractSheet::SheetType type = AbstractSheet::ST_WorkSheet);
    AbstractSheet *insertSheet(int index,
                               const QString &name           = QString(),
                               AbstractSheet::SheetType type = AbstractSheet::ST_WorkSheet);
    bool renameSheet(int index, const QString &name);
    bool deleteSheet(int index);
    bool copySheet(int index, const QString &newName = QString());
    bool moveSheet(int srcIndex, int distIndex);

    AbstractSheet *activeSheet() const;
    bool setActiveSheet(int index);

    //    void addChart();
    bool defineName(const QString &name,
                    const QString &formula,
                    const QString &comment = QString(),
                    const QString &scope   = QString());
    bool isDate1904() const;
    void setDate1904(bool date1904);
    bool isStringsToNumbersEnabled() const;
    void setStringsToNumbersEnabled(bool enable = true);
    bool isStringsToHyperlinksEnabled() const;
    void setStringsToHyperlinksEnabled(bool enable = true);
    bool isHtmlToRichStringEnabled() const;
    void setHtmlToRichStringEnabled(bool enable = true);
    QString defaultDateFormat() const;
    void setDefaultDateFormat(const QString &format);

    // internal used member
    void addMediaFile(std::shared_ptr<MediaFile> media, bool force = false);
    QList<std::shared_ptr<MediaFile>> mediaFiles() const;
    void addChartFile(std::shared_ptr<Chart> chartFile);
    QList<std::shared_ptr<Chart>> chartFiles() const;

private:
    friend class Worksheet;
    friend class Chartsheet;
    friend class WorksheetPrivate;
    friend class Document;
    friend class DocumentPrivate;

    Workbook(Workbook::CreateFlag flag);

    void saveToXmlFile(QIODevice *device) const override;
    bool loadFromXmlFile(QIODevice *device) override;

    SharedStrings *sharedStrings() const;
    Styles *styles();
    Theme *theme();
    QList<QImage> images();
    QList<Drawing *> drawings();
    QList<std::shared_ptr<AbstractSheet>> getSheetsByTypes(AbstractSheet::SheetType type) const;
    QStringList worksheetNames() const;
    AbstractSheet *addSheet(const QString &name,
                            int sheetId,
                            AbstractSheet::SheetType type = AbstractSheet::ST_WorkSheet);
};

QT_END_NAMESPACE_XLSX

#endif // XLSXWORKBOOK_H
