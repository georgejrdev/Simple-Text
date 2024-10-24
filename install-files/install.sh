#!/bin/bash

pathFile="$HOME/Simple-Text"

if [ ! -d "$pathFile" ]; then
    mkdir -p "$pathFile"
    echo "Created $pathFile"
fi

if [ -f ./SimpleText ]; then
    mv ./SimpleText "$pathFile"
    echo "Moved SimpleText to $pathFile"
fi

if [ -f ./ico.png ]; then
    mv ./ico.png "$pathFile"
    echo "Moved ico.png to $pathFile"
fi

chmod +x "$pathFile/SimpleText"

echo "Exec=$pathFile/SimpleText" >> ./Simple-Text.desktop
echo "Icon=$pathFile/ico.png" >> ./Simple-Text.desktop

if [ -f ./Simple-Text.desktop ]; then
    mv ./Simple-Text.desktop "$HOME/.local/share/applications"
    echo "Moved Simple-Text.desktop to $HOME/.local/share/applications"
fi

chmod +x "$HOME/.local/share/applications/Simple-Text.desktop"

echo "Installed SimpleText"