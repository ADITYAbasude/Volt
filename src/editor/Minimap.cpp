#include "Minimap.h"
#include "CodeEditor.h"
#include <Qsci/qsciscintilla.h>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QFontMetrics>
#include <QDebug>

Minimap::Minimap(CodeEditor *editor, QWidget *parent)
    : QWidget(parent), m_editor(editor), m_scale(0.0)
{
    setMinimumWidth(80);
    setMaximumWidth(240);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(120);

    connect(&m_updateTimer, &QTimer::timeout, this, &Minimap::doUpdate);

    connect(m_editor, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(scheduleUpdate()));
    connect(m_editor, SIGNAL(textChanged()), this, SLOT(scheduleUpdate()));
    if (m_editor->verticalScrollBar()) {
        connect(m_editor->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scheduleUpdate()));
    }

    scheduleUpdate();
}

QSize Minimap::sizeHint() const
{
    return QSize(100, 200);
}

void Minimap::scheduleUpdate()
{
    if (!m_updateTimer.isActive()) m_updateTimer.start();
}

void Minimap::doUpdate()
{
    regeneratePixmap();
    update();
}

void Minimap::regeneratePixmap()
{
    // Use the high-level QsciScintilla API to get text (avoids sptr_t casts)
    QString fullText = m_editor->text();
    if (fullText.isEmpty()) {
        m_pixmap = QPixmap();
        return;
    }
    QStringList qlines = fullText.split('\n');

    QFont baseFont = m_editor->font();
    int targetPoint = qMax(4, baseFont.pointSize() / 3);
    QFont drawFont = baseFont;
    drawFont.setPointSize(targetPoint);
    QFontMetrics fm(drawFont);

    int linesCount = qlines.count();
    if (linesCount <= 0) {
        m_pixmap = QPixmap();
        return;
    }

    qreal heightPerLine = qreal(height()) / qreal(linesCount);
    if (heightPerLine < 1.0) heightPerLine = 1.0;

    int pixH = qMax(1, int(linesCount * heightPerLine));
    int w = width();
    QSize pixSize(w, pixH);
    QPixmap pm(pixSize);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setFont(drawFont);
    QColor textColor = Qt::lightGray;
    QColor background = Qt::black;
    QPalette pal = m_editor->palette();
    background = pal.color(QPalette::Base);
    textColor = pal.color(QPalette::Text).lighter(170);

    p.fillRect(pm.rect(), background);

    p.setPen(textColor);
    for (int i = 0; i < linesCount; ++i) {
        qreal y = i * heightPerLine;
        QRectF r(0, y, pm.width(), heightPerLine);
        QString elided = qlines.at(i);
        elided = elided.left(200);
        p.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, elided);
    }
    p.end();

    m_pixmap = pm;
    m_scale = heightPerLine;
}

void Minimap::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), palette().color(QPalette::Window));

    if (m_pixmap.isNull()) {
        regeneratePixmap();
    }

    if (!m_pixmap.isNull()) {
        QPixmap scaled = m_pixmap.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        p.drawPixmap(0, 0, scaled);

        QRectF vp = viewportRectOnMinimap();
        if (!vp.isNull()) {
            QPen pen(QColor(200, 200, 200, 180));
            pen.setWidth(1);
            p.setPen(pen);
            p.setBrush(QColor(0,0,0,0));
            p.drawRect(vp);
        }
    } else {
        p.setPen(Qt::gray);
        p.drawText(rect(), Qt::AlignCenter, "Minimap");
    }
}

QRectF Minimap::viewportRectOnMinimap() const
{
    int firstLine = m_editor->SendScintilla(QsciScintillaBase::SCI_GETFIRSTVISIBLELINE, 0UL, 0L);
    int visibleLines = m_editor->SendScintilla(QsciScintillaBase::SCI_LINESONSCREEN, 0UL, 0L);
    int totalLines = m_editor->SendScintilla(QsciScintillaBase::SCI_GETLINECOUNT, 0UL, 0L);
    if (totalLines <= 0) return QRectF();

    qreal y = (firstLine / (qreal)totalLines) * height();
    qreal h = (visibleLines / (qreal)totalLines) * height();
    return QRectF(0, y, width(), qMax<qreal>(1.0, h));
}

void Minimap::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    scheduleUpdate();
}

void Minimap::mousePressEvent(QMouseEvent *event)
{
    if (!m_pixmap.isNull()) {
        qreal ratio = qreal(event->position().y()) / qreal(height());
        int totalLines = m_editor->SendScintilla(QsciScintillaBase::SCI_GETLINECOUNT, 0UL, 0L);
        int targetLine = qBound(0, int(ratio * totalLines), totalLines - 1);

        int visible = m_editor->SendScintilla(QsciScintillaBase::SCI_LINESONSCREEN, 0UL, 0L);
        int first = qMax(0, targetLine - visible / 2);
        m_editor->SendScintilla(QsciScintillaBase::SCI_LINESCROLL, 0UL, first);
    }
}

void Minimap::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        mousePressEvent(event);
    }
}
