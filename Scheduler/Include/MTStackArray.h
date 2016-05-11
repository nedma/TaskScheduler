// The MIT License (MIT)
// 
// 	Copyright (c) 2015 Sergey Makeev, Vadim Slyusarev
// 
// 	Permission is hereby granted, free of charge, to any person obtaining a copy
// 	of this software and associated documentation files (the "Software"), to deal
// 	in the Software without restriction, including without limitation the rights
// 	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// 	copies of the Software, and to permit persons to whom the Software is
// 	furnished to do so, subject to the following conditions:
// 
//  The above copyright notice and this permission notice shall be included in
// 	all copies or substantial portions of the Software.
// 
// 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// 	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// 	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// 	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// 	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// 	THE SOFTWARE.

#pragma once


namespace MT
{

	/// \class StackArray
	/// \brief Array, allocated on stack.
	template<class T, size_t capacity>
	class StackArray
	{
		static const int32 ALIGNMENT = 16;
		static const int32 ALIGNMENT_MASK = (ALIGNMENT-1);

		uint32 count;

		byte rawMemory_[sizeof(T) * capacity + ALIGNMENT];

		inline T* IndexToObject(int32 index)
		{
			byte* alignedMemory = (byte*)( ( (intptr_t)&rawMemory_[0] + ALIGNMENT_MASK ) & ~ALIGNMENT_MASK );
			T* pObjectMemory = (T*)(alignedMemory + index * sizeof(T));
			return pObjectMemory;
		}

		inline void CopyCtor(T* element, const T & val)
		{
			new(element) T(val);
		}

		inline void MoveCtor(T* element, T && val)
		{
			new(element) T(std::move(val));
		}

		inline void Dtor(T* element)
		{
			MT_UNUSED(element);
			element->~T();
		}

	public:

		MT_NOCOPYABLE(StackArray);

		inline StackArray()
			: count(0)
		{
		}

		inline StackArray(uint32 _count, const T & defaultElement = T())
			: count(_count)
		{
			MT_ASSERT(count <= capacity, "Too big size");
			for (size_t i = 0; i < count; i++)
			{
				CopyCtor(Begin() + i, defaultElement);
			}
		}

		inline ~StackArray()
		{
			for (size_t i = 0; i < count; i++)
			{
				Dtor(Begin() + i);
			}
		}

		inline const T &operator[]( uint32 i ) const
		{
			MT_ASSERT( i < Size(), "bad index" );
			return *IndexToObject(i);
		}

		inline T &operator[]( uint32 i )
		{
			MT_ASSERT( i < Size(), "bad index" );
			return *IndexToObject(i);
		}

		inline void PushBack(T && val)
		{
			MT_ASSERT(count < capacity, "Can't add element");
			uint32 lastElementIndex = count;
			count++;
			MoveCtor( IndexToObject(lastElementIndex), std::move(val) );
		}

		inline size_t Size() const
		{
			return count;
		}

		inline bool IsEmpty() const
		{
			return count == 0;
		}

		inline T* Begin()
		{
			return IndexToObject(0);
		}
	};



}

