

//// xcopy /�ɼ� .������������ �ִ� ���			.����������

xcopy /y/s .\Engine\Public\*.*				.\Reference\Headers\
xcopy /y .\Engine\Bin\*.lib					.\Reference\Librarys\
xcopy /y .\ThirdParty\*.lib				.\Reference\Librarys\

xcopy /y .\Engine\Bin\*.dll					.\Client\Bin\
xcopy /y .\Engine\Bin\ShaderFiles\*.hlsl	.\Client\Bin\ShaderFiles\