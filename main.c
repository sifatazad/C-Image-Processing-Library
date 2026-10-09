#include <stdio.h>
#include "img_processing.h"

int main(void) {
    int h,w;
    char *img_address="C:/Users/Rifat/Downloads/amar.jpg";
    char *new_address="C:/Users/Rifat/Downloads/gauss.jpg";
    char *text_address="C:/Users/Rifat/OneDrive/Documents/pixel.txt";
    get_properties(img_address,&w,&h,3);
    printf("%d %d",w,h);
    unsigned char ***ptr_3d;
    get_3D_pixel_array(img_address,&ptr_3d,3);
    pixel_array_to_jpg("C:/Users/Rifat/Downloads/2-3.jpg",ptr_3d,w,h,3);
    for (int i=0; i<h; i++) {
        for (int j=0; j<w; j++) {
            free(ptr_3d[i][j]);
        }
        free(ptr_3d[i]);
    }
    free(ptr_3d);
    save_3D_pixel_array(text_address,img_address,3);
    crop_img(img_address,new_address,3,430,400,850,400,430,1100,850,1100);
    adjust_brightness(img_address,new_address,3,-30);
    resize_img(img_address,new_address,3,w,h,500,600,"nearest");
    resize_img(img_address,new_address,3,w,h,500,600,"bilinear");
    resize_img(img_address,new_address,3,w,h,500,600,"bicubic");
    blur_img(img_address,new_address,3,597,448,800,1100,7,"box");
    blur_img(img_address,new_address,3,597,448,800,1100,5,"gaussian");
    blur_img(img_address,new_address,3,597,448,800,1000,5,"median");
    mirror_img(img_address,new_address,3,"vertical");
    mirror_img(img_address,new_address,3,"horizontal");
}
