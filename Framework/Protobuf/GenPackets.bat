pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto
IF ERRORLEVEL 1 PAUSE



XCOPY /Y Enum.pb.h "../Client/Public"
XCOPY /Y Enum.pb.cc "../Client/Private"

XCOPY /Y Struct.pb.h "../Client/Public"
XCOPY /Y Struct.pb.cc "../Client/Private"

XCOPY /Y Protocol.pb.h "../Client/Public"
XCOPY /Y Protocol.pb.cc "../Client/Private"

XCOPY /Y Enum.pb.h "../Server/Public"
XCOPY /Y Enum.pb.cc "../Server/Private"

XCOPY /Y Struct.pb.h "../Server/Public"
XCOPY /Y Struct.pb.cc "../Server/Private"

XCOPY /Y Protocol.pb.h "../Server/Public"
XCOPY /Y Protocol.pb.cc "../Server/Private"
