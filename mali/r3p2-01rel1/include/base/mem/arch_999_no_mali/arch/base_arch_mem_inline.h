/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2007-2012 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */
#ifndef _MALI_BASE_ARCH_MEM_INLINE_H_
#define _MALI_BASE_ARCH_MEM_INLINE_H_

#include <base/mali_memory_types.h>
#include <base/mali_byteorder.h>

#ifdef __cplusplus
extern "C" {
#endif

struct arch_memory_bank
{
    u32 capabilities;
    u32 size;
    mali_bool direct_read_available;
    mali_bool block_in_use[4];
    /* implement it as 4 individual malloc buffers*/
    void * block[4];
    void * realblock[4];
};

typedef struct arch_mem
{
    struct arch_memory_bank * bank;
    u32 block_number;
    mali_mem embedded_mali_mem;
    mali_bool is_head_of_block;
    mali_bool is_tail_of_block;
} arch_mem;

MALI_STATIC_INLINE arch_mem * arch_mem_from_mali_mem(mali_mem * mem)
{
    return (arch_mem*)(((u8*)mem) - offsetof(arch_mem, embedded_mali_mem));
}

MALI_STATIC_INLINE void _mali_base_arch_mem_read(void * to, mali_mem * from_mali, u32 from_offset, u32 size)
{
    arch_mem * from_mem = arch_mem_from_mali_mem(from_mali);
    MALI_DEBUG_ASSERT_POINTER(to);
    MALI_DEBUG_ASSERT_POINTER(from_mali);
    MALI_DEBUG_ASSERT( MALI_TRUE == from_mali->is_allocated, ("Operation on free memory block %p detected", from_mali));
    MALI_DEBUG_ASSERT(from_offset <= from_mali->size, ("Offset outside memory block during read"));
    MALI_DEBUG_ASSERT((from_offset + size) <= from_mali->size, ("Would read outside memory block"));
    MALI_DEBUG_ASSERT(0 != from_mem->embedded_mali_mem.mali_addr, ("Arch requested to write to mali memory address 0"));

    _mali_sys_memcpy(to, MALI_REINTERPRET_CAST(void*)(from_mem->embedded_mali_mem.mali_addr + from_offset), size);
}

MALI_STATIC_INLINE void _mali_base_arch_mem_read_mali_to_cpu(void * to, mali_mem * from_mali, u32 from_offset, u32 size, u32 typesize)
{
    /* implemented in user space since all memory is currently mappable */
    arch_mem * mem;

	(void)typesize;

    mem = arch_mem_from_mali_mem(from_mali);

    MALI_DEBUG_ASSERT_POINTER(to);
    MALI_DEBUG_ASSERT_POINTER(from_mali);
    MALI_DEBUG_ASSERT( MALI_TRUE == from_mali->is_allocated, ("Operation on free memory block 0x%X detected", from_mali));
    MALI_DEBUG_ASSERT(from_offset <= from_mali->size, ("Offset outside memory block during read"));
    MALI_DEBUG_ASSERT((from_offset + size) <= from_mali->size, ("Would read outside memory block"));
    MALI_DEBUG_PRINT(6, ("Reading from Mali memory, endian safe routine\n"));

    _mali_byteorder_copy_mali_to_cpu(to, MALI_REINTERPRET_CAST(void*)((MALI_REINTERPRET_CAST(u32)mem->embedded_mali_mem.mali_addr) + from_offset), size, typesize);
}

MALI_STATIC_INLINE void _mali_base_arch_mem_write(mali_mem * to_mali, u32 to_offset, const void* from, u32 size)
{
    arch_mem * to_mem = arch_mem_from_mali_mem(to_mali);

    MALI_DEBUG_ASSERT_POINTER(to_mali);
    MALI_DEBUG_ASSERT_POINTER(from);
    MALI_DEBUG_ASSERT( MALI_TRUE == to_mali->is_allocated, ("Operation on free memory block %p detected", to_mali));
    MALI_DEBUG_ASSERT(to_offset <= to_mali->size, ("Attempt to write outside mali memory write detected"));
    MALI_DEBUG_ASSERT((to_offset + size) <= to_mali->size, ("Attempt to write outside mali memory write detected"));
    MALI_DEBUG_ASSERT(0 != to_mem->embedded_mali_mem.mali_addr, ("Arch requested to write to mali memory address 0"));

    _mali_sys_memcpy(MALI_REINTERPRET_CAST(void*)(to_mem->embedded_mali_mem.mali_addr + to_offset), from, size);
}

MALI_STATIC_INLINE void _mali_base_arch_mem_write_cpu_to_mali(mali_mem * to_mali, u32 to_offset, const void* from, u32 size, u32 typesize)
{
    arch_mem * mem;

	(void)typesize;

    mem = arch_mem_from_mali_mem(to_mali);

    MALI_DEBUG_ASSERT_POINTER(to_mali);
    MALI_DEBUG_ASSERT_POINTER(from);
    MALI_DEBUG_ASSERT( MALI_TRUE == to_mali->is_allocated, ("Operation on free memory block 0x%X detected", to_mali));
    MALI_DEBUG_ASSERT(to_offset <= to_mali->size, ("Attempt to write outside mali memory write detected"));
    MALI_DEBUG_ASSERT((to_offset + size) <= to_mali->size, ("Attempt to write outside mali memory write detected"));
    MALI_DEBUG_PRINT(6, ("Writing to Mali memory @ 0x%X, endian safe routine\n", (void*)(((u32)mem->embedded_mali_mem.mali_addr) + to_offset)));
    MALI_DEBUG_PRINT(3, ("----- write %d bytes @ 0x%08x, endian safe routine\n", size, mem->embedded_mali_mem.mali_addr + to_offset));

    _mali_byteorder_copy_cpu_to_mali(MALI_REINTERPRET_CAST(void*)((MALI_REINTERPRET_CAST(u32)mem->embedded_mali_mem.mali_addr) + to_offset), from, size, typesize);
}

MALI_STATIC_INLINE void _mali_base_arch_mem_copy(mali_mem * to_mali, u32 to_offset, mali_mem * from_mali, u32 from_offset, u32 size)
{
    arch_mem * to_mem = arch_mem_from_mali_mem(to_mali);
    arch_mem * from_mem = arch_mem_from_mali_mem(from_mali);


    MALI_DEBUG_ASSERT_POINTER(to_mali);
    MALI_DEBUG_ASSERT( MALI_TRUE == to_mali->is_allocated, ("Operation on free memory block %p detected", to_mali));
    MALI_DEBUG_ASSERT(to_offset <= to_mali->size, ("Write outside copy destination"));
    MALI_DEBUG_ASSERT((to_offset + size) <= to_mali->size, ("Write outside copy destination"));
    MALI_DEBUG_ASSERT(0 != to_mem->embedded_mali_mem.mali_addr, ("Arch requested to copy to mali memory address 0"));
    MALI_DEBUG_ASSERT_POINTER(from_mali);
    MALI_DEBUG_ASSERT( MALI_TRUE == from_mali->is_allocated, ("Operation on free memory block %p detected", from_mali));
    MALI_DEBUG_ASSERT(from_offset <= from_mali->size, ("Read from outside copy source buffer"));
    MALI_DEBUG_ASSERT((from_offset + size) <= from_mali->size, ("Read from outside copy source buffer"));
    MALI_DEBUG_ASSERT(0 != from_mem->embedded_mali_mem.mali_addr, ("Arch requested to copy from mali memory address 0"));

    if (to_mali == from_mali)
    {
        if (to_offset < from_offset)
        {
            MALI_DEBUG_ASSERT(to_offset + size <= from_offset, ("Overlapping memory copy detected"));
            if (!(to_offset + size <= from_offset)) return;
        }
        else
        {
            MALI_DEBUG_ASSERT(from_offset + size <= to_offset, ("Overlapping memory copy detected"));
            if (!(from_offset + size <= to_offset)) return;
        }
    }

    _mali_sys_memcpy(
                     MALI_REINTERPRET_CAST(void*)(to_mem->embedded_mali_mem.mali_addr + to_offset),
                     (void*)(from_mem->embedded_mali_mem.mali_addr + from_offset),
                     size
                    );
}

MALI_STATIC_INLINE void _mali_base_arch_mem_write_cpu_to_mali_32(mali_mem * to_mali, u32 to_offset, const void* from, u32 size, u32 typesize)
{
	arch_mem * mem;
	const u32 *src32;
	u32 *dst32;

	(void)typesize;
 
	mem = arch_mem_from_mali_mem(to_mali);

	dst32 = MALI_REINTERPRET_CAST(u32*)((MALI_REINTERPRET_CAST(u32)mem->embedded_mali_mem.mali_addr) + to_offset);
	src32 = (const u32 *)from;

	MALI_DEBUG_ASSERT_POINTER(to_mali);
	MALI_DEBUG_ASSERT_POINTER(from);
	MALI_DEBUG_ASSERT( MALI_TRUE == to_mali->is_allocated, ("Operation on free memory block 0x%X detected", to_mali));
	MALI_DEBUG_ASSERT(to_offset <= to_mali->size, ("Attempt to write outside mali memory write detected"));
	MALI_DEBUG_ASSERT((to_offset + size) <= to_mali->size, ("Attempt to write outside mali memory write detected"));
	MALI_DEBUG_PRINT(6, ("Writing to Mali memory @ 0x%X, endian safe routine\n", (void*)(((u32)mem->embedded_mali_mem.mali_addr) + to_offset)));
	MALI_DEBUG_PRINT(3, ("----- write %d bytes @ 0x%08x, endian safe routine\n", size, mem->embedded_mali_mem.mali_addr + to_offset));
	MALI_DEBUG_ASSERT_ALIGNMENT(src32, 4);
	MALI_DEBUG_ASSERT_ALIGNMENT(dst32, 4);

	_mali_byteorder_copy_cpu_to_mali(dst32, src32, size, typesize);
}

#ifdef __cplusplus
}
#endif

#endif /* _MALI_BASE_ARCH_MEM_INLINE_H_ */

