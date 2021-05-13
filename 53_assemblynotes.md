# An analysis of generated assemly code.

## How to make the code

* use `-fverbose-asm` flag with `-S`
* `g++ -fverbose-asm -S -std=c++17 5_3_betterbenchmark.cpp -o 53_O0`
* `g++ -fverbose-asm -S -std=c++17 5_3_betterbenchmark.cpp -o 53_O1`

Curation follows.

## 53_O0
### not unrolled

```
132701 # 5_3_betterbenchmark.cpp:54:     for(size_t i=0;i<howmany;i++){
132702     movq    $0, -24(%rbp)   #, i
132703 .L73:
132704 # 5_3_betterbenchmark.cpp:54:     for(size_t i=0;i<howmany;i++){
132705     movl    -52(%rbp), %eax # howmany, tmp180
132706     cltq
132707     cmpq    %rax, -24(%rbp) # _2, i
132708     jnb .L72    #,
```

Seems to be checking loop termination conditions.

Because `size_t` is 64-bit, and `howmany` is an 32-bit integer, 
we had to convert it - We presumably lost a little bit of time due to this.

```
132709 # 5_3_betterbenchmark.cpp:55:         sum *= testees[i];
132710     movq    -24(%rbp), %rdx # i, tmp181
132711     leaq    -160(%rbp), %rax    #, tmp182
132712     movq    %rdx, %rsi  # tmp181,
132713     movq    %rax, %rdi  # tmp182,
132714     call    _ZNSt6vectorIN7GF2Five3GF2ILi285EEESaIS2_EEixEm #
132715     movq    %rax, %rdx  #, _3
132716     leaq    -193(%rbp), %rax    #, tmp183
132717     movq    %rdx, %rsi  # _3,
132718     movq    %rax, %rdi  # tmp183,
132719     call    _ZN7GF2Five3GF2ILi285EEmLERKS1_ #
```

`_ZNSt6vectorIN7GF2Five3GF2ILi285EEESaIS2_EEixEm` seems to handle calls to `vector<GF2Five::GF2>::operator[]`.
`_ZN7GF2Five3GF2ILi285EEmLERKS1_` seems to handle calls to `GF2Five::GF2::operator*=`. 
Very straightforward.

I observed that beacuse the operations were not inlined at all, and much overhead was incurred from preparing to call routines.
If `operator[]` and `operator*=` were some other functions with lots and lots of arguments, this would have been much more disasterous.

```
132720 # 5_3_betterbenchmark.cpp:54:     for(size_t i=0;i<howmany;i++){
132721     addq    $1, -24(%rbp)   #, i
132722     jmp .L73    #
132723 .L72:
```

adds 1 to i and them jumps to .L73, which is on line 132703 above. Straightforward.

### unrolled

* For O0, the assembly code for unfolled GF2 multiplication loop seems to be quite similar: it's just the block above written four times.

