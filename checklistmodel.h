#ifndef CUSTOMLISTMODEL_H
#define CUSTOMLISTMODEL_H

#include <QStringListModel>
#include <QFile>
#include <QTextStream>
#include <QColor>
#include <QSet>

class CustomListModel : public QStringListModel
{
public:
    CustomListModel(QObject* parent = 0);
    CustomListModel(const QStringList & strings, QObject* parent = 0);
    Qt::ItemFlags flags (const QModelIndex& index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role);
    void save();
private:
    QSet<QPersistentModelIndex> checkedItems;
};

#endif // CUSTOMLISTMODEL_H
