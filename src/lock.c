#include "hypermail.h"
#include "setup.h"

#define LOCKBASE       ".hypermail.lock"

int i_locked_it = 0;

void lock_archive(char *dir)
{
    FILE *fp;
    char buffer[MAXLINE];

    int count = 0;		/* # minutes waited */
    i_locked_it = 0;		/* guilty until proven innocent */

    sprintf(lockfile, "%s/%s", dir, LOCKBASE);

    while ((fp = fopen(lockfile, "r")) != NULL) {
	fgets(buffer, sizeof(buffer), fp);
	fclose(fp);
	/*  
         * "set_locktime" is the config file item named 'locktime', 
         * default is 3600 seconds 
         */
	if (time(NULL) > (atol(buffer) + set_locktime))
	    break;		/* lock over hour old - break it */

	++count;
	if (set_showprogress)
	    fprintf(stderr, "Waiting for lock (file '%s')\n", lockfile);
	sleep(30);
    }

    if ((fp = fopen(lockfile, "w")) != NULL) {
	i_locked_it = 1;
	fprintf(fp, "%ld\n", time(NULL));
	fclose(fp);
    }
    else if (dir[0]) {
	sprintf(errmsg, "Couldn't create lock file \"%s\".", lockfile);
	progerr(errmsg);
    }
}

void unlock_archive(void)
{
    if (lockfile && i_locked_it)
	remove(lockfile);
    lockfile[0] = '\0';
}