Readme file for Small-C Compiler version 2.1B for TRS-80 Model I/III running Newdos/80 2.0

Emulator used: TRS80GP in Model I mode. Very complete but frequent crashes.
<br>
trstools to transfer files between windows and TRS-80 disks
<br>
Editor used: Scripsit V1.0 (patched for Newdos/80)
<br>
Other tools: Microsoft M80 and L80 for TRSDOS

Note:
	CCC50 run-time support added requires an updated cclib/rel.
 
Compiler source Files:
<br>
cc/def	(remove #define MSC when compiling on a TRS-80)
<br>
ccint/h
<br>
cc0/c
<br>
cc01/c
<br>
cc1/c
<br>
cc11/c
<br>
cc12/c
<br>
cc2/c
<br>
cc21/c
<br>
cc22/c
<br>
cc3/c
<br>
cc31/c
<br>
cc32/c
<br>
cc33/c
<br>
cc4/c
<br>
cc41/c
<br>
cc42/c
<br>

When compiling on a TRS-80

iolibcc/mac

When cross compiling on windows using Visual C (version 2022)

stdio.h
stdlib.h
string.h

Work environment (floppy system). Area between 5200H and 0FFFFH available.

disk0 contains cmd files
<br>
disk1 contains compiler sources
<br>
disk2 is used in self-compilation
<br>
disk3 contains sample and utility files
<br>
Building the compiler (on a TRS-80)

Compile Small-C sources

cc cc0:1
<br>
cc cc1:1
<br>
cc cc2:1
<br>
cc cc3:1
<br>
cc cc4:1
<br>

Assemble generated /mac files and custom library

m80 cc0:1=cc0
<br>
m80 cc1:1=cc1
<br>
m80 cc2:1=cc2
<br>
m80 cc3:1=cc3
<br>
m80 cc4:1=cc4
<br>
m80 iolibcc:1=iolibcc
<br>

l80 cc0:1,cc1:1,cc2:1,cc3:1,cc4:1,iolibcc:1,ccnew/cmd:0-n-e

This creates a new compiler called ccnew.cmd


Cross compiling on Windows works only as a WIN32 app since int and pointers are mixed.
Make sure to add #define MSC to the beginning of file cc.def.
Output is a .mac file which then needs to be transferred to the TRS-80 (use trstools app)
since there is no windows hosted version of M80 and L80 available.
There is a project running the CP/M versions of M80 and L80 on a PC
(https://github.com/Konamiman/M80dotNet) but it won't generate TRS-80 /cmd files.

Cross compiling will result in many warnings where ints, unsigned ints, chars and
pointers are mixed. Small-C does not support casts so there is no way to remove
the warnings while maintaining a single source code base.



Sample programs:
<br>
	qsort/c
<br>
	grdemo/c
<br>
	real/c
<br>
	sieve/c
<br>
	fprint/c
<br>
	hilbert/c
<br>
	life/c

Utility programs:
<br>
comptxt/c
<br>
compbin/c
<br>
layout/c
<br>
makelib/c
<br>
dumpcmd/c


Work Flow:
<br>
CC hello/c:n		=> hello/mac:n
<br>
M80 hello:n=hello:n	=> hello/rel:n
<br>
L80 hello:n,hello:n-n-e	=> hello/cmd:n

C-library

Source code for C library on TRS-80 available, mostly written in Z-80 assembler.
Source files have no comments, presumably to save disk space!
<br>
ccmain/mac
<br>
iolib1/mac
<br>
iolib2/mac
<br>
iolib3/mac
<br>
calloc/c
<br>
malloc/mac
<br>
printf/c
<br>
atoi/c
<br>
dtoi/c
<br>
itoa/c
<br>
itoab/c
<br>
reverse/c
<br>
strlen/c
<br>
scanf/c
<br>
utoi/c
<br>
ctype/c
<br>
getarg/c
<br>
string1/c
<br>
string2/c
<br>
string3/c
<br>
floatsim/mac
<br>
circle/c
<br>
romlib1/mac
<br>
romlib2/mac
<br>
romlib3/mac


the C library file is cclib/rel. There is a utility, makelib, to build this library using
the chain file cclib/jcl. Compile and assemble the sources first. The source code makelib/c
is included.
<br>
A dummy library called forlib/rel is also included since L80 will look for this library
if there are unresolved externals.
<br>
<br>
See cclib/jcl for all modules in the library cclib/rel.
<br>

Note: the trig functions in floatsim/mac give the wrong answers. Needs debugging.

Issues with peepholes where HL does not contain the expression anymore while HL is tested later.
Conditioned as "BADPEEP". They will work most of the time but not all the time so these
peepholes are currently disabled until this issue is resolved.

Using malloc() is very expensive in terms of code size. Almost all malloc() in the compiler are one time allocations, never freed. There is a calloc() in the code generator which frees the allocated buffer almost instantly. The issue is fopen() used for up to 3 input files, 1 output file and 1 error messages file plus potential
command line redirection files. The library function malloc() has been replaced with a local, simplified, version to save code space.

The code area before main(), string "ENDINIT" is used as the stagebuffer in the TRS-80 hosted compiler (STAGESIZE bytes, currently 1835 bytes). This stagebuffer is used to buffer intermediate code for each function, which is then used for peephole optimizations and code generation for each function. This limits the size of each function allowed.

IOLIBCC.MAC is a customized file I/O library. It removes unused code.

TRS-80 keyboard limitations:
<br>
	{		-> begin
<br>
	}		-> end
<br>
	[		-> (.
<br>
	]		-> .)
<br>
	#define or ||
<br>
	#define bitor |
<br>
	#define bitneg ~
<br>
	#define biteor ^
<br>
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
compbin/cmd		Compare 2 binary files for equality
comptxt/cmd		Compare 2 text files (naively)
makelib/cmd		Create library cclib/rel.


General observations:
Amazingly difficult to work on a TRS-80 (emulated) whan used to conveniences of Windows.
Specially text editing very cumbersome. I've been using Scripsit since didn't want to deal
with the linenumbers based editors. Emulator keyboard made this even more difficult.
Debugging another nightmare. Memory limitations next problem. Initially unable to use Newdos/80 V2.5
since that Newdos version reserves part of the High Memory (himem is F8FF under Newdos/80 v2.5), which doesn't leave enough memory
for the self-compilation process. The compiler memory requirements have been reduced so self-compilation now works with Himem=F800H.
However, the compiler uses a lot of stack space when parsing nested expressions and will run of of memory quickly with Himem == F800H. This can be avoided by using the -D or -S compile options.


