#include <stdio.h>
#include <string.h>
#include <unixio.h>

extern unsigned ovsize;
extern char *ovstart;
extern int ovbgn(int);

/* Overlay loader for Hi-Tech C */
ovloader(ovname,args)
char *ovname;

{
	int fd, size, err;
	char filename[17], *p;

	strcpy(filename,ovname);	/* Copy the filename */
	strcat(filename,".ovr");	/* add the extent */
	if ((fd = open(filename, 0)) < 0)
	{
		strcpy(filename,"0:A:");	/* not there -- see if it's on A0: */
		strcat(filename, (p = strchr(ovname,':')) ? p+1 : ovname);
		strcat(filename,".ovr");
		err = 1;
		if ((fd = open(filename,0)) < 0)
			goto error;		/* Not there either */
	}
	size = read(fd,ovstart,ovsize);
	close(fd);
	err = 2;
	if (size < 0)
		goto error;			/* read error */
	return ovbgn(args);		/* ok, execute the overlay */

error:
	printf("\nError %d in loading %s.ovr\n",err,ovname);
	return -1;
}