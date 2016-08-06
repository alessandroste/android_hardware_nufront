/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2011-2012 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

/**
 * @file mali_function_tracing.h
 * Functionality for tracing function calls in order to produce backtraces or logs.
 */

#ifndef MALI_FUNCTION_TRACING_H_INCLUDED
#define MALI_FUNCTION_TRACING_H_INCLUDED

#include <base/mali_macros.h>
#if MALI_TIMELINE_PROFILING_ENABLED
#include "base/mali_profiling.h"
#endif

#define MALI_FUNCTION_TRACING_ASSERT_TRUE(cond, msg) if (!(cond)) {printf("Fatal error!\nFunction:\t%s\nFile:\t%s:%d\nMessage:\t%s\n", __func__, __FILE__, __LINE__, msg); exit (0); }

typedef struct _mali_sys_trace_thread_call_stack
{
	int indent;              /* current indent level */
	void* func_addrs[128];   /* last function address for each indent level */
	unsigned long thread_id; /* current thread's id */
} _mali_sys_trace_thread_call_stack;

typedef struct _mali_sys_trace_stack
{
	int depth;		/* stach depth */
	void* func_addrs[128];	/* max of 128 functions in the stack */
} _mali_sys_trace_stack;

MALI_NOTRACE void __cyg_profile_func_enter(void *this_fn, void *call_site);
MALI_NOTRACE void __cyg_profile_func_exit(void *this_fn, void *call_site);
MALI_IMPORT MALI_NOTRACE void _mali_sys_trace_print_all_backtraces();
MALI_IMPORT MALI_NOTRACE void _mali_sys_trace_print_thread_backtrace(unsigned long threadid);
MALI_IMPORT MALI_NOTRACE void _mali_sys_trace_start();
MALI_IMPORT MALI_NOTRACE void _mali_sys_trace_stop();
MALI_IMPORT MALI_NOTRACE void _mali_sys_trace_dump_last_64k(const char* file_name);
MALI_IMPORT MALI_NOTRACE void _mali_sys_trace_get_stack(_mali_sys_trace_stack* stack);
MALI_IMPORT MALI_NOTRACE void _mali_sys_trace_print_stack(_mali_sys_trace_stack* stack);

#endif
