#pragma once
#include <QTreeView>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QStyledItemDelegate>
#include <QFileSystemModel>

class FileItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit FileItemDelegate(QObject *parent = nullptr);
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, 
               const QModelIndex &index) const override;
    
    QSize sizeHint(const QStyleOptionViewItem &option, 
                   const QModelIndex &index) const override;
                   
private:
    QChar getFileIcon(const QString &fileName) const;
    QChar getFolderIcon(bool isExpanded) const;
    QColor getFileIconColor(const QString &fileName) const;
    mutable QFont m_iconFont;
};

class CustomTreeView : public QTreeView {
    Q_OBJECT
public:
    explicit CustomTreeView(QWidget *parent = nullptr);
    
protected:
    void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const override;
    
private:
    FileItemDelegate *m_itemDelegate;
};
