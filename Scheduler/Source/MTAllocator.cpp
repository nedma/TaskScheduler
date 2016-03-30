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

#include <MTAllocator.h>
#include <MTTools.h>

#if MT_SSE_INTRINSICS
#include <xmmintrin.h>
#else
#include <malloc.h>
#endif

namespace MT
{

	void* Memory::Alloc(size_t size, size_t align)
	{
#if MT_SSE_INTRINSICS
		return _mm_malloc(size, align);
#else
		return memalign(size, align);
#endif
	}

	void Memory::Free(void* p)
	{
#if MT_SSE_INTRINSICS
		_mm_free(p);
#else
		free(p);
#endif
	}

	Memory::StackDesc Memory::AllocStack(size_t size)
	{
		StackDesc desc;

#ifdef _WIN32

		MW_SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);

		int pageSize = (int)systemInfo.dwPageSize;
		int pagesCount = (int)size / pageSize;

		//need additional page for stack guard
		if ((size % pageSize) > 0)
		{
			pagesCount++;
		}

		//protected guard page
		pagesCount++;

		desc.stackMemoryBytesCount = pagesCount * pageSize;
		desc.stackMemory = (char*)VirtualAlloc(NULL, desc.stackMemoryBytesCount, MW_MEM_COMMIT, MW_PAGE_READWRITE);
		MT_ASSERT(desc.stackMemory != NULL, "Can't allocate memory");

		desc.stackBottom = desc.stackMemory + pageSize;
		desc.stackTop = desc.stackMemory + desc.stackMemoryBytesCount;

		MW_DWORD oldProtect = 0;
		MW_BOOL res = VirtualProtect(desc.stackMemory, pageSize, MW_PAGE_NOACCESS, &oldProtect);
		MT_USED_IN_ASSERT(res);
		MT_ASSERT(res != 0, "Can't protect memory");

#else

		int pageSize = sysconf(_SC_PAGE_SIZE);
		int pagesCount = size / pageSize;

		//need additional page for stack tail
		if ((size % pageSize) > 0)
		{
			pagesCount++;
		}

		//protected guard page
		pagesCount++;

		desc.stackMemoryBytesCount = pagesCount * pageSize;
		desc.stackMemory = (char*)mmap(NULL, desc.stackMemoryBytesCount, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

		MT_ASSERT((void *)desc.stackMemory != (void *)-1, "Can't allocate memory");

		desc.stackBottom = desc.stackMemory + pageSize;
		desc.stackTop = desc.stackMemory + desc.stackMemoryBytesCount;

		int res = mprotect(desc.stackMemory, pageSize, PROT_NONE);
		MT_USED_IN_ASSERT(res);
		MT_ASSERT(res == 0, "Can't protect memory");


#endif

		return desc;
	}

	void Memory::FreeStack(const Memory::StackDesc & desc)
	{
#ifdef _WIN32

		int res = VirtualFree(desc.stackMemory, 0, MW_MEM_RELEASE);
		MT_USED_IN_ASSERT(res);
		MT_ASSERT(res != 0, "Can't free memory");

#else

		int res = munmap(desc.stackMemory, desc.stackMemoryBytesCount);
		MT_USED_IN_ASSERT(res);
		MT_ASSERT(res == 0, "Can't free memory");

#endif
	}





}
