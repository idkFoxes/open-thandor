@echo off
rem Starts a skirmish on "mittelpunkt" with the hidden map editor open (experimental branch).
rem Copy next to thandor.exe or run from the game directory. Other maps: change -KARTE.
set OPEN_THANDOR_SCRIPT=%~dp0editor_start.txt
start "" thandor_editor.exe -NOINTRO -EDITOR -KARTE="mittelpunkt"
