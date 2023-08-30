#pragma once

namespace Engine
{
	class CTag_Finder
	{
	private:
		std::wstring	m_pStr;

	public:
		CTag_Finder(const std::wstring& pStr)
			: m_pStr(pStr)
		{

		}

	public:
		template<typename T>
		bool	operator()(T& Pair)
		{
			if (m_pStr == Pair.first)
				return true;

			return false;
		}
	};
}
