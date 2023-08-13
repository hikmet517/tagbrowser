#!/usr/bin/sh

install -D tagbrowser.desktop ~/.local/share/applications/tagbrowser.desktop
install -D tagbrowser.png ~/.local/share/icons/hicolor/512x512/apps/tagbrowser.png
ln -f -s "$(realpath ./build/tagbrowser)" ~/.local/bin/tagbrowser

gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor/
