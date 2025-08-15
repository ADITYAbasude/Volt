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

    // Get file/folder data
    QString fileName = model->fileName(index);
    bool isDir = model->isDir(index);

    // Handle selection and hover states
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    else if (option.state & QStyle::State_MouseOver)
    {
        painter->fillRect(option.rect, Theme::instance().getColor("explorer.hoverBackground"));
    }

    // Set font for icon
    painter->setFont(m_iconFont);

    // Set icon color based on file type
    QColor iconColor = isDir ? Theme::instance().getColor("explorer.foreground") : getFileIconColor(fileName);
    painter->setPen(iconColor);

    QChar iconChar = isDir ? getFolderIcon(option.state & QStyle::State_Open) : getFileIcon(fileName);

    // Determine rect for icon drawing
    QRect iconRect = QRect(option.rect.left() + 2, option.rect.top() + 2, 16, option.rect.height() - 4);

    // Draw icon
    painter->drawText(iconRect, Qt::AlignCenter, iconChar);

    // Prepare and draw text
    QRect textRect = option.rect;
    textRect.setLeft(iconRect.right() + 4); // Provide spacing between icon and text
    painter->setFont(option.font);
    painter->setPen(QPen(option.palette.text().color(), 0));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, fileName);

    painter->restore();
}

QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(22); // Uniform row height
    return size;
}

QChar FileItemDelegate::getFileIcon(const QString &fileName) const
{
    QString extension = QFileInfo(fileName).suffix().toLower();
    QChar iconChar;

    // Programming languages
    if (extension == "cpp" || extension == "cxx" || extension == "cc" ||
        extension == "h" || extension == "hpp" || extension == "hxx")
    {
        iconChar = Theme::instance().getCarbonIconChar("code");
    }
    else if (extension == "js" || extension == "ts" || extension == "jsx" || extension == "tsx")
    {
        iconChar = Theme::instance().getCarbonIconChar("code");
    }
    else if (extension == "py" || extension == "pyw")
    {
        iconChar = Theme::instance().getCarbonIconChar("code");
    }
    else if (extension == "java" || extension == "kt" || extension == "scala")
    {
        iconChar = Theme::instance().getCarbonIconChar("code");
    }
    else if (extension == "cs" || extension == "vb")
    {
        iconChar = Theme::instance().getCarbonIconChar("code");
    }
    else if (extension == "go" || extension == "rs" || extension == "swift")
    {
        iconChar = Theme::instance().getCarbonIconChar("code");
    }
    // Web files
    else if (extension == "html" || extension == "htm" || extension == "xhtml")
    {
        iconChar = Theme::instance().getCarbonIconChar("html");
    }
    else if (extension == "css" || extension == "scss" || extension == "sass" || extension == "less")
    {
        iconChar = Theme::instance().getCarbonIconChar("css");
    }
    // Data files
    else if (extension == "json" || extension == "xml" || extension == "yaml" || extension == "yml")
    {
        iconChar = Theme::instance().getCarbonIconChar("data-vis-1");
    }
    // Images
    else if (extension == "png" || extension == "jpg" || extension == "jpeg" ||
             extension == "gif" || extension == "bmp" || extension == "svg" ||
             extension == "ico" || extension == "webp")
    {
        iconChar = Theme::instance().getCarbonIconChar("image");
    }
    // Documents
    else if (extension == "md" || extension == "markdown")
    {
        iconChar = Theme::instance().getCarbonIconChar("document");
    }
    else if (extension == "txt" || extension == "log")
    {
        iconChar = Theme::instance().getCarbonIconChar("document");
    }
    else if (extension == "pdf")
    {
        iconChar = Theme::instance().getCarbonIconChar("document-pdf");
    }
    // Config files
    else if (extension == "ini" || extension == "conf" || extension == "cfg" ||
             extension == "toml" || extension == "env")
    {
        iconChar = Theme::instance().getCarbonIconChar("settings");
    }
    // Default
    else
    {
        iconChar = Theme::instance().getCarbonIconChar("document-blank");
    }

    // Fallback if icon not found
    if (iconChar.isNull())
    {
        iconChar = QChar(0xf15b); // Default file icon
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

    // Programming languages - blue
    if (extension == "cpp" || extension == "cxx" || extension == "cc" ||
        extension == "h" || extension == "hpp" || extension == "hxx" ||
        extension == "cs" || extension == "vb" || extension == "java")
    {
        return QColor("#519aba");
    }
    // JavaScript/TypeScript - yellow
    else if (extension == "js" || extension == "ts" || extension == "jsx" || extension == "tsx")
    {
        return QColor("#cbcb41");
    }
    // Python - blue/yellow
    else if (extension == "py" || extension == "pyw")
    {
        return QColor("#3572a5");
    }
    // HTML - orange
    else if (extension == "html" || extension == "htm" || extension == "xhtml")
    {
        return QColor("#e34c26");
    }
    // CSS - blue
    else if (extension == "css" || extension == "scss" || extension == "sass" || extension == "less")
    {
        return QColor("#563d7c");
    }
    // JSON/Data files - green
    else if (extension == "json" || extension == "xml" || extension == "yaml" || extension == "yml")
    {
        return QColor("#a8c023");
    }
    // Markdown - blue
    else if (extension == "md" || extension == "markdown")
    {
        return QColor("#083fa1");
    }
    // Images - purple
    else if (extension == "png" || extension == "jpg" || extension == "jpeg" ||
             extension == "gif" || extension == "bmp" || extension == "svg")
    {
        return QColor("#a074c4");
    }
    // Default
    else
    {
        return theme.getColor("explorer.foreground");
    }
}

CustomTreeView::CustomTreeView(QWidget *parent)
    : QTreeView(parent),
      m_itemDelegate(new FileItemDelegate(this))
{
    setItemDelegate(m_itemDelegate);
}

void CustomTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    QTreeView::drawBranches(painter, rect, index); 
}
