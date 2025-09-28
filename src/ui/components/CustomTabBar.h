#pragma once

#include <QTabBar>

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

    QSize tabSizeHint(int index) const override;
};
