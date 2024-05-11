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

// 定义中值滤波器大小
#define FILTER_SIZE 3
// 中值滤波函数
void median_filter(gray8Pixel* input, gray8Pixel* output, int width, int height) {
    int i, j, m, n, k, l, temp, median;
    int window[FILTER_SIZE * FILTER_SIZE];
    
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            k = 0;
    
            // 将窗口内像素值复制到数组中
            for (m = -1; m <= 1; m++) {
                for (n = -1; n <= 1; n++) {
                    if (i + m >= 0 && i + m < height && j + n >= 0 && j + n < width) {
                        window[k++] = input[(i + m) * width + (j + n)].grayscale;
                    }
                } 
            }

            // 对窗口内像素值进行排序
            for (m = 0; m < FILTER_SIZE * FILTER_SIZE - 1; m++) {
                for (n = 0; n < FILTER_SIZE * FILTER_SIZE - m - 1; n++) {
                    if (window[n] > window[n + 1]) {
                        temp = window[n];
                        window[n] = window[n + 1];
                        window[n + 1] = temp;
                    }
                }
            }
    
            // 获取中间值
            median = window[FILTER_SIZE * FILTER_SIZE / 2];
            output[i * width + j].grayscale = median;
        }
    }
}

void mean_filter(gray8Pixel* input, gray8Pixel* output, int width, int height) {
    int i, j, m, n, index, count;
    long sum;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            sum = 0;
            count = 0;

            // 计算窗口内像素值的总和
             for (m = -FILTER_SIZE/2; m <= FILTER_SIZE/2; m++) {
                for (n = -FILTER_SIZE/2; n <= FILTER_SIZE/2; n++) {
                    if (i + m >= 0 && i + m < height && j + n >= 0 && j + n < width) {
                        index = (i + m) * width + (j + n);
                        sum += input[index].grayscale;
                        count++;
                    }
                }
            }
            printf("old is %d\n",input[index].grayscale);

            // 计算均值
            output[i * width + j].grayscale = sum / count;
            printf("sum is %d\n",sum / count);
        }
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
    FILE* file = fopen(argv[1], "rb"); if (file==NULL){ printf("readfile is error\n"); }
    fread(&fileheader, sizeof(BitmapFileHeader), 1, file);
    fread(&infoheader, sizeof(BitmapInfoHeader), 1, file);
    printf("file kind     is %d\n",infoheader.bitCount   );
    char* headdata;
    headdata = (char*)malloc(fileheader.offset);
    fseek(file, 0, SEEK_SET);
    fread(headdata, fileheader.offset, 1, file);

    //skip datasetoff
    fseek(file, fileheader.offset, SEEK_SET);


    // 分配内存用于存储原始和处理后的图像数据
    gray8Pixel* input_image  = (gray8Pixel*)malloc(infoheader.width * infoheader.height * sizeof(gray8Pixel));
    gray8Pixel* median_output_image = (gray8Pixel*)malloc(infoheader.width * infoheader.height * sizeof(gray8Pixel));
    gray8Pixel* mean_output_image = (gray8Pixel*)malloc(infoheader.width * infoheader.height * sizeof(gray8Pixel));
    // 读取原始图像数据                                                                                                                                                                                                                                                                                                      
    fread(input_image, sizeof(gray8Pixel), infoheader.width * infoheader.height, file);
median_filter(input_image, median_output_image, infoheader.width, infoheader.height);
     mean_filter  (input_image,  mean_output_image, infoheader.width, infoheader.height);


    FILE* median_output_file = fopen("./median_postprocess.bmp","wb");
    FILE* mean_output_file  = fopen("./mean_postprocess.bmp","wb");

    fwrite(headdata,fileheader.offset,1,median_output_file);
    fwrite(headdata,fileheader.offset,1,mean_output_file);
fwrite(median_output_image, sizeof(gray8Pixel), infoheader.width * infoheader.height, median_output_file);
    fwrite(mean_output_image, sizeof(gray8Pixel), infoheader.width * infoheader.height, mean_output_file);

    fclose(file);
    fclose(median_output_file);
    fclose(mean_output_file);


    return 0;
}

