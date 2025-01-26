/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

 /*
 * This is an example on how to make a simple VNC client with
 * Qt5 Widgets. It suitable for desktop apps, but may not be
 * good for mobile.
 * It does not implement any form of cryptography,
 * authentication support, client-side cursors or framebuffer
 * resizing. If you want to make this a part of your
 * application, please notice that you may need to change
 * the while(true) loop to disconnect the client.
 *
 * To build this example with all the other components of
 * libvncserver, you may need to explicitly define a C++
 * compiler and the path to Qt libs when calling CMake.
 * e.g. cmake -DCMAKE_PREFIX_PATH=~/Qt/5.15.2/gcc_64
 * -DCMAKE_CXX_COMPILER=g++
 */

#include <QApplication>
#include <iostream>
#include <thread>
#include <QPaintEvent>
#include <QPainter>
#include "rfb/rfbclient.h"
#include <QWidget>
#include <QDebug>

//#ifdef Q_OS_WIN
//#include <windows.h>
//#elif defined(Q_OS_LINUX)
//#include <X11/XKBlib.h>
//#endif

class VncViewer : public QWidget
{
    //if you want to make use of signals/slots, uncomment the line bellow:
    //Q_OBJECT
public:
    VncViewer(QWidget *parent = nullptr) {};
    virtual ~VncViewer() {};
    void start();
    std::string serverIp;
    int serverPort;
    std::string pwd;
    std::thread *vncThread() const;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    bool m_startFlag = false;
    QImage m_image;
    rfbClient *cl;
    std::thread *m_vncThread;
    static void finishedFramebufferUpdateStatic(rfbClient *cl);
    void finishedFramebufferUpdate(rfbClient *cl);
};

void VncViewer::finishedFramebufferUpdateStatic(rfbClient *cl)
{
    VncViewer *ptr = static_cast<VncViewer*>(rfbClientGetClientData(cl, nullptr));
    ptr->finishedFramebufferUpdate(cl);
}

void VncViewer::finishedFramebufferUpdate(rfbClient *cl)
{
    m_image = QImage(cl->frameBuffer, cl->width, cl->height, QImage::Format_RGB16);

    update();
}

void VncViewer::paintEvent(QPaintEvent *event)
{
    event->accept();

    QPainter painter(this);
    painter.drawImage(this->rect(), m_image);
}

void VncViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         (event->buttons() & Qt::LeftButton) ? 1 : 0);
    }
}

void VncViewer::mousePressEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         1);
    }
}

void VncViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         0);
    }
}

//bool isCapsLockOn() {
//#ifdef Q_OS_WIN
//    return GetKeyState(VK_CAPITAL) & 0x0001;
//#elif defined(Q_OS_LINUX)
//    Display *d = XOpenDisplay((char *)0);
//    if (!d) return false;
//    unsigned n;
//    XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
//    XCloseDisplay(d);
//    return (n & 0x01) == 1;
//#else
//    return false;
//#endif
//}

