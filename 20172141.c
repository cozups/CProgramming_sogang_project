#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	unsigned char RGB[3];
} RGBPixel;

typedef struct {
	int width;
	int height;
	RGBPixel **pixels;
	int max;
} RGB_Image;

typedef struct {
	int width;
	int height;
	RGBPixel **pixels;
	int max;
} GRAY_Image;

int resize(RGB_Image *src, RGB_Image *dst, char *cmd);
int rotate(RGB_Image *src, RGB_Image *dst, char *cmd);
void flip_RGB(RGB_Image *img, char *cmd);
void grayscale(RGB_Image *src, GRAY_Image *dst);
void colorSmooth(RGB_Image *src, RGB_Image *outsrc);

int main(int argc, char **argv) {
	FILE *fp = fopen(*(argv+1), "rb");
	char m, n;
	RGB_Image r1, output, curr;
	GRAY_Image g_output;
	int i, j;
	char cmd[20];
	if (fp == NULL)
	{
		printf("Reading error!\n");
	}
	//매직넘버, 넓이, 높이, max 값 받기
	fscanf(fp, "%c%c\n", &m, &n);
	fscanf(fp, "%d %d\n", &r1.width, &r1.height);
	fscanf(fp, "%d\n", &r1.max);
	
	//pixel의 RGB값을 저장할 2차원배열 선언
	r1.pixels = (RGBPixel**)malloc(sizeof(RGBPixel*)*r1.height);
	for (i = 0; i < r1.height; i++) {
		r1.pixels[i] = (RGBPixel*)malloc(sizeof(RGBPixel)*r1.width);
	}
	
	//pixel값 초기화
	for (i = 0; i < r1.height; i++) {
		for (j = 0; j < r1.width; j++) {
			r1.pixels[i][j].RGB[0] = 0;
			r1.pixels[i][j].RGB[1] = 0;
			r1.pixels[i][j].RGB[2] = 0;
		}
	}

	//pixel의 RGB값 저장
	for (i = 0; i < r1.height; i++) {
		for (j = 0; j < r1.width; j++) {
			fscanf(fp, "%c", &r1.pixels[i][j].RGB[0]);
			fscanf(fp, "%c", &r1.pixels[i][j].RGB[1]);
			fscanf(fp, "%c", &r1.pixels[i][j].RGB[2]);
		}
	}
	//커맨드 메뉴 작성
	printf("----------Command----------\n");
	printf("1. scaleup\n");		//이미지 확대
	printf("2. scaledown\n");	//이미지 축소
	printf("3. left\n");		//좌로 90도 회전
	printf("4. right\n");		//우로 90도 회전
	printf("5. horizon\n");		//수평 뒤집기
	printf("6. vertical\n");	//좌우 뒤집기
	printf("7. grayscale\n");	//흑백처리
	printf("8. smoothing\n");	//스무딩
	printf("9. print\n");		//아무런 기능도 실행하지 않고 그대로 출력
	printf("10. end\n");		//종료
	printf("---------------------------\n");
	curr = r1;
	while (1) {
		//커맨드에 따라서 함수 실행
		printf("Input the command : ");
		scanf("%s", cmd);
		if (strcmp(cmd, "scaleup") == 0 || strcmp(cmd, "scaledown") == 0) {
			resize(&curr, &output, cmd);
			curr = output;
		}
		if (strcmp(cmd, "left") == 0 || strcmp(cmd, "right") == 0) {
			rotate(&curr, &output, cmd);
			curr = output;
		}
		if (strcmp(cmd, "horizon") == 0 || strcmp(cmd, "vertical") == 0) {
			flip_RGB(&curr, cmd);
		}
		if (strcmp(cmd, "grayscale") == 0) {
			grayscale(&curr, &g_output);
			for (i = 0; i < curr.height; i++) {
				for (j = 0; j < curr.width; j++) {
					curr.pixels[i][j] = g_output.pixels[i][j];
				}
			}
		}
		if (strcmp(cmd, "smoothing") == 0) {
			colorSmooth(&curr, &output);
			curr = output;
		}
		if (strcmp(cmd, "print") == 0) {
			//파일 저장
			FILE *fp2 = fopen("print.ppm", "w");
			if (fp2 == NULL) {
				printf("Writing Error! \n");
				return 1;
			}
			fprintf(fp2, "%c%c\n", 'P', '3');
			fprintf(fp2, "%d %d\n", r1.width, r1.height);
			fprintf(fp2, "%d\n", r1.max);
			for (i = 0; i < r1.height; i++) {
				for (j = 0; j < r1.width; j++) {
					fprintf(fp2, "%d ", r1.pixels[i][j].RGB[0]);
					fprintf(fp2, "%d ", r1.pixels[i][j].RGB[1]);
					fprintf(fp2, "%d ", r1.pixels[i][j].RGB[2]);
				}
				fprintf(fp2, "\n");
			}
			fclose(fp2);
		}
		if (strcmp(cmd, "end") == 0) {
			return 0;
		}
	}
	
	fclose(fp);
	free(r1.pixels);

	return 0;
}

