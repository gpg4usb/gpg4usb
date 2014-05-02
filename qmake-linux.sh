#!/bin/sh

qtwind="$(pwd)/winbuild/qt4.8"
qtx11d="$(pwd)/linbuild/qt4.8"

$qtx11d/bin/qmake -spec $qtx11d/mkspecs/linux-g++-32 "TARGET=gpg4usb" \
	"LIBS += -static-libgcc" \
	"DEFINES += STATICLINUX" "QMAKE_INCDIR_QT = $qtx11d/include" "QMAKE_LIBDIR_QT = $qtx11d/lib" \
	"QMAKE_QMAKE = $qtx11d/bin/qmake" "QMAKE_MOC = $qtx11d/bin/moc" "QMAKE_UIC = $qtx11d/bin/uic" \
	"QMAKE_RCC = $qtx11d/bin/rcc" "$@"

