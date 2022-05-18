#include "checktreeitem.h"

CheckTreeItem::CheckTreeItem(const QList<QVariant> &data, CheckTreeItem *parent)
{
	parentItem = parent;
	itemData = data;
}

CheckTreeItem::~CheckTreeItem()
{
	qDeleteAll(childItems);
}

void
CheckTreeItem::appendChild(CheckTreeItem *item)
{
	childItems.append(item);
}

CheckTreeItem *
CheckTreeItem::child(int row)
{
	return childItems.value(row);
}

int
CheckTreeItem::childCount() const
{
	return childItems.count();
}

int CheckTreeItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<CheckTreeItem*>(this));

	return 0;
}
int CheckTreeItem::columnCount() const
{
	return itemData.count();
}

QVariant CheckTreeItem::data(int column) const
{
	return itemData.value(column);
}
CheckTreeItem *CheckTreeItem::parent()
{
	return parentItem;
}

//

CheckTreeModel::CheckTreeModel(const QString &data, QObject *parent)
	: QAbstractItemModel(parent)
{
	QList<QVariant> rootData;
	rootData << "Title" << "Summary";
	rootItem = new CheckTreeItem(rootData);
	setupModelData(data.split(QString("\n")), rootItem);
}

CheckTreeModel::~CheckTreeModel()
{
	delete rootItem;
}

QModelIndex CheckTreeModel::index(int row, int column, const QModelIndex &parent)
			const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	CheckTreeItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<CheckTreeItem*>(parent.internalPointer());

	CheckTreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex CheckTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	CheckTreeItem *childItem = static_cast<CheckTreeItem*>(index.internalPointer());
	CheckTreeItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}
int CheckTreeModel::rowCount(const QModelIndex &parent) const
{
	CheckTreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<CheckTreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}
int CheckTreeModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return static_cast<CheckTreeItem*>(parent.internalPointer())->columnCount();
	else
		return rootItem->columnCount();
}
QVariant CheckTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	CheckTreeItem *item = static_cast<CheckTreeItem*>(index.internalPointer());

	return item->data(index.column());
}
Qt::ItemFlags CheckTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
QVariant CheckTreeModel::headerData(int section, Qt::Orientation orientation,
							   int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}
void CheckTreeModel::setupModelData(const QStringList &lines, CheckTreeItem *parent)
{
	QList<CheckTreeItem*> parents;
	QList<int> indentations;
	parents << parent;
	indentations << 0;

	int number = 0;

	while (number < lines.count()) {
		int position = 0;
		while (position < lines[number].length()) {
			if (lines[number].mid(position, 1) != " ")
				break;
			position++;
		}

		QString lineData = lines[number].mid(position).trimmed();

		if (!lineData.isEmpty()) {
			// Read the column data from the rest of the line.
			QStringList columnStrings = lineData.split("\t", Qt::SkipEmptyParts);
			QList<QVariant> columnData;
			for (int column = 0; column < columnStrings.count(); ++column)
				columnData << columnStrings[column];

			if (position > indentations.last()) {
				// The last child of the current parent is now the new parent
				// unless the current parent has no children.

				if (parents.last()->childCount() > 0) {
					parents << parents.last()->child(parents.last()->childCount()-1);
					indentations << position;
				}
			} else {
				while (position < indentations.last() && parents.count() > 0) {
					parents.pop_back();
					indentations.pop_back();
				}
			}

			// Append a new item to the current parent's list of children.
			parents.last()->appendChild(new CheckTreeItem(columnData, parents.last()));
		}

		number++;
	}
}
