#include <stdio.h>
#include <unistd.h>

int main(){
        char currwd[1024];
        getcwd(currwd,1024);
        if(currwd)
        {printf("%s\n",currwd);
                return 0;}
        else {printf("error");
                return -1;}

}
