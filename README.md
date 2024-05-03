# mmtest
- memcmp benchmark

## Variatns
- `memcmp`: memcmp in string.h
- `_memcmp`: memcmp in linux/tools/include/nolibc/string.h
- `newlibc_memcmp`: newlib/libc/string/memcmp.c
- `musl_memcmp`: https://git.musl-libc.org/cgit/musl/tree/src/string/memcmp.c
- `glibc_memcmp`: glibc/string/memcmp.c
- `__sse_memcmp` (SSE2): https://gist.github.com/karthick18/1361842
- `rte_memcmp` (AVX512): https://patchwork.dpdk.org/project/dpdk/patch/1429716828-19012-2-git-send-email-rkerur@gmail.com/
- `__builtin_memcmp`: gcc builtin (same as `memcmp`?)

## Compile
```
nix-shell '<nixpkgs>' -A linuxPackages.kernel.dev

# user
gcc -O2 -mavx512f -o mm_test.c
./a.out

# kernel module
make -C $(nix-build -E '(import <nixpkgs> {}).linuxPackages.kernel.dev' --no-out-link)/lib/modules/*/build M=$(pwd) modules
insmod ./mm_test.ko
dmesg
```

## Result
- Intel(R) Xeon(R) Gold 5317 CPU @ 3.00GHz
- Linux 6.6
- gcc 12.3.0

### User
```
           memcmp, 0,   74943220,0
           memcmp, 1,   75613218,0
           memcmp, 2,   73780188,0
           memcmp, 3,   75331590,0
           memcmp, 4,   75963408,0
           memcmp, 5,   75258058,0
           memcmp, 6,   72358702,0
           memcmp, 7,   76048524,0
           memcmp, 8,   74609378,0
           memcmp, 9,   75732212,0
           memcmp,10,   75405228,0
          _memcmp, 0,   64618060,0
          _memcmp, 1,   40108866,0
          _memcmp, 2,   70105436,0
          _memcmp, 3,  100177220,0
          _memcmp, 4,  130194448,0
          _memcmp, 5,  129352952,0
          _memcmp, 6,  150904432,0
          _memcmp, 7,  160289268,0
          _memcmp, 8,  132721138,0
          _memcmp, 9,  140238626,0
          _memcmp,10,  150366298,0
   newlibc_memcmp, 0,   60754748,0
   newlibc_memcmp, 1,   90130872,0
   newlibc_memcmp, 2,  120474122,0
   newlibc_memcmp, 3,  168679210,0
   newlibc_memcmp, 4,  180603522,0
   newlibc_memcmp, 5,  151037862,0
   newlibc_memcmp, 6,  160653706,0
   newlibc_memcmp, 7,  170748704,0
   newlibc_memcmp, 8,   70099998,0
   newlibc_memcmp, 9,  120228174,0
   newlibc_memcmp,10,  140258998,0
      musl_memcmp, 0,   27109388,0
      musl_memcmp, 1,   53992084,0
      musl_memcmp, 2,   88889826,0
      musl_memcmp, 3,  109582626,0
      musl_memcmp, 4,   98683134,0
      musl_memcmp, 5,  100404058,0
      musl_memcmp, 6,  110687832,0
      musl_memcmp, 7,  130199514,0
      musl_memcmp, 8,  140217838,0
      musl_memcmp, 9,  150733014,0
      musl_memcmp,10,  160240978,0
     glibc_memcmp, 0,   70307574,0
     glibc_memcmp, 1,   94410504,0
     glibc_memcmp, 2,  120678448,0
     glibc_memcmp, 3,  165070304,0
     glibc_memcmp, 4,  180362786,0
     glibc_memcmp, 5,  176026390,0
     glibc_memcmp, 6,  213197062,0
     glibc_memcmp, 7,  235452758,0
     glibc_memcmp, 8,  182499852,0
     glibc_memcmp, 9,  192595348,0
     glibc_memcmp,10,  200517154,0
     __sse_memcmp, 0,   60091446,0
     __sse_memcmp, 1,   70107028,0
     __sse_memcmp, 2,   80115576,0
     __sse_memcmp, 3,  100159286,0
     __sse_memcmp, 4,   80413878,0
     __sse_memcmp, 5,   91041446,0
     __sse_memcmp, 6,  100147902,0
     __sse_memcmp, 7,  110173546,0
     __sse_memcmp, 8,   60090424,0
     __sse_memcmp, 9,   59958514,0
     __sse_memcmp,10,   60779614,0
       rte_memcmp, 0,   70097256,0
       rte_memcmp, 1,   59049062,0
       rte_memcmp, 2,   71234398,0
       rte_memcmp, 3,   71437756,0
       rte_memcmp, 4,   64533882,0
       rte_memcmp, 5,   65894586,0
       rte_memcmp, 6,   81115662,0
       rte_memcmp, 7,   80145912,0
       rte_memcmp, 8,   70475816,0
       rte_memcmp, 9,   70808298,0
       rte_memcmp,10,   73326774,0
 __builtin_memcmp, 0,   74850638,0
 __builtin_memcmp, 1,   71438928,0
 __builtin_memcmp, 2,   73220436,0
 __builtin_memcmp, 3,   74366264,0
 __builtin_memcmp, 4,   63049092,0
 __builtin_memcmp, 5,   69862346,0
 __builtin_memcmp, 6,   76362026,0
 __builtin_memcmp, 7,   73797816,0
 __builtin_memcmp, 8,   76439368,0
 __builtin_memcmp, 9,   73945308,0
 __builtin_memcmp,10,   67519276,0
```

