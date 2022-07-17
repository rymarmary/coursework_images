#include <stdio.h>

typedef struct{
    unsigned short signature;  // 0x4d42 | 0x4349 | 0x5450
    unsigned int filesize;  // размер файла
    unsigned short reserved1;  // 0
    unsigned short reserved2;  // 0
    unsigned int pixelArrOffset; // смещение до поля данных (обычно 54=16+sizeOfStruct)
} BitmapFileHeader;

typedef struct{
    unsigned int headerSize;  // размер структуры
    unsigned int width;  // ширина в точках
    unsigned int height;  // высота в точках
    unsigned short planes;  // всегда должно быть 1
    unsigned short bitsPerPixel;  // 0 | 1 | 4 | 8 | 16 | 24 | 32
    unsigned int compression;  // BI_RGB | BI_RLE8 | BI_RLE4 |
                               // BI_BITFIELDS | BI_JPEG | BI_PNG
                               // реально используется лишь BI_RGB
    unsigned int imageSize;  // количество байт в поле данных
                             // обычно устанавливается в 0
    unsigned int xPixelsPerMeter;  // горизонтальное разрешение
    unsigned int yPixelsPerMeter;  // вертикальное разрешение
    unsigned int colorsInColorTable;  // кол-во используемых цветов
    unsigned int importantColorCount;  // кол-во существенных цветов
} BitmapInfoHeader;

void printFileHeader(BitmapFileHeader header){
    printf("signature:\t%x (%hu)\n", header.signature, header.signature);
    printf("filesize:\t%x (%u)\n", header.filesize, header.filesize);
    printf("reserved1:\t%x (%hu)\n", header.reserved1, header.reserved1);
    printf("reserved2:\t%x (%hu)\n", header.reserved2, header.reserved2);
    printf("pixelArrOffset:\t%x (%u)\n", header.pixelArrOffset, header.pixelArrOffset);
}

void printInfoHeader(BitmapInfoHeader header){
    printf("headerSize:\t%x (%u)\n", header.headerSize, header.headerSize);
    printf("width:     \t%x (%u)\n", header.width, header.width);
    printf("height:    \t%x (%u)\n", header.height, header.height);
    printf("planes:    \t%x (%hu)\n", header.planes, header.planes);
    printf("bitsPerPixel:\t%x (%hu)\n", header.bitsPerPixel, header.bitsPerPixel);
    printf("compression:\t%x (%u)\n", header.compression, header.compression);
    printf("imageSize:\t%x (%u)\n", header.imageSize, header.imageSize);
    printf("xPixelsPerMeter:\t%x (%u)\n", header.xPixelsPerMeter, header.xPixelsPerMeter);
    printf("yPixelsPerMeter:\t%x (%u)\n", header.yPixelsPerMeter, header.yPixelsPerMeter);
    printf("colorsInColorTable:\t%x (%u)\n", header.colorsInColorTable, header.colorsInColorTable);
    printf("importantColorCount:\t%x (%u)\n", header.importantColorCount, header.importantColorCount);
}


int main(){
    FILE *f = fopen("korgi.bmp", "rb");
    BitmapFileHeader bmfh;
    BitmapInfoHeader bmih;
    fread(&bmfh, 1, sizeof(BitmapFileHeader), f);
    fread(&bmih, 1, sizeof(BitmapInfoHeader), f);
    printFileHeader(bmfh);
    printInfoHeader(bmih);

    return 0;
}
