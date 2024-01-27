# cop4520PA1
This was in theory, a lockless sieve of eratosthenes, however it's my first time using C++. 

The general idea was to allocate an array of booleans, which is standard for this sieve, have the threads work on individual areas of the array, which eliminates race conditions since writing to seperate areas doesn't effect the other areas. it's like having some n coffee machines manned by some t number of threads. this eliminates the need for a lock, since they should not interfere with eachother. There were 3 sections I identified that could be parallelized, the setting of every value in the boolean array to true, the filtering stage where you remove composites, and the final counting and summing area. All  of those areas are ones that can be done lockless, by using the same coffee machine allegory above. Didn't seem to work that way though sadly.

# running
compile with g++, no particular options, run ./executable_name num outfile
