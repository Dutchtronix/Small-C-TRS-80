Readme file for Small-C Compiler version 2.1B for TRS-80 Model I/III running Newdos/80 2.0

Emulator used: TRS80GP in Model I mode. Very complete but frequent crashes.
Editor used: Scripsit V1.0 (patched for Newdos/80)
Other tools: Microsoft M80 and L80 for TRSDOS



General observations:
Amazingly difficult to work on a TRS-80 (emulated) whan used to conveniences of Windows.
Specially text editing very cumbersome. I've been using Scripsit since didn't want to deal
with the linenumbers based editors. Emulator keyboard made this even more difficult.
Debugging another nightmare. Memory limitations next problem. Unable to use Newdos/80 V2.5
since that version reserves part of the High Memory, which doesn't leave enough memory
for the self-compilation process.


Sample programs:
qsort/c
grdemo/c
realdemo/c
sieve/c
fprint/c
hilbert/c
life/c

Utility programs:
comptxt/c
compbin/c
layout/c
makelib/c
dumpcmd/c
real/c
life/c


Work Flow:
CC hello/c:n			=> hello/mac:n
M80 hello:n=hello:n		=> hello/rel:n
L80 hello:n,hello:n-n-e	=> hello/cmd:n

Compiler source Files:

cc/def
ccint/h
cc0/c
cc01/c
cc1/c
cc11/c
cc12/c
cc2/c
cc21/c
cc22/c
cc3/c
cc31/c
cc32/c
cc33/c
cc4/c
cc41/c
cc42/c

When cross compiling on windows using Visual C (version 2022)

stdio.h
stdlib.h
string.h

Cross compiling on Windows works as a WIN32 app. Make sure to add #define MSC to the file cc.def.
Output is a .mac file which then needs to be transferred to the TRS-80 (use trstools app)
since there is no windows hosted version of M80 and L80 available.
There is a project running the CP/M versions of M80 and L80 on a PC
(https://github.com/Konamiman/M80dotNet) but it won't generate TRS-80 /cmd files.

Cross compiling will result in many warnings where ints, unsigned ints, chars and
pointers are mixed. Small-C does not support casts so there is no way to remove
the warnings while maintaining a single source code base.

C-library

Source code for C library on TRS-80 available, mostly written in Z-80 assembler.
Source files have no comments, presumably to save disk space!

ccmain/mac
iolib1/mac
iolib2/mac
iolib3/mac
malloc/mac

the C library file is cclib/rel. There is a utility, makelib/c, to build this library.
A dummy library called forlib/rel is also included since L80 will look for this library
if there are unresolved externals.

There are also some library files allowing access to the Level II rom routines from small-c:
romlib1/mac
romlib2/mac
romlib3/mac
floatsim/mac

See cclib/jcl for all modules in the library cclib/rel.

Note: the trig functions in floatsim/mac give the wrong answers. Needs debugging.

This C Library was created around 1984 and has not been touched since.

Issues with peepholes where HL does not contain the expression anymore while HL is tested later.
Conditioned as "BADPEEP". They will work most of the time but not all the time so these
peepholes are currently disabled until this issue is resolved.

Using malloc() is very expensive in terms of code size. Almost all malloc() are one time allocations,
never freed. There is a malloc() in the peephole optimizer but could easily be replaced by a fixed
size buffer. The issue is fopen() used for up to 3 input files, 1 output file and 1 error messages file.
The output file is closed before the error file is opened if an error occurs. But the output file
is opened with just 1 input file open. If an error occurs if a second or third input file has
been opened (#include), the file buffers are not LIFO anymore. Malloc() will reuse the output file
buffer when opening the error file but an efficient LIFO scheme will not. Closing the input files
will also cause buffers to be non-LIFO if the error file is open. One solution is to open the
error file at start of compilation at the expense of the buffer size.
Could we store other data in the error file?

The code area before main(), string "ENDINIT" is used as the stagebuffer in the TRS-80 hosted
compiler (STAGESIZE bytes, currently 1835 bytes). This stagebuffer is used to buffer intermediate
code for each function, which is then used for peephole optimizations and code generation for
each function. This limits the size of each function allowed.

Tried IOLIBCC.MAC to customize the library but resulting /cmd file didn't work properly

TRS-80 keyboard limitations:
{		-> begin
}		-> end
[		-> (.
]		-> .)
#define or ||
#define bitor |
#define bitneg ~
#define biteor ^
\ use command line option -@

Tools:
The tools folders contains binaries that will run on a TRS-80 Model I running Newdos/80 V2.0.
List of files:
cc/cmd			Small-C compiler V2.1B. Converts Small-C source code to assembly files
				in M80 format.
m80/cmd			Microsoft's assembler for the TRS-80. Converts .mac files to .rel files
l80/cmd			Microsoft's linker for the TRS-80. Collects one or more .rel files and
				combines it with the C library file CCLIB/REL into a executable for
				the TRS-80 (/cmd extension)
sc/cmd			Scripsit for the TRS-80. Full screen editor. Clumsy user interface but still
				better than the various line editors available. Use the ESC key on an emulated
				TRS-80 to issue commands like L file, S,A file (save as Ascii). Exit by typing
				END after the ESC key.
				Documentation in the docs folder.
dumpcmd/cmd		Dumps the sections of a cmd file
