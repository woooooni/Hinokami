#include "FileUtils.h"
#include "Utils.h"

CFileUtils::CFileUtils()
{
}

CFileUtils::~CFileUtils()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}
}

HRESULT CFileUtils::Open(wstring filePath, FileMode mode)
{
	if (mode == FileMode::Write)
	{
		_handle = ::CreateFile(
			filePath.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}
	else
	{
		_handle = ::CreateFile
		(
			filePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}

	if (_handle == INVALID_HANDLE_VALUE)
	{
		wstring msg = wstring(L"File Open Failed : ") + filePath;
		MessageBox(nullptr, msg.c_str(), L"System Message", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void CFileUtils::Write(void* data, _uint dataSize)
{
	_uint numOfBytes = 0;
	assert(::WriteFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void CFileUtils::Write(const string& data)
{
	_uint size = (_uint)data.size();
	Write(size);

	if (data.size() == 0)
		return;

	Write((void*)data.data(), size);
}



void CFileUtils::Read(void** data, _uint dataSize)
{
	_uint numOfBytes = 0;
	assert(::ReadFile(_handle, *data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void CFileUtils::Read(OUT string& data)
{
	_uint size = Read<_uint>();

	if (size == 0)
		return;

	char* temp = new char[size + 1];
	temp[size] = 0;
	Read((void**)&temp, size);
	data = temp;
	delete[] temp;
}