int resize(RGB_Image *src, RGB_Image *dst, char *cmd)
{
	int i, j;
	int temp = 0;
	//기존 src의 가로, 세로의 크기를 두 배로 늘려서 동적할당
	if (strcmp(cmd, "scaleup") == 0) {
		dst->pixels = (RGBPixel **)malloc(sizeof(RGBPixel*)*src->height*2);
		for (i = 0; i < src->height*2; i++) {
			dst->pixels[i] = (RGBPixel*)malloc(sizeof(RGBPixel)*src->width*2);
		}
		dst->height = src->height * 2;
		dst->width = src->width * 2;
		dst->max = src->max;
		//확대의 경우, 각각의 픽셀 값을 가로로 2번, 세로로 2번씩 찍는다.
		for (i = 0; i < dst->height; i+=2) {
			for (j = 0; j < dst->width; j+=2) {
				dst->pixels[i][j] = src->pixels[(i + 1) / 2][(j + 1) / 2];
				dst->pixels[i][j+1] = src->pixels[(i + 1) / 2][(j + 1) / 2];
				dst->pixels[i+1][j] = src->pixels[(i + 1) / 2][(j + 1) / 2];
				dst->pixels[i+1][j+1] = src->pixels[(i + 1) / 2][(j + 1) / 2];
			}
		}
		//파일 저장
		FILE *fp = fopen("scaleup_output.ppm", "w");
		if (fp == NULL) {
			printf("Writing Error!\n");
			return 1;
		}
		fprintf(fp, "%c%c\n", 'P', '3');
		fprintf(fp, "%d %d\n", dst->width, dst->height);
		fprintf(fp, "%d\n", dst->max);
		for (i = 0; i < dst->height; i++) {
			for (j = 0; j < dst->width; j++) {
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[0]);
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[1]);
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[2]);
			}
			fprintf(fp, "\n");
		}
		
	}
	//기존 src의 가로, 세로의 크기를 절반으로 줄여서 동적할당
	if (strcmp(cmd ,"scaledown")==0) {
		//픽셀의 크기가 1픽셀보다 줄어들 경우 에러 메시지
		if (src->height / 2 < 1 || src->width / 2 < 1) {
			printf("Size error! smaller than 1px.");
			return 1;
		}
		else {
			dst->pixels = (RGBPixel **)malloc(sizeof(RGBPixel*)*src->height / 2);
			for (i = 0; i < src->height / 2; i++) {
				dst->pixels[i] = (RGBPixel*)malloc(sizeof(RGBPixel)*src->width / 2);
			}
			dst->height = src->height / 2;
			dst->width = src->width / 2;
			dst->max = src->max;
			//축소의 경우, 4칸의 픽셀의 값을 평균을 구하여 dst픽셀 한 칸에 저장한다.
			if (src->height % 2 != 0) {			//높이가 홀수일 때
				for (i = 0; i < src->height - 1; i += 2) {
					for (j = 0; j < src->width; j += 2) {
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[0] = (src->pixels[i][j].RGB[0] + src->pixels[i][j + 1].RGB[0] + src->pixels[i + 1][j].RGB[0] + src->pixels[i + 1][j + 1].RGB[0]) / 4;
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[1] = (src->pixels[i][j].RGB[1] + src->pixels[i][j + 1].RGB[1] + src->pixels[i + 1][j].RGB[1] + src->pixels[i + 1][j + 1].RGB[1]) / 4;
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[2] = (src->pixels[i][j].RGB[2] + src->pixels[i][j + 1].RGB[2] + src->pixels[i + 1][j].RGB[2] + src->pixels[i + 1][j + 1].RGB[2]) / 4;
					}
				}
			}
			else if (src->width % 2 != 0) {		//너비가 홀수일 때
				for (i = 0; i < src->height; i += 2) {
					for (j = 0; j < src->width - 1; j += 2) {
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[0] = (src->pixels[i][j].RGB[0] + src->pixels[i][j + 1].RGB[0] + src->pixels[i + 1][j].RGB[0] + src->pixels[i + 1][j + 1].RGB[0]) / 4;
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[1] = (src->pixels[i][j].RGB[1] + src->pixels[i][j + 1].RGB[1] + src->pixels[i + 1][j].RGB[1] + src->pixels[i + 1][j + 1].RGB[1]) / 4;
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[2] = (src->pixels[i][j].RGB[2] + src->pixels[i][j + 1].RGB[2] + src->pixels[i + 1][j].RGB[2] + src->pixels[i + 1][j + 1].RGB[2]) / 4;
					}
				}
			}
			else {								//높이, 너비가 짝수일 때
				for (i = 0; i < src->height; i += 2) {
					for (j = 0; j < src->width; j += 2) {
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[0] = (src->pixels[i][j].RGB[0] + src->pixels[i][j + 1].RGB[0] + src->pixels[i + 1][j].RGB[0] + src->pixels[i + 1][j + 1].RGB[0]) / 4;
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[1] = (src->pixels[i][j].RGB[1] + src->pixels[i][j + 1].RGB[1] + src->pixels[i + 1][j].RGB[1] + src->pixels[i + 1][j + 1].RGB[1]) / 4;
						dst->pixels[(i + 1) / 2][(j + 1) / 2].RGB[2] = (src->pixels[i][j].RGB[2] + src->pixels[i][j + 1].RGB[2] + src->pixels[i + 1][j].RGB[2] + src->pixels[i + 1][j + 1].RGB[2]) / 4;
					}
				}
			}	
			//파일 저장
			FILE *fp = fopen("scaledown_output.ppm", "w");
			if (fp == NULL) {
				printf("Writing Error!\n");
				return 1;
			}
			fprintf(fp, "%c%c\n", 'P', '3');
			fprintf(fp, "%d %d\n", dst->width, dst->height);
			fprintf(fp, "%d\n", dst->max);
			for (i = 0; i < dst->height; i++) {
				for (j = 0; j < dst->width; j++) {
					fprintf(fp, "%d ", dst->pixels[i][j].RGB[0]);
					fprintf(fp, "%d ", dst->pixels[i][j].RGB[1]);
					fprintf(fp, "%d ", dst->pixels[i][j].RGB[2]);
				}
				fprintf(fp, "\n");
			}
		}
	}
}

