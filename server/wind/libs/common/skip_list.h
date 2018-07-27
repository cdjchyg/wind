#pragma once

//https://www.cnblogs.com/xuqiang/archive/2011/05/22/2053516.html

#include <functional>
#include <cmath>

// TODO “Ï≥£¥¶¿Ì

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
				//mHeader = new SNodeStruct();
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
	public:
		CSkipList()
			:mList(NULL)
		{
			mList = new SListStruct();
		}
		~CSkipList
		{
			if (mList)
			{
				delete mList;
				mList = NULL;
			}
		}

		void Add(KeyType& key, ValueType& value)
		{
			if (mList->mHeader == NULL)
			{
				PNodeStruct pNode = new SNodeStruct();
				pNode->mKey = key;
				pNode->mValue = value;
				mList->mHeader = pNode;
				return;
			}

			PNodeStruct p = mList->mHeader;
			PNodeStruct q = NULL;
			PNodeStruct update[MAXLEVEL];
			int k = mList->mLevel;
			do
			{

				while (q = p->mForward[k], q->mKey < key)
				{
					p = q;
				}

				update[k] = p;

			} while (--k >= 0);

			if (q->mKey == key)
			{
				q->mValue = value;
				return;
			}

			k = RandomLevel();
			if (k > mList->mLevel)
			{
				k = ++mList->mLevel;
			}

			q = new SNodeStruct;
			q->mKey = key;
			q->mValue = value;

			do
			{
				p = update[k];
				q->mForward[k] = p->mForward[k];
				p->mForward[k] = q;
			} while (--k >= 0);
		}

		void Remove(KeyType& key)
		{
			PNodeStruct update[MAXLEVEL];
			PNodeStruct p = mList->mHeader;
			PNodeStruct q = NULL;
			int k = mList->mLevel;
			int m = k;

			do
			{
				while (q = p->mForward[k], q->mKey < mKey)
				{
					p = q;
				}
				update[k] = p;
			} while (--k >= 0);

			if (q->mKey != key)
			{
				return;
			}

			for (k = 0; k <= m && (p= update[k])->mForward[k] == q; ++k)
			{
				p->mForward[k] = q->mForward[k];
			}
			delete q;

			while(mList->mHeader->mForward[m] == NULL && m > 0)
			{
				--m;
			}
			mList->mLevel = m;
		}


		ValueType* Find(KeyType& key)
		{
			int k = mList->mLevel;
			PNodeStruct p = mList->mHeader;
			PNodeStruct q = NULL;

			do
			{
				while (q = p->mForward[k], q->mKey < key)
				{
					p = q;
				}
			} while (--k >= 0);

			if (q->mKey != key)
				return NULL;
			else
				return &q->mValue;
		}

	private:
		int RandomLevel()
		{
			return rand() % MAXLEVEL;
		}

		PListStruct mList;
	};
}