#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

/* GLOBAL DEFINITIONS */

int GBLR_loss_fraction = 0;
int GBLR_corruption_fraction = 0;
int GBLR_duplication_fraction = 0;


/* ALL DEFAULTS SET TO ZERO */

int set_garbler(int L,int C,int D)
{
  if((L <0) || (L>100)) { 
      /*printf("Invalid value of loss fraction\n"); */
      return -1;
  }
  
  if((C<0) || (C>100)) {
      /* printf("Invalid value of corruption fraction\n"); */
      return -2;
  }
  
  if((D<0) || (D>100)) { 
      /* printf("Invalid value of duplication fraction\n"); */
      return -3;
  }
  
  if(L+C+D>100) {
      /* printf("Sum of L,C,D cannot exceed 100\n"); */
      return -4;
  }  
  

  GBLR_loss_fraction = L;
  GBLR_corruption_fraction = C;
  GBLR_duplication_fraction = D;
  
/*printf("Values successfully updated\n");*/
  return 1;
}

/* garbled version of sendto..accepting the same parameters */
ssize_t sendto_garbled(int s, const void *msg, size_t len,int flags,const struct sockaddr *to,int tolen) {
    struct timeval current_time;
    int randomvalue;
    int nbytes;
    void * temp_buffer;
    int error_pos;
    char error_value;
     
    gettimeofday(&current_time,NULL);
    srand(current_time.tv_usec);
    randomvalue = rand();
    randomvalue = randomvalue % 100;  
    
    if(randomvalue<GBLR_loss_fraction) {  /* packet is lost --- do nothing */
	return(len);
    }
    
    if (randomvalue<GBLR_loss_fraction + GBLR_corruption_fraction)  { /* corrupt the packet */
	/*working on a temp buffer rather than then orignal msg */
	temp_buffer = malloc((size_t) len);  
	memcpy(temp_buffer, msg, (size_t) len);
	
	/*set a random byte in the temp buffer to a random value */
	gettimeofday(&current_time,NULL);

	randomvalue = rand();
	error_pos = randomvalue % len;  
	error_value = (char)(randomvalue % 256); 
	//printf("%d %c \n", error_pos, error_value);
	((char *) temp_buffer)[error_pos] = error_value;
	
	/* call sendto with corrupted message */
	nbytes = sendto(s,temp_buffer,len,flags,to,tolen);
	
	/*release temp buffer */
	free(temp_buffer);
	return(nbytes);
    }
    
    if (randomvalue<GBLR_loss_fraction + GBLR_corruption_fraction + GBLR_duplication_fraction) { /* duplicate the packet */
	/* call sendto twice */
	nbytes = sendto(s,msg,len,flags,to,tolen);
	nbytes = sendto(s,msg,len,flags,to,tolen);
	return(nbytes);        
    }
    
    /* default: nothing wrong, call sendto as is.. */
    nbytes = sendto(s,msg,len,flags,to,tolen);
    return(nbytes);
}