int rotate(RGB_Image *src, RGB_Image *dst, char *cmd)
{
	int i, j;
	//dst에 pixel을 회전했을 때 오는 width, height를 이용하여 동적할당. 회전했으므로 가로 세로 크기를 거꾸로 선언한다.
	dst->pixels = (RGBPixel **)malloc(sizeof(RGBPixel*)*src->width);
	for (i = 0; i < src->width; i++) {
		dst->pixels[i] = (RGBPixel*)malloc(sizeof(RGBPixel)*src->height);
	}
	dst->width = src->height;
	dst->height = src->width;
	dst->max = src->max;
	//왼쪽으로 90도 회전 --> 픽셀 값을 왼쪽 아래에서 위로 입력 받는다. 
	if (strcmp(cmd, "left") == 0) {
		for (i = 0; i < dst->height; i++) {
			for (j = 0; j < dst->width; j++) {
				dst->pixels[i][j].RGB[0] = 0;
				dst->pixels[i][j].RGB[1] = 0;
				dst->pixels[i][j].RGB[2] = 0;
			}
		}
		for (i = 0; i < src->height; i++) {
			for (j = 0; j < src->width; j++) {
				dst->pixels[src->width - j - 1][i].RGB[0] = src->pixels[i][j].RGB[0];
				dst->pixels[src->width - j - 1][i].RGB[1] = src->pixels[i][j].RGB[1];
				dst->pixels[src->width - j - 1][i].RGB[2] = src->pixels[i][j].RGB[2];
			}
		}
		//파일 저장
		FILE *fp = fopen("left_output.ppm", "w");
		if (fp == NULL) {
			printf("Writing Error! \n");
			return 1;
		}
		fprintf(fp, "%c%c\n", 'P', '3');
		fprintf(fp, "%d %d\n", dst->width, dst->height);
		fprintf(fp, "%d\n", dst->max);
		for (i = 0; i < dst->height; i++) {
			for (j = 0; j < dst->width; j++) {
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[0]);
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[1]);
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[2]);
			}
			fprintf(fp, "\n");
		}
		
		fclose(fp);
	}
	//오른쪽으로 90도 회전 --> 픽셀 값을 오른쪽 위에서 아래로 입력 받는다.
	if (strcmp(cmd, "right") == 0) {
		for (i = 0; i < dst->height; i++) {
			for (j = 0; j < dst->width; j++) {
				dst->pixels[j][dst->height - i-1] = src->pixels[i][j];
			}
		}
		//파일 저장
		FILE *fp = fopen("right_output.ppm", "w");
		if (fp == NULL) {
			printf("Writing Error! \n");
			return 1;
		}
		fprintf(fp, "%c%c\n", 'P', '3');
		fprintf(fp, "%d %d\n", dst->width, dst->height);
		fprintf(fp, "%d\n", dst->max);
		for (i = 0; i < dst->height; i++) {
			for (j = 0; j < dst->width; j++) {
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[0]);
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[1]);
				fprintf(fp, "%d ", dst->pixels[i][j].RGB[2]);
			}
			fprintf(fp, "\n");
		}
	}
}