```
132807 .L75:
132808 # 5_3_betterbenchmark.cpp:70:     for(size_t i=0;i<howmany;i+=4){
132809     movl    -52(%rbp), %eax # howmany, tmp203
132810     cltq
132811     cmpq    %rax, -32(%rbp) # _9, i
132812     jnb .L74    #,
132813 # 5_3_betterbenchmark.cpp:71:         sum1 *= testees[i];
132814     movq    -32(%rbp), %rdx # i, tmp204
132815     leaq    -160(%rbp), %rax    #, tmp205
132816     movq    %rdx, %rsi  # tmp204,
132817     movq    %rax, %rdi  # tmp205,
132818     call    _ZNSt6vectorIN7GF2Five3GF2ILi285EEESaIS2_EEixEm #
132819     movq    %rax, %rdx  #, _10
132820     leaq    -217(%rbp), %rax    #, tmp206
132821     movq    %rdx, %rsi  # _10,
132822     movq    %rax, %rdi  # tmp206,
132823     call    _ZN7GF2Five3GF2ILi285EEmLERKS1_ #
132824 # 5_3_betterbenchmark.cpp:72:         sum2 *= testees[i+1];
132825     movq    -32(%rbp), %rax # i, tmp207
132826     leaq    1(%rax), %rdx   #, _11
132827     leaq    -160(%rbp), %rax    #, tmp208
132828     movq    %rdx, %rsi  # _11,
132829     movq    %rax, %rdi  # tmp208,
132830     call    _ZNSt6vectorIN7GF2Five3GF2ILi285EEESaIS2_EEixEm #
132831     movq    %rax, %rdx  #, _12
132832     leaq    -218(%rbp), %rax    #, tmp209
132833     movq    %rdx, %rsi  # _12,
132834     movq    %rax, %rdi  # tmp209,
132835     call    _ZN7GF2Five3GF2ILi285EEmLERKS1_ #
132836 # 5_3_betterbenchmark.cpp:73:         sum3 *= testees[i+2];
132837     movq    -32(%rbp), %rax # i, tmp210
132838     leaq    2(%rax), %rdx   #, _13
132839     leaq    -160(%rbp), %rax    #, tmp211
132840     movq    %rdx, %rsi  # _13,
132841     movq    %rax, %rdi  # tmp211,
132842     call    _ZNSt6vectorIN7GF2Five3GF2ILi285EEESaIS2_EEixEm #
132843     movq    %rax, %rdx  #, _14
132844     leaq    -219(%rbp), %rax    #, tmp212
132845     movq    %rdx, %rsi  # _14,
132846     movq    %rax, %rdi  # tmp212,
132847     call    _ZN7GF2Five3GF2ILi285EEmLERKS1_ #
132848 # 5_3_betterbenchmark.cpp:74:         sum4 *= testees[i+3];
132849     movq    -32(%rbp), %rax # i, tmp213
132850     leaq    3(%rax), %rdx   #, _15
132851     leaq    -160(%rbp), %rax    #, tmp214
132852     movq    %rdx, %rsi  # _15,
132853     movq    %rax, %rdi  # tmp214,
132854     call    _ZNSt6vectorIN7GF2Five3GF2ILi285EEESaIS2_EEixEm #
132855     movq    %rax, %rdx  #, _16
132856     leaq    -220(%rbp), %rax    #, tmp215
132857     movq    %rdx, %rsi  # _16,
132858     movq    %rax, %rdi  # tmp215,
132859     call    _ZN7GF2Five3GF2ILi285EEmLERKS1_ #
132860 # 5_3_betterbenchmark.cpp:70:     for(size_t i=0;i<howmany;i+=4){
132861     addq    $4, -32(%rbp)   #, i
132862 # 5_3_betterbenchmark.cpp:70:     for(size_t i=0;i<howmany;i+=4){
132863     jmp .L75    #
```

## 53_O1

* On this file, there are lines of code I have never actually written in the comments.

### Not unrolled
* The relevant assembly code for the not-unrolled loop for GF2 multiplication looks like this: 
  * This is nine non-comment lines in total.
  * Total number of instructions handled in `.L170` is 9 * 20M = 180M.
  * **We use 6 lines to fetch multiplication result, and 3 to keep the loop going.**
```
  1955 .L170:
  1956 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  1957     movzbl  15(%rsp), %eax  # sum.rep, sum.rep
  1958 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  1959     salq    $8, %rax    #, tmp232
  1960     addq    %rsi, %rax  # tmp231, tmp233
  1961 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  1962     movzbl  (%rdx), %ecx    # MEM[base: _296, offset: 0B], MEM[base: _296, offset: 0B]
  1963 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  1964     movzbl  (%rax,%rcx), %eax   # MEM[(const value_type &)_72], tmp235
  1965     movb    %al, 15(%rsp)   # tmp235, sum.rep
  1966     addq    $1, %rdx    #, ivtmp.192
  1967 # 5_3_betterbenchmark.cpp:54:     for(size_t i=0;i<howmany;i++){
  1968     cmpq    %rdi, %rdx  # _298, ivtmp.192
  1969     jne .L170   #,
```

We immediately observe that there are no `call`s to anything: operations have been inlined! Now we will analyze this code, line by line...

```
  1957     movzbl  15(%rsp), %eax  # sum.rep, sum.rep
```
From line 1946, we see that on 15(%rsp) is `sum.rep`; `sum` is our `GF2Five::GF2` variable that stores the product - `rep` being its inner representation (see `5_refactored_allcaches.cpp` for detail)
`sum.rep` is a `std::uint8_t`; therefore we need to zero-extend this to a doubleword to put this to %eax, a 32-bit register.

