/* Jennifer Maunder and Richa Gadgil
 * CPE 357-03
 * Project 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECKLEN 6

int check(int *checkarray, char *argv)
{
/* Arguments to be checked: c, t, x, v, f, s
 * Indexes in check array:
 * 		c = 0
 * 		t = 1
 * 		x = 2
 * 		v = 3
 * 		f = 4
 * 		s = 5
 * If the index = 1, the option is set
 * If the index = 0, the option isn't set
 */

	int arglen;
	int i;

	/* Initalize all indexes of checkarray to 0 */
	for (i = 0; i < CHECKLEN; i++)
		checkarray[i] = 0;

	arglen = strlen(argv);
	
	/* Loops once per char in argv */
	for (i = 0; i < arglen; i++)
	{
		if (argv[i] == 'c')
			checkarray[0] = 1;
		else if (argv[i] == 't')
			checkarray[1] = 1;
		else if (argv[i] == 'x')
			checkarray[2] = 1;
		else if (argv[i] == 'v')
			checkarray[3] = 1;
		else if (argv[i] == 'f')
			checkarray[4] = 1;
		else if (argv[i] == 's')
			checkarray[5] = 1;
		/* If a char in argv isn't an option */
		else
		{
			fprintf(stderr, "%c is not a valid option\n", argv[i]);
			return -1;
		}
	}
	
	/* After reading in all the options, check to make
 	   sure that none of them contradict each other */
 	if ((argv[0] && argv[1]) || (argv[0] && argv[2])
		|| (argv[1] && argv[2]))
	{
		fprintf(stderr, "Only one of c, t, or x can be specified\n");
		return -1;
	}
	
	return 0;
}
