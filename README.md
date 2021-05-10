# many-gf2n-cpp
Various implementations of GF(2^n), done in c++17. Intended as a practice (or something similar to a lab session)

## Objective

* ~~Build a "vanilla" GF(2^n) module.~~
* ~~Use templates, constexpr & various metaprogramming techniques to pre-calculate exponents and logs in compile-time~~
  * the way array< T,n > is constructed ensures we have initialized it in compile-time
  * but is there a way to deassemble this and see for ourselves?
* ~~make sure that we will not compile if n > 16.~~
* ~~Find a way to use a *constexpr if* to change `Rep` depending on the length of genpoly~~
* Find a way to check if genpoly is actually irreducible, and print a warning if it isn't. (i.e. if this "field" isn't actually a field)
* Come up with a way to *accurately* measure performances
  * define *accurate*
* ~~Wrap each in namespace~~
* ~~Write code to measure performance~~
* Compare & suggest a plausible explanation for all observed phonomena.


## Experiment Set 1 (make 20M pairs, multiply and add to `sum`)

* In all measurements, the generating polynomial is 0x11d, and gen(if needed) is 0x02
* in `4_refactored.cpp`, only the exp and log tables(relative to the generator element 0x02) are calculated in compile time & cached.
* in `5_refactored_allcaches.cpp`, the multiplication and division tables are calculated in compile time & cached.
* the file starting with `4_1_` measures performance of `4_refactored.cpp`
* the file starting with `5_1_` measures performance of `5_refactored.allcaches.cpp`
* the results are as follows:

![41and51]

[41and51]: https://github.com/stet-stet/many-gf2n-cpp/blob/main/41and51.png?raw=true

For each entry, timespans are measured in microseconds. 
The addition results are printed because calculation is sometimes omitted entirely with `-O1` or `-O2` flags (if we choose not to print this).

### Comment

* I fail to see how 20,000,000 GF2 multiplications+additions can be faster than 20,000,000 int multiplication+additions (with `5_` file and `-O2` flag on)
  * int multiplications are handled by one instruction, addition by another.
  * GF2 multiplications are handled by at least one memory access, addition with at least one more.
  * why & how is memory access faster than an instruction?
  * have I missed something? is something being pipelined in that, and not in this?
  * or have I messed up (possibly by using std::vector<pair< T,T >>)?

## Experiment Set 2 (multiply 20M numbers to `sum`)

* relevant: `4_3_betterbenchmark.cpp` and `5_3_betterbenchmark.cpp` (and the `4_2` and `5_2` files)
* `DoNotOptimize` combats calculation elision
  * this function allocates the value provided to a register
  * this is an *unobservable* event, apparently - hence any alteration in the value provided should take place between two calls.
  * see [this answer at stackoverflow](https://stackoverflow.com/a/38025837)
* convention is same as above.
* Also, I tried to manually "unroll the loop", because `5_2` with O2 ran much slower than expected.
  * See the code to see how I did this
* the results are as follows:

![43and53]

[43and53]: https://github.com/stet-stet/many-gf2n-cpp/blob/main/43and53.png?raw=true

### Comment

* Dramatic change when O0->O1.
  * currently looking at [this](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html) to see what's responsible for the 4x change in speed
  * promising candidate is: -fforward-propagate - four memory accesses to one instruction?? is that even possible, even with SSE or AVX?


