#ifdef __KERNEL__
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>

#define MODULE_NAME "hello"
#define PRINT pr_info

MODULE_LICENSE("GPL");
#else
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define PRINT printf
#endif

uint64_t _rdtsc(void){
    union {
        uint64_t val;
        struct {
            uint32_t lo;
            uint32_t hi;
        };
    } tsc;
    asm volatile ("rdtsc" : "=a" (tsc.lo), "=d" (tsc.hi));
    return tsc.val;
}

//--------------------------------------------------
// from linux/tools/include/nolibc/string.h
int _memcmp(const void *s1, const void *s2, size_t n)
{
    size_t ofs = 0;
    int c1 = 0;

    while (ofs < n && !(c1 = ((unsigned char *)s1)[ofs] - ((unsigned char *)s2)[ofs])) {
        ofs++;
    }
    return c1;
}

//--------------------------------------------------
// newlib/libc/string/memcmp.c

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED(X, Y) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))

/* How many bytes are copied each iteration of the word copy loop.  */
#define LBLOCKSIZE (sizeof (long))

/* Threshhold for punting to the byte copier.  */
#define TOO_SMALL(LEN)  ((LEN) < LBLOCKSIZE)

int
newlibc_memcmp (const void *m1,
    const void *m2,
    size_t n)
{
  unsigned char *s1 = (unsigned char *) m1;
  unsigned char *s2 = (unsigned char *) m2;
  unsigned long *a1;
  unsigned long *a2;

  /* If the size is too small, or either pointer is unaligned,
     then we punt to the byte compare loop.  Hopefully this will
     not turn up in inner loops.  */
  if (!TOO_SMALL(n) && !UNALIGNED(s1,s2))
    {
      /* Otherwise, load and compare the blocks of memory one 
         word at a time.  */
      a1 = (unsigned long*) s1;
      a2 = (unsigned long*) s2;
      while (n >= LBLOCKSIZE)
        {
          if (*a1 != *a2) 
        break;
          a1++;
          a2++;
          n -= LBLOCKSIZE;
        }

      /* check m mod LBLOCKSIZE remaining characters */

      s1 = (unsigned char*)a1;
      s2 = (unsigned char*)a2;
    }

  while (n--)
    {
      if (*s1 != *s2)
    return *s1 - *s2;
      s1++;
      s2++;
    }

  return 0;
}

//--------------------------------------------------

int musl_memcmp(const void *vl, const void *vr, size_t n)
{
    const unsigned char *l=vl, *r=vr;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l-*r : 0;
}

//--------------------------------------------------
//glibc memcmp
//glibc/string/memcmp.c

#ifndef __KERNEL__
# include <sys/types.h>
#endif

typedef unsigned long int __attribute__ ((__may_alias__)) op_t;

#define OP_T_THRES  16
# define OPSIZ  (sizeof (op_t))

typedef unsigned char byte;

# ifndef WORDS_BIGENDIAN
#  define MERGE(w0, sh_1, w1, sh_2) (((w0) >> (sh_1)) | ((w1) << (sh_2)))
# else
#  define MERGE(w0, sh_1, w1, sh_2) (((w0) << (sh_1)) | ((w1) >> (sh_2)))
# endif

#ifdef WORDS_BIGENDIAN
# define CMP_LT_OR_GT(a, b) ((a) > (b) ? 1 : -1)
#else
# define CMP_LT_OR_GT(a, b) memcmp_bytes ((a), (b))
#endif

static int
memcmp_bytes (op_t a, op_t b)
{
  long int srcp1 = (long int) &a;
  long int srcp2 = (long int) &b;
  op_t a0, b0;

  do
    {
      a0 = ((byte *) srcp1)[0];
      b0 = ((byte *) srcp2)[0];
      srcp1 += 1;
      srcp2 += 1;
    }
  while (a0 == b0);
  return a0 - b0;
}

