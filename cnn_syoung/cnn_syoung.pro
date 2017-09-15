TEMPLATE = app

TARGET = yoloV2
QT += qml quick
CONFIG += c++11

QMAKE_CFLAGS_DEBUG = -Ofast
QMAKE_LFLAGS_DEBUG = -Ofast

QMAKE_CXXFLAGS_DEBUG += -msse4
QMAKE_CFLAGS_DEBUG += -msse4

QMAKE_CFLAGS_DEBUG += -DOPENCV

INCLUDEPATH += /usr/local/include \
               /usr/local/include/opencv \
               /usr/local/include/opencv2 \
               /opt/intel/compilers_and_libraries_2017.4.196/linux/mkl/include

LIBS += -lpthread \
        -L/usr/local/lib -lopencv_core \
        -lopencv_highgui -lopencv_imgproc \
        -lopencv_video\

LIBS += -L/usr/lib -lblas

SOURCES += ../src/main.c \
    ../src/list.c \
    ../src/option_list.c \
    ../src/utils.c \
    ../src/data.c \
    ../src/yolo.c \
    ../src/parser.c \
    ../src/network.c \
    ../src/activations.c \
    ../src/convolutional_layer.c \
    ../src/region_layer.c \
    ../src/maxpool_layer.c \
    ../src/im2col.c \
    ../src/gemm.c \
    ../src/reorg_layer.c \
    ../src/route_layer.c \
    ../src/box.c \
    ../src/image.c \
    ../src/layer.c

HEADERS += \
    ../src/list.h \
    ../src/option_list.h \
    ../src/utils.h \
    ../src/data.h \
    ../src/yolo.h \
    ../src/parser.h \
    ../src/network.h \
    ../src/layer.h \
    ../src/activations.h \
    ../src/convolutional_layer.h \
    ../src/region_layer.h \
    ../src/maxpool_layer.h \
    ../src/im2col.h \
    ../src/gemm.h \
    ../src/reorg_layer.h \
    ../src/route_layer.h \
    ../src/image.h \
    ../src/box.h
