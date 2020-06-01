QT          += widgets

HEADERS     = glwidget.h \
              helper.h \
              window.h
SOURCES     = glwidget.cpp \
              helper.cpp \
              main.cpp \
              window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/2dpainting
INSTALLS += target
