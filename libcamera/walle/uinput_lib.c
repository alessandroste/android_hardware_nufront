#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include "uinput_lib.h"
#include <linux/fb.h>

#define UI_SET_PROPBIT          _IOW(UINPUT_IOCTL_BASE, 110, int) 

#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE); \
} while(0)

static UInput *thiz = NULL;
static int move = 0;
static int xres = 1024;
static int yres = 600;
static int relx = 512;
static int rely = 300;

void uinput_create(const char* dev, const char* display)
{
	struct uinput_user_dev uidev = {0};
	int fbfd;
	struct fb_var_screeninfo vinfo;                          

	dev = dev != NULL ? dev : "/dev/uinput";
	display = display != NULL ? display : "/dev/graphics/fb0";

        if ((fbfd = open(display, O_RDWR)) < 0)
		die("error: open");

	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
 		printf("Bad vscreeninfo ioctl/n");
	}else{
		printf("lcd0 info xres %d info yres %d\n", vinfo.xres, vinfo.yres);
	}

	xres = vinfo.xres;
	yres = vinfo.yres;

	relx = xres/2;
	rely = yres/2;

	if((thiz = (UInput *)calloc(1, sizeof(UInput))) == NULL)
		die("error: calloc");

	if((thiz->fd = open(dev, O_WRONLY | O_NONBLOCK)) <= 0)
		die("error: open");

	if(ioctl(thiz->fd, UI_SET_EVBIT, EV_KEY) < 0)
		die("error: ioctl");

	if(ioctl(thiz->fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_KEYBIT, KEY_LEFT) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_KEYBIT, KEY_RIGHT) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_KEYBIT, KEY_UP) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_KEYBIT, KEY_DOWN) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_KEYBIT, KEY_ENTER) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_KEYBIT, KEY_PAGEUP) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_KEYBIT, KEY_PAGEDOWN) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_KEYBIT, KEY_ESC) < 0)
		die("error: ioctl");

	if(ioctl(thiz->fd, UI_SET_EVBIT, EV_SYN) < 0)
		die("error: ioctl");

	if(ioctl(thiz->fd, UI_SET_EVBIT, EV_REL) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_RELBIT, REL_X) < 0)
		die("error: ioctl");
	if(ioctl(thiz->fd, UI_SET_RELBIT, REL_Y) < 0)
		die("error: ioctl");

	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 0x1;
	uidev.id.product = 0x1;
	uidev.id.version = 1;


	if(write(thiz->fd, &uidev, sizeof(uidev)) < 0)
		die("error: write");

	if(ioctl(thiz->fd, UI_DEV_CREATE) < 0)
		die("error: ioctl");

	//printf("open touch/n");
	return;
}

void uinput_destroy()
{
	if(thiz != NULL)
	{
		if(ioctl(thiz->fd, UI_DEV_DESTROY) < 0)
			die("error: ioctl");
		close(thiz->fd);
		free(thiz);
	}

	return;
}

static void uinput_input_report_abs(int code, int value)
{
	struct input_event event = {0};

	event.type  = EV_ABS;
	event.code  = code ;
	event.value = value;
	write(thiz->fd, &event, sizeof(event));

	return;
}

static void uinput_input_report_rel(int code, int value)
{
	struct input_event event = {0};

	event.type  = EV_REL;
	event.code  = code ;
	event.value = value;
	write(thiz->fd, &event, sizeof(event));

	return;
}

static void uinput_input_report_sync()
{
	struct input_event event = {0};

	event.type  = EV_SYN;
	event.code  = SYN_REPORT;
	event.value = 0;
	write(thiz->fd, &event, sizeof(event));

	return;
}

static void uinput_input_report_mt_sync()
{
	struct input_event event = {0};

	event.type  = EV_SYN;
	event.code  = SYN_MT_REPORT;
	event.value = 0;
	write(thiz->fd, &event, sizeof(event));

	return;
}

static void uinput_input_report_key(int key, int press)
{
	struct input_event event = {0};

	event.type  = EV_KEY;
	event.code  = key;
	event.value = press ? 1:0;
	write(thiz->fd, &event, sizeof(event));

	return;
}

void uinput_report_key_event(int x)
{
	uinput_input_report_key(x, 1);
	uinput_input_report_sync();
	uinput_input_report_key(x, 0);
	uinput_input_report_sync();
}

