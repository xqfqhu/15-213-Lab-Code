#include <unistd.h>
int main(){

    
    write(1, "ab ", 4);
    write(1, "c", 2);
    write(1, "\n", 2);
    write(1, "d", 2);
    return 1;
}