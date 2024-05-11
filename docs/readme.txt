Small-C V2.1B for TRS-80 Model I running Newdos/80 V2

Enhanced features:

	. function arguments are pushed in the K&R C order, i.e.
	  last argument is pushed first. This allows variable
	  arguments.
	. type "double" available for declarations only. variables
	  of this type can be used as arguments for the floatsim
	  library; the address of the variable is passed.
	. more support for pointers to functions.
	. use "(." for "[", ".)" for "]", "begin" for "{",
      "end" for "}" when entering C programs on a TRS-80
	. use the compiler option "-@" to use the "@" character
	  in stead of "\".
	  

Compiler will run and self-compile with HIMEM 63488 (0xF800).
Complex nested expressions are parsed using recursive descent
operations which require a lot of stack space. Use the
command line options -D# and -S# to reduce the memory
requirements of the compiler. Defaults are:

	. -S200 (200 symbols)
	. -D215 (215 macros)

The compiler sources will compile using Visual Studio 2022 but
the file cc.def must include the line #define MSC at the beginning.
This resulting cross compiler will generate .mac files from .c files

Version 2.1A of this project was originally released in 1984 when I
still lived in The Netherlands. Most of the documentation from that
time was written in Dutch.

