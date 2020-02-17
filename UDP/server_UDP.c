#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>
#include <arpa/inet.h>

#define SERWER_PORT 4321
#define SERWER_IP "192.168.43.159"


#define SA struct sockaddr
#define REF_TIME 1900




static void s_daemon()
{
	pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
      exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/home/bronsun/Pulpit/PS/IOT/Server");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    /* Open the log file */
    openlog ("firstdaemon", LOG_PID, LOG_DAEMON);

}


int main()
{
	/*Starting daemon process*/
	s_daemon();
	while(1)
	{
    struct sockaddr_in serwer =
    {
        .sin_family = AF_INET, // Set IPV4 
        .sin_port = htons( SERWER_PORT ) // Set Port
    };

	
	/*Conversion IP address to binary*/
    if( inet_pton( AF_INET, SERWER_IP, & serwer.sin_addr ) <= 0 )
    {
        perror( "inet_pton() ERROR" );
        exit( 1 );
    }
	/*Setting socket*/
    const int socket_ = socket( AF_INET, SOCK_DGRAM, 0 );
    if(( socket_ ) < 0 )
    {
        perror( "socket() ERROR" );
        exit( 2 );
    }
	/*Collect data from client (DHT11 Sensor)*/
    char buffer[ 4096 ] = { };
	/*Bind socket*/
    socklen_t len = sizeof( serwer );
    if( bind( socket_,( struct sockaddr * ) & serwer, len ) < 0 )
    {
        perror( "bind() ERROR" );
        exit( 3 );
    }
	/*Creating and saving data from sensor to .csv file*/
	FILE *f = fopen("Logs_Sensor.csv","a");
	
	if (f == NULL)
	{
		printf("ERROR\n");
		exit(1);
	}	

	fprintf(f, "     Wilgotność           Temperatura        Czas \n");
	
	fclose(f);

    while( 1 )
	
    {
	/*Function which is showing actual time*/
	time_t current_time = time(NULL);
		struct tm *local_time;
		local_time = localtime(&current_time);

		int hh = local_time->tm_hour;
		int mm = local_time->tm_min;
		int ss = local_time->tm_sec;
		int yyyy = REF_TIME + local_time->tm_year;
		int month = local_time->tm_mon + 1;
		int month_day = local_time->tm_mday;
		
		time_t time_new, time_old;
		time_new = current_time;





        struct sockaddr_in client = { };
	/*Clearing buffer*/
        memset( buffer, 0, sizeof( buffer ) );
	/*Waiting for connection and start reciving data*/ 
	int connfd=recvfrom( socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) & client, & len );
        //printf( "Waiting for connection...\n" );
        if(connfd < 0 )
        {
            perror( "recvfrom() ERROR" );
            exit( 4 );
	
        }
      
           printf( "Dane:%s \n ", buffer);
        FILE *f = fopen("Logs_Sensor.csv","a");
	
       
	if (f == NULL)
	{
		printf("ERROR\n");
		exit(1);
	}	
	fprintf(f, "%s            %04d:%02d:%02d | %02d:%02d:%02d\n",buffer,hh, mm, ss,month_day, month, yyyy);
	
	fclose(f);
}

    /*Closing socket and Daemon process*/

    shutdown( socket_, SHUT_RDWR );
	syslog(LOG_NOTICE,"First deamon started");
	sleep(10);
	break;

}
	syslog(LOG_NOTICE,"First termianted");
	closelog();
	return EXIT_SUCCESS;
}
