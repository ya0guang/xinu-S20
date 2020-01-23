/* xsh_hello.c - xsh_hello */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_hello - Echo one <string> and displays welcome message
 *------------------------------------------------------------------------
 */
shellcmd xsh_date(int nargs, char *args[])
{


    /* Output info for '--help' argument */

    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s <string>\n\n", args[0]);
        printf("Description:\n");
        printf("\tEcho the <string> and displays welcome message\n");
        return 0;
    }

    /* Check argument count */

    if (nargs != 2)
    {
        if( nargs < 2)
        {
            fprintf(stderr, "%s: too few arguments\n", args[0]);
        }
        else
        {
            fprintf(stderr, "%s: too many arguments\n", args[0]);
        }
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    /* Normal execution */

    if (nargs == 2)
    {
        printf("Hello %s, Welcome to the world of Xinu", args[1]);
    }

}
