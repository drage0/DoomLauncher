#ifndef CHECKTREEITEM_H
#define CHECKTREEITEM_H

#include <QTreeView>


class CheckTreeItem
{
public:
	CheckTreeItem(const QList<QVariant> &data, CheckTreeItem *parent = 0);
	~CheckTreeItem();

	void appendChild(CheckTreeItem *child);

	CheckTreeItem *child(int row);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	int row() const;
	CheckTreeItem *parent();

private:
	QList<CheckTreeItem*> childItems;
	QList<QVariant> itemData;
	CheckTreeItem *parentItem;
};

class CheckTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	CheckTreeModel(const QString &data, QObject *parent = 0);
	~CheckTreeModel();

	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
	void setupModelData(const QStringList &lines, CheckTreeItem *parent);

	CheckTreeItem *rootItem;
};

class CheckTreeView : public QTreeView
{
};

#endif // CHECKTREEITEM_H
