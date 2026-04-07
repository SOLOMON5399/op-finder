#include <stdio.h>

int main() {

    int image[5][5];
    int edge[5][5];

    int i,j;

    for(i=1;i<4;i++){
        for(j=1;j<4;j++){

            int gx =
                image[i-1][j-1] + 2*image[i][j-1] + image[i+1][j-1]
              - image[i-1][j+1] - 2*image[i][j+1] - image[i+1][j+1];

            int gy =
                image[i-1][j-1] + 2*image[i-1][j] + image[i-1][j+1]
              - image[i+1][j-1] - 2*image[i+1][j] - image[i+1][j+1];

            edge[i][j] = gx + gy;
        }
    }

    printf("Edge detection done\n");

    return 0;
}
