/*
 * MIT License
 *
 * Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)
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

#pragma once

#include <FramelessHelper/Core/framelesshelpercore_global.h>

#if FRAMELESSHELPER_CONFIG(titlebar)

FRAMELESSHELPER_BEGIN_NAMESPACE

class ChromePalette;
class FRAMELESSHELPER_CORE_API ChromePalettePrivate : public QObject
{
    FRAMELESSHELPER_PRIVATE_QT_CLASS(ChromePalette)

public:
    explicit ChromePalettePrivate(ChromePalette *q);
    ~ChromePalettePrivate() override;

    Q_SLOT void refresh();

    // System-defined ones:
    QColor titleBarActiveBackgroundColor_sys = {};
    QColor titleBarInactiveBackgroundColor_sys = {};
    QColor titleBarActiveForegroundColor_sys = {};
    QColor titleBarInactiveForegroundColor_sys = {};
    QColor chromeButtonNormalColor_sys = {};
    QColor chromeButtonHoverColor_sys = {};
    QColor chromeButtonPressColor_sys = {};
    QColor closeButtonNormalColor_sys = {};
    QColor closeButtonHoverColor_sys = {};
    QColor closeButtonPressColor_sys = {};
    // User-defined ones:
    QSharedPointer<QColor> titleBarActiveBackgroundColor = nullptr;
    QSharedPointer<QColor> titleBarInactiveBackgroundColor = nullptr;
    QSharedPointer<QColor> titleBarActiveForegroundColor = nullptr;
    QSharedPointer<QColor> titleBarInactiveForegroundColor = nullptr;
    QSharedPointer<QColor> chromeButtonNormalColor = nullptr;
    QSharedPointer<QColor> chromeButtonHoverColor = nullptr;
    QSharedPointer<QColor> chromeButtonPressColor = nullptr;
    QSharedPointer<QColor> closeButtonNormalColor = nullptr;
    QSharedPointer<QColor> closeButtonHoverColor = nullptr;
    QSharedPointer<QColor> closeButtonPressColor = nullptr;
};

FRAMELESSHELPER_END_NAMESPACE

#endif
