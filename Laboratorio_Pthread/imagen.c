#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct bmpFileHeader
{
  /* 2 bytes de identificación */
  uint32_t size;        /* Tamaño del archivo */
  uint16_t resv1;       /* Reservado */
  uint16_t resv2;       /* Reservado */
  uint32_t offset;      /* Offset hasta hasta los datos de imagen */
} bmpFileHeader;

typedef struct bmpInfoHeader
{
  uint32_t headersize;      /* Tamaño de la cabecera */
  uint32_t width;           /* Ancho */
  uint32_t height;          /* Alto */
  uint16_t planes;          /* Planos de color (Siempre 1) */
  uint16_t bpp;             /* bits por pixel */
  uint32_t compress;        /* compresión */
  uint32_t imgsize;         /* tamaño de los datos de imagen */
  uint32_t bpmx;            /* Resolución X en bits por metro */
  uint32_t bpmy;            /* Resolución Y en bits por metro */
  uint32_t colors;          /* colors used en la paleta */
  uint32_t imxtcolors;      /* Colores importantes. 0 si son todos */
} bmpInfoHeader;

unsigned char *LoadBMP(char *filename, bmpInfoHeader *bInfoHeader, bmpFileHeader *fileHeader);
void *WriteBMP(char *filename, bmpInfoHeader *bInfoHeader, unsigned char *image, bmpFileHeader *fileHeader);
void DisplayInfo(bmpInfoHeader *info);
unsigned char *EditImage(bmpInfoHeader *info, unsigned char *img);

int main()
{
  bmpInfoHeader info;
  unsigned char *img;
  unsigned char *img2;
  bmpFileHeader fileHeader;

  img=LoadBMP("paisaje.bmp", &info, &fileHeader);
  img2=EditImage(&info, img);
  WriteBMP("paisaje2.bmp", &info, img2, &fileHeader);
  DisplayInfo(&info);

  return 0;
}

unsigned char *EditImage(bmpInfoHeader *info, unsigned char *img){
unsigned char *img2;
int i;

img2=img;
for(i=0; i<info->imgsize; i=i+3){
	img2[i]=0;
}

return img2;
}


unsigned char *LoadBMP(char *filename, bmpInfoHeader *bInfoHeader, bmpFileHeader *fileHeader)
{

  FILE *f;
  unsigned char *imgdata;   /* datos de imagen */
  uint16_t type;        /* 2 bytes identificativos */

  f=fopen (filename, "r");
  if (!f)
    return NULL;        /* Si no podemos leer, no hay imagen*/

  /* Leemos los dos primeros bytes */
  fread(&type, sizeof(uint16_t), 1, f);
  if (type !=0x4D42)        /* Comprobamos el formato */
    {
      fclose(f);
      return NULL;
    }

  /* Leemos la cabecera de fichero completa */

  fread(fileHeader, sizeof(bmpFileHeader), 1, f);

  /* Leemos la cabecera de información completa */
  fread(bInfoHeader, sizeof(bmpInfoHeader), 1, f);

  /* Reservamos memoria para la imagen, ¿cuánta? 
     Tanto como indique imgsize */
  imgdata=(unsigned char*)malloc(bInfoHeader->imgsize);

  /* Nos situamos en el sitio donde empiezan los datos de imagen,
   nos lo indica el offset de la cabecera de fichero*/
  fseek(f, fileHeader->offset, SEEK_SET);

  /* Leemos los datos de imagen, tantos bytes como imgsize */
  fread(imgdata, bInfoHeader->imgsize,1, f);

  /* Cerramos */
  fclose(f);

  /* Devolvemos la imagen */
  return imgdata;
}


void *WriteBMP(char *filename, bmpInfoHeader *bInfoHeader, unsigned char *image, bmpFileHeader *fileHeader)
{

  FILE *f;
  unsigned char *imgdata;   /* datos de imagen */
  uint16_t type;        /* 2 bytes identificativos */

  f=fopen (filename, "w");
  if (!f)
    return NULL;        /* Si no podemos leer, no hay imagen*/

  /* Escribimos los dos primeros bytes */
  type=0x4D42;
  fwrite(&type, sizeof(uint16_t), 1, f);

  /* Escribimos la cabecera de fichero completa */
  fwrite(fileHeader, sizeof(bmpFileHeader), 1, f);


  /* Escribimos la cabecera de información completa */
  fwrite(bInfoHeader, sizeof(bmpInfoHeader), 1, f);


  /* Nos situamos en el sitio donde empiezan los datos de imagen,
   nos lo indica el offset de la cabecera de fichero*/
  fseek(f, fileHeader->offset, SEEK_SET);


  /* Escribimos los datos de imagen, tantos bytes como imgsize */
  fwrite(image, bInfoHeader->imgsize,1, f);

  /* Cerramos */
  fclose(f);

  /* Devolvemos la imagen */
  return 0;
}
void DisplayInfo(bmpInfoHeader *info)
{
  printf("Tamaño de la cabecera: %u\n", info->headersize);
  printf("Anchura: %d\n", info->width);
  printf("Altura: %d\n", info->height);
  printf("Planos (1): %d\n", info->planes);
  printf("Bits por pixel: %d\n", info->bpp);
  printf("Compresión: %d\n", info->compress);
  printf("Tamaño de datos de imagen: %u\n", info->imgsize);
  printf("Resolucón horizontal: %u\n", info->bpmx);
  printf("Resolucón vertical: %u\n", info->bpmy);
  printf("Colores en paleta: %d\n", info->colors);
  printf("Colores importantes: %d\n", info->imxtcolors);
}

