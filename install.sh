#!/usr/bin/sh

cp tagbrowser.desktop ~/.local/share/applications/tagbrowser.desktop
cp tagbrowser.png ~/.local/share/icons/tagbrowser.png
ln -f -s $PWD/build/tagbrowser ~/.local/bin/tagbrowser
