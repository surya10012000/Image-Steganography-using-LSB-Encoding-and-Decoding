#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include"common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

/* Function definition for read and validate encode args */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2],".bmp") ==NULL)
	return e_failure;
    encInfo->src_image_fname = argv[2];
    if(argv[3] == NULL)
	return e_failure;
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file,strstr(encInfo->secret_fname,"."));
    if(argv[4] == NULL)
    {
	encInfo->stego_image_fname="stego_img.bmp";
    }
    else
	encInfo->stego_image_fname=argv[4];
    return e_success;
}
uint get_image_size_for_bmp(FILE *fptr_image) 
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

/* function defintion for open file */
Status open_files(EncodeInfo *encInfo)      
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* Function definition for getting file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

/* Function definition for check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(get_image_size_for_bmp(encInfo->fptr_src_image)>= 54+(strlen(MAGIC_STRING)+4+strlen(encInfo->extn_secret_file)+4+get_file_size(encInfo->fptr_secret))*8)
	return e_success;
}

/* Function definition for copying 1st 54 bytes from header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(arr,1,54,fptr_src_image);
    int result = fwrite(arr,1,54,fptr_dest_image);
    if(result == 54)
	return e_success;
  //  printf("copied the bmp header success\n");
}

/*  Function definition for encoding magic string */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if(encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    return e_success;
}

/* Function definition for Encode data to image */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];
    for(int j=0;j<size;j++)
    {
	fread(arr,1,8,fptr_src_image);
	encode_byte_to_lsb(data[j],arr);
	fwrite(arr,1,8,fptr_stego_image);
    }
    return e_success;
}

/* Function definition for encode byte to lsb */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    { 
	image_buffer[i]=((image_buffer[i]&0XFE)|(((1<<i)&data)>>i)); 
    }
}

/* Function definition to encode secret file extn*/
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)            
{
    if(encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_success)
    return e_success;
}

/* Function definition for encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)     
{
    if( encode_size_to_lsb( file_size,encInfo->fptr_src_image,encInfo->fptr_stego_image ) == e_success )    
    {
	return e_success;
    }
}

/* Function definition for encode size to lsb */
Status encode_size_to_lsb(int size,FILE*fptr_src_image,FILE*fptr_stego_image)
{
    char num[32];
    fread(num,1,32,fptr_src_image);
    for(int j=0;j<32;j++)
    {
	num[j]=(num[j]&0XFE)|(((1<<j)&size)>>j);
    }
    fwrite(num,1,32,fptr_stego_image);
    return e_success;
}

/* Function definition for encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)                
{
    rewind(encInfo->fptr_secret);
    char str[encInfo->size_secret_file];
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

/* Function definition for copying remaining data */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{    
    char ch;
    while(fread(&ch,1,1,fptr_src)>0)
    {
	fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}

/* Function definition for do encoding call in main function */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)
    {
	printf("open file is failed\n");
	return e_failure;
    }
    else
	printf("open file is success\n");

    if(check_capacity(encInfo) == e_failure)
    {
	printf("check capacity is failed\n");
	return e_failure;
    }
    else
	printf("check capacity is success\n");
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
	printf("failed to copy bmp header\n");
	return e_failure;
    }
    else
	printf("copy bmp header is success\n");
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
	printf("magic string is failed\n");
	return e_failure;
    }
    else
	printf("magic string is success\n");
    if(encode_size_to_lsb(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_failure)
    {
	printf("encoding extension size is failed\n");
	return e_failure;
    }
    else
	printf("encoding extension size is success\n");
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
	printf("Encode secret file extension is failed\n");
	return e_failure;
    }
    else
	printf("encode secret file extension is success\n");
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
	printf("Encode secret file size is failed\n");
	return e_failure;
    }
    else
	printf("Encode secret file size is success\n");
    if (encode_secret_file_data(encInfo) == e_failure)
    {
	printf("Encoded secret file data is failed\n");
    }
    else
	printf("encode secret file data is success\n");
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
	printf("Copied remaining data is failed\n");
    }
    else
	printf("copied the remaining data is success\n");
    return e_success;
}

