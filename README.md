# mmtest

## Compile
```
nix-shell '<nixpkgs>' -A linuxPackages.kernel.dev

# user
gcc -O2 -o hello.c
./a.out

# kernel module
make -C $(nix-build -E '(import <nixpkgs> {}).linuxPackages.kernel.dev' --no-out-link)/lib/modules/*/build M=$(pwd) modules
insmod ./hello.ko
dmesg
```

## Result
- Intel(R) Xeon(R) Gold 5317 CPU @ 3.00GHz
- Linux 6.6
- gcc 12.3.0

### User
```
           memcmp, 0, 7508059280,0
           memcmp, 1, 7487105314,0
           memcmp, 2, 7509339482,0
           memcmp, 3, 7530509426,0
           memcmp, 4, 7500724014,0
           memcmp, 5, 7509820038,0
          _memcmp, 0, 6040040466,0
          _memcmp, 1, 4485490192,0
          _memcmp, 2, 7009463326,0
          _memcmp, 3,12007660718,0
          _memcmp, 4,15018989300,0
          _memcmp, 5,14109988700,0
   newlibc_memcmp, 0, 5229470052,0
   newlibc_memcmp, 1, 9012247622,0
   newlibc_memcmp, 2,12044230904,0
   newlibc_memcmp, 3,16941223646,0
   newlibc_memcmp, 4,18062036144,0
   newlibc_memcmp, 5,15023593576,0
      musl_memcmp, 0, 4423117470,0
      musl_memcmp, 1, 6229250076,0
      musl_memcmp, 2, 7132367490,0
      musl_memcmp, 3, 9078138278,0
      musl_memcmp, 4,10691345322,0
      musl_memcmp, 5,14989973630,0
     glibc_memcmp, 0, 6834575084,0
     glibc_memcmp, 1, 6942452534,0
     glibc_memcmp, 2,10059180554,0
     glibc_memcmp, 3,10716785256,0
     glibc_memcmp, 4,12052863842,0
     glibc_memcmp, 5,18172893800,0
 __builtin_memcmp, 0, 7131712328,0
 __builtin_memcmp, 1, 7133138106,0
 __builtin_memcmp, 2, 7132919124,0
 __builtin_memcmp, 3, 7135642722,0
 __builtin_memcmp, 4, 7132152626,0
 __builtin_memcmp, 5, 7134945756,0
```

### Kernel
```
[425535.352942]            memcmp, 0, 6058685024,0
[425538.357912]            memcmp, 1, 9014902960,0
[425542.033645]            memcmp, 2,11027195278,0
[425546.212130]            memcmp, 3,12535456150,0
[425550.887444]            memcmp, 4,14025942288,0
[425556.597253]            memcmp, 5,17129426782,0
[425558.830615]           _memcmp, 0, 6700084836,0
[425560.505822]           _memcmp, 1, 5025617362,0
[425562.857385]           _memcmp, 2, 7054685216,0
[425565.868255]           _memcmp, 3, 9032609540,0
[425569.217697]           _memcmp, 4,10048327776,0
[425574.225104]           _memcmp, 5,15022222298,0
[425576.126982]    newlibc_memcmp, 0, 5705629192,0
[425579.465330]    newlibc_memcmp, 1,10015044260,0
[425583.805214]    newlibc_memcmp, 2,13019652810,0
[425589.471091]    newlibc_memcmp, 3,16997632202,0
[425595.814058]    newlibc_memcmp, 4,19028903374,0
[425603.169126]    newlibc_memcmp, 5,22065206664,0
[425604.512399]       musl_memcmp, 0, 4029814842,0
[425606.515666]       musl_memcmp, 1, 6009799480,0
[425609.301751]       musl_memcmp, 2, 8358252372,0
[425612.720297]       musl_memcmp, 3,10255638364,0
[425617.062010]       musl_memcmp, 4,13025140608,0
[425622.070183]       musl_memcmp, 5,15024519672,0
[425624.115100]      glibc_memcmp, 0, 6134749366,0
[425626.405960]      glibc_memcmp, 1, 6872578296,0
[425628.953528]      glibc_memcmp, 2, 7642700366,0
[425632.004636]      glibc_memcmp, 3, 9153325092,0
[425636.003662]      glibc_memcmp, 4,11997078450,0
[425639.758006]      glibc_memcmp, 5,11263031926,0
[425641.558123]  __builtin_memcmp, 0, 5400347908,0
[425644.581852]  __builtin_memcmp, 1, 9071184864,0
[425648.587989]  __builtin_memcmp, 2,12018412372,0
[425654.263165]  __builtin_memcmp, 3,17025527326,0
[425660.164779]  __builtin_memcmp, 4,17704842598,0
[425666.887317]  __builtin_memcmp, 5,20167617270,0
```
