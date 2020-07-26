memset(p, c, n)
register unsigned char *p;
register unsigned char c;
unsigned n;
{
	while(n--)
		*p++ = c;
}
