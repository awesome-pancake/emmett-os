

int main(){
    // Fill in a pixel
    char *px = (char*)0xC0000002;
    *px = 0xFF;
    for(;;);
}