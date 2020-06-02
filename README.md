# FramelessHelper (Win32 version)

If you are using part of or all of the code from this repository in your own projects, it's my pleasure and I'll be happy if it can help you. But I hope you can tell me the URL of the homepage or repository of your projects, whether your projects are close-sourced or commercial products do not matter. I'll link to your homepage or repository in this README file. It would be much better if you can provide me some screenshots of your software for demonstration at the same time.

如果您正在使用此项目的部分或全部代码，这是我的荣幸，我很高兴能帮到您，但我同时也希望，您能将您项目的首页或仓库的网址告诉我（闭源或收费都没关系），我将在这个自述文件中链接到您所提供的网址，以供展示。如果您能一并提供一些软件运行时的截图，那就更好了。

## Screenshots

![zhihuiyanglao](/screenshots/zhihuiyanglao.png)

![QQ Player](/screenshots/qqplayer.png)

![Qt Widgets example](/screenshots/widgets.png)

![Qt Quick example](/screenshots/quick.png)

## Features

- Frameless but have frame shadow.
- Draggable and resizeable.
- Automatically high DPI scaling.
- Multi-monitor support (different resolution and DPI).
- Have animations when minimizing and maximizing.
- Tiled/Stack windows by DWM.
- Won't cover the task bar when maximized.
- Won't block the auto-hide task bar when maximized.
- No flickers when resizing.
- Load all APIs at run-time, no need to link to any system libraries directly.

## Usage

```cpp
// include other files ...
#include "winnativeeventfilter.h"
// include other files ...

// Anywhere you like, we use the main function here.
int main(int argc, char *argv[]) {
    // ...
    QWidget widget;
    // Do this before the widget is shown.
    WinNativeEventFilter::addFramelessWindow(reinterpret_cast<HWND>(widget.winId()));
    widget.show();
    // ...
}
```

Please refer to [**main_windows.cpp**](/main_windows.cpp) for more detailed information.

### Ignore areas and etc

```cpp
WinNativeEventFilter::WINDOWDATA data;
// The window can't be resized if fixedSize is set to TRUE.
data.fixedSize = FALSE;
// All the following values should not be DPI-aware, just use the
// original numbers, assuming the scale factor is 1.0, don't scale
// them yourself, this code will do the scaling according to DPI
// internally and automatically.
// Maximum window size
data.maximumSize = QSize(1280, 720);
// Minimum window size
data.minimumSize = QSize(800, 540);
// How to set ignore areas:
// The geometry of something you already know, in window coordinates
data.ignoreAreas.append({100, 0, 30, 30});
// The geometry of a widget, in window coordinates.
// It won't update automatically when the geometry of that widget has
// changed, so if you want to add a widget, which is in a layout and
// it's geometry will possibly change, to the ignore list, try the
// next method (ignoreObjects) instead.
data.ignoreAreas.append(pushButton_close.geometry());
// The **POINTER** of a QWidget or QQuickItem
data.ignoreObjects.append(ui->pushButton_minimize);
// Pass data as the second parameter
WinNativeEventFilter::addFramelessWindow(reinterpret_cast<HWND>(widget.winId()), &data);
// Or
WinNativeEventFilter::setWindowData(reinterpret_cast<HWND>(widget.winId()), &data);
// Or modify the window data of a specific window directly:
const auto data = WinNativeEventFilter::windowData(reinterpret_cast<HWND>(widget.winId()));
data.borderWidth = 5;
data.borderHeight = 5;
data.titleBarHeight = 30;
```

## Supported Platforms

Windows 7 ~ 10, 32 bit & 64 bit

The code itself should be able to work on Windows Vista in theory, but Qt has drop Vista support long time ago.

## Requirements

| Component | Requirement | Additional Information |
| --- | --- | --- |
| Qt | >= 5.6 | No modules are required explicitly, but to make full use of this repository, you'd better install the `gui`, `widgets` and `quick` modules |
| Compiler | >= C++11 | MSVC, MinGW, Clang-CL, Intel-CL or cross compile from Linux are all supported |

## Known Bugs

Please refer to [BUGS.md](/BUGS.md) for more information.

## Notes for developers

