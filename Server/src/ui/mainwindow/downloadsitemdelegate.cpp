#include "downloadsitemdelegate.h"

#include "model/downloadsitemmodel.h"

#include <downloadpackage.h>
#include <download.h>

#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QBrush>
#include <QApplication>

DownloadsItemDelegate::DownloadsItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void DownloadsItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == DownloadsItemModel::UserInputColumn) {
        QPixmap pixmap = index.data().value<QPixmap>();
        if(pixmap.isNull()) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, NULL);
        painter->drawPixmap(option.rect, pixmap);
        return;
    }
    else if(index.column() == DownloadsItemModel::ProgressColumn) {
        QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, NULL);

        int progress = 0;

        if(!index.parent().isValid()) {
            DownloadPackage *package = static_cast<DownloadPackage *>(index.internalPointer());
            progress = package->progress() * 100;
        }
        else {
            Download *download = static_cast<Download *>(index.internalPointer());
            progress = download->progress() * 100;
        }

        QPoint topLeft = option.rect.topLeft() + QPoint(5, option.rect.height() / 2 - 10);
        QPoint bottomRight = option.rect.bottomRight() - QPoint(5, 0);

        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = QRect(topLeft, bottomRight);
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = progress;

        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                           &progressBarOption, painter);

        painter->drawText(option.rect, Qt::AlignCenter, QString("%1 %").arg(progress));

        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QWidget *DownloadsItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() != DownloadsItemModel::UserInputColumn)
        return QStyledItemDelegate::createEditor(parent, option, index);

    QPixmap pixmap = index.data().value<QPixmap>();
    if(pixmap.isNull())
        return QStyledItemDelegate::createEditor(parent, option, index);

    QWidget *editor = new CaptchaWidget(pixmap, parent);
    return editor;
}

void DownloadsItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Q_UNUSED(model);

    CaptchaWidget *widget = static_cast<CaptchaWidget *>(editor);
    DownloadPackage *package = static_cast<DownloadPackage *>(index.internalPointer());
    package->setCaptchaString(widget->text());
}

void DownloadsItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(index);
}

QSize DownloadsItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(32);
    return size;
}

void DownloadsItemDelegate::onEditingFinished()
{
    CaptchaWidget *editor = qobject_cast<CaptchaWidget *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

CaptchaWidget::CaptchaWidget(const QPixmap &pixmap, QWidget *parent) :
    QWidget(parent)
{
    QLayout *l = new QVBoxLayout;
    setLayout(l);

    lineEdit = new QLineEdit(this);
    l->addWidget(lineEdit);

    connect(lineEdit, &QLineEdit::editingFinished, this, &CaptchaWidget::editingFinished);
}

QString CaptchaWidget::text() const
{
    return lineEdit->text();
}
