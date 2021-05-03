# many-gf2n-cpp
Various implementations of GF(2^n), done in c++17. Intended as a practice (or something similar to a lab session)

## Objective

* ~~Build a "vanilla" GF(2^n) module.~~
* ~~Use templates, constexpr & various metaprogramming techniques to pre-calculate exponents and logs in compile-time~~
** the way array< T,n > is constructed ensures we have initialized it in compile-time
** but is there a way to deassemble this and see for ourselves?
* ~~make sure that we will not compile if n > 16.~~
* Come up with a way to *accurately* measure performances
** define *accurate* -> number of cpu clocks.
* Wrap each in namespace
* Write code to measure performance
* Compare & suggest a plausible explanation for all observed phonomena.
