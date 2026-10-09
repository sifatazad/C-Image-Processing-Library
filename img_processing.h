//
// Created by Rifat on 10/9/2026.
//

#ifndef IMAGE_PROJECT_IMG_PROCESSING_H
#define IMAGE_PROJECT_IMG_PROCESSING_H

#endif //IMAGE_PROJECT_IMG_PROCESSING_H
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include<stb_image.h>
#include<stb_image_write.h>

#define max(a,b) (((a)>=(b))?(a):(b))
#define min(a,b) (((a)<=(b))?(a):(b))
#define Abs(x) ((x)>=0?(x):-(x))

// it's for sorting in median blur
static int compare_bytes(const void *a, const void *b) {
    return (*(unsigned char*)a - *(unsigned char*)b);
}

// for checking the file extension
static int is_jpg(const char *address) {
    if (!address)
        return 0;
    const char *dot= strrchr(address,'.');
    if (!dot || dot==address)
        return 0;
    if (!strcasecmp(".jpg",dot) || !strcasecmp(".jpeg",dot))
        return 1;
    return 0;
}

// for checking the file extension
static int is_png(const char *address) {
    if (!address)
        return 0;
    const char *dot= strrchr(address,'.');
    if (!dot || dot==address)
        return 0;
    if (!strcasecmp(".png",dot))
        return 1;
    return 0;
}

// for getting the value of the h,w of image
static void get_properties(const char *img_address, int *w, int *h, const int cmp) {
    int chn;
    unsigned char *rgb_array=stbi_load(img_address,w,h,&chn,cmp);
    if (!rgb_array)
        return;
    stbi_image_free(rgb_array);
}

