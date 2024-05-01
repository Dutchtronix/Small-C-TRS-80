/*
** ccint.h
*/

#ifdef MSC

typedef unsigned char 	uint8_t;
typedef   signed char 	int8_t;
typedef unsigned short 	uint_t;
typedef unsigned long 	ulong_t;
typedef unsigned short	uint16_t;
typedef 		 short 	int16_t;
typedef 		 long	int32_t;

typedef uint8_t byte;
typedef uint8_t BOOL;
#endif

/*
** stdio/h -- header for standard i/o library
*/
#define ERR -2
#define FALSE 0
#define TRUE 1
#define CR 13
#define LF 10

#ifdef MSC
#undef EOF
#define EOF (FILE *)-1
#else
#define EOF -1
#define NULL 0

#define int16_t int
#define void  
#define FILE int

extern FILE stdin[], stdout[], stderr[];
#endif

#define CCALLOC ccalloc
