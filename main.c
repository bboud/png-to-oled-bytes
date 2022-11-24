#include <png.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char* convert(const char *file_name){
	FILE *fp = fopen(file_name, "rb");
	if(!fp){
		printf("This is not a valid file..\n");
		return NULL;
	}

	unsigned char header[8];

	fread(header, 1, 8, fp);
	
	if(png_sig_cmp(header, 0, 8)){
		printf("This is not a PNG!!");
		fclose(fp);
		fp = NULL;
		return NULL;
	}

	//Create the structs
	png_structp png_ptr = png_create_read_struct("1.6.38", NULL, NULL, NULL );

	if(!png_ptr){
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(fp);
		fp = NULL;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr){
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		fp = NULL;
	}

	png_init_io (png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	png_bytepp row_ptr = png_get_rows(png_ptr, info_ptr);

	unsigned char *data_t = malloc(5*sizeof(unsigned char));

	//Height
	for (int height = 0; height < 8; height++){
		png_bytep row_data_ptr = row_ptr[height];

		//Width
		for(int x = 0; x < 5; x++){
			//R G B A per pixel
			//Get the address of the pointer at our x pixel.
			png_bytep png_pix_ptr = &row_data_ptr[x*4];

			printf("[%d, %d, %d, %d]\n", png_pix_ptr[0], png_pix_ptr[1], png_pix_ptr[2], png_pix_ptr[3]);

			if (png_pix_ptr[3] != 0){
				//Write one bit
				data_t[x] = data_t[x] | 0x01 << height;
			}
		}
	}


	//Verification
	for(int i = 0; i < 5; i++){
		printf("%#02x\n", data_t[i]);
		printf("%#02x\n", (unsigned char)~data_t[i]);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);

	fp=NULL;

	return data_t;
}

int main(int argc, char *argv[]){

	if(argc == 1){
		printf("Usage: pngtby [width] [height] [file]\n");
		return 0;
	}

	printf("%s\n", argv[1]);
	unsigned char* data = convert(argv[1]);
	if(!data){
		return -1;
	}

	printf("Output of bytes: \n");
	printf("%#02x, %#02x, %#02x, %#02x, %#02x,\n", data[0], data[1], data[2], data[3], data[4]);

	free(data);
}

