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

#include <MTTools.h>
#include <MTPlatform.h>
#include <MTConcurrentQueueLIFO.h>
#include <MTStackArray.h>
#include <MTArrayView.h>


#define MT_COLOR_DEFAULT (0)
#define MT_COLOR_BLUE (1)
#define MT_COLOR_RED (2)
#define MT_COLOR_YELLOW (3)

#ifdef MT_INSTRUMENTED_BUILD

#define DECLARE_DEBUG(name, colorID) \
	static const char * GetDebugID() \
	{ \
		return name; \
	} \
	static int GetDebugColorIndex() \
	{ \
		return colorID; \
	}

#else

#define DECLARE_DEBUG(name, colorID)

#endif






namespace MT
{
		template<typename T>
		struct TaskBase
		{
			static void TaskEntryPoint(MT::FiberContext& fiberContext, void* userData)
			{
				T* task = static_cast<T*>(userData);
				task->Do(fiberContext);
			}
		};
}
