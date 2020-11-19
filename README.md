deep-instinct home assignment

this repo contains the code for the lkm_sort kernel module
this is a __super__ naiiver module that supports sorting data
under the following constrints:
1. data sent should only be char [or equivalent one byte sized]
2. amount of data in buffer should never exceed: MAX_MEM_ALLOC
   which is defaultly set to 1Mb

TODO:
1. currently the module handles state, as the intention was to
   support reading and writing in chunks [to allow performance
   considerations when using the module].
   this raises many issues with regard to concurrent use of the
   modules.
2. the sorting code that is used in the module is __super__ simplistic.
   its a naiive quick sort that uses the get_random_bytes linux 
   facility for its antropy, with no clever optimizations.
   this is __not__ what the doctor ordered, as there are some really
   good implementations out there to be used, instead.

usage:
open a shell, and run:
1. ./make [this uses the Makefile recipe to build all needed products]
2. ./deploy-lkm_sort.sh [deploys the module, and creates the special device under /dev]
3. ./main [this runs the usermod code. note that the buffer is hard coded for now ... 
           also, its using a simple output stream to output the result]

