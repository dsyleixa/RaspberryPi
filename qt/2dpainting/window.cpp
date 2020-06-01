/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include "window.h"

#include <QGridLayout>
#include <QPushButton>
#include <QTimer>


Window::Window()
{
    setWindowTitle(tr("2D Painting on OpenGL Widgets"));

    // Here the OpenGL widget is created (see also the .h file)
    GLWidget *openGL = new GLWidget(&helper, this);

    // Here the QPushButton widget is created (see also the .h file)
    QPushButton *pushButton = new QPushButton(tr("Quit"));

    // Here the event button "clicked" is associated with the "event handler"
    connect(pushButton, SIGNAL(clicked()), SLOT(onQuitPushed()));

    // Here the "layout" of the window is defined . It is a grid just because
    // we started with the Qt example
    QGridLayout *layout = new QGridLayout;

    // Here the OpenGL widget is added to the window layout
    layout->addWidget(openGL, 0, 0);

    // And here the Button is added to the layout
    layout->addWidget(pushButton, 1, 0);

    // Here the layout is assigned to the window
    setLayout(layout);

    // Here we define a timer for the periodic update of the OpenGL widget
    QTimer *timer = new QTimer(this);

    // Here we link the periodic "time elapesed" event of the timer
    // with the "event handler" (that you will find in the "glwidget.*")
    connect(timer, SIGNAL(timeout()), openGL, SLOT(animate()));

    // Finally we start the timer
    timer->start(50);
}


void
Window::onQuitPushed() {
    exit(EXIT_SUCCESS);
}

