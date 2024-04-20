#include "stdio.h"                                                                                                                                                                                                                        
#include "stdint.h"
#include "stdlib.h"

#pragma pack(push, 1)
    typedef struct {
        uint16_t type;       // 文件类型，必须为 'BM'（0x4D42）
        uint32_t size;       // 文件大小
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t offset;     // 图像数据偏移量
    } BitmapFileHeader;

    typedef struct {
        unsigned int headerSize;
        int width;
        int height;
        unsigned short planes;
        unsigned short bitCount;
        unsigned int compression;
        unsigned int imageSize;
        int xResolution;
        int yResolution;
        unsigned int colorsUsed;
        unsigned int importantColors;
    } BitmapInfoHeader;

    typedef struct{
        uint8_t grayscale;
    } gray8Pixel;
#pragma pack(pop)
int main(int argc, char *argv[])
{
    BitmapFileHeader fileheader;
    BitmapInfoHeader infoheader;
    //check if input filepath
    if(argc < 2){
        printf("input a path for bmp,please\n");
        return 1;
    }

    //get file handle
    FILE* file = fopen(argv[1], "rb"); if (file==NULL){ printf("readfile is error\n"); }
    fread(&fileheader, sizeof(BitmapFileHeader), 1, file);
    fread(&infoheader, sizeof(BitmapInfoHeader), 1, file);
    //EndianSwap((uint8_t *)&(fileheader->size),0,4);
    printf("file kind     is %xKB\n",fileheader.type    );
    printf("file size     is %dKB\n",fileheader.size    );
   printf("file bitcount is %d  \n",infoheader.bitCount);
    printf("file width    is %d  \n",infoheader.width   );
    printf("file height   is %d  \n",infoheader.height  );
    printf("file offset   is %d  \n",fileheader.offset  );

    //get before data 
    char* headdata;
    headdata = (char*)malloc(fileheader.offset);
    fseek(file, 0, SEEK_SET);
    fread(headdata, fileheader.offset, 1, file);

    //skip datasetoff
    fseek(file, fileheader.offset, SEEK_SET);

    gray8Pixel** imgdata = (gray8Pixel**)malloc(infoheader.height * sizeof(gray8Pixel*));
    //calculate number of padding byte
    int paddingsize;
    int i;
    paddingsize = (infoheader.width%4==0) ? 0 : 4-infoheader.width%4;
    for( i=0;i<infoheader.height;i++){
        //imgdata[infoheader.height-1-i] = (gray8Pixel*)malloc(infoheader.width*sizeof(gray8Pixel));
        imgdata[i] = (gray8Pixel*)malloc(infoheader.width*sizeof(gray8Pixel));
        fread(imgdata[i],infoheader.width*sizeof(gray8Pixel),1,file);
        //skip padding
        fseek(file, paddingsize, SEEK_CUR);
    }

    //get histogram
    unsigned long histogram[256]={0};
    float   pixel_percent[256]={0};
    float   pixel_accum[256]={0};
    float  pixel_sum;

    pixel_sum = (float)(infoheader.height*infoheader.width);

    for(i=0;i<infoheader.height;i++)
    {
        for(int j=0;j<infoheader.width;j++)
        {
            histogram[imgdata[i][j].grayscale]++;
        }
    }
   for(i=0;i<256;i++)                                                                                                                                                                                                                    
    {
        pixel_percent[i] = (float)(histogram[i]/pixel_sum);
    }
        printf("pixel summary is %f\n",pixel_sum);

    //acul 
    pixel_accum[0] = pixel_percent[0];
    for(i=1;i<256;i++)
    {
        pixel_accum[i] = pixel_accum[i-1]+pixel_percent[i];
    }
    for(i=0;i<256;i++)
    {
        printf("histogram[%d] :  %d\n",i,histogram[i]);
        printf("%d    :    %f\n",i,pixel_percent[i]);
        printf("%d    :    %f\n",i,pixel_accum[i]);
        printf("------------------------------\n");

    }
    for(i=0;i<infoheader.height;i++)
    {
        for(int j=0;j<infoheader.width;j++)
        {
            imgdata[i][j].grayscale = (uint8_t)((float)(pixel_accum[imgdata[i][j].grayscale])*255);
        }
    }



    FILE* output_file_ptr = fopen("./postprocess.bmp","wb");

    uint8_t pad = 0;
    //fwrite(&fileheader,sizeof(BitmapFileHeader),1,output_file_ptr);
    //fwrite(&infoheader,sizeof(BitmapInfoHeader),1,output_file_ptr);
    //fwrite(&pad,sizeof(char),1026,output_file_ptr);
    fwrite(headdata,fileheader.offset,1,output_file_ptr);
    for ( i=0;i<infoheader.height;i++)
    {
        fwrite(imgdata[i],infoheader.width*sizeof(gray8Pixel),1,output_file_ptr);
        fwrite(&pad,sizeof(char),paddingsize,output_file_ptr);
    }
}

    fclose(file);
    fclose(output_file_ptr);


    return 0;
}                 
