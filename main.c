#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    unsigned short signature;   // 0x4d42 | 0x4349 | 0x5450
    unsigned int filesize;      // размер файла
    unsigned short reserved1;   // 0
    unsigned short reserved2;   // 0
    unsigned int pixelArrOffset;// смещение до поля данных (обычно 54=16+sizeOfStruct)
} BitmapFileHeader;

typedef struct {
    unsigned int headerSize;         // размер структуры
    unsigned int width;              // ширина в точках
    unsigned int height;             // высота в точках
    unsigned short planes;           // всегда должно быть 1
    unsigned short bitsPerPixel;     // 0 | 1 | 4 | 8 | 16 | 24 | 32
    unsigned int compression;        // BI_RGB | BI_RLE8 | BI_RLE4 |
                                     // BI_BITFIELDS | BI_JPEG | BI_PNG
                                     // реально используется лишь BI_RGB
    unsigned int imageSize;          // количество байт в поле данных
                                     // обычно устанавливается в 0
    unsigned int xPixelsPerMeter;    // горизонтальное разрешение
    unsigned int yPixelsPerMeter;    // вертикальное разрешение
    unsigned int colorsInColorTable; // кол-во используемых цветов
    unsigned int importantColorCount;// кол-во существенных цветов
} BitmapInfoHeader;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB;

typedef struct {
    BitmapFileHeader bmfh;
    BitmapInfoHeader bmih;
    RGB **rgb;
} image;
#pragma pack(pop)

image readImage(char *name) {
    FILE *f = fopen("korgi.bmp", "rb");
    image img;
    fread(&img.bmfh, 1, sizeof(BitmapFileHeader), f);
    fread(&img.bmih, 1, sizeof(BitmapInfoHeader), f);

    unsigned int H = img.bmih.height;
    unsigned int W = img.bmih.width;

    img.rgb = malloc(H * sizeof(RGB *));
    for (int i = 0; i < H; i++) {
        img.rgb[i] = malloc(W * sizeof(RGB) + (W * 3) % 4);
        fread(img.rgb[i], 1, W * sizeof(RGB) + (W * 3) % 4, f);
    }
    return img;
}

void saveImage(image *img, char *outName) {
    int length = (int) strlen(outName);
    if (outName[length - 1] != 'p' || outName[length - 2] != 'm' || outName[length - 3] != 'b' || outName[length - 4] != '.') {
        printf("Error: invalid file name, it must end with '.bmp'\n");
        return;
    }
    FILE *f = fopen(outName, "wb");
    if (!f) {
        printf("Error: unable to open the file\n");
        return;
    }
    fwrite(&img->bmfh, 1, sizeof(BitmapFileHeader), f);
    fwrite(&img->bmih, 1, sizeof(BitmapInfoHeader), f);

    unsigned int w = (img->bmih.width) * sizeof(RGB) + ((img->bmih.width * sizeof(RGB)) * 3) % 4;
    for (int i = 0; i < img->bmih.height; i++) {
        fwrite(img->rgb[i], 1, w, f);
        free(img->rgb[i]);
    }
    // TODO можно отладить - корректно ли сохраняется файл
    printf("saved to %s\n", outName);
    fclose(f);
}

void printFileInfo(image *img) {
    printf("signature:\t%x (%hu)\n", img->bmfh.signature, img->bmfh.signature);
    printf("filesize:\t%x (%u)\n", img->bmfh.filesize, img->bmfh.filesize);
    printf("reserved1:\t%x (%hu)\n", img->bmfh.reserved1, img->bmfh.reserved1);
    printf("reserved2:\t%x (%hu)\n", img->bmfh.reserved2, img->bmfh.reserved2);
    printf("pixelArrOffset:\t%x (%u)\n", img->bmfh.pixelArrOffset, img->bmfh.pixelArrOffset);
    printf("headerSize:\t%x (%u)\n", img->bmih.headerSize, img->bmih.headerSize);
    printf("width:     \t%x (%u)\n", img->bmih.width, img->bmih.width);
    printf("height:    \t%x (%u)\n", img->bmih.height, img->bmih.height);
    printf("planes:    \t%x (%hu)\n", img->bmih.planes, img->bmih.planes);
    printf("bitsPerPixel:\t%x (%hu)\n", img->bmih.bitsPerPixel, img->bmih.bitsPerPixel);
    printf("compression:\t%x (%u)\n", img->bmih.compression, img->bmih.compression);
    printf("imageSize:\t%x (%u)\n", img->bmih.imageSize, img->bmih.imageSize);
    printf("xPixelsPerMeter:\t%x (%u)\n", img->bmih.xPixelsPerMeter, img->bmih.xPixelsPerMeter);
    printf("yPixelsPerMeter:\t%x (%u)\n", img->bmih.yPixelsPerMeter, img->bmih.yPixelsPerMeter);
    printf("colorsInColorTable:\t%x (%u)\n", img->bmih.colorsInColorTable, img->bmih.colorsInColorTable);
    printf("importantColorCount:\t%x (%u)\n", img->bmih.importantColorCount, img->bmih.importantColorCount);
}

// Command Line Interface

void help_output() {
}

int main() {

    return 0;
}