void flip_RGB(RGB_Image *img, char *cmd)
{
	int i, j;
	RGBPixel temp;
	if (strcmp(cmd, "horizon")==0) {
		//행의 시작, 끝 픽셀을 세로 픽셀의 절반만큼 서로 바꾼다.
		for (i = 1; i < (img->height + 1) / 2; i++)
		{
			for (j = 1; j < img->width; j++) {
				temp = img->pixels[i][j];
				img->pixels[i][j] = img->pixels[img->height-i-1][j];
				img->pixels[img->height - i-1][j] = temp;
			}
		}
		//파일 저장
		FILE *fp = fopen("horizon_output.ppm", "w");
		if (fp == NULL) {
			printf("Writing Error! \n");
		}
		fprintf(fp, "%c%c\n", 'P', '3');
		fprintf(fp, "%d %d\n", img->width, img->height);
		fprintf(fp, "%d\n", img->max);

		for (i = 0; i < img->height; i++) {
			for (j = 0; j < img->width; j++) {
				fprintf(fp, "%d ", img->pixels[i][j].RGB[0]);
				fprintf(fp, "%d ", img->pixels[i][j].RGB[1]);
				fprintf(fp, "%d ", img->pixels[i][j].RGB[2]);
			}
			fprintf(fp, "\n");
		}
	}
	if (strcmp(cmd, "vertical") == 0) {
		//열의 시작과 끝의 픽셀을 가로 크기의 절반만큼 서로 바꾼다.
		for (i = 0; i < img->height; i++)
		{
			for (j = 0; j < (img->width+1)/2; j++) {
				temp = img->pixels[i][j];
				img->pixels[i][j] = img->pixels[i][img->width-j-1];
				img->pixels[i][img->width - j-1] = temp;
			}
		}
		//파일 저장
		FILE *fp = fopen("vertical_output.ppm", "w");
		if (fp == NULL) {
			printf("Writing Error! \n");
		}
		fprintf(fp, "%c%c\n", 'P', '3');
		fprintf(fp, "%d %d\n", img->width, img->height);
		fprintf(fp, "%d\n", img->max);
		for (i = 0; i < img->height; i++) {
			for (j = 0; j < img->width; j++) {
				fprintf(fp, "%d ", img->pixels[i][j].RGB[0]);
				fprintf(fp, "%d ", img->pixels[i][j].RGB[1]);
				fprintf(fp, "%d ", img->pixels[i][j].RGB[2]);
			}
			fprintf(fp, "\n");
		}
	}
}

