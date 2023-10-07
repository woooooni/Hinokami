pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=C_ --send=S_
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=S_ --send=C_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../Client/Public"
XCOPY /Y Enum.pb.cc "../Client/Private"

XCOPY /Y Struct.pb.h "../Client/Public"
XCOPY /Y Struct.pb.cc "../Client/Private"

XCOPY /Y Protocol.pb.h "../Client/Public"
XCOPY /Y Protocol.pb.cc "../Client/Private"

XCOPY /Y Enum.pb.h "../Server/Public"
XCOPY /Y Enum.pb.cc "../Server/Public"

XCOPY /Y Struct.pb.h "../Server/Public"
XCOPY /Y Struct.pb.cc "../Server/Private"

XCOPY /Y Protocol.pb.h "../Server/Public"
XCOPY /Y Protocol.pb.cc "../Server/Private"

XCOPY /Y ClientPacketHandler.h "../Client/Public"
XCOPY /Y ServerPacketHandler.h "../Server/Public"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE