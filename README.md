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
 * define *accurate* -> number of cpu clocks.
* ~~Wrap each in namespace~~
* ~~Write code to measure performance~~
* Compare & suggest a plausible explanation for all observed phonomena.


## Preliminary measurements

* In all experiments, the generating polynomial is 0x11d, and gen(if needed) is 0x02
* in `4_refactored.cpp`, only the exp and log tables(relative to the generator element 0x02) are calculated in compile time & cached.
* in `5_refactored_allcaches.cpp`, the multiplication and division tables are calculated in compile time & cached.
* the file starting with `4_1_` prefix measures performance of `4_refactored.cpp`
* the file starting with `5_1_` prefix measures performance of `5_refactored.allcaches.cpp`
* the results are as follows:

![41and51]

[41and51]: https://github.com/

For each entry, timespans are measured in microseconds. 
The addition results are printed because calculation is sometimes omitted with -O1 or -O2 flags on if we choose not to print this.

### Comment

* I currently fail to see how 20,000,000 int multiplications+additions can be faster than 20,000,000 GF2Five::GF2<0x11d> multiplication+additions.
 * int multiplications are handled by one instruction, addition another.
 * GF2 multiplications are handled by at least one memory access, addition with at least one instruction
 * why & how is memory access faster than an int multiplication?
 * or have I messed up (possibly by using std::vector<pair< T,T >>)?

