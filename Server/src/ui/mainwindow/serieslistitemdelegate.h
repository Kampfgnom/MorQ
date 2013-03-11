#ifndef SERIESLISTITEMDELEGATE_H
#define SERIESLISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class SeriesListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SeriesListItemDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // SERIESLISTITEMDELEGATE_H
