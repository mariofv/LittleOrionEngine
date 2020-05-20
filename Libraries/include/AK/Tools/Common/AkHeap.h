/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

Version: v2019.2.1  Build: 7250
Copyright (c) 2006-2020 Audiokinetic Inc.
*******************************************************************************/

#pragma once

#include <AK/Tools/Common/AkKeyArray.h>


template <class T_KEY, class T_ITEM, class U_POOL, class U_KEY = AkGetArrayKey< T_KEY, T_ITEM >, class TGrowBy = AkGrowByPolicy_DEFAULT, class TMovePolicy = AkAssignmentMovePolicy<T_ITEM>, class TComparePolicy = AkDefaultSortedKeyCompare<T_KEY> >
class CAkHeap : public AkArray< T_ITEM, const T_ITEM &, U_POOL, TGrowBy, TMovePolicy >
{
	typedef AkArray< T_ITEM, const T_ITEM &, U_POOL, TGrowBy, TMovePolicy > Base;

public:
	T_ITEM * Insert(T_KEY in_Key)
	{
		if (Base::AddLast())
		{
			int insertIdx = Base::m_uLength - 1;

			while (insertIdx != 0)
			{
				int parentIdx = Parent(insertIdx);

				if (Lesser(in_Key, U_KEY::Get(Base::m_pItems[parentIdx])))
				{
					TMovePolicy::Move(Base::m_pItems[insertIdx], Base::m_pItems[parentIdx]);
					insertIdx = parentIdx;
				}
				else
				{
					break;
				}
			}

			T_ITEM* pItem = &Base::m_pItems[insertIdx];

			// Set key
			U_KEY::Get(*pItem) = in_Key;

			return pItem;
		}

		return NULL;
	}

	void RemoveRoot()
	{
		if (Base::m_uLength <= 1)
		{
			Base::m_uLength = 0;
			return;
		}

		Base::m_pItems[0] = Base::m_pItems[Base::m_uLength - 1];
		Base::m_uLength--;

		Heapify();
	}

	void Heapify()
	{
		AkUInt32 idx = 0;

		while(true)
		{
			AkUInt32 left = LeftChild(idx);
			AkUInt32 right = RightChild(idx);
			AkUInt32 smallest = idx;
			
			if (left < Base::m_uLength && Lesser(Base::m_pItems[left].key, Base::m_pItems[idx].key) )
				smallest = left;
			
			if (right < Base::m_uLength && Lesser(Base::m_pItems[right].key, Base::m_pItems[smallest].key) )
				smallest = right;

			if (smallest == idx)
				break;
			
			Swap(idx, smallest);

			idx = smallest;
		}
	}

private:
	AkForceInline bool Lesser(T_KEY &a, T_KEY &b) const
	{
		return TComparePolicy::Lesser((void*)this, a, b);
	}

	AkForceInline void Swap(AkUInt32 in_i0, AkUInt32 in_i1)
	{
		T_ITEM temp;
		TMovePolicy::Move(temp, Base::m_pItems[in_i0]);
		TMovePolicy::Move(Base::m_pItems[in_i0], Base::m_pItems[in_i1]);
		TMovePolicy::Move(Base::m_pItems[in_i1], temp);
	}

	AkForceInline AkUInt32 Parent(AkUInt32 i) 
	{ 
		return (i - 1U) / 2U; 
	}

	AkForceInline AkUInt32 LeftChild(AkUInt32 i) 
	{ 
		return (2U * i + 1U); 
	}

	AkForceInline AkUInt32 RightChild(AkUInt32 i) 
	{
		return (2U * i + 2U); 
	}
};
