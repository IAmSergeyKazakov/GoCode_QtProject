// xlsxworkbook_p.h

#ifndef XLSXWORKBOOK_P_H
#define XLSXWORKBOOK_P_H

#include "xlsxabstractooxmlfile_p.h"
#include "xlsxrelationships_p.h"
#include "xlsxsimpleooxmlfile_p.h"
#include "xlsxtheme_p.h"
#include "xlsxworkbook.h"

#include <QStringList>

QT_BEGIN_NAMESPACE_XLSX

struct XlsxDefineNameData {
    XlsxDefineNameData()
        : sheetId(-1)
    {
    }
    XlsxDefineNameData(const QString &name,
                       const QString &formula,
                       const QString &comment,
                       int sheetId = -1)
        : name(name)
        , formula(formula)
        , comment(comment)
        , sheetId(sheetId)
    {
    }
    QString name;
    QString formula;
    QString comment;
    // using internal sheetId, instead of the localSheetId(order in the workbook)
    int sheetId;
};

class WorkbookPrivate : public AbstractOOXmlFilePrivate
{
    Q_DECLARE_PUBLIC(Workbook)
public:
    WorkbookPrivate(Workbook *q, Workbook::CreateFlag flag);

    std::shared_ptr<SharedStrings> sharedStrings;
    QList<std::shared_ptr<AbstractSheet>> sheets;
    QList<std::shared_ptr<SimpleOOXmlFile>> externalLinks;
    QStringList sheetNames;
    std::shared_ptr<Styles> styles;
    std::shared_ptr<Theme> theme;
    QList<std::shared_ptr<MediaFile>> mediaFiles;
    QList<std::shared_ptr<Chart>> chartFiles;
    QList<XlsxDefineNameData> definedNamesList;

    bool strings_to_numbers_enabled;
    bool strings_to_hyperlinks_enabled;
    bool html_to_richstring_enabled;
    bool date1904;
    QString defaultDateFormat;

    int x_window;
    int y_window;
    int window_width;
    int window_height;

    int activesheetIndex;
    int firstsheet;
    int table_count;

    // Used to generate new sheet name and id
    int last_worksheet_index;
    int last_chartsheet_index;
    int last_sheet_id;
};

QT_END_NAMESPACE_XLSX

#endif // XLSXWORKBOOK_P_H
