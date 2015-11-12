# IndriDumpMultiDocs
Dump multiple documents from Indri Index

You need Indri to ```make``` the binary

1. Compile

```make -f Makefile.app

2. Usage

    IndriDumpMultiDocs -index=... -docnofile=... [-outputdir=...]
    1. index - index path
    2. docnofile - input file that contains all the docno
    3. outputdir - outputdir, optional

