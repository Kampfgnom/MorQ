#include "serieslistitemdelegate.h"

SeriesListItemDelegate::SeriesListItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void SeriesListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize SeriesListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(48);
    return size;
}
