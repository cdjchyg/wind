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
			SNodeStruct()
				:mKey()
				:mValue()
			{
				for (int i = 0; i < MAXLEVEL; ++i)
				{
					mForward[i] = NULL;
				}
			}

			KeyType		mKey;
			ValueType	mValue;
			PNodeStruct	mForward[MAXLEVEL];
		};

		struct SListStruct
		{
			SListStruct()
				:mLevel(0)
			{
				mHeader = new SNodeStruct();
			}
			~SListStruct()
			{
				if (mHeader)
				{
					delete mHeader;
					mHeader = NULL;
				}
			}

			int			mLevel;
			PNodeStruct	mHeader;
		};
		typedef SListStruct* PListStruct;

		CSkipList()
		{
			Init();
		}
		~CSkipList
		{
			if (mList)
			{
				delete mList;
				mList = NULL;
			}
		}

		void Init()
		{
			mList = new SListStruct;
		}

		void Add()
		{

		}

		void Remove(KeyType& key)
		{

		}


		ValueType* Find(KeyType& key)
		{

		}

		PListStruct mList;
	};
}