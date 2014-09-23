#include <sys/stat.h>
#include <sys/types.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "usbd_cdc_vcp.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "debug.h"

void USART3Put(uint8_t ch);

/*
 * _sbrk() -  allocate incr bytes of memory from the heap.
 *
 *            Return a pointer to the memory, or abort if there
 *            is insufficient memory available on the heap.
 *
 *            The heap is all the RAM that exists between _end and
 *            __Stack_Init, both of which are calculated by the linker.
 *
 *            _end marks the end of all the bss segments, and represents
 *            the highest RAM address used by the linker to locate data
 *            (either initialised or not.)
 *
 *            __Stack_Init marks the bottom of the stack, as reserved
 *            in the linker script (../linker/linker_stm32f10x_md*.ld)
 */
caddr_t _sbrk(int incr)
{
    extern char _end, __Stack_Init;
    static char *heap_end = &_end;
    char *prev_heap_end = heap_end;

    heap_end += incr;

    if (heap_end > &__Stack_Init)
    {
        PANIC(OutOfHeap,"Out Of Heap");
        abort();
    }
    return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len) {
    int n;
    switch (file) {
    case STDOUT_FILENO: /*stdout*/
        for (n = 0; n < len; n++)
        {
            USART3Put(*ptr++ & (uint16_t)0x01FF);
        }
        break;
    case STDERR_FILENO: /* stderr */
        for (n = 0; n < len; n++)
        {   
            USART3Put(*ptr++ & (uint16_t)0x01FF);
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}