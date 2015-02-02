#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>

 int main()
 {
     int fbfd = 0;
     int x1,y1,max_color;
     struct fb_var_screeninfo vinfo;
     struct fb_fix_screeninfo finfo;
     long int screensize = 0;
     char *fbp = 0;
     int x = 0, y = 0;
     long int location = 0;
	 system("clear");
     // Open the file for reading and writing
     fbfd = open("/dev/fb0", O_RDWR);
     if (fbfd == -1) {
         perror("Error: cannot open framebuffer device");
         exit(1);
     }
     printf("The framebuffer device was opened successfully.\n");

     // Get fixed screen information
     if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
         perror("Error reading fixed information");
         exit(2);
     }

     // Get variable screen information
     if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
         perror("Error reading variable information");
         exit(3);
     }

     printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

     // Figure out the size of the screen in bytes
     screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

     // Map the device to memory
     fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fbfd, 0);
     if ((int)fbp == -1) {
         perror("Error: failed to map framebuffer device to memory");
         exit(4);
     }
     printf("The framebuffer device was mapped to memory successfully.\n");

     //x = 50; y = 50;       // Where we are going to put the pixel
	 max_color = 256;
	 int x2,y2,x3,y3;
	 x1 = 10;
	 y1 = 400;
	 x2 = x1+max_color;
	 y2 = 400;
	 x3 = x2 + 300;
	 y3 = 400;
	 double pow_x,pow_y,pow_sum;
     // Figure out where in memory to put the pixel
     for (y = y1; y < y1+max_color; y++){
         for (x = x1; x < x1+max_color; x++) {

             location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                        (y+vinfo.yoffset) * finfo.line_length;
			pow_x = pow(x-x1,2);
			pow_y = pow(y-y1,2);
			if (sqrt(pow_x+pow_y) < 256)
				pow_sum = sqrt(pow_x+pow_y);
			else
				pow_sum = 0;
             if (vinfo.bits_per_pixel == 32) {
                 *(fbp + location) = (x-x1);        // Some blue
                 *(fbp + location + 1) = pow_sum;     // A little green
                 *(fbp + location + 2) = (y-y1);    // A lot of red
                 *(fbp + location + 3) = 0;      // No transparency
             } else  { //assume 16bpp
                 int b = 10;
                 int g = (x-100)/6;     // A little green
                 int r = 31-(y-100)/16;    // A lot of red
                 unsigned short int t = r<<11 | g << 5 | b;
                 *((unsigned short int*)(fbp + location)) = t;
             }

         }
	 }
	 for (y = y2; y < y2+max_color; y++){
         for (x = x2; x < x2+max_color; x++) {
             location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                        (y+vinfo.yoffset) * finfo.line_length;
             if (vinfo.bits_per_pixel == 32) {
                 *(fbp + location) = (max_color-1-(x-x2));        // Some blue
                 *(fbp + location + 1) = (x-x2);     // A little green
                 *(fbp + location + 2) = (y-y2);    // A lot of red
                 *(fbp + location + 3) = 0;      // No transparency
             } else  { //assume 16bpp
                 int b = 10;
                 int g = (x-100)/6;     // A little green
                 int r = 31-(y-100)/16;    // A lot of red
                 unsigned short int t = r<<11 | g << 5 | b;
                 *((unsigned short int*)(fbp + location)) = t;
             }

         }
	 }
	 //selecting eyedropper
     int  select_x,select_y;
     u_int r,g,b,t;
     printf ("What color to pick?\n");
     printf ("x (0-511) : ");
     scanf ("%d",&select_x);
     printf ("y (0-255) : ");
     scanf ("%d",&select_y);
     if ((select_x < 0) || (select_x > 511) || (select_y < 0) || (select_y > 255)){
		 printf ("Coordinate is invalid. Try rerun the program with make run.\nX = %d Y = %d.\n",select_x,select_y);
		 exit(1);
	 }
     location = (x1+select_x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                (y1+select_y+vinfo.yoffset) * finfo.line_length;
     b = *(fbp + location);
     g = *(fbp + location + 1);
     r = *(fbp + location + 2);
     t = *(fbp + location + 3);
     if (r < 0)
		r += 128;
	 if (g < 0)
		g += 128;
	 if (b < 0)
		b += 128;
     printf("R = %d G = %d B = %d T = %d\n",r,g,b,t);
     for (y = y3; y < y3+max_color; y++){
         for (x = x3; x < x3+max_color; x++) {
             location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                        (y+vinfo.yoffset) * finfo.line_length;
             if (vinfo.bits_per_pixel == 32) {
                 *(fbp + location) = b;        // Some blue
                 *(fbp + location + 1) = g;     // A little green
                 *(fbp + location + 2) = r;    // A lot of red
                 *(fbp + location + 3) = t;      // No transparency
             } else  { //assume 16bpp
                 int b = 10;
                 int g = (x-100)/6;     // A little green
                 int r = 31-(y-100)/16;    // A lot of red
                 unsigned short int t = r<<11 | g << 5 | b;
                 *((unsigned short int*)(fbp + location)) = t;
             }

         }
	 }
     munmap(fbp, screensize);
     close(fbfd);
     return 0;	
 } 

