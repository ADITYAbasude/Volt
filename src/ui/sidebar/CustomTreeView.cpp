#include "CustomTreeView.h"
#include "../../themes/Theme.h"
#include "../../logging/VoltLogger.h"
#include <QFileSystemModel>
#include <QFileInfo>
#include <QStyleOptionViewItem>

FileItemDelegate::FileItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      m_iconFont("icons-carbon")
{
    m_iconFont.setPixelSize(16);
}

void FileItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QFileSystemModel *model = static_cast<QFileSystemModel *>(const_cast<QAbstractItemModel *>(index.model()));
    if (!model)
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();

    QRect itemRect = option.rect;
    QRect fullRect = option.rect;
    int indentPixels = 0;

    if (option.widget)
    {
        const QTreeView *view = qobject_cast<const QTreeView *>(option.widget);
        if (view)
        {
            itemRect = view->visualRect(index);
            indentPixels = view->indentation();
            QRect viewportRect = view->viewport()->rect();
            fullRect = QRect(viewportRect.left(), itemRect.top(), viewportRect.width(), itemRect.height());
        }
    }

    QString fileName = model->fileName(index);
    bool isDir = model->isDir(index);

    if ((option.state & QStyle::State_Selected))
    {
        QColor selection = Theme::instance().getColor("hoverColor");

        painter->fillRect(fullRect, selection);
    }

    if ((option.state & QStyle::State_MouseOver) && !(option.state & QStyle::State_Selected))
    {
        QColor hover = Theme::instance().getColor("hoverColor");

        painter->fillRect(fullRect, hover);
    }

    painter->setFont(m_iconFont);

    QColor iconColor = isDir ? Theme::instance().getColor("editor.foreground") : getFileIconColor(fileName);
    painter->setPen(iconColor);

    QChar iconChar = isDir ? getFolderIcon(option.state & QStyle::State_Open) : getFileIcon(fileName);

    int iconSize = 16;
    int iconLeft = itemRect.left() + 4;
    if (indentPixels > iconSize + 8)
    {
        iconLeft = itemRect.left() + (indentPixels / 2) - (iconSize / 2);
    }
    QRect iconRect(iconLeft, itemRect.top() + 3, iconSize, itemRect.height() - 6);

    painter->drawText(iconRect, Qt::AlignCenter, iconChar);

    // Draw text starting after the icon, allow text to span fullRect width
    QRect textRect = fullRect;
    textRect.setLeft(iconRect.right() + 6);
    painter->setFont(option.font);
    painter->setPen(QPen(option.palette.text().color(), 0));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, fileName);

    painter->restore();
}

QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(22);
    return size;
}

QChar FileItemDelegate::getFileIcon(const QString &fileName) const
{
    QString extension = QFileInfo(fileName).suffix().toLower();
    QChar iconChar;

    //TODO@ADITYAbasude: Add more file types and icons
    if (extension == "ini" || extension == "conf" || extension == "cfg" ||
             extension == "toml" || extension == "env")
    {
        iconChar = Theme::instance().getCarbonIconChar("settings");
    }
    else
    {
        iconChar = Theme::instance().getCarbonIconChar("document-blank");
    }

    if (iconChar.isNull())
    {
        iconChar = QChar(0xe095);
    }

    return iconChar;
}

QChar FileItemDelegate::getFolderIcon(bool isExpanded) const
{
    if (isExpanded)
    {
        return Theme::instance().getCarbonIconChar("chevron-down");
    }
    else
    {
        return Theme::instance().getCarbonIconChar("chevron-right");
    }
}

QColor FileItemDelegate::getFileIconColor(const QString &fileName) const
{
    QString extension = QFileInfo(fileName).suffix().toLower();
    Theme &theme = Theme::instance();

    //TODO@ADITYAbasude: Add more file types and colors
    return theme.getColor("editor.foreground");
}

CustomTreeView::CustomTreeView(QWidget *parent)
    : QTreeView(parent),
      m_itemDelegate(new FileItemDelegate(this))
{
    setUniformRowHeights(true);
    setItemDelegate(m_itemDelegate);
}

void CustomTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QTreeView::drawRow(painter, option, index);
}

void CustomTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    QTreeView::drawBranches(painter, rect, index);
}
