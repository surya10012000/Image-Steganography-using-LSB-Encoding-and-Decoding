#include<stdio.h>
#include"decode.h"
#include<string.h>
#include"types.h"
#include"common.h"
#include<stdlib.h>

/* Function definition for read and validate decode args */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if( argv[2] == NULL && strstr(argv[2],".bmp") == NULL)
    {
	return e_failure;
    }
    else
	decInfo->stego_img_name = argv[2];
    if(argv[3] == NULL)
    {
	decInfo->output_file_name = "output.txt";
    }
    else
	decInfo->output_file_name = argv[3];
    return e_success;
}

/* Function definition for open files for decode */
Status open_file(DecodeInfo *decInfo)      
{
    decInfo->fptr_stego_img = fopen(decInfo->stego_img_name, "r");
    // Do Error handling
    if (decInfo->fptr_stego_img == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_img_name);
	return e_failure;
    }

    decInfo->fptr_output_file = fopen(decInfo->output_file_name, "w");
    // Do Error handling
    if (decInfo->fptr_output_file == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_file_name);
	return e_failure;
    }
    return e_success;
}

/* Function definition for decode magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_img,54,SEEK_SET);
    int val=strlen(MAGIC_STRING);
    decInfo->magic_str=malloc(val+1);   
    decode_data_from_image(val,decInfo->fptr_stego_img,decInfo);
    decInfo->magic_str[val]='\0';
    if((strcmp(decInfo->magic_str,MAGIC_STRING))== 0)
	return e_success;
    else
	return e_failure;

}

/* Function definition for decode data fom image */
Status decode_data_from_image(int size, FILE *fptr_stego_img,DecodeInfo *decInfo)
{
    char buff[8];
    for(int i=0;i<size;i++)
    {
	fread(buff,8,1,fptr_stego_img);
	decode_byte_from_lsb(&decInfo->magic_str[i],buff);
	//printf("%s\n",&decInfo->magic_str[i]);
    }
    return e_success;
}

/* Function definition for decode byte from lsb */
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    unsigned char num =0x00;
    for(int i=0;i<=7;i++)
    { 
	num=((image_buffer[i] &0x01)<<i) |num;

    }
    *data = num;
    return e_success;
}

/* Function definition for decode file extension size */
Status decode_file_extern_size(FILE *fptr_stego_img,DecodeInfo *decInfo)
{
    char arr[32];
    int num1;
    fread(arr,32,1,fptr_stego_img);
    decode_size_from_lsb(arr,&num1);
    decInfo->secret_file_extension_size=num1;
    //printf("%d\n",num1);
    return e_success;
}

/* Function definition for decode size from lsb */
Status decode_size_from_lsb(char *buffer,int *size)
{
    int num=0;
    for(int i=0;i<=31;i++)
    {
	num = ((buffer[i] & 0x01)<<i) | num;
    }
    *size =num;
 //  printf("%d\n",num);
}

/* Function definition for decode secret file extension */
Status decode_secret_file_extension(char *secret_file_extension,DecodeInfo *decInfo)
{
    int ext_len = decInfo->secret_file_extension_size;
    decInfo->secret_file_extension=malloc((ext_len+1)* sizeof(char));
    decode_data_from_image(ext_len,decInfo->fptr_stego_img,decInfo);
    decInfo->secret_file_extension[ext_len]='\0';
    return e_success;
}

/* Function definition for decode secret file size */
Status decode_secret_file_size(FILE *fptr_stego_img,DecodeInfo *decInfo)
{
    char val[32];
    int num =0;
    fread(val,32,1,fptr_stego_img);
    for(int i=0;i<=31;i++)
    {
	num = ((val[i] & 0X01)<<i)| num;
    }
    decInfo->size_of_secret_file = num;
    //printf("%d",num);
    return e_success;
}

/* Function definition for decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char cha;
    for(int i=0;i<decInfo->size_of_secret_file;i++)
    {
	fread(decInfo->stego_img_name,8,1,decInfo->fptr_stego_img);
	decode_byte_from_lsb(&cha,decInfo->stego_img_name);
	fputc(cha,decInfo->fptr_output_file);
    }
 }

/* Function definition for do decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_file(decInfo) == e_failure)
    {
	printf("open file is failed\n");
	return e_failure;
    }
    else
	printf("open file is success\n");
    if(decode_magic_string(decInfo) == e_failure)
    {
	printf("magic string is failed\n");
	return e_failure;
    }
    else
	printf("magic string is success\n");    
    if(decode_file_extern_size(decInfo->fptr_stego_img,decInfo) == e_failure)  
    {
	printf("decode the file extension size is failed\n");
	return e_failure;
    }
    else
	printf("decode the file externsion  size is success\n");
    if(decode_secret_file_extension(decInfo->secret_file_extension,decInfo) == e_failure)
    {
	printf("decode secret file extension is failed\n");
	return e_failure;
    }
    else
	printf("decode secret file extension is success\n");
    if(decode_secret_file_size(decInfo->fptr_stego_img,decInfo) == e_failure)
    {
	printf("decode secret file size is failed\n");
	return e_failure;
    }
    else
	printf("decode secret file size is success\n");
   if(decode_secret_file_data(decInfo) == e_failure)
      {
      printf("decode the data  from secret file is failed\n");
      return e_failure;
      }
      else
      printf("decode data from the secret file is success\n");
    return e_success;
}