static int
memcmp_common_alignment (long int srcp1, long int srcp2, size_t len)
{
  op_t a0, a1;
  op_t b0, b1;

  switch (len % 4)
    {
    default: /* Avoid warning about uninitialized local variables.  */
    case 2:
      a0 = ((op_t *) srcp1)[0];
      b0 = ((op_t *) srcp2)[0];
      srcp1 -= 2 * OPSIZ;
      srcp2 -= 2 * OPSIZ;
      len += 2;
      goto do1;
    case 3:
      a1 = ((op_t *) srcp1)[0];
      b1 = ((op_t *) srcp2)[0];
      srcp1 -= OPSIZ;
      srcp2 -= OPSIZ;
      len += 1;
      goto do2;
    case 0:
      if (OP_T_THRES <= 3 * OPSIZ && len == 0)
    return 0;
      a0 = ((op_t *) srcp1)[0];
      b0 = ((op_t *) srcp2)[0];
      goto do3;
    case 1:
      a1 = ((op_t *) srcp1)[0];
      b1 = ((op_t *) srcp2)[0];
      srcp1 += OPSIZ;
      srcp2 += OPSIZ;
      len -= 1;
      if (OP_T_THRES <= 3 * OPSIZ && len == 0)
    goto do0;
      /* Fall through.  */
    }

  do
    {
      a0 = ((op_t *) srcp1)[0];
      b0 = ((op_t *) srcp2)[0];
      if (a1 != b1)
    return CMP_LT_OR_GT (a1, b1);

    do3:
      a1 = ((op_t *) srcp1)[1];
      b1 = ((op_t *) srcp2)[1];
      if (a0 != b0)
    return CMP_LT_OR_GT (a0, b0);

    do2:
      a0 = ((op_t *) srcp1)[2];
      b0 = ((op_t *) srcp2)[2];
      if (a1 != b1)
    return CMP_LT_OR_GT (a1, b1);

    do1:
      a1 = ((op_t *) srcp1)[3];
      b1 = ((op_t *) srcp2)[3];
      if (a0 != b0)
    return CMP_LT_OR_GT (a0, b0);

      srcp1 += 4 * OPSIZ;
      srcp2 += 4 * OPSIZ;
      len -= 4;
    }
  while (len != 0);

  /* This is the right position for do0.  Please don't move
     it into the loop.  */
 do0:
  if (a1 != b1)
    return CMP_LT_OR_GT (a1, b1);
  return 0;
}

/* memcmp_not_common_alignment -- Compare blocks at SRCP1 and SRCP2 with LEN
   `op_t' objects (not LEN bytes!).  SRCP2 should be aligned for memory
   operations on `op_t', but SRCP1 *should be unaligned*.  */
static int
memcmp_not_common_alignment (long int srcp1, long int srcp2, size_t len)
{
  op_t a0, a1, a2, a3;
  op_t b0, b1, b2, b3;
  op_t x;
  int shl, shr;

  /* Calculate how to shift a word read at the memory operation
     aligned srcp1 to make it aligned for comparison.  */

  shl = 8 * (srcp1 % OPSIZ);
  shr = 8 * OPSIZ - shl;

  /* Make SRCP1 aligned by rounding it down to the beginning of the `op_t'
     it points in the middle of.  */
  srcp1 &= -OPSIZ;

  switch (len % 4)
    {
    default: /* Avoid warning about uninitialized local variables.  */
    case 2:
      a1 = ((op_t *) srcp1)[0];
      a2 = ((op_t *) srcp1)[1];
      b2 = ((op_t *) srcp2)[0];
      srcp1 -= 1 * OPSIZ;
      srcp2 -= 2 * OPSIZ;
      len += 2;
      goto do1;
    case 3:
      a0 = ((op_t *) srcp1)[0];
      a1 = ((op_t *) srcp1)[1];
      b1 = ((op_t *) srcp2)[0];
      srcp2 -= 1 * OPSIZ;
      len += 1;
      goto do2;
    case 0:
      if (OP_T_THRES <= 3 * OPSIZ && len == 0)
    return 0;
      a3 = ((op_t *) srcp1)[0];
      a0 = ((op_t *) srcp1)[1];
      b0 = ((op_t *) srcp2)[0];
      srcp1 += 1 * OPSIZ;
      goto do3;
    case 1:
      a2 = ((op_t *) srcp1)[0];
      a3 = ((op_t *) srcp1)[1];
      b3 = ((op_t *) srcp2)[0];
      srcp1 += 2 * OPSIZ;
      srcp2 += 1 * OPSIZ;
      len -= 1;
      if (OP_T_THRES <= 3 * OPSIZ && len == 0)
    goto do0;
      /* Fall through.  */
    }

  do
    {
      a0 = ((op_t *) srcp1)[0];
      b0 = ((op_t *) srcp2)[0];
      x = MERGE(a2, shl, a3, shr);
      if (x != b3)
    return CMP_LT_OR_GT (x, b3);

    do3:
      a1 = ((op_t *) srcp1)[1];
      b1 = ((op_t *) srcp2)[1];
      x = MERGE(a3, shl, a0, shr);
      if (x != b0)
    return CMP_LT_OR_GT (x, b0);

    do2:
      a2 = ((op_t *) srcp1)[2];
      b2 = ((op_t *) srcp2)[2];
      x = MERGE(a0, shl, a1, shr);
      if (x != b1)
    return CMP_LT_OR_GT (x, b1);

    do1:
      a3 = ((op_t *) srcp1)[3];
      b3 = ((op_t *) srcp2)[3];
      x = MERGE(a1, shl, a2, shr);
      if (x != b2)
    return CMP_LT_OR_GT (x, b2);

      srcp1 += 4 * OPSIZ;
      srcp2 += 4 * OPSIZ;
      len -= 4;
    }
  while (len != 0);

  /* This is the right position for do0.  Please don't move
     it into the loop.  */
 do0:
  x = MERGE(a2, shl, a3, shr);
  if (x != b3)
    return CMP_LT_OR_GT (x, b3);
  return 0;
}

