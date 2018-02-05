#pragma once

//https://www.cnblogs.com/xuqiang/archive/2011/05/22/2053516.html

#include <functional>

namespace wind
{
	template<class KeyType, 
		class ValueType,
		class Pr = std::less<_KT>,
		int MAXLEVEL = 16
	>
	class CSkipList
	{
		typedef SNodeStruct* PNodeStruct;
		struct SNodeStruct
		{
			KeyType		mKey;
			ValueType	mValue;
			PNodeStruct	mForward[1];
		};

		struct SListStruct
		{
			int			mLevel;
			PNodeStruct	mHeader;
		};
		typedef SListStruct* PListStruct;

		CSkipList()
		{
			Init();
		}

		void Init()
		{
			mList = new SListStruct;
			mList->mLevel = 0;
		}


		PListStruct mList;
	};
}