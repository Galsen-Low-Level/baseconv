/* @file  bcvrepl.c
 * @brief  base convertion  with interactive  repl  
 * @author Umar Ba <jUmarB@protonmail.com>  <github/Jukoo> 
 */ 

#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<ctype.h>
#include <errno.h> 

#include"bcvrepl.h" 
#include "baseconv.h"


void bcrepl_shell(const char *  prompt)
{
   const char * prmpt =  !prompt ? pname :  prompt ;  
   int proceed =1 ; 
   char prompt_buffer[bcrepl_buffer_limit]={0}; 
   int  line = 0; 
   fprintf(stdout , "%s" , BCV_STARTUP_MESG) ; 
   while (proceed, ++line) 
   { 
     printf(bpf, line,  prmpt , bcrepl_symbole_prompt) ;  
     if(!(fgets(prompt_buffer ,  bcrepl_buffer_limit, stdin)))
       continue ; 
    
     char *n  = strchr(prompt_buffer , 0xa) ;
     if (n)  *n  = 0;    
     
     bcrepl_compute(prompt_buffer) ; 
     /* NOTICE :always clean the repl buffer */
     bzero(prompt_buffer , bcrepl_buffer_limit) ; 

     fflush(stdin) ; 
   }
} 


void bcrepl_compute(const char * buffer) 
{

  bcrepl_listen_special_cmd(buffer) ; 
  struct bcv_parse_t {
     char  _sym_instruction:8;  
     int _value ; 
  }  bcv_scaner = { 0,0 };  

  char *buffer_clone =  (char *) buffer, 
       *token = __nptr  ; 
  int  scan_limite= 2 ; 
  while( __nptr != (token = strtok(buffer_clone , "/" ))) 
  {
    if(scan_limite & 0b10) 
    {
       bcv_scaner._sym_instruction= (char) *token  & 0xff ;  
       buffer_clone = __nptr; 
    }
    if(scan_limite & 0b01)
    {
      bcv_scaner._value = strtol(token , __nptr  , 0xa) ;  
      if(errno!= 0)
      {
         fprintf(stderr , "%s \n",  strerror(*__errno_location())) ; 
         bcv_scaner._value =  0; 
      }
    }

    if(scan_limite ==0) break ;  
    scan_limite+=~(scan_limite^scan_limite)  ; 
  }


   printf(" |-> "); 
   switch(bcv_scaner._sym_instruction & 0xff ) 
   {
      case 'x' : 
         bc_hex(bcv_scaner._value); break ; 
      case 'b': 
         bc_binv2(bcv_scaner._value, 1); break ; 
      case 'o': 
         bc_oct(bcv_scaner._value); break ; 
      default : 
         fprintf(stderr ,"Unknow operation see -h to print the usage\n") ;
         break ; 
   }
} 

void bcrepl_listen_special_cmd(const char * buffer) 
{
   char * cmd =  (char *) buffer;  
   __trimlower(cmd) ; 
    
   if(0 == strcmp(cmd, "quit") || \
      0 == strcmp(cmd, "exit")) 
     exit(0) ; 

}

static void __trimlower(char* bcv_cmd) 
{
  int len  = strlen(bcv_cmd) , i =~0, j=0 ;  
  char auto_format_cmd[0x14] ={0}; 

  while( ++i < len )  
  {
     if(isspace(*(bcv_cmd+i) & 0xff)) 
       continue ; 
      
    *(auto_format_cmd+j++) = tolower(*(bcv_cmd+i)) ;  
  }

  bzero(bcv_cmd, len) ; 
  memcpy(bcv_cmd , auto_format_cmd ,  j) ; 
}
