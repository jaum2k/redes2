#ifndef _GARBLER_H_
#define _GARBLER_H_
int set_garbler(int L,int C,int D);
/*
L, C, D are values between 0 and 100, inclusively. 
L: loss rate
C: corrupt rate
D: duplicate rate

Return value:
1: success
-1: Invalid value of loss fraction
-2: Invalid value of corruption fraction
-3: Invalid value of duplication fraction
-4: Sum of L,C,D cannot exceed 100
*/

ssize_t sendto_garbled(int s, const void *msg, size_t len,int flags,const struct sockaddr *to,int tolen);
/*
The parameters are the same as those in standard sendto() function
*/

#endif
