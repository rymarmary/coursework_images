#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define length_file 256

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
    FILE *f = fopen(name, "rb");
    image img;
    fread(&img.bmfh, 1, sizeof(BitmapFileHeader), f);
    fread(&img.bmih, 1, sizeof(BitmapInfoHeader), f);

    unsigned int H = img.bmih.height;
    unsigned int W = img.bmih.width;

    img.rgb = malloc(H * sizeof(RGB *)+(4-(H*sizeof(RGB))%4)%4);
    for (int i = 0; i < H; i++) {
        img.rgb[i] = malloc(W * sizeof(RGB) + (4-(W*sizeof(RGB))%4)%4);
        fread(img.rgb[i], 1, W * sizeof(RGB) + (4-(W*sizeof(RGB))%4)%4, f);
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
    printf("The processed file is saved to %s\n", outName);
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

int ifCorrect(image* img, char* name){
    unsigned long len = strlen(name);
    if (name[len - 1] != 'p' || name[len - 2] != 'm' || name[len - 3] != 'b' || name[len - 4] != '.') {
        printf("Error: invalid file name, it must end with '.bmp'\n");
        return 1;
    }
    FILE* bmpfile = fopen(name, "rb");
    *img = readImage(name);
    if (!bmpfile){
        printf("There is not such file in the directory.\n");
        return 1;
    }
    if (img->bmih.compression != 0){
        printf("File is compressed, it shouldn't be.\n");
        return 1;
    }
    if (img->bmih.bitsPerPixel != 24){
        printf("File with such depth of encoding is not supported, it should be 24 bits per pixel.\n");
        return 1;
    }
    if (img->bmih.colorsInColorTable != 0 || img->bmih.importantColorCount != 0){
        printf("File shouldn't use color table.\n");
        return 1;
    }
    return 0;
}

// cut function
// TODO: сделать обработку ошибок
void cut(image *img, char* nameOut, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2){
    image newImage;
    newImage.bmfh = img->bmfh;
    newImage.bmih = img->bmih;
    int W = x2-x1+1;
    int H = y2-y1+1;
    newImage.bmih.height = H;
    newImage.bmih.width = W;
    newImage.rgb = malloc(H*sizeof(RGB*));
    for (int i=0; i<H; i++){
        newImage.rgb[i] = malloc(W*sizeof(RGB)+(4-(W*sizeof(RGB))%4)%4);
    }
    for (int i=0; i<H; i++){
        for (int j=0; j<W; j++){
            newImage.rgb[i][j] = img->rgb[i+y1][j+x1];
        }
    }
    saveImage(&newImage, nameOut);
}

// paint over the circle function
// TODO: сделать обработку ошибок
void paintOverTheCircle(image *img, char* nameOut, unsigned int x, unsigned int y, int R){
    for (int i=x-R; i<x+R; i++){
        for (int j=y-R; j<y+R; j++){
            int radius = (x-j)*(x-j)+(y-i)*(y-i);
            if (radius<=(R*R)){
                img->rgb[i][j].r = 255 - img->rgb[i][j].r;
                img->rgb[i][j].g = 255 - img->rgb[i][j].g;
                img->rgb[i][j].b = 255 - img->rgb[i][j].b;
            }
        }
    }
    saveImage(img, nameOut);
}

// draw segment function
// TODO: сделать обработку ошибок
int accuracy(double arg1, double arg2, double accur){
    if (fabs(arg1-arg2) <= accur){
        return 1;
    }
    return 0;
}

int isLine(double x1, double y1, double x2, double y2, double i, double j, double bold){
    if (x1 == x2){
        if ((j >= y1-bold/2 && j <= y2+bold/2) || (j >= y2-bold/2 && j<= y2+bold/2)){
            if (accuracy(x1, i, 0.5*bold)){
                return 1;
            }
        }
    }
    double deltaX = x1 - x2;
    double deltaY = y1 - y2;
    double k = deltaY/deltaX;
    double b = (y1+y2-k*x1-k*x2) / 2;
    if (x1 < x2-bold/2 && x2 <= x1 && y2 >= y1){
        if (i >= x2 - bold / 2 && i <= x1 + bold / 2 && j >= y1 - bold / 2 && j <= y2 + bold / 2) {
            if (accuracy(j, k * i + b, 0.5 * bold))
                return 1;
            if (accuracy(i, (j - b) / k, 0.5 * bold))
                return 1;
        }
        if (i >= x1 - bold / 2 && j <= x2 + bold / 2 && j >= y2 - bold / 2 && j <= y1 + bold / 2) {
            if (accuracy(j, k * i + b, 0.5 * bold))
                return 1;
            if (accuracy(i, (j - b) / k, 0.5 * bold))
                return 1;
        }
    }
    if (x2 <= x1 && y2 <= y1 || x1 <= x2 && y1 <= y2) {
        if (i >= x2 - bold / 2 && i <= x1 + bold / 2 && j >= y2 - bold / 2 && j <= y1 + bold / 2) {
            if (accuracy(j, k * i + b, 0.5 * bold))
                return 1;
            if (accuracy(i, (j - b) / k, 0.5 * bold))
                return 1;
        }
        if (i >= x1 - bold / 2 && i <= x2 + bold / 2 && j >= y1 - bold / 2 && j <= y2 + bold / 2) {
            if (accuracy(j, k * i + b, 0.5 * bold))
                return 1;
            if (accuracy(i, (j - b) / k, 0.5 * bold))
                return 1;
        }
    }
    return 0;
}

void drawSegment(image *img, char* nameOut, unsigned int x1, unsigned int y1, unsigned int x2,
                 unsigned int y2, int color1, int color2, int color3, int bold){
    int H = img->bmih.height;
    int W = img->bmih.width;
    for (int i=0; i<H; ++i){
        for (int j=0; j<W; ++j){
            if (isLine(x1, y1, x2, y2, j, i, bold)){
                img->rgb[H-i-1][j].r = color1;
                img->rgb[H-i-1][j].g = color2;
                img->rgb[H-i-1][j].b = color3;
            }
        }
    }
    saveImage(img, nameOut);
}

// Command Line Interface

void help_output() {
    char info[] = "Hey, currently u're working with BMP Photo Editor 'Time for Edit'.\n"
                  "Here u can see a description of this program: it supports files of only 3rd version; "
                  "encoding depth is 24 bits per color; file shouldn't be compressed.\n"
                  "Functions:\n1). -i -- if u want to see an information about bmp-file\n"
                  "2). -h -- if u need to find out what this program can do (btw, u're reading it now)\n"
                  "3). -c -- if u want to cut file: eg, ./main -c korgi.bmp -o out.bmp -t 200,200,800,800\n"
                  "4). -n -- if u want to invert colors in a circle: eg, ./main -n korgi.bmp -o out.bmp -p 500,500 -r 30\n"
                  "5). -s -- if u want to draw a segment: eg, ./main -s korgi.bmp -o out.bmp -t 200,200,500,500 -b 0,185,0,30\n"
                  "Additional keys:\na). -o -- name of file, where u want to save a processed picture\n"
                  "b). -t -- two pairs of arguments, every number is divided from another by comma\n"
                  "c). -p -- one pair of arguments, every number is divided from another by comma\n"
                  "d). -r -- radius of the Circle u want to invert colors in\n"
                  "e). -b -- here u should enter 3 numbers of rgb color palette and bold of the segment, all numbers are divided by comma\n"
                  "Thanks for using 'Time for edit'.";
    puts(info);
}

int main(int argc, char *argv[]) {
    char *opts = "hi:c:t:s:n:o:p:b:r:"; //если без аргументов, то без двоеточия
    struct option longOpts[] = {{"help", no_argument, NULL, 'h'},
                                {"info", required_argument, NULL, 'i'},
                                {"cut", required_argument, NULL, 'c'},
                                {"segment", required_argument, NULL, 's'},
                                {"negate", required_argument, NULL, 'n'},
                                {"outputFile", required_argument, NULL, 'o'},
                                {"twoPairOfCoordinates", required_argument, NULL, 't'},
                                {"onePairOfCoordinates", required_argument, NULL, 'p'},
                                {"radiusCircle", required_argument, NULL, 'r'},
                                {"colorsBold", required_argument, NULL, 'b'},
                                {NULL, 0, NULL}};
    int opt;
    int longOpt;
    opt = getopt_long(argc, argv, opts, longOpts, &longOpt);
    image img;
    char inputFile[length_file];
    char outputFile[length_file];
    int numArgs;
    int x1Coordinate=0, y1Coordinate=0, x2Coordinate=0, y2Coordinate=0;
    int color1=0, color2=0, color3=0, bold=0;
    int radius=-1;
    int funcName=0;

    if (argc<2){
        printf("Wrong input, please enter keys to use the program.\n");
        help_output();
        return 0;
    }

    while (opt != -1){
        switch (opt) {
            case 'h':{
                help_output();
                return 0;
            }
            case 'i':{
                sscanf(optarg, "%s", inputFile);
                img = readImage(inputFile);
                if (ifCorrect(&img, inputFile) != 0){
                    printf("Incorrect input file, please try another.\n");
                    return 1;
                }
                printFileInfo(&img);
                return 0;
            }
            case 'c':{
                numArgs = sscanf(optarg, "%s", inputFile);
                if (numArgs<1){
                    printf("Too few arguments.\n");
                    return 1;
                }
                if (ifCorrect(&img, inputFile) != 0){
                    printf("Incorrect input file, please try another.\n");
                    return 1;
                }
                funcName = 1;
                break;
            }
            case 'n':{
                numArgs = sscanf(optarg, "%s", inputFile);
                if (numArgs<1){
                    printf("Too few arguments.\n");
                    return 1;
                }
                if (ifCorrect(&img, inputFile) != 0){
                    printf("Incorrect input file, please try another.\n");
                    return 1;
                }
                funcName = 2;
                break;
            }
            case 's':{
                numArgs = sscanf(optarg, "%s", inputFile);
                if (numArgs<1){
                    printf("Too few arguments.\n");
                    return 1;
                }
                if (ifCorrect(&img, inputFile) != 0){
                    printf("Incorrect input file, please try another.\n");
                    return 1;
                }
                funcName = 3;
                break;
            }
            case 't':{
                numArgs = sscanf(optarg, "%d,%d,%d,%d", &x1Coordinate, &y1Coordinate, &x2Coordinate, &y2Coordinate);
                if (numArgs<4){
                    printf("Too few arguments for two pair of coordinates.\n");
                    return 1;
                }
                break;
            }
            case 'p':{
                numArgs = sscanf(optarg, "%d,%d", &x1Coordinate, &y1Coordinate);
                if (numArgs<2){
                    printf("Too few arguments for one pair of coordinates.\n");
                    return 1;
                }
                break;
            }
            case 'r':{
                numArgs = sscanf(optarg, "%d", &radius);
                if (numArgs<1){
                    printf("You haven't entered radius.\n");
                    return 1;
                }
                break;
            }
            case 'b':{
                numArgs = sscanf(optarg, "%d,%d,%d,%d", &color1, &color2, &color3, &bold);
                if (numArgs<4){
                    printf("Too few arguments for colors and bold. There should be 3 arguments for colors and 1 for bold.\n");
                    return 1;
                }
                break;
            }
            case 'o':{
                sscanf(optarg, "%s", outputFile);
                break;
            }
            default:{
                printf("Wrong key\n");
                return 1;
            }
        }
        opt = getopt_long(argc, argv, opts, longOpts, &longOpt);
    }
    switch (funcName){
        case 1: {
            cut(&img, outputFile, x1Coordinate, y1Coordinate, x2Coordinate, y2Coordinate);
            break;
        }
        case 2:{
            if (radius==-1) {
                radius = abs((x2Coordinate - x1Coordinate) / 2);
                x1Coordinate = (x2Coordinate + x1Coordinate) / 2;
                x2Coordinate = (y2Coordinate + y1Coordinate) / 2;
            }
            paintOverTheCircle(&img, outputFile, x1Coordinate, y1Coordinate, radius);
            break;
        }
        case 3:{
            drawSegment(&img, outputFile, x1Coordinate, y1Coordinate, x2Coordinate, y2Coordinate, color1, color2, color3, bold);
            break;
        }
        default:{
            printf("No function called.\n");
            break;
        }
    }
    return 0;
}