### Kernel
```
[452441.878836]            memcmp, 0,   51875248,0
[452441.908895]            memcmp, 1,   90164928,0
[452441.949038]            memcmp, 2,  120425058,0
[452442.004563]            memcmp, 3,  166572482,0
[452442.064651]            memcmp, 4,  180261262,0
[452442.129987]            memcmp, 5,  196001316,0
[452442.201084]            memcmp, 6,  213290396,0
[452442.279560]            memcmp, 7,  235423472,0
[452442.303755]            memcmp, 8,   72580546,0
[452442.340934]            memcmp, 9,  111534344,0
[452442.387230]            memcmp,10,  138883690,0
[452442.409403]           _memcmp, 0,   66516366,0
[452442.426115]           _memcmp, 1,   50130946,0
[452442.452828]           _memcmp, 2,   80134656,0
[452442.490634]           _memcmp, 3,  113415256,0
[452442.535688]           _memcmp, 4,  135159600,0
[452442.592467]           _memcmp, 5,  170333386,0
[452442.650033]           _memcmp, 6,  172694302,0
[452442.713479]           _memcmp, 7,  190332462,0
[452442.783611]           _memcmp, 8,  210392066,0
[452442.860414]           _memcmp, 9,  230407304,0
[452442.944105]           _memcmp,10,  251068710,0
[452442.964087]    newlibc_memcmp, 0,   59942716,0
[452442.997474]    newlibc_memcmp, 1,  100156968,0
[452443.037634]    newlibc_memcmp, 2,  120476792,0
[452443.084452]    newlibc_memcmp, 3,  140450000,0
[452443.138023]    newlibc_memcmp, 4,  160708358,0
[452443.211532]    newlibc_memcmp, 5,  220523780,0
[452443.282126]    newlibc_memcmp, 6,  211778212,0
[452443.358907]    newlibc_memcmp, 7,  230340142,0
[452443.382331]    newlibc_memcmp, 8,   70268016,0
[452443.429073]    newlibc_memcmp, 9,  140223432,0
[452443.482683]    newlibc_memcmp,10,  160826492,0
[452443.489425]       musl_memcmp, 0,   20221948,0
[452443.506626]       musl_memcmp, 1,   51601632,0
[452443.536875]       musl_memcmp, 2,   90742528,0
[452443.574967]       musl_memcmp, 3,  114272362,0
[452443.618392]       musl_memcmp, 4,  130273458,0
[452443.681833]       musl_memcmp, 5,  190318248,0
[452443.757862]       musl_memcmp, 6,  228086330,0
[452443.824221]       musl_memcmp, 7,  199072484,0
[452443.894332]       musl_memcmp, 8,  210328190,0
[452443.971116]       musl_memcmp, 9,  230351070,0
[452444.054609]       musl_memcmp,10,  250475534,0
[452444.077172]      glibc_memcmp, 0,   67682872,0
[452444.099188]      glibc_memcmp, 1,   66044814,0
[452444.127075]      glibc_memcmp, 2,   83658398,0
[452444.163991]      glibc_memcmp, 3,  110744782,0
[452444.215898]      glibc_memcmp, 4,  155718892,0
[452444.277310]      glibc_memcmp, 5,  184230190,0
[452444.337749]      glibc_memcmp, 6,  181315622,0
[452444.403025]      glibc_memcmp, 7,  195824844,0
[452444.462563]      glibc_memcmp, 8,  178610636,0
[452444.519321]      glibc_memcmp, 9,  170268418,0
[452444.582916]      glibc_memcmp,10,  190783462,0
[452444.602957]  __builtin_memcmp, 0,   60117444,0
[452444.633008]  __builtin_memcmp, 1,   90149350,0
[452444.669902]  __builtin_memcmp, 2,  110678872,0
[452444.711785]  __builtin_memcmp, 3,  125647822,0
[452444.758544]  __builtin_memcmp, 4,  140272902,0
[452444.816252]  __builtin_memcmp, 5,  173121464,0
[452444.880828]  __builtin_memcmp, 6,  193723708,0
[452444.954680]  __builtin_memcmp, 7,  221552258,0
[452444.982473]  __builtin_memcmp, 8,   83373458,0
[452445.019289]  __builtin_memcmp, 9,  110445352,0
[452445.063410]  __builtin_memcmp,10,  132358226,0
```