static rfbKeySym qtKey2rfbKeySym(QKeyEvent *event)
{
    uint keyval = event->key();
    rfbKeySym k = 0;

//    if (keyval >= Qt::Key_A && keyval <= Qt::Key_Z) {
//        // 检查修饰键状态
//        bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
//        bool capsLockOn = isCapsLockOn();
//        if (shiftPressed ^ capsLockOn){
//            k = XK_A + (keyval - Qt::Key_A);
//        }
//        else{
//            k = XK_a + (keyval - Qt::Key_A);
//        }
//        return k;
//    }

        if (keyval >= Qt::Key_A && keyval <= Qt::Key_Z) {
            // 检查修饰键状态, strade the capslock as not active.
            bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
            if (shiftPressed){
                k = XK_A + (keyval - Qt::Key_A);
            }
            else{
                k = XK_a + (keyval - Qt::Key_A);
            }
            return k;
        }
    switch(keyval) {
    case Qt::Key_Backspace: k = XK_BackSpace; break;
    case Qt::Key_Tab: k = XK_Tab; break;
    case Qt::Key_Clear: k = XK_Clear; break;
    case Qt::Key_Return: k = XK_Return; break;
    case Qt::Key_Pause: k = XK_Pause; break;
    case Qt::Key_Escape: k = XK_Escape; break;
    case Qt::Key_Space: k = XK_space; break;
    case Qt::Key_Delete: k = XK_Delete; break;
    // case Qt::Key_KP_0: k = XK_KP_0; break;
    // case Qt::Key_KP_1: k = XK_KP_1; break;
    // case Qt::Key_KP_2: k = XK_KP_2; break;
    // case Qt::Key_KP_3: k = XK_KP_3; break;
    // case Qt::Key_KP_4: k = XK_KP_4; break;
    // case Qt::Key_KP_5: k = XK_KP_5; break;
    // case Qt::Key_KP_6: k = XK_KP_6; break;
    // case Qt::Key_KP_7: k = XK_KP_7; break;
    // case Qt::Key_KP_8: k = XK_KP_8; break;
    // case Qt::Key_KP_9: k = XK_KP_9; break;
    case Qt::Key_0: k = XK_KP_0; break;
    case Qt::Key_1: k = XK_KP_1; break;
    case Qt::Key_2: k = XK_KP_2; break;
    case Qt::Key_3: k = XK_KP_3; break;
    case Qt::Key_4: k = XK_KP_4; break;
    case Qt::Key_5: k = XK_KP_5; break;
    case Qt::Key_6: k = XK_KP_6; break;
    case Qt::Key_7: k = XK_KP_7; break;
    case Qt::Key_8: k = XK_KP_8; break;
    case Qt::Key_9: k = XK_KP_9; break;
    // case Qt::Key_KP_Decimal: k = XK_KP_Decimal; break;
    // case Qt::Key_KP_Divide: k = XK_KP_Divide; break;
    // case Qt::Key_KP_Multiply: k = XK_KP_Multiply; break;
    // case Qt::Key_KP_Subtract: k = XK_KP_Subtract; break;
    // case Qt::Key_KP_Add: k = XK_KP_Add; break;

    // case Qt::Key_KP_Enter: k = XK_KP_Enter; break;
    // case Qt::Key_KP_Equal: k = XK_KP_Equal; break;
    case Qt::Key_Enter: k = XK_KP_Enter; break;
    case Qt::Key_Equal: k = XK_KP_Equal; break;

    case Qt::Key_Up: k = XK_Up; break;
    case Qt::Key_Down: k = XK_Down; break;
    case Qt::Key_Right: k = XK_Right; break;
    case Qt::Key_Left: k = XK_Left; break;
    case Qt::Key_Insert: k = XK_Insert; break;
    case Qt::Key_Home: k = XK_Home; break;
    case Qt::Key_End: k = XK_End; break;
    case Qt::Key_PageUp: k = XK_Page_Up; break;
    case Qt::Key_PageDown: k = XK_Page_Down; break;
    case Qt::Key_F1: k = XK_F1; break;
    case Qt::Key_F2: k = XK_F2; break;
    case Qt::Key_F3: k = XK_F3; break;
    case Qt::Key_F4: k = XK_F4; break;
    case Qt::Key_F5: k = XK_F5; break;
    case Qt::Key_F6: k = XK_F6; break;
    case Qt::Key_F7: k = XK_F7; break;
    case Qt::Key_F8: k = XK_F8; break;
    case Qt::Key_F9: k = XK_F9; break;
    case Qt::Key_F10: k = XK_F10; break;
    case Qt::Key_F11: k = XK_F11; break;
    case Qt::Key_F12: k = XK_F12; break;
    case Qt::Key_F13: k = XK_F13; break;
    case Qt::Key_F14: k = XK_F14; break;
    case Qt::Key_F15: k = XK_F15; break;
    case Qt::Key_NumLock: k = XK_Num_Lock; break;
    case Qt::Key_CapsLock: k = XK_Caps_Lock; break;
    case Qt::Key_ScrollLock: k = XK_Scroll_Lock; break;
    // case Qt::Key_Shift_R: k = XK_Shift_R; break;
    // case Qt::Key_Shift_L: k = XK_Shift_L; break;
    case Qt::Key_Shift: k = XK_Shift_L; break;
    // case Qt::Key_Control_R: k = XK_Control_R; break;
    // case Qt::Key_Control_L: k = XK_Control_L; break;
    case Qt::Key_Control: k = XK_Control_L; break;
    // case Qt::Key_Alt_R: k = XK_Alt_R; break;
    // case Qt::Key_Alt_L: k = XK_Alt_L; break;
    case Qt::Key_Alt: k = XK_Alt_L; break;
    // case Qt::Key_Meta_R: k = XK_Meta_R; break;
    // case Qt::Key_Meta_L: k = XK_Meta_L; break;
    case Qt::Key_Meta: k = XK_Meta_L; break;
#if 0
    /* TODO: find out keysyms */
    case Qt::Key_Super_L: k = XK_LSuper; break;      /* left "windows" key */
    case Qt::Key_Super_R: k = XK_RSuper; break;      /* right "windows" key */
    case Qt::Key_Multi_key: k = XK_Compose; break;
#endif
    case Qt::Key_Mode_switch: k = XK_Mode_switch; break;
    case Qt::Key_Help: k = XK_Help; break;
    case Qt::Key_Print: k = XK_Print; break;
    case Qt::Key_SysReq: k = XK_Sys_Req; break;
    // case Qt::Key_Break: k = XK_Break; break;
    default: break;
    }
    if (k == 0) {
        if (keyval < 0x100)
            k = keyval;
        else
            rfbClientLog ("Unknown keysym: %d\n", keyval);
    }

    return k;
}


