/*
** readcmd
** Dump the record information for a TRS-80 /CMD
** executable file
**
** License: MIT / X11
** Copyright (c) 2009, 2015, 2023 by James K. Lawless
** See license at https://github.com/jimlawless/readcmd/blob/main/LICENSE
** Source: https://github.com/jimlawless/readcmd
** https://jimlawless.net
*/
 
#include <stdio/h>
 
main(argc, argv) int argc;  char* argv[];
{
    FILE *fp;
    char buff[258];
    int len;
    int address;
    printf("readcmd v1.20 by Jim Lawless\n");
    printf("https://jimlawless.net/posts/trs80-cmd/\n\n");
    fp=fopen(argv[1],"r");
    if(fp==NULL) {
        fprintf(stderr,"Cannot open file %s\n",argv[1]);
       exit(1);
    }
    for(;;) {
		/*
		** fread() should return number of items read but
		** small-C library returns 0
        if(!fread(buff,1,1,fp))
            break;
		*/
		fread(buff,1,1,fp);
		/* need to test fp->SLRSLT for error */
            /* record type is "load block" */
        if(*buff==1) {
            fread(buff,1,1,fp);
            len=*buff;
                /* compensate for special values 0,1, and 2.*/
            if(len<3)
                len+=256;
                /* read 16-bit load-address */
            fread(&address,1,2,fp);
            printf("Reading 01 block, addr %x, length = %u.\n",address,len-2);
            fread(buff,1,len-2,fp);
        }
        else {
            /* record type is "entry address" */
			if(*buff==2) {
				fread(buff,1,1,fp);
				len=*buff;
				printf("Reading 02 block length = %u.\n",len);
				fread(&address,1,len,fp);
				printf("Entry point is %d %x\n",address,address);
				break;
			}
			else {
				/* record type is "load module header" */
				if(*buff==5) {
					fread(buff,1,1,fp);
					len=*buff;
					printf("Reading 05 block length = %u.\n",len);
					fread(buff,1,len,fp);
				}
				else {
					printf("Unknown code %u at %lx\n",*buff,ftell(fp)-1);
					break;
				}
			}
		}
    }
    fclose(fp);
}
 