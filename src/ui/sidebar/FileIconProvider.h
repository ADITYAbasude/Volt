#pragma once

#include <QFileIconProvider>
#include <QFileInfo>
#include <QIcon>
#include <QHash>
#include <QFont>

class FileIconProvider : public QFileIconProvider
{
public:
    FileIconProvider();
    
    QIcon icon(IconType type) const override;
    QIcon icon(const QFileInfo &info) const override;

private:
    QIcon createIconFromFont(const QString &iconChar, const QColor &color = QColor("#CCCCCC")) const;
    QString getFileIconChar(const QString &extension) const;
    
    mutable QHash<QString, QIcon> m_iconCache;
    QFont m_iconFont;
    QIcon m_folderIcon;
    QIcon m_fileIcon;
};