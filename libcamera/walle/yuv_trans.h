#ifndef __YUV_TRANS_H__
#define __YUV_TRANS_H__

#ifdef __cplusplus
extern "C"  {
#endif

    void yuv_trans(unsigned char *vport, unsigned char *yuv, int vport_width, int vport_height, int bgrg_mode, int nv_mode);

#ifdef __cplusplus
}
#endif
#endif /*__YUV_TRANS_H__*/
