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

void PringGray8Pixel(gray8Pixel** imgdata,int y1,int y2,int x1,int x2){
    for(int y=y1;y<=y2;y++){
        for(int x=x1;x<=x2;x++){
            printf(" %3d |",imgdata[y][x]);
        }
        printf("\n");
    }
}

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
    FILE* file = fopen(argv[1], "rb");
    if (file==NULL){
        printf("readfile is error\n");
    }
    fread(&fileheader, sizeof(BitmapFileHeader), 1, file);
    fread(&infoheader, sizeof(BitmapInfoHeader), 1, file);
    //EndianSwap((uint8_t *)&(fileheader->size),0,4);
    printf("file size     is %dKB\n",fileheader.size    );
    printf("file bitcount is %d  \n",infoheader.bitCount);
    printf("file width    is %d  \n",infoheader.width   );
    printf("file height   is %d  \n",infoheader.height  );

    //skip datasetoff
    fseek(file, fileheader.offset, SEEK_SET);

    gray8Pixel** imgdata = (gray8Pixel**)malloc(infoheader.height * sizeof(gray8Pixel*));
    //calculate number of padding byte
    int paddingsize;
    paddingsize = (infoheader.width%4==0) ? 0 : 4-infoheader.width%4;
    for(int i=0;i<infoheader.width;i++){
        imgdata[infoheader.height-1-i] = (gray8Pixel*)malloc(infoheader.width*sizeof(gray8Pixel));
        fread(imgdata[infoheader.height-1-i],infoheader.width,1,file);
        //skip padding
        fseek(file, paddingsize, SEEK_CUR);
    }
    PringGray8Pixel(imgdata,50,50,5,10);

    fclose(file);

    return 0;
}               
