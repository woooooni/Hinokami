#include "../Public/Utils.h"
#include <random>

bool CUtils::StartsWith(string str, string comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && index == 0)
		return true;

	return false;
}

bool CUtils::StartsWith(wstring str, wstring comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && index == 0)
		return true;

	return false;
}

void CUtils::Replace(OUT string& str, string comp, string rep)
{
	string temp = str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	str = temp;
}

void CUtils::Replace(OUT wstring& str, wstring comp, wstring rep)
{
	wstring temp = str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	str = temp;
}

wstring CUtils::PathToWString(wstring strPath)
{
	Replace(strPath, L"\\", L"/");
	
	return strPath;
}

_float3 CUtils::ToEulerAngles(_vector Quaternion)
{
	
	_float3 vAngles;
	ZeroMemory(&vAngles, sizeof(_float3));

	_float4 vQuaternion;

	XMStoreFloat4(&vQuaternion, Quaternion);


	// roll (x-axis rotation)
	_float sinr_cosp = 2 * (vQuaternion.w * vQuaternion.x + vQuaternion.y * vQuaternion.z);
	_float cosr_cosp = 1 - 2 * (vQuaternion.x * vQuaternion.x + vQuaternion.y * vQuaternion.y);
	vAngles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	_float sinp = std::sqrt(1 + 2 * (vQuaternion.w * vQuaternion.y - vQuaternion.x * vQuaternion.z));
	_float cosp = std::sqrt(1 - 2 * (vQuaternion.w * vQuaternion.y - vQuaternion.x * vQuaternion.z));
	vAngles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	_float siny_cosp = 2 * (vQuaternion.w * vQuaternion.z + vQuaternion.x * vQuaternion.y);
	_float cosy_cosp = 1 - 2 * (vQuaternion.y * vQuaternion.y + vQuaternion.z * vQuaternion.z);
	vAngles.z = std::atan2(siny_cosp, cosy_cosp);

	return vAngles;
	
}

_int CUtils::Random_Int(_int fMin, _int fMax)
{
	_int iRealMin = min(fMin, fMax);
	_int iRealMax = max(fMin, fMax);


	std::random_device Random_Device;

	std::mt19937 Generater(Random_Device());

	std::uniform_int_distribution<_int> Distribution(iRealMin, iRealMax);
	return Distribution(Generater);
}

_float CUtils::Random_Float(_float fMin, _float fMax)
{	
	_float fRealMin = min(fMin, fMax);
	_float fRealMax = max(fMin, fMax);

	std::random_device Random_Device;

	std::mt19937 Generater(Random_Device());

	std::uniform_real_distribution<_float> Distribution(fRealMin, fRealMax);
	return Distribution(Generater);
}

std::wstring CUtils::ToWString(string value)
{
	return wstring(value.begin(), value.end());
}

std::string CUtils::ToString(wstring value)
{
	return string(value.begin(), value.end());
}