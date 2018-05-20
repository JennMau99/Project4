/* Jennifer Maunder and Richa Gadgil
 * CPE 357-03
 * Project 4
 */

/* We need to add all the functions to header.h */
#include <stdio.h>
#include <stdlib.h>

#include "header.h"



int main(int argc, char *argv[])
{
	int options[6];

	if (argc < 3)
	{
		fprintf(stderr, "Format: ./mytar options file1 file2 ...\n");
		return -1;
	}
	
	/* check takes a six long int array and the *argv that points to the options */
	if (check(options, argv[1]) < 0)
	{
		return -1;
	}
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

	/* create takes int argc and char **argv, same as main */
	/* I don't think this checks for whether the file/dir names are valid, 
     * should we add that here or in that file? 
     * I'm thinking it should go in create since that already returns 0 if success. */
	if (options[0] == 1)
	{
		if (create(argc, argv) < 0)
		{
			fprintf(stderr, "Nonexistent files given to create");
			return -1;
		}
	}
	/* list takes int argc and char **argv, same as main */
	/* Same as create, we should probably add a "return -1 if failed" in there. */
	if (options[1] == 1)
	{
		if (list(argc, argv) < 0)
		{
			fprintf(stderr, "Nonexistent files given to list");
			return -1;
		}
	}

	/* extract takes the argv of tar and an int indicating verbose (1 = true, 0 = false) */	
	if (options[2] == 1)
	{
		if (extract(argv[2], options[3]) < 0)
			return -1;
	}


	return 0;
}
