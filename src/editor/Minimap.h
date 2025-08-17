#pragma once

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QRectF>

class CodeEditor;

class Minimap : public QWidget
{
    Q_OBJECT
public:
    explicit Minimap(CodeEditor *editor, QWidget *parent = nullptr);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void scheduleUpdate();
    void doUpdate();

private:
    void regeneratePixmap();
    QRectF viewportRectOnMinimap() const;

    CodeEditor *m_editor;
    QPixmap m_pixmap;
    QTimer m_updateTimer;
    qreal m_scale;
};
