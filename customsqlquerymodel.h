#ifndef CUSTOMSQLQUERYMODEL_H
#define CUSTOMSQLQUERYMODEL_H

#include <QSqlQuery>
#include <QSqlQueryModel>

class CustomSqlQueryModel : public QSqlQueryModel {
    Q_OBJECT

public:
    CustomSqlQueryModel(const QStringList &headers, QObject *parent = nullptr)
        : QSqlQueryModel(parent), m_headers(headers) {}

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {

            if (section < m_headers.size()) {
                return m_headers.at(section);
            }
        }
        return QSqlQueryModel::headerData(section, orientation, role);
    }

private:
    QStringList m_headers;
};


#endif