int
glibc_memcmp (const void *s1, const void *s2, size_t len)
{
  op_t a0;
  op_t b0;
  long int srcp1 = (long int) s1;
  long int srcp2 = (long int) s2;
  op_t res;

  if (len >= OP_T_THRES)
    {
      /* There are at least some bytes to compare.  No need to test
     for LEN == 0 in this alignment loop.  */
      while (srcp2 % OPSIZ != 0)
    {
      a0 = ((byte *) srcp1)[0];
      b0 = ((byte *) srcp2)[0];
      srcp1 += 1;
      srcp2 += 1;
      res = a0 - b0;
      if (res != 0)
        return res;
      len -= 1;
    }

      /* SRCP2 is now aligned for memory operations on `op_t'.
     SRCP1 alignment determines if we can do a simple,
     aligned compare or need to shuffle bits.  */

      if (srcp1 % OPSIZ == 0)
    res = memcmp_common_alignment (srcp1, srcp2, len / OPSIZ);
      else
    res = memcmp_not_common_alignment (srcp1, srcp2, len / OPSIZ);
      if (res != 0)
    return res;

      /* Number of bytes remaining in the interval [0..OPSIZ-1].  */
      srcp1 += len & -OPSIZ;
      srcp2 += len & -OPSIZ;
      len %= OPSIZ;
    }

  /* There are just a few bytes to compare.  Use byte memory operations.  */
  while (len != 0)
    {
      a0 = ((byte *) srcp1)[0];
      b0 = ((byte *) srcp2)[0];
      srcp1 += 1;
      srcp2 += 1;
      res = a0 - b0;
      if (res != 0)
    return res;
      len -= 1;
    }

  return 0;
}

#ifndef __KERNEL__

// Taken from https://gist.github.com/karthick18/1361842
#include <emmintrin.h>

static int __sse_memcmp_tail(const uint16_t *a, const uint16_t *b, int len)
{
    switch(len)
    {
    case 8:
        if(*a++ != *b++) return -1;
    case 7:
        if(*a++ != *b++) return -1;
    case 6:
        if(*a++ != *b++) return -1;
    case 5:
        if(*a++ != *b++) return -1;
    case 4:
        if(*a++ != *b++) return -1;
    case 3:
        if(*a++ != *b++) return -1;
    case 2:
        if(*a++ != *b++) return -1;
    case 1:
        if(*a != *b) return -1;
    }
    return 0;
}

static int __sse_memcmp(const uint16_t *a, const uint16_t *b, int half_words)
{
    int i = 0;
    int len = half_words;
    int aligned_a = 0, aligned_b = 0;
    if(!len) return 0;
    if(!a && !b) return 0;
   if(!a || !b) return -1;
    if( (unsigned long) a & 1 ) return -1;
    if( (unsigned long) b & 1 ) return -1;
    aligned_a = ( (unsigned long)a & (sizeof(__m128i)-1) );
    aligned_b = ( (unsigned long)b & (sizeof(__m128i)-1) );
    if(aligned_a != aligned_b) return -1; /* both has to be unaligned on the same boundary or aligned */
    if(aligned_a)
    {
        while( len && 
               ( (unsigned long) a & ( sizeof(__m128i)-1) ) )
        {
            if(*a++ != *b++) return -1;
            --len; 
        }
    }
    if(!len) return 0;
    while( len && !(len & 7 ) )
    {
        __m128i x = _mm_load_si128( (__m128i*)&a[i]);
        __m128i y = _mm_load_si128( (__m128i*)&b[i]);
        /*
         * _mm_cmpeq_epi16 returns 0xffff for each of the 8 half words when it matches
         */
        __m128i cmp = _mm_cmpeq_epi16(x, y);
        /* 
         * _mm_movemask_epi8 creates a 16 bit mask with the MSB for each of the 16 bytes of cmp
         */
        if ( (uint16_t)_mm_movemask_epi8(cmp) != 0xffffU) return -1; 
        len -= 8;
        i += 8;
    }
    return __sse_memcmp_tail(&a[i], &b[i], len);
}

