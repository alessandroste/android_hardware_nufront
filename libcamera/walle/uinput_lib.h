#ifndef __UINPUT_LIB_H__
#define __UINPUT_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

struct _UInput
{
	int fd;
};
typedef struct _UInput UInput;

void uinput_create(const char* dev, const char* display);
void uinput_report_key_event(int x);
void uinput_report_touch_event(int x, int y, int press);
void uinput_destroy();

#ifdef __cplusplus
}
#endif
#endif/*__UINPUT_LIB_H__*/