void uinput_report_touch_event(int x, int y, int press/*1 0 -1*/)
{
	int dx, dy;
	if (x > 1000||x < 0)
		return;
	dx = x * xres/1000;
	dy = y * yres/1000;

	if(thiz != NULL && thiz->fd > 0)
	{
		if (press > 0)
		{
			if (move == 0)
			{
				move =1;
				uinput_input_report_rel(REL_X, dx -relx);
				uinput_input_report_rel(REL_Y, dy -rely);
				uinput_input_report_sync();
				uinput_input_report_key(BTN_LEFT, 1);
				uinput_input_report_sync();
			}
			else if(move == 1)
			{
				uinput_input_report_rel(REL_X, dx -relx);
				uinput_input_report_rel(REL_Y, dy -rely);
				uinput_input_report_sync();
			}
		}
		else if (press == 0)
		{
			if (move == 1)
			{
				move = 0;
				uinput_input_report_rel(REL_X, dx -relx);
				uinput_input_report_rel(REL_Y, dy -rely);
				uinput_input_report_sync();
				uinput_input_report_key(BTN_LEFT, 0);
				uinput_input_report_sync();
			}
			else if (move == 0)
			{
				move = 0;
				uinput_input_report_rel(REL_X, dx - relx);
				uinput_input_report_rel(REL_Y, dy - rely);
				uinput_input_report_sync();
			}
		}
		relx = dx;
		rely = dy;
	//	switch(press)
	//	{
	//		case 0:
	//		{
	//			/*release*/
	//			uinput_input_report_abs(ABS_MT_TOUCH_MAJOR, 0);
	//			uinput_input_report_abs(ABS_MT_WIDTH_MAJOR, 0);
	//			uinput_input_report_key(BTN_TOUCH, 0);
	//			uinput_input_report_mt_sync();
	//			uinput_input_report_sync();
	//			break;
	//		}
	//		case 1:
	//		{
	//			/*press*/
	//			uinput_input_report_abs(ABS_MT_POSITION_X, dx);
	//			uinput_input_report_abs(ABS_MT_POSITION_Y, dy);
	//			uinput_input_report_abs(ABS_MT_TOUCH_MAJOR, 100);
	//			uinput_input_report_abs(ABS_MT_WIDTH_MAJOR, 100);
	//			uinput_input_report_abs(ABS_MT_TRACKING_ID, 0);
	//			uinput_input_report_key(BTN_TOUCH, 1);
	//			uinput_input_report_mt_sync();
	//			uinput_input_report_sync();
	//			break;
	//		}
	//		default:
	//		{
	//			/*press*/
	//			uinput_input_report_abs(ABS_MT_POSITION_X, dx);
	//			uinput_input_report_abs(ABS_MT_POSITION_Y, dy);
	//			uinput_input_report_abs(ABS_MT_TOUCH_MAJOR, 100);
	//			uinput_input_report_abs(ABS_MT_WIDTH_MAJOR, 100);
	//			uinput_input_report_abs(ABS_MT_TRACKING_ID, 0);
	//			uinput_input_report_key(BTN_TOUCH, 1);
	//			uinput_input_report_mt_sync();
	//			uinput_input_report_sync();
	//			break;
	//		}
	//	}

		//printf("%s: x=%d y=%d press=%d\n", __func__, x, y, press);
	}

	return;
}

//int main(void)
//{
//	int    dx, dy;
//	int    i;
//
//	dx = 0;
//	dy = 0;
//
//	uinput_create(NULL,NULL);
//
//	uinput_report_touch_event(dx, dy, 0);
//
//	for(i = 0; i < 150; i++) {
//		dx += 2;
//		dy += 2;
//		uinput_report_touch_event(dx, dy, 0);
//		usleep(15000);
//	}
//	uinput_report_key_event(KEY_DOWN);
//	usleep(1500000);
//	uinput_report_key_event(KEY_UP);
//	usleep(1500000);
//	uinput_report_key_event(KEY_LEFT);
//	usleep(1500000);
//	uinput_report_key_event(KEY_RIGHT);
//	usleep(1500000);
//	uinput_report_key_event(KEY_PAGEDOWN);
//	usleep(1500000);
//	uinput_report_key_event(KEY_PAGEUP);
//	usleep(1500000);
//	uinput_report_key_event(KEY_ENTER);
//	usleep(1500000);
//	uinput_report_key_event(KEY_ESC);
//
//	//uinput_report_touch_event(dx, dy, 1);
//
//	//	for(i = 0; i < 100; i++) {
//	//		dx += 2;
//	//		//dy += 4;
//	//		uinput_report_touch_event(dx, dy, 1);
//	//		usleep(15000);
//	//	}
//	//uinput_report_touch_event(dx, dy, 0);
//
//	uinput_destroy();
//	return 0;
//}