void VncViewer::keyPressEvent(QKeyEvent *event) {
    qDebug() << "Key Pressed:" << event->key();
    if (m_startFlag) {
        SendKeyEvent(cl, qtKey2rfbKeySym(event), true);
    }
}

void VncViewer::keyReleaseEvent(QKeyEvent *event) {
    qDebug() << "Key Released:" << event->key();

    if (m_startFlag) {
        SendKeyEvent(cl, qtKey2rfbKeySym(event), false);
    }
}

void VncViewer::closeEvent(QCloseEvent *event)
{
    m_startFlag = false;
    if (m_vncThread->joinable()) {
        m_vncThread->join();
    }
    QWidget::closeEvent(event);
}

char* getPassword(rfbClient* client) {
    // Use a mutable string
    char* password = strdup("hexagon123."); // Replace with your actual password
    return password; // Return the mutable string
}

void VncViewer::start()
{
    cl = rfbGetClient(8, 3, 4);
    cl->format.depth = 24;
    cl->format.depth = 16;
    cl->format.bitsPerPixel = 16;
    cl->format.redShift = 11;
    cl->format.greenShift = 5;
    cl->format.blueShift = 0;
    cl->format.redMax = 0x1f;
    cl->format.greenMax = 0x3f;
    cl->format.blueMax = 0x1f;
    cl->appData.compressLevel = 9;
    cl->appData.qualityLevel = 1;
    cl->appData.encodingsString = "tight ultra";
    cl->FinishedFrameBufferUpdate = finishedFramebufferUpdateStatic;
    cl->serverHost = strdup(serverIp.c_str());
    cl->serverPort = serverPort; 
    cl->appData.useRemoteCursor = TRUE;
    cl->GetPassword = getPassword;
    rfbClientSetClientData(cl, nullptr, this);

    if (rfbInitClient(cl, 0, nullptr)) {
    } else {
        std::cout << "[INFO] disconnected" << std::endl;
        return;
    }
    m_startFlag = true;

    std::cout << "[INFO] screen size: " << cl->width << " x " << cl->height << std::endl;
    this->resize(cl->width, cl->height);

    m_vncThread = new std::thread([this]() {
        while (m_startFlag) {
            int i = WaitForMessage(cl, 500);
            if (i < 0) {
                std::cout << "[INFO] disconnected" << std::endl;
                rfbClientCleanup(cl);
                break;
            }

            if (i && !HandleRFBServerMessage(cl)) {
                std::cout << "[INFO] disconnected" << std::endl;
                rfbClientCleanup(cl);
                break;
            }
        };
    });
}

int main(int argc, char *argv[])
{
    // if(argc < 3) {
    //     std::cout << "Usage: "
    //               << argv[0]
    //               << " <server ip>"
    //               << " <port>"
    //               << "\n";
    //     return 1;
    // }

    QApplication a(argc, argv);
    VncViewer vncViewer;
    // vncViewer.serverIp = std::string{argv[1]};
    // vncViewer.serverPort = std::atoi(argv[2]);
    vncViewer.serverIp = "10.136.7.178";
    vncViewer.serverPort = 5900;
    vncViewer.show();
    vncViewer.start();
    return a.exec();
}
