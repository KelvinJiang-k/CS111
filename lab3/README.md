# Hash Hash Hash
Demo of 3 hash tables: a basic hash table, a correct hash table through locking, a performant correct hash table with optimized locking

## Building
```shell
make
```

## Running
```shell
./hash-table-tester -t 3 -s 150000

Generation: 77,681 usec
Hash table base: 1,543,140 usec
  - 0 missing
Hash table v1: 2,721,249 usec
  - 0 missing
Hash table v2: 617,324 usec
  - 0 missing

  - 0 missing
```

## First Implementation
In the `hash_table_v1_add_entry` function, I implemented a lock in the very beginning of the add_entry code and unlocked at the very end.
To do so, I had to add a lock property to the hash_table_v2 structure to prevent all access to the hash table if it was acquired.

### Performance
```shell
./hash-table-tester -t 3 -s 150000

Generation: 77,681 usec
Hash table base: 1,543,140 usec
  - 0 missing
Hash table v1: 2,721,249 usec
  - 0 missing
Hash table v2: 617,324 usec
  - 0 missing

```
Version 1 is a little slower than the base version. As implementing a lock requires thread and code overhead that may slow down the program.

## Second Implementation
In the `hash_table_v2_add_entry` function, I chose to add a lock to each entry of the hash table instead, as there was no need to lock the
entire hash table when I was just accessing one entry. I implemented a lock right after I obtained the hash_table_entry and unlocked it
after operations done on the list of the hash_table_entry.

### Performance
```shell
./hash-table-tester -t 3 -s 150000

Generation: 77,681 usec
Hash table base: 1,543,140 usec
  - 0 missing
Hash table v1: 2,721,249 usec
  - 0 missing
Hash table v2: 617,324 usec
  - 0 missing

```
UCLA lnxsrv11 servers only have 4 cores:

./hash-table-tester -t 2 -s 150000

Generation: 54,767 usec
Hash table base: 707,014 usec
  - 0 missing
Hash table v1: 1,053,968 usec
  - 0 missing
Hash table v2: 410,368 usec
  - 0 missing

Generation: 52,045 usec
Hash table base: 545,828 usec
  - 0 missing
Hash table v1: 899,455 usec
  - 0 missing
Hash table v2: 358,576 usec
  - 0 missing

Generation: 52,552 usec
Hash table base: 596,858 usec
  - 0 missing
Hash table v1: 987,285 usec
  - 0 missing
Hash table v2: 334,040 usec
  - 0 missing

On average in 3 tests, (707014 + 545828 + 596858) / (410368 + 358576 + 334040) = v2 is 1.68 times faster than base


./hash-table-tester -t 3 -s 150000

Generation: 77,681 usec
Hash table base: 1,543,140 usec
  - 0 missing
Hash table v1: 2,721,249 usec
  - 0 missing
Hash table v2: 617,324 usec
  - 0 missing

Generation: 78,884 usec
Hash table base: 1,955,058 usec
  - 0 missing
Hash table v1: 3,581,172 usec
  - 0 missing
Hash table v2: 722,612 usec
  - 0 missing

Generation: 79,471 usec
Hash table base: 1,699,032 usec
  - 0 missing
Hash table v1: 2,991,104 usec
  - 0 missing
Hash table v2: 725,170 usec
  - 0 missing

On average in 3 tests, (1543140 + 1955058 + 1699032) / (617324 + 722612 + 725170) = v2 is 2.52 times faster than base


./hash-table-tester -t 4 -s 150000

Generation: 120,239 usec
Hash table base: 3,572,358 usec
  - 0 missing
Hash table v1: 5,991,326 usec
  - 0 missing
Hash table v2: 988,383 usec
  - 0 missing

Generation: 104,916 usec
Hash table base: 4,215,398 usec
  - 0 missing
Hash table v1: 6,798,501 usec
  - 0 missing
Hash table v2: 1,119,387 usec
  - 0 missing

Generation: 103,717 usec
Hash table base: 3,456,064 usec
  - 0 missing
Hash table v1: 5,445,791 usec
  - 0 missing
Hash table v2: 964,825 usec
  - 0 missing

On average in 3 tests, (3572358 + 4215398 + 3456064) / (988383 + 1119387 + 964825) = v2 is 3.66 times faster than base

## Cleaning up
```shell
make clean
```