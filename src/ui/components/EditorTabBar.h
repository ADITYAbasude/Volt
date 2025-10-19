#pragma once

#include <QTabBar>
#include <QMap>

class EditorTabBar : public QTabBar {
    Q_OBJECT
public:
    explicit EditorTabBar(QWidget *parent = nullptr);
    
    void setTabModified(int index, bool modified);
    bool isTabModified(int index) const;
    
    void applyTheme();

protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    QMap<int, bool> m_modifiedTabs;
};
