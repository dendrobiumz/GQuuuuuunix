#include "string.h"
#include "console.h"


static void memcpy_1(u8* dst, const u8 *src, i64 n)
{
    while (n--)
        *dst++ = *src++;
}


static void memcpy_n(void* dst, const void *src, i64 n)
{
    i64 eight_bytes_cnt = n / 8;
    i64 remain_four_bytes_cnt = (n - eight_bytes_cnt * 8) / 4;
    i64 remain_two_bytes_cnt = (n - eight_bytes_cnt * 8 - remain_four_bytes_cnt * 4) / 2;
    i64 remain_bytes = n - eight_bytes_cnt * 8 - remain_four_bytes_cnt * 4 - remain_two_bytes_cnt * 2;
    u64 *_dst8 = (u64*) dst, *_src8 = (u64*) src;

    while (eight_bytes_cnt--) {
        *_dst8++ = * _src8++;
    }
    u32 *_dst4 = (u32*)_dst8, *_src4 = (u32*) _src8;

    while (remain_four_bytes_cnt--) {
        *_dst4++ = * _src4++;
    }

    u16 *_dst2 = (u16*)_dst4, *_src2 = (u16*) _src4;

    while (remain_two_bytes_cnt--) {
        *_dst2++ = * _src2++;
    }

    u8 *_dst = (u8*) _dst2, *_src = (u8*) _src2;

    while (remain_bytes--) {
        *_dst++ = * _src++;
    }
}

void * memcpy(void* dst, const void *src, i64 n)
{
    //u64 _dst = (u64) dst, _src = (u64) src;
    int dst_align = ((u64) dst ) % 8, src_align = ((u64) src) % 8;

    if (dst_align == 0 && src_align == 0) {
        memcpy_n(dst, src, n);
    } else if (dst_align > 0 && src_align > 0 && dst_align == src_align) {
        u8 *dst_8 = (u8*) dst, *src_8 = (u8*) src;
        while (dst_align--) {
            *dst_8++ = *src_8++;
        }
        memcpy_n(dst_8, src_8, n - src_align);
    } else {
        memcpy_1(dst, src, n);
    }

    return dst;
}


i64 strlen(const char* str)
{
    if (str == NULL)
        PANIC("[STRLEN] NULL PTR\n");

    i64 len = 0;
    while (*str++ != '\0')
        ++len;
    return len;
}