```
  1959     salq    $8, %rax    #, tmp232
```
We left-shifted `sum.rep` by 8 bits.
```
  1960     addq    %rsi, %rax  # tmp231, tmp233
```
In register `rsi` is the address for `multTable`. Now, we almost have the mamory address of the `multTable` element we seek to find - we now only nees `other.rep`
```
  1962     movzbl  (%rdx), %ecx    # MEM[base: _296, offset: 0B], MEM[base: _296, offset: 0B]
```
What's in `rdx`? Presumably address to `other` or `other.rep`. We fetched other.rep to register `ecx`
```
  1964     movzbl  (%rax,%rcx), %eax   # MEM[(const value_type &)_72], tmp235
```
Then this fetches value of `multTable[rep][other.rep]` into `eax`
```
  1965     movb    %al, 15(%rsp)   # tmp235, sum.rep
```
value in the LSB(yte) of `rax`, a.k.a. `al`, is moved to sum.rep.

```
  1966     addq    $1, %rdx    #, ivtmp.192
  1968     cmpq    %rdi, %rdx  # _298, ivtmp.192
  1969     jne .L170   #,
```
adds 1 to `i`, compares `i` to `howmany`, see if we are to continue or break. 
However, from line 1952 we see that in `rdi` is actually `20000000(%rdx)` - thus we are comparing pointers, not variables. Pretty neat!


### Unrolled
* now for the unrolled loop:
  * This is 24 non-comment lines in total. 
  * total number of instructions in the loop(subtotal) is 24 * 5M = 120M.
  * this is much lower than 180M in the non-unrolled loop, but still not enough why this code runs at 4x speed compared to above

```
  2029 .L171:
  2030 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2031     movzbl  14(%rsp), %r8d  # sum1.rep, sum1.rep
  2032 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2033     salq    $8, %r8 #, tmp245
  2034     addq    %rdi, %r8   # tmp244, tmp246
  2035 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2036     movzbl  (%rsi), %r10d   # MEM[base: _257, offset: 0B], MEM[base: _257, offset: 0B]
  2037 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2038     movzbl  (%r8,%r10), %r8d    # MEM[(const value_type &)_107], _110
  2039     movb    %r8b, 14(%rsp)  # _110, sum1.rep
  2040 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2041     movzbl  %cl, %ecx   # sum2$rep, sum2$rep
  2042 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2043     salq    $8, %rcx    #, tmp250
  2044     addq    %rdi, %rcx  # tmp244, tmp251
  2045 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2046     movzbl  1(%rsi), %r10d  # MEM[base: _257, offset: 1B], MEM[base: _257, offset: 1B]
  2047 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2048     movzbl  (%rcx,%r10), %ecx   # MEM[(const value_type &)_100], sum2$rep
  2049 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2050     movzbl  %dl, %edx   # sum3$rep, sum3$rep
  2051 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2052     salq    $8, %rdx    #, tmp255
  2053     addq    %rdi, %rdx  # tmp244, tmp256
  2054 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2055     movzbl  2(%rsi), %r10d  # MEM[base: _257, offset: 2B], MEM[base: _257, offset: 2B]
  2056 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2057     movzbl  (%rdx,%r10), %edx   # MEM[(const value_type &)_94], sum3$rep
  2058 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2059     movzbl  %al, %eax   # sum4$rep, sum4$rep
  2060 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2061     salq    $8, %rax    #, tmp260
  2062     addq    %rdi, %rax  # tmp244, tmp261
  2063 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2064     movzbl  3(%rsi), %r10d  # MEM[base: _257, offset: 3B], MEM[base: _257, offset: 3B]
  2065 # 5_refactored_allcaches.cpp:109:             rep = multTable[rep][other.rep]; return *this;
  2066     movzbl  (%rax,%r10), %eax   # MEM[(const value_type &)_88], sum4$rep
```

Yes, we removed three lines by re-using calculation results. 

```
  2067     addq    $4, %rsi    #, ivtmp.185
  2068 # 5_3_betterbenchmark.cpp:70:     for(size_t i=0;i<howmany;i+=4){
  2069     cmpq    %r9, %rsi   # _292, ivtmp.185
  2070     jne .L171   #,
```

And loop condition checking runs 5M times instead of 20M.

## Conclusion

Inlining helped much in decreasing the total time taken, but is insufficient in explaining why the "unrolled" loop should run 4x faster with O1 flag.

