#include <stdio.h>

int main() {
    int image[5][5];
    int blur[5][5];

    for(int i=1;i<4;i++){
        for(int j=1;j<4;j++){
            blur[i][j] = (
                image[i][j] +
                image[i-1][j] +
                image[i+1][j] +
                image[i][j-1] +
                image[i][j+1]
            ) / 5;
        }
    }

    printf("Blur done\n");
    return 0;
}
