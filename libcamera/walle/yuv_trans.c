//==============================================================
//Filename:     yuv_trans.c
//Description:  split image data from vport;
//              bgrg_mode    0: BG/RG
//                           1: GR/BG
//                           2: RG/GB
//                           3: GB/RG
//
//              nv_mode      0: nv21
//                           1: nv12
//
//Authour:      Rui
//Revision:     20121120 first release
//              
//Copy Right:   Nufront 3D
//=============================================================

#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif

#ifndef MAX
#define MAX(a,b) ((a)<(b)?(b):(a))
#endif

#define CLIP(x,a,b) MIN(MAX(x,a),b)

void yuv_trans(unsigned char *vport, unsigned char *yuv, short vport_width, short vport_height, short bgrg_mode, short nv_mode)
{
    short image_width;
    short image_height;
    short image_width_quarter;
    short image_width_half;
    short image_width_5_4;
    short vport_width_half;
    short x, y;
    short y_and_01;
    short y_mode2;
    short B = 0;
	short G = 0;
	short R = 0;
	short yuv_y,yuv_u,yuv_v;
	short center_edge;
    int J;
    int aa,bb,cc,dd;
    int offset_0;
    int offset_1;
    int offset_2;
    int raw_step;
    int reg_temp;
	int y_pos;
	int y_pos_1;
	int y_pos_2;
	int y_pos_3;
	int u_pos;
//	int v_pos;           //YV12 use this position
    
    image_width         = (vport_width/3) << 1;
    image_height        = vport_height;
    image_width_quarter = image_width>>2;
    image_width_half    = image_width>>1;
    image_width_5_4     = (image_width*5)/4;
    vport_width_half    = vport_width>>1;

    y_pos = 0;
    u_pos = vport_width*vport_height;
    center_edge = image_width - 2;
//    v_pos = (vport_width*vport_height*5)>>2;

////////////////////////////////////////////////////
//        raw to image
////////////////////////////////////////////////////

	for(y = 0; y < image_height; y++)
	{
	    offset_0     = y*vport_width_half + u_pos;
	    offset_1     = offset_0 + image_width_quarter;
	    raw_step     = y*vport_width;
	    offset_2     = raw_step + image_width_quarter;
	    y_and_01     = (y & 0x01) << 1;
	    y_mode2      = ((y%2)==0);
		for(x = 0; x < center_edge; x++)
		{
            ////////////////////////////////////////////////////
            //           Y part
            ////////////////////////////////////////////////////
            
            J = (x & 0x01) + y_and_01;

			aa = x+raw_step;
        	bb = aa+1;
        	cc = aa+vport_width;
        	dd = cc+1;
        	
        	if(bgrg_mode==0)
        	{
        	    switch(J)
			    {
			    case 0:
			    	R=vport[dd];
			    	G=vport[bb];
			    	B=vport[aa];
			    	break;
			    case 1:
			    	R=vport[cc];
			    	G=vport[aa];
			    	B=vport[bb];
			    	break;
			    case 2:
			    	R=vport[bb];
			    	G=vport[aa];
			    	B=vport[dd];
			    	break;
			    case 3:
			    	R=vport[aa];
			    	G=vport[bb];
			    	B=vport[dd];
			    	break;
			    default:
			    	printf("fail to compute J value\n");
			    }
        	}
        	else if(bgrg_mode==1)
        	{
        	    switch(J)
			    {
			    case 0:
			    	R=vport[bb];
			    	G=vport[aa];
			    	B=vport[cc];
			    	break;
			    case 1:
			    	R=vport[aa];
			    	G=vport[bb];
			    	B=vport[dd];
			    	break;
			    case 2:
			    	R=vport[dd];
			    	G=vport[bb];
			    	B=vport[aa];
			    	break;
			    case 3:
			    	R=vport[cc];
			    	G=vport[bb];
			    	B=vport[aa];
			    	break;
			    default:
			    	printf("fail to compute J value\n");
			    }
        	}
        	else if(bgrg_mode==2)
        	{
        	    switch(J)
			    {
			    case 0:
			    	R=vport[aa];
			    	G=vport[bb];
			    	B=vport[dd];
			    	break;
			    case 1:
			    	R=vport[bb];
			    	G=vport[aa];
			    	B=vport[cc];
			    	break;
			    case 2:
			    	R=vport[cc];
			    	G=vport[aa];
			    	B=vport[bb];
			    	break;
			    case 3:
			    	R=vport[dd];
			    	G=vport[bb];
			    	B=vport[aa];
			    	break;
			    default:
			    	printf("fail to compute J value\n");
			    }
        	}
        	else
        	{
        	    switch(J)
			    {
			    case 0:
			    	R=vport[cc];
			    	G=vport[aa];
			    	B=vport[bb];
			    	break;
			    case 1:
			    	R=vport[dd];
			    	G=vport[bb];
			    	B=vport[aa];
			    	break;
			    case 2:
			    	R=vport[aa];
			    	G=vport[bb];
			    	B=vport[dd];
			    	break;
			    case 3:
			    	R=vport[bb];
			    	G=vport[aa];
			    	B=vport[cc];
			    	break;
			    default:
			    	printf("fail to compute J value\n");
			    }
        	}

//			yuv_y = CLIP(( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16, 0, 255);
			yuv_y =  (((R<<6) + (G<<7) +  25 * B) >> 8) +  16;
            yuv[x + offset_2] = (unsigned char)yuv_y;
            
            ////////////////////////////////////////////////////
            //           UV part
            ////////////////////////////////////////////////////

            if(y_mode2)
            {
                if(x%2==0)
                {
                    if(nv_mode)
                    {
                        yuv_u = MAX(( ( -38 * R -  74 * G + 112 * B) >> 8) + 128, 0);
                        yuv[x + offset_1] = (unsigned char)yuv_u;
                    }
                    else
                    {
                        yuv_v = MAX(( ( 112 * R -  94 * G -  18 * B) >> 8) + 128, 0);
                        yuv[x + offset_1] = (unsigned char)yuv_v;
                    }
                }
                else
                {
                    if(nv_mode)    //1: NV12        2: NV21
                    {
                        yuv_v = MAX(( ( 112 * R -  94 * G -  18 * B) >> 8) + 128, 0);
                        yuv[x + offset_1] = (unsigned char)yuv_v;
                    }
                    else
                    {
                        yuv_u = MAX(( ( -38 * R -  74 * G + 112 * B) >> 8) + 128, 0);
                        yuv[x + offset_1] = (unsigned char)yuv_u;
                    }
                }
            }
		}
		
		if(y_mode2)
        {
		    for(x = 0; x < image_width_quarter; x++)
		    {
		        yuv[x + raw_step] = 0;
		        yuv[x + offset_0] = 128;
		    }
		    for(x = center_edge + image_width_quarter; x < vport_width; x++)
		    {
		        yuv[x + raw_step] = 0;
		        yuv[x + offset_0] = 128;
		    }
		}
	}
}


