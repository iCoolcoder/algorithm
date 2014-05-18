@ECHO OFF
REM ========================================
REM Name        : MakeDoxygenDoku.bat
REM Description : Windows Batch-File
REM Purpose     : Start automatic Source-Code Dokumentation Generation via "Doxygen"
REM Version     : 1.0
REM Note(s)     : The "Doxygen.exe" has to be in the System Search-Path (PATH)
REM ========================================
ECHO Generating Source-Code Dokumentation....

Doxygen

ECHO Generating Source-Code Dokumentation done !
ECHO.
CLS



