#! /bin/bash
# ./PPD --sync

export LD_LIBRARY_PATH=${MYGCC}lib64

# Debug/Oxyz3d.exe -load "Fichiers3D/toto.oxyz" $*
Debug/Oxyz3d.exe
# -load "vaisseau.oxyz" $*

# valgrind -v --tool=memcheck --leak-check=yes  --show-reachable=yes --error-limit=no --num-callers=50  --freelist-vol=20000000  --log-file=mem.err ./PPD --sync $*
