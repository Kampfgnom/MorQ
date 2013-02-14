#include "downloadsitemdelegate.h"

#include "model/downloadsitemmodel.h"

#include <downloadpackage.h>

#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>

DownloadsItemDelegate::DownloadsItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void DownloadsItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() != DownloadsItemModel::UserInputColumn) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QPixmap pixmap = index.data().value<QPixmap>();
    if(pixmap.isNull()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

//    QStyledItemDelegate::paint(painter, option, index);
    painter->drawPixmap(option.rect, pixmap);
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
    if(index.column() != DownloadsItemModel::UserInputColumn)
        return QStyledItemDelegate::sizeHint(option, index);

    QPixmap pixmap = index.data().value<QPixmap>();
    if(pixmap.isNull())
        return QStyledItemDelegate::sizeHint(option, index);

    return pixmap.size();
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
