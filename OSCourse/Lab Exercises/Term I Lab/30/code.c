#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

int  init_daemon ( void ) {
	if ( ! fork ( ) ) {  	
		setsid ( ) ;
		chdir ( " / " ) ;
		umask ( 0 ) ;
        /* Specify Your Job  */
        int ret = 1;
        char buf[80];
        struct tm* ts = (struct tm*) calloc(1, sizeof(struct tm));
        long int* t = (long int*) calloc(1, sizeof(long int));
        printf("This program executes a specific script after 3 seconds!\n");
        long int to_exec = time(t) + 3;
        ts = localtime(t);

        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        printf("Current time: %s\n", buf);
        while(1)
        {
            if(time(t) == to_exec)
            {
                ts = localtime(t);
                strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
                printf("Current time: %s\n", buf);
                ret = execve("1.sh",NULL,NULL);
            }
        }
		return ( 0 );
	}
	else 
		exit ( 0 );	
}

int main()
{
    init_daemon();
    return 0;
}

