pushd %-dp0
pyinstaller --onefile PacketGenerator.py
MOVE .\dist\PacketGenerator.exe .\GenPackets.exe
@RD /S /Q .\build
@RD /S /Q .\dist
DEL /S /F /Q .\PacketGenerator.spec


XCOPY /Y .\GenPackets.exe ..\..\Protocol\
XCOPY /Y .\Templates ..\..\Protocol\

..\..\Protocol\GenPackets.bat

PAUSE
