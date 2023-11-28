#ifndef DECODE_H
#define DECODE_H
#include "types.h"



typedef struct _DecodeInfo
{
    /*source file Info */
    char *stego_img_name;
    FILE *fptr_stego_img;
    int secret_file_extension_size;
    int size_of_secret_file;
    char *secret_file_extension;
    char *magic_str;


    /* Output file Info */
    char *output_file_name;
    FILE *fptr_output_file;

} DecodeInfo;

/* Decoding function prototype */



/* Check operation type */ 
OperationType check_operation_type(char *argv[]);     

/* Read and validate Decode args from argv */ 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo); 

/* Perform the decoding */
Status do_decoding(DecodeInfo *encInfo); 

/* Get File pointers for i/p and o/p files */  
Status open_file(DecodeInfo *decInfo); 

/* decode magicstring */
Status decode_magic_string(DecodeInfo *decInfo);  

/*Decode function ,which does the real decoding */
Status decode_data_from_image(int size, FILE *fptr_stego_img,DecodeInfo *decInfo);  

/* Decode a byte from LSB of image data array */ 
Status decode_byte_from_lsb(char *data, char *image_buffer); 

/* Decode a file extension size */
Status decode_file_extern_size(FILE *fptr_stego_img,DecodeInfo *decInfo);

/* Decode a size from the Lsb */
Status decode_size_from_lsb(char *buffer,int *size);

/* decode a secret file extn */
Status decode_secret_file_extension( char *secret_file_extension,DecodeInfo *decInfo);      

/* decode the secret file size  */
Status decode_secret_file_size(FILE *fptr_stego_img,DecodeInfo *decInfo);          

/* decode the secret file data from the image */
Status decode_secret_file_data(DecodeInfo *decInfo); 

#endif