#if 1
// DPDK
#include <immintrin.h>

#define likely(condition) __builtin_expect(condition, 1)
#define unlikely(condition) __builtin_expect(condition, 0)

static inline int
rte_cmp16(const uint8_t *src_1, const uint8_t *src_2)
{
    __m128i xmm0;
    __m128i xmm1;
    __m128i vcmp;
    uint32_t vmask;

    xmm0 = _mm_loadu_si128((const __m128i *)src_1);
    xmm1 = _mm_loadu_si128((const __m128i *)src_2);

    vcmp = _mm_cmpeq_epi16(xmm0, xmm1);
    vmask = _mm_movemask_epi8(vcmp);
    return (!(vmask == 0xffffU));
}

/**
 * Compare 32 bytes between two locations.
 * Locations should not overlap.
 */
static inline int
rte_cmp32(const uint8_t *src_1, const uint8_t *src_2)
{
    __m256i xmm0;
    __m256i xmm1;
    __m256i vcmp;
    uint64_t vmask;

    xmm0 = _mm256_loadu_si256((const __m256i *)src_1);
    xmm1 = _mm256_loadu_si256((const __m256i *)src_2);

    vcmp = _mm256_cmpeq_epi32(xmm0, xmm1);
    vmask = _mm256_movemask_epi8(vcmp);
    return (!(vmask == 0xffffffffU));
}

/**
 * Compare 64 bytes between two locations.
 * Locations should not overlap.
 */
static inline int
rte_cmp64(const uint8_t *src_1, const uint8_t *src_2)
{
    int ret;

    ret = rte_cmp32(src_1 + 0 * 32, src_2 + 0 * 32);

    if (likely(ret == 0))
        ret = rte_cmp32(src_1 + 1 * 32, src_2 + 1 * 32);

    return ret;
}

/**
 * Compare 128 bytes between two locations.
 * Locations should not overlap.
 */
static inline int
rte_cmp128(const uint8_t *src_1, const uint8_t *src_2)
{
    int ret;

    ret = rte_cmp32(src_1 + 0 * 32, src_2 + 0 * 32);

    if (likely(ret == 0))
        ret = rte_cmp32(src_1 + 1 * 32, src_2 + 1 * 32);

    if (likely(ret == 0))
        ret = rte_cmp32(src_1 + 2 * 32, src_2 + 2 * 32);

    if (likely(ret == 0))
        ret = rte_cmp32(src_1 + 3 * 32, src_2 + 3 * 32);

    return ret;
}

static inline int
rte_memcmp_remainder(const void *_src_1, const void *_src_2, size_t n)
{
    uintptr_t src_1u = (uintptr_t)_src_1;
    uintptr_t src_2u = (uintptr_t)_src_2;

    int ret_1 = 1, ret_2 = 1, ret_4 = 1, ret_8 = 1;

    /**
     * Compare less than 16 bytes
     */
    if (n & 0x01) {
        ret_1 = (*(uint8_t *)src_1u ==
                *(const uint8_t *)src_2u);
        src_1u = (uintptr_t)((const uint8_t *)src_1u + 1);
        src_2u = (uintptr_t)((const uint8_t *)src_2u + 1);
    }
    if (n & 0x02) {
        ret_2 = (*(uint16_t *)src_1u ==
                *(const uint16_t *)src_2u);
        src_1u = (uintptr_t)((const uint16_t *)src_1u + 1);
        src_2u = (uintptr_t)((const uint16_t *)src_2u + 1);
    }
    if (n & 0x04) {
        ret_4 = (*(uint32_t *)src_1u ==
                *(const uint32_t *)src_2u);
        src_1u = (uintptr_t)((const uint32_t *)src_1u + 1);
        src_2u = (uintptr_t)((const uint32_t *)src_2u + 1);
    }
    if (n & 0x08) {
        ret_8 = (*(uint64_t *)src_1u ==
                *(const uint64_t *)src_2u);
    }
    return (!(ret_1 && ret_2 && ret_4 && ret_8));
}

