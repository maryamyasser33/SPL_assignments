
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    FILE  *fptr1,*fptr2;
    fptr1 = fopen(argv[1], "r");

     if (fptr1 == NULL ) {
        printf("error has occured");
        return -1;
    }
    fptr2 = fopen(argv[2], "w");
    if (fptr2 == NULL ) {
        printf("error has occured");
        return -1;
    }
    char c = fgetc(fptr1);
    while(c!=EOF){fputc(c,fptr2);c = fgetc(fptr1);}
    fclose(fptr1);
    fclose(fptr2);


  
  const char *file = argv[1];  
    if (remove(file) != 0) {
      return -1;
    }

    return 0;

}
