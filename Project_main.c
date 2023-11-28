/*
Name          : SURYA S
Date          : 27.08.2023
Project Title : Steganography
 */

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>


/* Passing arguments through command line arguments */
int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo; 
    uint img_size;
    if(argc>=3)
    {
	/*Function call for check operation type */
	if(check_operation_type(argv)==e_encode)
	{
	    /*// Read and validate encode arguments */
	    if(read_and_validate_encode_args(argv,&encInfo)== e_success)
	    {
		/*Function call for encoding*/
		if(do_encoding(&encInfo) == e_success)
		    printf("Encoding successful\n");
		else
		    printf("ERROR function is failed\n");
	    }
	}
	else if(check_operation_type(argv)==e_decode)
	{
	    /*Read and validate decode arguments */
	    if(read_and_validate_decode_args(argv,&decInfo)== e_success)
	    {
		/*Function call for do decoding */
		if(do_decoding(&decInfo) == e_success)
		    printf("Decoding successful\n");
		else
		    printf("ERROR function is failed\n");
	    }
	} 
    }
    else
	printf("Invalid option\n");
}
OperationType check_operation_type(char *argv[])                             /* Function definition for check operation type */
{
    if(strcmp(argv[1],"-e")==0)
	return e_encode;
    else if(strcmp(argv[1],"-d")==0)
	return e_decode;
    else
	return e_unsupported;
}
