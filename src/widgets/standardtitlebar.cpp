/*
 * MIT License
 *
 * Copyright (C) 2022 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "standardtitlebar.h"
#include "standardtitlebar_p.h"
#include "standardsystembutton.h"
#include <QtCore/qcoreevent.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qboxlayout.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcStandardTitleBar, "wangwenx190.framelesshelper.widgets.standardtitlebar")
#define INFO qCInfo(lcStandardTitleBar)
#define DEBUG qCDebug(lcStandardTitleBar)
#define WARNING qCWarning(lcStandardTitleBar)
#define CRITICAL qCCritical(lcStandardTitleBar)

using namespace Global;

StandardTitleBarPrivate::StandardTitleBarPrivate(StandardTitleBar *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
    initialize();
}

StandardTitleBarPrivate::~StandardTitleBarPrivate() = default;

StandardTitleBarPrivate *StandardTitleBarPrivate::get(StandardTitleBar *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

const StandardTitleBarPrivate *StandardTitleBarPrivate::get(const StandardTitleBar *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

Qt::Alignment StandardTitleBarPrivate::titleLabelAlignment() const
{
    return m_labelAlignment;
}

void StandardTitleBarPrivate::setTitleLabelAlignment(const Qt::Alignment value)
{
    if (m_labelAlignment == value) {
        return;
    }
    m_labelAlignment = value;
    Q_Q(StandardTitleBar);
    q->update();
    Q_EMIT q->titleLabelAlignmentChanged();
}

bool StandardTitleBarPrivate::isExtended() const
{
    return m_extended;
}

void StandardTitleBarPrivate::setExtended(const bool value)
{
    if (m_extended == value) {
        return;
    }
    m_extended = value;
    Q_Q(StandardTitleBar);
    q->setFixedHeight(m_extended ? kDefaultExtendedTitleBarHeight : kDefaultTitleBarHeight);
    Q_EMIT q->extendedChanged();
}

bool StandardTitleBarPrivate::isHideWhenClose() const
{
    return m_hideWhenClose;
}

void StandardTitleBarPrivate::setHideWhenClose(const bool value)
{
    if (m_hideWhenClose == value) {
        return;
    }
    m_hideWhenClose = value;
    Q_Q(StandardTitleBar);
    Q_EMIT q->hideWhenCloseChanged();
}

ChromePalette *StandardTitleBarPrivate::chromePalette() const
{
    return m_chromePalette.data();
}

void StandardTitleBarPrivate::paintTitleBar(QPaintEvent *event)
{
    Q_UNUSED(event);
    Q_Q(StandardTitleBar);
    if (!m_window || m_chromePalette.isNull()) {
        return;
    }
    const bool active = m_window->isActiveWindow();
    const QColor backgroundColor = (active ?
        m_chromePalette->titleBarActiveBackgroundColor() :
        m_chromePalette->titleBarInactiveBackgroundColor());
    const QColor foregroundColor = (active ?
        m_chromePalette->titleBarActiveForegroundColor() :
        m_chromePalette->titleBarInactiveForegroundColor());
    QPainter painter(q);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing |
        QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter.fillRect(QRect(QPoint(0, 0), q->size()), backgroundColor);
    int titleLabelLeftOffset = 0;
    if (m_windowIconVisible) {
        const QIcon icon = m_window->windowIcon();
        if (!icon.isNull()) {
            const QSize size = (m_windowIconSize.isEmpty() ? kDefaultWindowIconSize : m_windowIconSize);
            const int y = qRound(qreal(q->height() - size.height()) / qreal(2));
            const QRect rect = {QPoint(kDefaultTitleBarContentsMargin, y), size};
            titleLabelLeftOffset = (rect.left() + rect.width());
            icon.paint(&painter, rect);
        }
    }
    if (m_titleLabelVisible) {
        const QString text = m_window->windowTitle();
        if (!text.isEmpty()) {
            painter.setPen(foregroundColor);
            painter.setFont(m_titleFont.value_or([q]() -> QFont {
                QFont f = q->font();
                f.setPointSize(kDefaultTitleBarFontPointSize);
                return f;
            }()));
            const QRect rect = [this, q, titleLabelLeftOffset]() -> QRect {
                const int w = q->width();
                int leftMargin = 0;
                int rightMargin = 0;
                if (m_labelAlignment & Qt::AlignLeft) {
                    leftMargin = (kDefaultTitleBarContentsMargin + titleLabelLeftOffset);
                }
                if (m_labelAlignment & Qt::AlignRight) {
                    rightMargin = (w - m_minimizeButton->x() + kDefaultTitleBarContentsMargin);
                }
                const int x = leftMargin;
                const int y = 0;
                const int width = (w - leftMargin - rightMargin);
                const int height = q->height();
                return {QPoint(x, y), QSize(width, height)};
            }();
            painter.drawText(rect, m_labelAlignment, text);
        }
    }
    painter.restore();
}

bool StandardTitleBarPrivate::titleLabelVisible() const
{
    return m_titleLabelVisible;
}

void StandardTitleBarPrivate::setTitleLabelVisible(const bool value)
{
    if (m_titleLabelVisible == value) {
        return;
    }
    m_titleLabelVisible = value;
    Q_Q(StandardTitleBar);
    q->update();
    Q_EMIT q->titleLabelVisibleChanged();
}

QSize StandardTitleBarPrivate::windowIconSize() const
{
    return m_windowIconSize;
}

void StandardTitleBarPrivate::setWindowIconSize(const QSize &value)
{
    Q_ASSERT(!value.isEmpty());
    if (value.isEmpty()) {
        return;
    }
    if (m_windowIconSize == value) {
        return;
    }
    m_windowIconSize = value;
    Q_Q(StandardTitleBar);
    q->update();
    Q_EMIT q->windowIconSizeChanged();
}

bool StandardTitleBarPrivate::windowIconVisible() const
{
    return m_windowIconVisible;
}

void StandardTitleBarPrivate::setWindowIconVisible(const bool value)
{
    if (m_windowIconVisible == value) {
        return;
    }
    m_windowIconVisible = value;
    Q_Q(StandardTitleBar);
    q->update();
    Q_EMIT q->windowIconVisibleChanged();
}

QFont StandardTitleBarPrivate::titleFont() const
{
    return m_titleFont.value_or(QFont());
}

void StandardTitleBarPrivate::setTitleFont(const QFont &value)
{
    if (titleFont() == value) {
        return;
    }
    m_titleFont = value;
    Q_Q(StandardTitleBar);
    q->update();
    Q_EMIT q->titleFontChanged();
}

void StandardTitleBarPrivate::updateMaximizeButton()
{
    const bool max = m_window->isMaximized();
    m_maximizeButton->setButtonType(max ? SystemButtonType::Restore : SystemButtonType::Maximize);
    m_maximizeButton->setToolTip(max ? tr("Restore") : tr("Maximize"));
}

void StandardTitleBarPrivate::updateTitleBarColor()
{
    Q_Q(StandardTitleBar);
    q->update();
}

void StandardTitleBarPrivate::updateChromeButtonColor()
{
    const bool active = m_window->isActiveWindow();
    const QColor activeForeground = m_chromePalette->titleBarActiveForegroundColor();
    const QColor inactiveForeground = m_chromePalette->titleBarInactiveForegroundColor();
    const QColor normal = m_chromePalette->chromeButtonNormalColor();
    const QColor hover = m_chromePalette->chromeButtonHoverColor();
    const QColor press = m_chromePalette->chromeButtonPressColor();
    m_minimizeButton->setActiveForegroundColor(activeForeground);
    m_minimizeButton->setInactiveForegroundColor(inactiveForeground);
    m_minimizeButton->setNormalColor(normal);
    m_minimizeButton->setHoverColor(hover);
    m_minimizeButton->setPressColor(press);
    m_minimizeButton->setActive(active);
    m_maximizeButton->setActiveForegroundColor(activeForeground);
    m_maximizeButton->setInactiveForegroundColor(inactiveForeground);
    m_maximizeButton->setNormalColor(normal);
    m_maximizeButton->setHoverColor(hover);
    m_maximizeButton->setPressColor(press);
    m_maximizeButton->setActive(active);
    m_closeButton->setActiveForegroundColor(activeForeground);
    m_closeButton->setInactiveForegroundColor(inactiveForeground);
    m_closeButton->setNormalColor(m_chromePalette->closeButtonNormalColor());
    m_closeButton->setHoverColor(m_chromePalette->closeButtonHoverColor());
    m_closeButton->setPressColor(m_chromePalette->closeButtonPressColor());
    m_closeButton->setActive(active);
}

void StandardTitleBarPrivate::retranslateUi()
{
    m_minimizeButton->setToolTip(tr("Minimize"));
    m_maximizeButton->setToolTip(m_window->isMaximized() ? tr("Restore") : tr("Maximize"));
    m_closeButton->setToolTip(tr("Close"));
}

bool StandardTitleBarPrivate::eventFilter(QObject *object, QEvent *event)
{
    Q_ASSERT(object);
    Q_ASSERT(event);
    if (!object || !event) {
        return false;
    }
    if (!object->isWidgetType()) {
        return QObject::eventFilter(object, event);
    }
    const auto widget = qobject_cast<QWidget *>(object);
    if (!widget->isWindow() || (widget != m_window)) {
        return QObject::eventFilter(object, event);
    }
    switch (event->type()) {
    case QEvent::WindowStateChange:
        updateMaximizeButton();
        break;
    case QEvent::ActivationChange:
        updateTitleBarColor();
        updateChromeButtonColor();
        break;
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}

void StandardTitleBarPrivate::initialize()
{
    Q_Q(StandardTitleBar);
    m_window = (q->nativeParentWidget() ? q->nativeParentWidget() : q->window());
    m_chromePalette.reset(new ChromePalette(this));
    connect(m_chromePalette.data(), &ChromePalette::titleBarColorChanged,
        this, &StandardTitleBarPrivate::updateTitleBarColor);
    connect(m_chromePalette.data(), &ChromePalette::chromeButtonColorChanged,
        this, &StandardTitleBarPrivate::updateChromeButtonColor);
    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    q->setFixedHeight(kDefaultTitleBarHeight);
    connect(m_window, &QWidget::windowIconChanged, this, [q](const QIcon &icon){
        Q_UNUSED(icon);
        q->update();
    });
    connect(m_window, &QWidget::windowTitleChanged, this, [q](const QString &title){
        Q_UNUSED(title);
        q->update();
    });
    m_minimizeButton.reset(new StandardSystemButton(SystemButtonType::Minimize, q));
    connect(m_minimizeButton.data(), &StandardSystemButton::clicked, m_window, &QWidget::showMinimized);
    m_maximizeButton.reset(new StandardSystemButton(SystemButtonType::Maximize, q));
    updateMaximizeButton();
    connect(m_maximizeButton.data(), &StandardSystemButton::clicked, this, [this](){
        if (m_window->isMaximized()) {
            m_window->showNormal();
        } else {
            m_window->showMaximized();
        }
    });
    m_closeButton.reset(new StandardSystemButton(SystemButtonType::Close, q));
    connect(m_closeButton.data(), &StandardSystemButton::clicked, this, [this](){
        if (m_hideWhenClose) {
            m_window->hide();
        } else {
            m_window->close();
        }
    });
    // According to the title bar design guidance, the system buttons should always be
    // placed on the top-right corner of the window, so we need the following additional
    // layouts to ensure this.
    const auto systemButtonsInnerLayout = new QHBoxLayout;
    systemButtonsInnerLayout->setSpacing(0);
    systemButtonsInnerLayout->setContentsMargins(0, 0, 0, 0);
    systemButtonsInnerLayout->addWidget(m_minimizeButton.data());
    systemButtonsInnerLayout->addWidget(m_maximizeButton.data());
    systemButtonsInnerLayout->addWidget(m_closeButton.data());
    const auto systemButtonsOuterLayout = new QVBoxLayout;
    systemButtonsOuterLayout->setSpacing(0);
    systemButtonsOuterLayout->setContentsMargins(0, 0, 0, 0);
    systemButtonsOuterLayout->addLayout(systemButtonsInnerLayout);
    systemButtonsOuterLayout->addStretch();
    const auto titleBarLayout = new QHBoxLayout(q);
    titleBarLayout->setSpacing(0);
    titleBarLayout->setContentsMargins(0, 0, 0, 0);
    titleBarLayout->addStretch();
    titleBarLayout->addLayout(systemButtonsOuterLayout);
    q->setLayout(titleBarLayout);
    setTitleLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    retranslateUi();
    updateTitleBarColor();
    updateChromeButtonColor();
    m_window->installEventFilter(this);
}

StandardTitleBar::StandardTitleBar(QWidget *parent)
    : QWidget(parent), d_ptr(new StandardTitleBarPrivate(this))
{
}

StandardTitleBar::~StandardTitleBar() = default;

Qt::Alignment StandardTitleBar::titleLabelAlignment() const
{
    Q_D(const StandardTitleBar);
    return d->titleLabelAlignment();
}

void StandardTitleBar::setTitleLabelAlignment(const Qt::Alignment value)
{
    Q_D(StandardTitleBar);
    d->setTitleLabelAlignment(value);
}

StandardSystemButton *StandardTitleBar::minimizeButton() const
{
    Q_D(const StandardTitleBar);
    return d->m_minimizeButton.data();
}

StandardSystemButton *StandardTitleBar::maximizeButton() const
{
    Q_D(const StandardTitleBar);
    return d->m_maximizeButton.data();
}

StandardSystemButton *StandardTitleBar::closeButton() const
{
    Q_D(const StandardTitleBar);
    return d->m_closeButton.data();
}

bool StandardTitleBar::isExtended() const
{
    Q_D(const StandardTitleBar);
    return d->isExtended();
}

void StandardTitleBar::setExtended(const bool value)
{
    Q_D(StandardTitleBar);
    d->setExtended(value);
}

bool StandardTitleBar::isHideWhenClose() const
{
    Q_D(const StandardTitleBar);
    return d->isHideWhenClose();
}

void StandardTitleBar::setHideWhenClose(const bool value)
{
    Q_D(StandardTitleBar);
    d->setHideWhenClose(value);
}

ChromePalette *StandardTitleBar::chromePalette() const
{
    Q_D(const StandardTitleBar);
    return d->chromePalette();
}

bool StandardTitleBar::titleLabelVisible() const
{
    Q_D(const StandardTitleBar);
    return d->titleLabelVisible();
}

void StandardTitleBar::setTitleLabelVisible(const bool value)
{
    Q_D(StandardTitleBar);
    d->setTitleLabelVisible(value);
}

QSize StandardTitleBar::windowIconSize() const
{
    Q_D(const StandardTitleBar);
    return d->windowIconSize();
}

void StandardTitleBar::setWindowIconSize(const QSize &value)
{
    Q_D(StandardTitleBar);
    d->setWindowIconSize(value);
}

bool StandardTitleBar::windowIconVisible() const
{
    Q_D(const StandardTitleBar);
    return d->windowIconVisible();
}

void StandardTitleBar::setWindowIconVisible(const bool value)
{
    Q_D(StandardTitleBar);
    d->setWindowIconVisible(value);
}

QFont StandardTitleBar::titleFont() const
{
    Q_D(const StandardTitleBar);
    return d->titleFont();
}

void StandardTitleBar::setTitleFont(const QFont &value)
{
    Q_D(StandardTitleBar);
    d->setTitleFont(value);
}

void StandardTitleBar::paintEvent(QPaintEvent *event)
{
    Q_D(StandardTitleBar);
    d->paintTitleBar(event);
}

FRAMELESSHELPER_END_NAMESPACE
