#include <png.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned char** pages;
	int size;
} PixelData;

//Free struct
void free_pixel_data(PixelData *p){
	unsigned char** pages = (*p).pages;

	for(int s = 0; s < (*p).size; s++){
		free(pages[s]);
	}

	free(p);
}

PixelData* convert(const char *file_name){
	FILE *fp = fopen(file_name, "rb");
	if(!fp){
		printf("This is not a valid file..\n");
		printf("Usage: pngtby [file]\n");
		return NULL;
	}

	unsigned char header[8];

	fread(header, 1, 8, fp);
	
	if(png_sig_cmp(header, 0, 8)){
		printf("This is not a PNG!!");
		printf("Usage: pngtby [file]\n");
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

	int width = png_get_image_width(png_ptr, info_ptr);
	int height = png_get_image_height(png_ptr, info_ptr); 

	if(width > 64 || height > 128){

		printf("This file is too large for the display!\n");

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		fclose(fp);

		fp=NULL;

		return NULL;
	}

	int size;
	if(height%2 == 0 && height >= 8){
		size = height/8;
	}else if(height < 8){
		size = 1;
	}else if(height%2 != 0 && height >= 8){
		//Truncate and add 1
		size = height/8 + 1;
	}

	unsigned char **pages = malloc(size * sizeof(unsigned char*));

	/*unsigned char *data_t = malloc(width*sizeof(unsigned char));*/

	for(int s = 0; s < size; s++){
		pages[s] = malloc(width*sizeof(unsigned char));

		//Height
		for (int h = 0; h < height; h++){
			png_bytep row_data_ptr = row_ptr[h];

			//Width
			for(int x = 0; x < width; x++){
				//R G B A per pixel
				//Get the address of the pointer at our x pixel.
				png_bytep png_pix_ptr = &row_data_ptr[x*4];

				if (png_pix_ptr[3] != 0){
					//Write one bit
					pages[s][x] = pages[s][x] | 0x01 << h;
				}
			}
		}
	}

	PixelData *p = malloc(sizeof(PixelData));  

	(*p).pages = pages;
	(*p).size = size;

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);

	fp=NULL;

	return p; 
}

int main(int argc, char *argv[]){

	if(argc == 1 || argc < 2){
		printf("Usage: pngtby [file]\n");
		return 0;
	}

	char *file = argv[1];

	PixelData *p_data = convert(file);
	if(!p_data){
		return -1;
	}

	printf("Output of bytes: \n");
	for(int p = 0; p <  (*p_data).size; p++){
		printf("{ ");
		unsigned char* page = (*p_data).pages[p];

		for(int d = 0; d < sizeof(page)-1; d++){
			if(page[d] == 0){
				printf("0x00, ");
				continue;
			}
			printf("%#02x, ", page[d]);
		}
		printf("},\n");
	}
	free_pixel_data(p_data);
}
