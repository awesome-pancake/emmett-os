

int main(){
    // Fill in a pixel
    char *px = (char*)0xC0000000;
    *px = 0xFF;
    for(;;);
}