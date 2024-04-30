char *line;
char mline[4];
int eofstatus;

doasm()
{
	asmstart();
	while (TRUE) {
		line = mline;
		doinline();
		if (match("#endasm") || match("#ENDASM") || eofstatus)
			break;
		asmline(line);
    }
	asmfinish();
	kill();
}
