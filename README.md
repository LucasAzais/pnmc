pnmc
====

A small Petri net model checker using Hierarchical Set Decision Diagrams.

To use the matrix algorithm, go to pnmc/build, use the cmake command :

cmake .. -DBOOST_ROOT=/home/elzeiz/usr -DNO_COROUTINE=1 -DCMAKE_BUILD_TYPE=Debug

Then use make, the executable will be added in pnmc/build