// This function was made for getting pixel values of the image in the form of 3D array.
// But must notice that an address of a 3d pointer is passed into the function.
// You must free the pointer in the place you have declared it, mustn't free here.
static void get_3D_pixel_array(const char *img_address, unsigned char ****pixel, const int cmp) {
    int width,height,chn;

    unsigned char *rgb_array=stbi_load(img_address,&width,&height,&chn,cmp);
    if (!rgb_array) {
        return;
    }

    (*pixel)=(unsigned char***)malloc(height*sizeof(unsigned char**));
    if (!(*pixel)) {
        stbi_image_free(rgb_array);
        return;
    }
    for (int i=0; i<height; i++) {
        (*pixel)[i]=(unsigned char**)malloc(width*sizeof(unsigned char*));
        for (int j=0; j<width; j++) {
            (*pixel)[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
            int base=(i*width+j)*cmp;
            for (int k=0; k<cmp; k++) {
                (*pixel)[i][j][k]=rgb_array[base+k];
            }
        }
    }
    stbi_image_free(rgb_array);
}

// This function saves the 3D array in text files
static void save_3D_pixel_array(const char *text_address, const char *img_address, const int cmp) {
    int h,w,chn;
    unsigned char *rgb_array=stbi_load(img_address,&w,&h,&chn,cmp);
    if (!rgb_array)
        return;

    unsigned char ***pixel= (unsigned char***)malloc(h*sizeof(unsigned char**));
    if (!pixel) {
        stbi_image_free(rgb_array);
        return;
    }
    for (int i=0; i<h; i++) {
        pixel[i]=(unsigned char**)malloc(w*sizeof(unsigned char*));
        for (int j=0; j<w; j++) {
            pixel[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
            int base=(i*w+j)*cmp;
            for (int k=0; k<cmp; k++) {
                pixel[i][j][k]=rgb_array[base+k];
            }
        }
    }

    FILE *fp=fopen(text_address,"w");
    if (!fp) {
        stbi_image_free(rgb_array);
        for (int i=0; i<h; i++) {
            for (int j=0; j<w; j++) {
                free(pixel[i][j]);
            }
            free(pixel[i]);
        }
        free(pixel);
        return;
    }

    for (int i=0; i<h; i++) {
        for (int j=0; j<w; j++) {
            for (int k=0; k<cmp; k++) {
                fprintf(fp,"%-3u ",pixel[i][j][k]);
            }
        }
        fprintf(fp,"\n");
    }

    stbi_image_free(rgb_array);
    for (int i=0; i<h; i++) {
        for (int j=0; j<w; j++) {
            free(pixel[i][j]);
        }
        free(pixel[i]);
    }
    free(pixel);
    fclose(fp);
}

// Btw before using these func, u must declare a pointer in main(). so pixel is not freed in
// the function. it must be freed in the main().
static void pixel_array_to_jpg(const char *new_img_address, unsigned char ***pixel,
    const int w, const int h, const int cmp) {
    if (!pixel) {
        return;
    }

    unsigned char *flat=(unsigned char*)malloc(sizeof(unsigned char)*h*w*cmp);
    if (!flat) {
        return;
    }
    for (int i=0; i<h; i++) {
        for (int j=0; j<w; j++) {
            for (int k=0; k<cmp; k++) {
                flat[(i*w+j)*cmp+k]=pixel[i][j][k];
            }
        }
    }

    stbi_write_jpg(new_img_address,w,h,cmp,flat,95);
    free(flat);
}

// Btw before using these func, u must declare a pointer in main(). so pixel is not freed in
// the function. it must be freed in the main().
static void pixel_array_to_png(const char *new_img_address, unsigned char ***pixel,
    const int w, const int h, const int cmp) {
    if (!pixel) {
        return;
    }

    unsigned char *flat=(unsigned char*)malloc(sizeof(unsigned char)*h*w*cmp);
    if (!flat) {
        return;
    }
    for (int i=0; i<h; i++) {
        for (int j=0; j<w; j++) {
            for (int k=0; k<cmp; k++) {
                flat[(i*w+j)*cmp+k]=pixel[i][j][k];
            }
        }
    }

    stbi_write_png(new_img_address,w,h,cmp,flat,w*cmp);
    free(flat);
}

// We must remember that here x,y is co-ordinates not row or column number!!
static void crop_img(const char *img_address,const char *new_address, const int cmp,
    const int x1,const int y1,const int x2,const int y2,
    const int x3,const int y3,const int x4,const int y4) {
    int w,h,chn;
    unsigned char *rgb_array= stbi_load(img_address,&w,&h,&chn,cmp);
    if (!rgb_array)
        return;

    unsigned char ***pixel= (unsigned char***)malloc((y3-y1)*sizeof(unsigned char**));
    if (!pixel) {
        stbi_image_free(rgb_array);
        return;
    }
    for (int i=0; i<y3-y1; i++) {
        pixel[i]=(unsigned char**)malloc((x2-x1)*sizeof(unsigned char*));
        for (int j=0; j<x2-x1; j++) {
            pixel[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
            int base=((i+y1)*w+(j+x1))*cmp;
            for (int k=0; k<cmp; k++) {
                pixel[i][j][k]=rgb_array[base+k];
            }
        }
    }

    if (is_jpg(new_address))
        pixel_array_to_jpg(new_address,pixel,x2-x1,y3-y1,cmp);
    else if (is_png(new_address))
        pixel_array_to_png(new_address,pixel,x2-x1,y3-y1,cmp);

    for (int i=0; i<y3-y1; i++) {
        for (int j=0; j<x2-x1; j++) {
            free(pixel[i][j]);
        }
        free(pixel[i]);
    }
    free(pixel);
    stbi_image_free(rgb_array);
}

// Here we have to pass an integer by which the brightness will change
static void adjust_brightness(const char *img_address,const char *new_address,const int cmp,const int a) {
    int h,w,chn;
    unsigned char *rgb_array=stbi_load(img_address,&w,&h,&chn,cmp);
    if (!rgb_array)
        return;

    unsigned char ***pixel= (unsigned char***)malloc(h*sizeof(unsigned char**));
    if (!pixel) {
        stbi_image_free(rgb_array);
        return;
    }
    for (int i=0; i<h; i++) {
        pixel[i]=(unsigned char**)malloc(w*sizeof(unsigned char*));
        for (int j=0; j<w; j++) {
            pixel[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
            int base=(i*w+j)*cmp;
            for (int k=0; k<cmp; k++) {
                pixel[i][j][k]=rgb_array[base+k];
            }
        }
    }
    for (int i=0; i<h; i++) {
        for (int j=0; j<w; j++) {
            for (int k=0; k<cmp; k++) {
                if (rgb_array[(i*w+j)*cmp+k]+a>255)
                    pixel[i][j][k]=255;
                else if (rgb_array[(i*w+j)*cmp+k]+a<0)
                    pixel[i][j][k]=0;
                else
                    pixel[i][j][k]=rgb_array[(i*w+j)*cmp+k]+a;
            }
        }
    }

    if (is_jpg(new_address))
        pixel_array_to_jpg(new_address,pixel,w,h,cmp);
    else if (is_png(new_address))
        pixel_array_to_png(new_address,pixel,w,h,cmp);
    stbi_image_free(rgb_array);

    for (int i=0; i<h; i++) {
        for (int j=0; j<w; j++) {
            free(pixel[i][j]);
        }
        free(pixel[i]);
    }
    free(pixel);
}

// Here x,y means scaled_row, scaled_col
static unsigned char bilinear_interpolate(const float x, const float y,
    const int a, const int b, const int c, const int d) {
    int xl=floor(x);
    int yl=floor(y);
    float v1= (1-x+xl)*a+(x-xl)*b;
    float v2= (1-x+xl)*c+(x-xl)*d;
    float v3= (1-y+yl)*v1+(y-yl)*v2;
    return (unsigned char)((int)(round(v3)));
}

// Here w_,h_ means resized width and resized height respectively
static void resize_img(const char *img_address,const char *new_address,const int cmp,
    int w, int h, const int w_, const int h_, const char *type) {
    const float row_fact=(float)h/h_;
    const float col_fact=(float)w/w_;
    int chn;
    unsigned char *rgb_array=stbi_load(img_address,&w,&h,&chn,cmp);
    if (!rgb_array)
        return;

    unsigned char ***new_pixel= (unsigned char***)malloc(h_*sizeof(unsigned char**));
    if (!new_pixel) {
        stbi_image_free(rgb_array);
        return;
    }
    for (int i=0; i<h_; i++) {
        new_pixel[i]=(unsigned char**)malloc(w_*sizeof(unsigned char*));
        for (int j=0; j<w_; j++) {
            new_pixel[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
        }
    }

    if (!strcmp(type,"nearest")) {
        for (int i=0; i<h_; i++) {
            for (int j=0; j<w_; j++) {
                for (int k=0; k<cmp; k++) {
                    new_pixel[i][j][k]=rgb_array[((int)(min(h-1,round(i*row_fact))*w+
                    min(w-1,round(j*col_fact))))*cmp+k];
                }
            }
        }
    }

    else if (!strcmp(type,"bilinear")) {
        for (int i=0; i<h_; i++) {
            for (int j=0; j<w_; j++) {
                for (int k=0; k<cmp; k++) {
                    float scaled_row=min(h-1,i*row_fact);
                    float scaled_col=min(w-1,j*col_fact);
                    int x_up=ceil(scaled_row);
                    int x_low=floor(scaled_row);
                    int y_up=ceil(scaled_col);
                    int y_low=floor(scaled_col);
                    new_pixel[i][j][k]=bilinear_interpolate(scaled_row,scaled_col,
                        rgb_array[(x_low*w+y_low)*cmp+k],rgb_array[(x_up*w+y_low)*cmp+k],
                        rgb_array[(x_low*w+y_up)*cmp+k],rgb_array[(x_up*w+y_up)*cmp+k]);
                }
            }
        }
    }

    else if (!strcmp(type,"bicubic")) {
        for (int i=0; i<h_; i++) {
            for (int j=0; j<w_; j++) {
                for (int kk=0; kk<cmp; kk++) {
                    float scaled_row=min(h-1,i*row_fact);
                    float scaled_col=min(w-1,j*col_fact);
                    float w_row[4]={0};
                    float dr=Abs(scaled_row-floor(scaled_row));
                    float dc=Abs(scaled_col-floor(scaled_col));
                    for (int k=0; k<4; k++) {
                        float d=Abs(dc-(k-1));
                        if (d<=1)
                            w_row[k]=1.5*d*d*d-2.5*d*d+1;
                        else if (d<2)
                            w_row[k]=-0.5*d*d*d+2.5*d*d-4*d+2;
                        else
                            w_row[k]=0;
                    }
                    float row[4]={0};
                    for (int k=0; k<4; k++) {
                        int r;
                        if (k==0)
                            r=floor(scaled_row)-1;
                        else if (k==1)
                            r=floor(scaled_row);
                        else if (k==2)
                            r=floor(scaled_row)+1;
                        else
                            r=floor(scaled_row)+2;
                        float s=0;
                        for (int l=0; l<4; l++) {
                            int c;
                            if (l==0)
                                c=floor(scaled_col)-1;
                            else if (l==1)
                                c=floor(scaled_col);
                            else if (l==2)
                                c=floor(scaled_col)+1;
                            else
                                c=floor(scaled_col)+2;
                            if (r<0)
                                r=0;
                            if (r>=h)
                                r=h-1;
                            if (c<0)
                                c=0;
                            if (c>=w)
                                c=w-1;
                            s+=w_row[l]*rgb_array[(r*w+c)*cmp+kk];
                        }
                        if (s<0)
                            s=0;
                        else if (s>255)
                            s=255;
                        row[k]=s;
                    }
                    float w_col[4]={0};
                    for (int k=0; k<4; k++) {
                        float d=Abs(dr-(k-1));
                        if (d<=1)
                            w_col[k]=1.5*d*d*d-2.5*d*d+1;
                        else if (d<2)
                            w_col[k]=-0.5*d*d*d+2.5*d*d-4*d+2;
                        else
                            w_col[k]=0;
                    }
                    int val= (int)(round(row[0]*w_col[0]+row[1]*w_col[1]+
                        row[2]*w_col[2]+row[3]*w_col[3]));
                    if (val<0)
                        val=0;
                    else if (val>255)
                        val=255;
                    new_pixel[i][j][kk]=(unsigned char) val;
                }
            }
        }
    }

    if (is_jpg(new_address))
        pixel_array_to_jpg(new_address,new_pixel,w_,h_,cmp);
    else if (is_png(new_address))
        pixel_array_to_png(new_address,new_pixel,w_,h_,cmp);

    stbi_image_free(rgb_array);
    for (int i=0; i<h_; i++) {
        for (int j=0; j<w_; j++) {
            free(new_pixel[i][j]);
        }
        free(new_pixel[i]);
    }
    free(new_pixel);
}

// Here the address of a 2d pointer is passed. Which must be freed where it was declared.
static void kernel_matrix(const int a,float ***p) {
    *p=(float**)malloc(a*sizeof(float*));
    if (!(*p))
        return;
    for (int i=0; i<a; i++) {
        (*p)[i]=malloc(a*sizeof(float));
    }
    float s=0;
    for (int i=0; i<a; i++) {
        for (int j=0; j<a; j++) {
            (*p)[i][j]=exp(-((i-a/2)*(i-a/2)+(j-a/2)*(j-a/2))/2.0)/(2*M_PI);
            s+=(*p)[i][j];
        }
    }
    for (int i=0; i<a; i++) {
        for (int j=0; j<a; j++) {
            (*p)[i][j]/=s;
        }
    }
}

// Here row, col means the center of the blurred area's row and column number respectively.
// a is the length of the area along width and b is the length along height.
// c is the order of the square by which the blur purpose will be done (must be odd).
static void blur_img(const char *img_address,const char *new_address,const int cmp,
    const int row,const int col, const int a,const int b,const int c,const char *type) {
    int h,w,chn;
    int row_start=row-b/2;
    int col_start=col-a/2;
    int row_end,col_end;
    if (b%2)
        row_end=row+b/2;
    else
        row_end=row+b/2-1;
    if (a%2)
        col_end=col+a/2;
    else
        col_end=col+a/2-1;
    unsigned char *rgb_array=stbi_load(img_address,&w,&h,&chn,cmp);
    if (!rgb_array)
        return;

    unsigned char ***pixel= (unsigned char***)malloc(h*sizeof(unsigned char**));
    if (!pixel) {
        stbi_image_free(rgb_array);
        return;
    }
    for (int i=0; i<h; i++) {
        pixel[i]=(unsigned char**)malloc(w*sizeof(unsigned char*));
        for (int j=0; j<w; j++) {
            pixel[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
            int base=(i*w+j)*cmp;
            for (int k=0; k<cmp; k++) {
                pixel[i][j][k]=rgb_array[base+k];
            }
        }
    }

    unsigned char ***new_pixel= (unsigned char***)malloc(h*sizeof(unsigned char**));
    if (!new_pixel) {
        stbi_image_free(rgb_array);
        for (int ii=0; ii<h; ii++) {
            for (int jj=0; jj<w; jj++) {
                free(pixel[ii][jj]);
            }
            free(pixel[ii]);
        }
        free(pixel);
        return;
    }

    for (int i=0; i<h; i++) {
        new_pixel[i]=(unsigned char**)malloc(w*sizeof(unsigned char*));
        for (int j=0; j<w; j++) {
            new_pixel[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
            int base=(i*w+j)*cmp;
            for (int k=0; k<cmp; k++) {
                new_pixel[i][j][k]=pixel[i][j][k];
            }
        }
    }

    if (!strcmp(type,"box")) {
        for (int i=row_start; i<=row_end; i++) {
            if (i<0 || i>h-1)
                continue;
            for (int j=col_start; j<=col_end; j++) {
                if (j<0 || j>w-1)
                    continue;
                for (int kk=0; kk<cmp; kk++) {
                    long long s=0;
                    int count=0;
                    for (int k=i-c/2; k<=i+c/2; k++) {
                        for (int l=j-c/2; l<=j+c/2; l++) {
                            if (k<0 || k>(h-1) || l<0 || l>(w-1))
                                s+=0;
                            else {
                                s+=pixel[k][l][kk];
                                count++;
                            }
                        }
                    }
                    new_pixel[i][j][kk]=(unsigned char)(round(s*1.0/count));
                }
            }
        }
    }

    else if (!strcmp(type,"gaussian")) {
        float **kernel;
        kernel_matrix(c,&kernel);
        for (int i=row_start; i<=row_end; i++) {
            if (i<0 || i>h-1)
                continue;
            for (int j=col_start; j<=col_end; j++) {
                if (j<0 || j>w-1)
                    continue;
                for (int kk=0; kk<cmp; kk++) {
                    double s=0;
                    double valid_weight_sum=0;
                    //int count=0;
                    for (int k=i-c/2; k<=i+c/2; k++) {
                        if (k<0 || k>(h-1))
                            continue;
                        for (int l=j-c/2; l<=j+c/2; l++) {
                            if (l<0 || l>(w-1))
                                continue;
                            s+=pixel[k][l][kk]*kernel[k-i+c/2][l-j+c/2];
                            valid_weight_sum+=kernel[k-i+c/2][l-j+c/2];
                        }
                    }
                    new_pixel[i][j][kk]=(unsigned char)(round(s/valid_weight_sum));
                }
            }
        }
        for (int i=0; i<c; i++) {
            free(kernel[i]);
        }
        free(kernel);
    }

    else if (!strcmp(type,"median")) {
        for (int i=row_start; i<=row_end; i++) {
            if (i<0 || i>h-1)
                continue;
            for (int j=col_start; j<=col_end; j++) {
                if (j<0 || j>w-1)
                    continue;
                for (int kk=0; kk<cmp; kk++) {
                    int count=0;
                    for (int k=i-c/2; k<=i+c/2; k++) {
                        if (k<0 || k>(h-1))
                            continue;
                        for (int l=j-c/2; l<=j+c/2; l++) {
                            if (l<0 || l>(w-1))
                                continue;
                            count++;
                        }
                    }
                    unsigned char *array= malloc(count*sizeof(unsigned char));
                    if (!array) {
                        stbi_image_free(rgb_array);
                        for (int ii=0; ii<h; ii++) {
                            for (int jj=0; jj<w; jj++) {
                                free(pixel[ii][jj]);
                            }
                            free(pixel[ii]);
                        }
                        free(pixel);
                        for (int ii=0; ii<h; ii++) {
                            for (int jj=0; jj<w; jj++) {
                                free(new_pixel[ii][jj]);
                            }
                            free(new_pixel[ii]);
                        }
                        free(new_pixel);
                        return;
                    }
                    int iter=0;
                    for (int k=i-c/2; k<=i+c/2; k++) {
                        if (k<0 || k>(h-1))
                            continue;
                        for (int l=j-c/2; l<=j+c/2; l++) {
                            if (l<0 || l>(w-1))
                                continue;
                            array[iter]=pixel[k][l][kk];
                            iter++;
                        }
                    }
                    qsort(array,count,sizeof(unsigned char),compare_bytes);
                    unsigned char med;
                    if (count%2) {
                        med=array[count/2];
                    }
                    else {
                        med=(int)(round((array[count/2]+array[count/2-1])/2.0));
                    }
                    new_pixel[i][j][kk]=(unsigned char) med;
                    free(array);
                }
            }
        }
    }

    if (is_jpg(new_address))
        pixel_array_to_jpg(new_address,new_pixel,w,h,cmp);
    else if (is_png(new_address))
        pixel_array_to_png(new_address,new_pixel,w,h,cmp);
    stbi_image_free(rgb_array);

    for (int ii=0; ii<h; ii++) {
        for (int jj=0; jj<w; jj++) {
            free(pixel[ii][jj]);
        }
        free(pixel[ii]);
    }
    free(pixel);
    for (int ii=0; ii<h; ii++) {
        for (int jj=0; jj<w; jj++) {
            free(new_pixel[ii][jj]);
        }
        free(new_pixel[ii]);
    }
    free(new_pixel);
}

// Here is only two types of mirror.
// (i)  vertical: here the mirror is the vertical axis
// (ii) horizontal: here the mirror is the horizontal axis.
static void mirror_img(const char *img_address,const char *new_address,const int cmp,const char *type) {
    int h,w,chn;
    unsigned char *rgb_array=stbi_load(img_address,&w,&h,&chn,cmp);
    if (!rgb_array)
        return;

    unsigned char ***pixel= (unsigned char***)malloc(h*sizeof(unsigned char**));
    if (!pixel) {
        stbi_image_free(rgb_array);
        return;
    }
    for (int i=0; i<h; i++) {
        pixel[i]=(unsigned char**)malloc(w*sizeof(unsigned char*));
        for (int j=0; j<w; j++) {
            pixel[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
            int base=(i*w+j)*cmp;
            for (int k=0; k<cmp; k++) {
                pixel[i][j][k]=rgb_array[base+k];
            }
        }
    }

    unsigned char ***new_pixel= (unsigned char***)malloc(h*sizeof(unsigned char**));
    if (!new_pixel) {
        stbi_image_free(rgb_array);
        for (int ii=0; ii<h; ii++) {
            for (int jj=0; jj<w; jj++) {
                free(pixel[ii][jj]);
            }
            free(pixel[ii]);
        }
        free(pixel);
        return;
    }
    for (int i=0; i<h; i++) {
        new_pixel[i]=(unsigned char**)malloc(w*sizeof(unsigned char*));
        for (int j=0; j<w; j++) {
            new_pixel[i][j]=(unsigned char*)malloc(cmp*sizeof(unsigned char));
            int base=(i*w+j)*cmp;
            for (int k=0; k<cmp; k++) {
                new_pixel[i][j][k]=pixel[i][j][k];
            }
        }
    }

    if (!strcmp(type,"vertical")) {
        for (int i=0; i<h; i++) {
            for (int j=0; j<w; j++) {
                for (int k=0; k<cmp; k++) {
                    new_pixel[i][j][k]=pixel[i][w-1-j][k];
                }
            }
        }
    }

    else if (!strcmp(type,"horizontal")) {
        for (int i=0; i<h; i++) {
            for (int j=0; j<w; j++) {
                for (int k=0; k<cmp; k++) {
                    new_pixel[i][j][k]=pixel[h-1-i][j][k];
                }
            }
        }
    }

    if (is_jpg(new_address))
        pixel_array_to_jpg(new_address,new_pixel,w,h,cmp);
    else if (is_png(new_address))
        pixel_array_to_png(new_address,new_pixel,w,h,cmp);
    stbi_image_free(rgb_array);
    for (int ii=0; ii<h; ii++) {
        for (int jj=0; jj<w; jj++) {
            free(pixel[ii][jj]);
        }
        free(pixel[ii]);
    }
    free(pixel);
    for (int ii=0; ii<h; ii++) {
        for (int jj=0; jj<w; jj++) {
            free(new_pixel[ii][jj]);
        }
        free(new_pixel[ii]);
    }
    free(new_pixel);
}