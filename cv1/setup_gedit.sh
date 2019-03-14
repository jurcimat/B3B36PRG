#!/bin/bash
# This script parametrize the working environment of the gedit editor

# setup environment
gsettings set org.gnome.gedit.preferences.editor display-line-numbers true
gsettings set org.gnome.gedit.preferences.editor highlight-current-line true 
gsettings set org.gnome.gedit.preferences.editor bracket-matching true
gsettings set org.gnome.gedit.preferences.editor auto-indent true
gsettings set org.gnome.gedit.preferences.editor tabs-size 4
gsettings set org.gnome.gedit.preferences.editor scheme 'Classic'

# allow external tools 
gsettings set org.gnome.gedit.plugins active-plugins "['spell', 'zeitgeistplugin', 'filebrowser', 'modelines', 'docinfo', 'time','externaltools']"

# add new external tool to format code
# first check if clang format is installed
if [ $(dpkg-query -W -f='${Status}' clang-format* 2>/dev/null | grep -c "ok installed") -eq 0 ]
then
# if not -> exit
	echo >&2 "clang-format not installed! Exiting"
	exit 1
fi

# otherwise get the version and add an external tool to gedit
CLANG_VERSION=$(ls /usr/bin/clang-format* | grep -o 'clang-format-[0-9]..')

echo >&2 "Using $CLANG_VERSION"
mkdir -p $HOME/.config/gedit/tools
cd $HOME/.config/gedit/tools
touch format
echo "#!/bin/sh
# [Gedit Tool]
# Name=Format
# Save-files=document
# Applicability=all
# Output=replace-document
# Input=document
# Shortcut=<Primary>b

$CLANG_VERSION -style=\"{BasedOnStyle: llvm, IndentWidth: 4, BreakBeforeBraces: Linux}\"" > format
chmod +x format
