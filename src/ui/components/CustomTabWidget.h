#pragma once

#include <QTabWidget>
#include <QTabBar>

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit CustomTabWidget(QWidget *parent = nullptr);
    void setCustomTabBar(QTabBar *tabBar) { setTabBar(tabBar); }
};