static inline int
rte_memcmp(const void *_src_1, const void *_src_2, size_t n)
{
    const uint8_t *src_1 = (const uint8_t *)_src_1;
    const uint8_t *src_2 = (const uint8_t *)_src_2;
    int ret;

    /**
     * Compare less than 16 bytes
     */
    if (n < 16)
        return rte_memcmp_remainder(_src_1, _src_2, n);

    /**
     * Fast way when compare size exceeds 16 bytes
     */
    if (n <= 32) {
        if (likely(n & 0x20))
            ret = rte_cmp32(src_1, src_2);
        else {
            ret = rte_cmp16(src_1 - 16 + n, src_2 - 16 + n);
            if (likely(ret == 0))
                ret = rte_memcmp_remainder(src_1 - 16 + n,
                        src_2 - 16 + n, n - 16);
        }
        return ret;
    }

    if (n <= 48) {
        if (likely(n & 0x30)) {
            ret = rte_cmp32(src_1, src_2);
            if (likely(ret == 0))
                ret = rte_cmp16(src_1 - 32 + n, src_2 - 32 + n);
        } else {
            ret = rte_cmp32(src_1, src_2);
            if (likely(ret == 0))
                ret = rte_memcmp_remainder(src_1 - 32 + n,
                        src_2 - 32 + n, n - 32);
        }
        return ret;
    }

    if (n <= 64) {
        if (likely(n & 0x40))
            ret = rte_cmp64(src_1, src_2);
        else {
            ret = rte_cmp32(src_1 - 32 + n, src_2 - 32 + n);
            if (likely(ret == 0))
                ret = rte_cmp16(src_1 - 32 + n,
                        src_2 - 32 + n);

            if (likely(ret == 0))
                ret = rte_memcmp_remainder(src_1 - 48 + n,
                        src_2 - 48 + n, n - 48);
        }
        return ret;
    }

    if (n <= 128) {
        if (likely(n & 0x80))
            ret = rte_cmp128(src_1, src_2);
        else {
            ret = rte_cmp64(src_1, src_2);
            if (likely(ret == 0))
                ret = rte_cmp32(src_1 - 64 + n, src_2 - 64 + n);

            if (likely(ret == 0))
                ret = rte_cmp16(src_1 - 96 + n, src_2 - 96 + n);

            if (likely(ret == 0))
                ret = rte_memcmp_remainder(src_1 - 112 + n,
                        src_2 - 112 + n, n - 112);
        }
        return ret;
    }

    return 0;
}
#endif

#endif // __KERNEL__

//--------------------------------------------------

void put_things_in_string(char* str, size_t n){
    for(int i=0; i<n; i++){
        char c = i%10;
        str[i] = c;
    }
    str[n-1] = '\0';
}

void test(void){
    // const uint64_t nb_op = 1000000000;
    const uint64_t nb_op = 10000000;
    char *og, *dest;
#ifdef __KERNEL__
    og = kvmalloc(4096, GFP_KERNEL);
    dest = kvmalloc(4096, GFP_KERNEL);
#else
    og = malloc(4096);
    dest = malloc(4096);
#endif
    int sizes_memcmp[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 25, 30, 35, 40, 41};
    uint64_t acc_time=0;
    int acc = 0;
    put_things_in_string(og, 4096);
    put_things_in_string(dest, 4096);
    int i = 0;

#define TEST(func, i_max) \
    for(i = 0; i < sizeof(sizes_memcmp)/sizeof(int); i++){ \
        int cmpSize = sizes_memcmp[i]; \
        acc = 0; \
        acc_time=0.0; \
        uint64_t start = _rdtsc(); \
        for(volatile uint64_t j = 0; j<nb_op; j++){ \
            acc += func(dest, og, cmpSize); \
        } \
        uint64_t stop = _rdtsc(); \
        acc_time += (stop-start); \
        PRINT("%17s,%2d,%11lld,%d\n", #func, cmpSize, acc_time, acc); \
        if (i >= i_max){ \
            break; \
        } \
    }

    //TEST(memcmp, 5);
    //TEST(_memcmp, 5);
    //TEST(newlibc_memcmp, 5);
    //TEST(musl_memcmp, 5);
    TEST(glibc_memcmp, 10);
#ifndef __KERNEL__
    TEST(__sse_memcmp, 10);
    TEST(rte_memcmp, 10);
#endif
    TEST(__builtin_memcmp, 10);
}

#ifdef __KERNEL__
static int __init hello_init(void) {
    test();
    return 0;
}

static void __exit hello_exit(void) {
}

module_init(hello_init);
module_exit(hello_exit);
#else
int main(){
    test();
    return 0;
}
#endif
