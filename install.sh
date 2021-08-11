#!/usr/bin/sh

cp tagbrowser.desktop ~/.local/share/applications/tagbrowser.desktop
cp tagbrowser.png ~/.local/share/icons/tagbrowser.png
ln -s $PWD/build/tagbrowser ~/.local/bin/tagbrowser
