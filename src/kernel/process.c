/**
 * @file process.c
 */

#include <kernel/process.h>
#include <kmath.h>
#include <memory/kglobals.h>
#include <memory/kmemory.h>
#include <memory/memoryMap.h>
#include <memory/paging.h>

/**
 * @brief Generates a new process id
 * @return Newly generated unique process id
 */
uint64_t process_genPID()
{
    return (*PID)++;
}

/**
 * @brief Translates memory address from user space to kernel space
 * @param addr address to be tranlated
 * @return kernel address
 */
uint64_t process_kernel_address(uint64_t addr)
{
    return (ADDRESS_SECTION_SIZE * (2 + (*CURRENT_PROCESS)->pid) + addr);
}

/**
 * @brief Maps a page to the process
 * @param process process that page gets mapped to
 * @param addr starting address of pages being added
 * @param page_number start page that is being added
 * @param page_count number of pages being mapped
 * @param pageSize size of page being added to process
 * @param use use of page
 * @return process space virtual address
 */
uint64_t process_add_page(uint64_t page_number,
                          uint64_t page_count,
                          uint64_t page_size,
                          process_page_use use)
{
    DECLARE_PROCESS;

    /* userspace address */
    uint64_t user_addr = 0;

    /* Lazy efficient paging for now */
    switch (use)
    {
    case PROCESS_PAGE_HEAP:
        /* Sets address to new heap page address */
        user_addr = ALIGN_UP(process->process_heap_ptr, page_size);
        process->process_heap_ptr = user_addr + page_size;
        break;
    case PROCESS_PAGE_SHARED: /* Starts at 128 gb and goes down */
        /* Sets address to new shared memory address */
        user_addr = process->process_shared_ptr =
            ALIGN_DOWN(process->process_shared_ptr - page_size, page_size);
        break;
    default:
        break;
    }

    /* Adds page at resulting user address */
    pageTable_addPage(process->page_table, process->process_heap_ptr, page_number, page_count,
                      page_size, 0);

    /* Adds page to kernel as well */
    uint64_t kernel_page_offset = process_kernel_address(0) / page_size;
    pageTable_addPage(process->page_table, process->process_heap_ptr,
                      page_number + kernel_page_offset, page_count, page_size, 0);

    return user_addr;
}

/**
 * @brief Validates a memory region in a process
 * @param vaddr virtual address checking
 * @param size size of memory region being checked
 * @return 1 for valid 0 for invalid
 */
bool process_validate_address(void* vaddr, size_t size)
{
    DECLARE_PROCESS;

    size_t start = (size_t)vaddr;
    size_t end = start + size;

    // TODO: implement this somehow

    return 1;
}