void grayscale(RGB_Image *src, GRAY_Image *dst)
{
	int i, j;
	//src와 같은 크기의 dst 동적 할당
	dst->pixels = (RGBPixel**)malloc(sizeof(RGBPixel*)*src->height);
	for (i = 0; i < src->height; i++) {
		dst->pixels[i] = (RGBPixel*)malloc(sizeof(RGBPixel)*src->width);
	}
	dst->height = src->height;
	dst->width = src->width;
	dst->max = src->max;
	//dst->pixel에 src->pixel의 RGB 평균값 저장
	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			dst->pixels[i][j].RGB[0] = (src->pixels[i][j].RGB[0] + src->pixels[i][j].RGB[1] + src->pixels[i][j].RGB[2]) / 3;
			dst->pixels[i][j].RGB[1] = (src->pixels[i][j].RGB[0] + src->pixels[i][j].RGB[1] + src->pixels[i][j].RGB[2]) / 3;
			dst->pixels[i][j].RGB[2] = (src->pixels[i][j].RGB[0] + src->pixels[i][j].RGB[1] + src->pixels[i][j].RGB[2]) / 3;
		}
	}
	//파일 저장
	FILE *fp = fopen("grayscale_output.ppm", "w");
	if (fp == NULL) {
		printf("Writing Error! \n");
	}
	fprintf(fp, "%c%c\n", 'P', '3');
	fprintf(fp, "%d %d\n", dst->width, dst->height);
	fprintf(fp, "%d\n", dst->max);
	for (i = 0; i < dst->height; i++) {
		for (j = 0; j < dst->width; j++) {
			fprintf(fp, "%d ", dst->pixels[i][j].RGB[0]);
			fprintf(fp, "%d ", dst->pixels[i][j].RGB[1]);
			fprintf(fp, "%d ", dst->pixels[i][j].RGB[2]);
		}
		fprintf(fp, "\n");
	}
}

void colorSmooth(RGB_Image *src, RGB_Image *outsrc)
{
	int i, j;
	//src 픽셀의 크기만큼 outsrc 픽셀 배열을 동적할당
	outsrc->pixels = (RGBPixel**)malloc(sizeof(RGBPixel*)*src->height);
	for (i = 0; i < src->height; i++) {
		outsrc->pixels[i] = (RGBPixel*)malloc(sizeof(RGBPixel)*src->width);
	}
	outsrc->height = src->height;
	outsrc->width = src->width;
	outsrc->max = src->max;
	//outsrc에 src의 픽셀값을 복사한다.
	for (i = 0; i < outsrc->height; i++) {
		for (j = 0; j < outsrc->width; j++) {
			outsrc->pixels[i][j] = src->pixels[i][j];
			}
	}
	//3x3 필터를 이용하여 스무딩한다.
	for (i = 0; i < outsrc->height-2; i++) {
		for (j = 0; j < outsrc->width-2; j++) {
			outsrc->pixels[i+1][j+1].RGB[0] = (src->pixels[i][j].RGB[0] + src->pixels[i][j + 1].RGB[0] + src->pixels[i][j+2].RGB[0] + src->pixels[i + 1][j].RGB[0] + +src->pixels[i + 1][j + 1].RGB[0]+ src->pixels[i + 1][j+2].RGB[0] + src->pixels[i+2][j].RGB[0] + src->pixels[i+2][j+1].RGB[0] + src->pixels[i+2][j+2].RGB[0]) / 9;
			outsrc->pixels[i+1][j + 1].RGB[1] = (src->pixels[i][j].RGB[1] + src->pixels[i][j + 1].RGB[1] + src->pixels[i][j + 2].RGB[1] + src->pixels[i + 1][j].RGB[1] + +src->pixels[i + 1][j + 1].RGB[1] + src->pixels[i + 1][j + 2].RGB[1] + src->pixels[i + 2][j].RGB[1] + src->pixels[i + 2][j + 1].RGB[1] + src->pixels[i + 2][j + 2].RGB[1]) / 9;
			outsrc->pixels[i + 1][j + 1].RGB[2] = (src->pixels[i][j].RGB[2] + src->pixels[i][j + 1].RGB[2] + src->pixels[i][j + 2].RGB[2] + src->pixels[i + 1][j].RGB[2] + +src->pixels[i + 1][j + 1].RGB[2] + src->pixels[i + 1][j + 2].RGB[2] + src->pixels[i + 2][j].RGB[2] + src->pixels[i + 2][j + 1].RGB[2] + src->pixels[i + 2][j + 2].RGB[2]) / 9;
		}
	}
	//파일 저장
	FILE *fp = fopen("smooth_output.ppm", "w");
	if (fp == NULL) {
		printf("Writing Error!\n");
	}
	fprintf(fp, "%c%c\n", 'P', '3');
	fprintf(fp, "%d %d\n", outsrc->width, outsrc->height);
	fprintf(fp, "%d\n", outsrc->max);
	for (i = 0; i < outsrc->height; i++) {
		for (j = 0; j < outsrc->width; j++) {
			fprintf(fp, "%d ", outsrc->pixels[i][j].RGB[0]);
			fprintf(fp, "%d ", outsrc->pixels[i][j].RGB[1]);
			fprintf(fp, "%d ", outsrc->pixels[i][j].RGB[2]);
		}
		fprintf(fp, "\n");
	}
}