- As you may have found, if you use this code, the resize areas will be inside the frameless window, however, a normal Win32 window can be resized outside of it. Here is the reason: the `WS_THICKFRAME` window style will cause a window has three transparent areas beside the window's left, right and bottom edge. Their width/height is 8px if the window is not scaled. In most cases, they are totally invisible. It's DWM's responsibility to draw and control them. They exist to let the user resize the window, visually outside of it. They are in the window area, but not the client area, so they are in the non-client area actually. But we have turned the whole window area into client area in `WM_NCCALCSIZE`, so the three transparent resize areas also become a part of the client area and thus they become visible. When we resize the window, it looks like we are resizing inside of it, however, that's because the transparent resize areas are visible now, we ARE resizing outside of the window actually. But I don't know how to make them become transparent again without breaking the frame shadow drawn by DWM. If you really want to solve it, you can try to embed your window into a larger transparent window and draw the frame shadow yourself. [See the discussions here](https://github.com/wangwenx190/framelesshelper/issues/3) for more detailed information.
- Don't change the window flags (for example, enable the Qt::FramelessWindowHint flag) because it will break the functionality of this code. I'll get rid of the window frame (including the titlebar of course) in Win32 native events.
- All traditional Win32 APIs are replaced by their DPI-aware ones, if there is one.
- Starting from Windows 10, normal windows usually have a one pixel width border line. After many times of trying, I still can't preserve it if I want to remove the window frame. I don't know how to solve this currently. If you really need it, you have to draw one manually by yourself. [See the discussions here](https://github.com/wangwenx190/framelesshelper/issues/3) for more detailed information.
- The frame shadow will get lost if the window is totally transparent. It can't be solved unless you draw the frame shadow manually.
- On Windows 7, if you disabled the Windows Aero, the frame shadow will be disabled as well because it's DWM's resposibility to draw the frame shadow.
- The border width (8 if not scaled), border height (8 if not scaled) and titlebar height (30 if not scaled) are acquired by Win32 APIs and are the same with other standard windows, and thus you should not modify them. Only modify them when you really have a good reason to do so.
- You can also copy all the code to `[virtual protected] bool QWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)` or `[virtual protected] bool QWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)`, it's the same with install a native event filter to the application.

## References for developers

### Microsoft Docs

- <https://docs.microsoft.com/en-us/archive/blogs/wpfsdk/custom-window-chrome-in-wpf>
- <https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-nccalcsize>
- <https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-nchittest>
- <https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-ncactivate>
- <https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-erasebkgnd>
- <https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-getminmaxinfo>
- <https://docs.microsoft.com/en-us/windows/win32/dwm/customframe>
- <https://docs.microsoft.com/en-us/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows>

### Chromium

- <https://github.com/chromium/chromium/blob/master/ui/base/win/hwnd_metrics.cc>
- <https://github.com/chromium/chromium/blob/master/ui/display/win/screen_win.cc>
- <https://github.com/chromium/chromium/blob/master/ui/views/win/hwnd_message_handler.cc>
- <https://github.com/chromium/chromium/blob/master/ui/views/widget/desktop_aura/desktop_window_tree_host_win.cc>
- <https://github.com/chromium/chromium/blob/master/ui/views/widget/desktop_aura/desktop_native_widget_aura.cc>
- <https://github.com/chromium/chromium/blob/master/ui/views/widget/native_widget_aura.cc>
- <https://github.com/chromium/chromium/blob/master/ui/views/widget/widget.cc>

### Mozilla Firefox

- <https://github.com/mozilla/gecko-dev/blob/master/widget/windows/nsWindow.cpp>

### Windows Terminal

- <https://github.com/microsoft/terminal/blob/master/src/cascadia/WindowsTerminal/IslandWindow.cpp>
- <https://github.com/microsoft/terminal/blob/master/src/cascadia/WindowsTerminal/NonClientIslandWindow.cpp>

### GitHub

- <https://github.com/rossy/borderless-window>
- <https://github.com/Bringer-of-Light/Qt-Nice-Frameless-Window>
- <https://github.com/dfct/TrueFramelessWindow>
- <https://github.com/qtdevs/FramelessHelper>

### Qt

- <https://doc.qt.io/qt-5/qcoreapplication.html#installNativeEventFilter>
- <https://doc.qt.io/qt-5/qcoreapplication.html#removeNativeEventFilter>
- <https://doc.qt.io/qt-5/qobject.html#installEventFilter>
- <https://doc.qt.io/qt-5/qobject.html#removeEventFilter>
- <https://doc.qt.io/qt-5/qwindow.html#startSystemMove>
- <https://doc.qt.io/qt-5/qwindow.html#startSystemResize>

### 窗口样式的作用

`WS_OVERLAPPEDWINDOW` = `WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |  WS_MINIMIZEBOX | WS_MAXIMIZEBOX`

`WS_OVERLAPPEDWINDOW`代表了一个正常的Win32窗口，也就是说，它就是正常窗口默认的窗口样式（`Window style`）。

`WS_OVERLAPPED`：窗口有标题栏和边界（功能性，并不是真的要有可视化的东西）；`WS_CAPTION`：窗口有标题栏功能（并不是真的要有个可视化的标题栏，而是拥有和DWM交互的能力）；`WS_SYSMENU`：窗口可以弹出系统菜单（ALT+空格）；`WS_THICKFRAME`：窗口有三个不可视的拖拽区域（前提是非客户区存在），并且拥有DWM绘制的边框阴影（没有这个样式，DWM一定不会绘制边框阴影，只要有，就一定是自绘的）；`WS_MINIMIZEBOX`：窗口支持最小化（与`WS_THICKFRAME`一起使用时最小化有动画）；`WS_MAXIMIZEBOX`：窗口支持最大化（与`WS_THICKFRAME`一起使用时最大化有动画）。

这些窗口样式，只有`WS_SYSMENU`是可以没有的（虽然这样会导致窗口无法弹出系统菜单，但如果非要做，也是能用其他API模拟的，比如火狐和Chrome浏览器的做法），如果缺少其他窗口样式，会导致窗口的正常功能产生残缺，比如最大化最小化时的动画丢失，缺少窗口阴影等。

参考资料：<https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles>

## Special Thanks

Thanks **Lucas** for testing this code in many various conditions.

Thanks [**Shujaat Ali Khan**](https://github.com/shujaatak) for searching so many useful articles and repositories for me.

## License

```text
MIT License

Copyright (C) 2020 by wangwenx190 (Yuhang Zhao)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
