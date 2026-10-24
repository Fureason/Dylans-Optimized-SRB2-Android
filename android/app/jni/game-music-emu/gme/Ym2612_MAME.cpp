



#include "Ym2612_MAME.h"

/*
**
** File: fm2612.c -- software implementation of Yamaha YM2612 FM sound generator
** Split from fm.c to keep 2612 fixes from infecting other OPN chips
**
** Copyright Jarek Burczynski (bujar at mame dot net)
** Copyright Tatsuyuki Satoh , MultiArcadeMachineEmulator development
**
** Version 1.5.1 (Genesis Plus GX ym2612.c rev. 368)
**
*/

  
           
  
                                        
                                                                                          
                                                                               
  
         
  
                        
                       
  
              
  
                                        
                                                                 
                                                       
  
             
                               
                                                                                                                    
                                                                                                                          
                                                                                                                                   
                                        
                                               
                                                                                       
                                                   
                                                                                                             
                                                                
                                                                                 
                                                                                                                    
                                             
                                                                                                       
                                                      
  
                            
                                                                              
  
                               
                                                   
                                              
                                         
  
                               
                                                         
                                                                   
                                                                              
  
                               
                                                   
  
                               
                       
  
                               
                                                                                 
  
                               
                                                             
  
                               
                                                                                           
                                                                           
  
                                                
                                                                                     
                                      
                                                                                        
  
                               
                                                                
                         
                                                                                  
                                                                                       
  
                               
                                                    
  
                               
                                                               
                                                             
                                                                    
                                                                       
                                                     
                                                                                    
                                                                       
                                                                                     
                                                                                          
                                    
  
                                                
                                                                                
  
                               
                                                       
                                                                                           
  
                     
                                            
                                                  
                             
                            
                               
                                             
  
                     
                                       
                                  
                                                                
                       
                                
  

 
 
 
 
 
 

#include <stdlib.h>
#include <string.h>	 
#include <stddef.h>	 
#include <math.h>
#include <stdint.h>

namespace Ym2612_MameImpl
{

 
 
 
typedef unsigned char						UINT8;
typedef signed char 						INT8;

 
typedef unsigned short						UINT16;
typedef signed short						INT16;

 
#ifndef _WINDOWS_H
typedef unsigned int						UINT32;
typedef signed int							INT32;
#endif

 
#ifndef _WINDOWS_H
#ifdef _MSC_VER
typedef signed __int64						INT64;
typedef unsigned __int64					UINT64;
#else
__extension__ typedef unsigned long long	UINT64;
__extension__ typedef signed long long		INT64;
#endif
#endif

 
typedef UINT32	offs_t;

 
typedef INT16 stream_sample_t;

#if defined(VGM_BIG_ENDIAN)
#define BYTE_XOR_BE(x)	 (x)
#elif defined(VGM_LITTLE_ENDIAN)
#define BYTE_XOR_BE(x)	((x) ^ 0x01)
#else
 
#endif

#if defined(_MSC_VER)

#define INLINE	static __inline
#elif defined(__GNUC__)
#define INLINE	static __inline__
#else
#define INLINE	static inline
#endif

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifdef _DEBUG
#define logerror	printf
#else
#define logerror
#endif

typedef void (*SRATE_CALLBACK)(void*, UINT32);
 

 
  
                                                                   
                                                                   
                                                                   
                                                                    
                                                                   
                                                                 
  
#define BUILD_YM2203  0
#define BUILD_YM2608  0
#define BUILD_YM2610  0
#define BUILD_YM2610B 0
#define BUILD_YM2612  1
#define BUILD_YM3438  0

#define FM_BUSY_FLAG_SUPPORT 0

 
#define FM_SAMPLE_BITS 16

 
#define FM_INTERNAL_TIMER 1

 
 
 

 
 
typedef struct _ssg_callbacks ssg_callbacks;
struct _ssg_callbacks
{
	void (*set_clock)(void *param, int clock);
	void (*write)(void *param, int address, int data);
	int (*read)(void *param);
	void (*reset)(void *param);
};

 

#if FM_BUSY_FLAG_SUPPORT
#define TIME_TYPE					attotime
#define UNDEFINED_TIME				attotime_zero
#define FM_GET_TIME_NOW(machine)			timer_get_time(machine)
#define ADD_TIMES(t1, t2)   		attotime_add((t1), (t2))
#define COMPARE_TIMES(t1, t2)		attotime_compare((t1), (t2))
#define MULTIPLY_TIME_BY_INT(t,i)	attotime_mul(t, i)
#endif

 
#if 0
#ifndef OSD_CPU_H
#define OSD_CPU_H
typedef unsigned char	UINT8;    
typedef unsigned short	UINT16;   
typedef unsigned int	UINT32;   
typedef signed char		INT8;     
typedef signed short	INT16;    
typedef signed int		INT32;    
#endif  
#endif

typedef stream_sample_t FMSAMPLE;
  
                        
                       
      
                       
                                
      
  

typedef void (*FM_TIMERHANDLER)(void *param,int c,int cnt,int clock);
typedef void (*FM_IRQHANDLER)(void *param,int irq);
 
 
 
 
 

 
 
 

   
                                                 
                                 
                                
                                 
                                
                              
                                             
   
static void * ym2612_init(void *param, int baseclock, int rate,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
   
                            
                            
   
static void ym2612_shutdown(void *chip);
   
                                 
                            
   
static void ym2612_reset_chip(void *chip);
   
                                                    
                            
                                    
                                                                                           
                                                                                        
   
static void ym2612_generate(void *chip, FMSAMPLE *buffer, int frames, int mix);
#define ym2612_update_one(chip, buffer, length) ym2612_generate(chip, buffer, length, 0)

   
                                          
                            
   
static void ym2612_pre_generate(void *chip);
   
                                                                                       
                            
                                     
   
static void ym2612_generate_one_native(void *chip, FMSAMPLE buffer[2]);

 

static int ym2612_write(void *chip, int a,unsigned char v);
#if 0
static unsigned char ym2612_read(void *chip,int a);
static int ym2612_timer_over(void *chip, int c );
#endif

#ifdef __STATE_H__
static void ym2612_postload(void *chip);
#endif

static void ym2612_set_mutemask(void *chip, UINT32 MuteMask);
#if 0
static void ym2612_setoptions(UINT8 Flags);
#endif


static stream_sample_t *DUMMYBUF = NULL;

 
#define BUILD_OPN (BUILD_YM2203||BUILD_YM2608||BUILD_YM2610||BUILD_YM2610B||BUILD_YM2612||BUILD_YM3438)
#define BUILD_OPN_PRESCALER (BUILD_YM2203||BUILD_YM2608)

#define RSM_ENABLE 1
#define RSM_FRAC 10

 
#define TYPE_SSG    0x01     
#define TYPE_LFOPAN 0x02     
#define TYPE_6CH    0x04     
#define TYPE_DAC    0x08     
#define TYPE_ADPCM  0x10     
#define TYPE_2610   0x20     


#define TYPE_YM2203 (TYPE_SSG)
#define TYPE_YM2608 (TYPE_SSG |TYPE_LFOPAN |TYPE_6CH |TYPE_ADPCM)
#define TYPE_YM2610 (TYPE_SSG |TYPE_LFOPAN |TYPE_6CH |TYPE_ADPCM |TYPE_2610)
#define TYPE_YM2612 (TYPE_DAC |TYPE_LFOPAN |TYPE_6CH)


 
#define FREQ_SH			16   
#define EG_SH			16   
#define LFO_SH			24   
#define TIMER_SH		16   

#define FREQ_MASK		((1<<FREQ_SH)-1)

#define MAXOUT    (+32767)
#define MINOUT    (-32768)

 
#define ENV_BITS		10
#define ENV_LEN			(1<<ENV_BITS)
#define ENV_STEP		(128.0/ENV_LEN)

#define MAX_ATT_INDEX	(ENV_LEN-1)  
#define MIN_ATT_INDEX	(0)			 

#define EG_ATT			4
#define EG_DEC			3
#define EG_SUS			2
#define EG_REL			1
#define EG_OFF			0

 
#define SIN_BITS		10
#define SIN_LEN			(1<<SIN_BITS)
#define SIN_MASK		(SIN_LEN-1)

#define TL_RES_LEN		(256)  

                                
                                          
                                          
                                          
  
#define TL_TAB_LEN (13*2*TL_RES_LEN)
static signed int tl_tab[TL_TAB_LEN];

#define ENV_QUIET		(TL_TAB_LEN>>3)

 
static unsigned int sin_tab[SIN_LEN];

 
 
 
 

 
 
#define SC(db) (UINT32) ( db * (4.0/ENV_STEP) )
static const UINT32 sl_table[16]={
 SC( 0),SC( 1),SC( 2),SC(3 ),SC(4 ),SC(5 ),SC(6 ),SC( 7),
 SC( 8),SC( 9),SC(10),SC(11),SC(12),SC(13),SC(14),SC(31)
};
#undef SC


#define RATE_STEPS (8)
static const UINT8 eg_inc[19*RATE_STEPS]={

 

  0,1, 0,1, 0,1, 0,1,  
  0,1, 0,1, 1,1, 0,1,  
  0,1, 1,1, 0,1, 1,1,  
  0,1, 1,1, 1,1, 1,1,  

  1,1, 1,1, 1,1, 1,1,  
  1,1, 1,2, 1,1, 1,2,  
  1,2, 1,2, 1,2, 1,2,  
  1,2, 2,2, 1,2, 2,2,  

  2,2, 2,2, 2,2, 2,2,  
  2,2, 2,4, 2,2, 2,4,  
  2,4, 2,4, 2,4, 2,4,  
  2,4, 4,4, 2,4, 4,4,  

  4,4, 4,4, 4,4, 4,4,  
  4,4, 4,8, 4,4, 4,8,  
  4,8, 4,8, 4,8, 4,8,  
  4,8, 8,8, 4,8, 8,8,  

  8,8, 8,8, 8,8, 8,8,  
  16,16,16,16,16,16,16,16,  
  0,0, 0,0, 0,0, 0,0,  
};


#define O(a) (a*RATE_STEPS)

 
static const UINT8 eg_rate_select2612[32+64+32]={   
 
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),

 
  
                        
                        
  
O(18),O(18),O( 2),O( 3),     
O( 0),O( 1),O( 2),O( 2),     

O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),

 
O( 4),O( 5),O( 6),O( 7),

 
O( 8),O( 9),O(10),O(11),

 
O(12),O(13),O(14),O(15),

 
O(16),O(16),O(16),O(16),

 
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16)

};
#undef O

 
 
 

#define O(a) (a*1)
static const UINT8 eg_rate_shift[32+64+32]={   
 
                                           
                                        
                                        
                                           

 
O(11),O(11),O(11),O(11),O(11),O(11),O(11),O(11),
O(11),O(11),O(11),O(11),O(11),O(11),O(11),O(11),
O(11),O(11),O(11),O(11),O(11),O(11),O(11),O(11),
O(11),O(11),O(11),O(11),O(11),O(11),O(11),O(11),

 
O(11),O(11),O(11),O(11),
O(10),O(10),O(10),O(10),
O( 9),O( 9),O( 9),O( 9),
O( 8),O( 8),O( 8),O( 8),
O( 7),O( 7),O( 7),O( 7),
O( 6),O( 6),O( 6),O( 6),
O( 5),O( 5),O( 5),O( 5),
O( 4),O( 4),O( 4),O( 4),
O( 3),O( 3),O( 3),O( 3),
O( 2),O( 2),O( 2),O( 2),
O( 1),O( 1),O( 1),O( 1),
O( 0),O( 0),O( 0),O( 0),

 
O( 0),O( 0),O( 0),O( 0),

 
O( 0),O( 0),O( 0),O( 0),

 
O( 0),O( 0),O( 0),O( 0),

 
O( 0),O( 0),O( 0),O( 0),

 
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0)

};
#undef O

static const UINT8 dt_tab[4 * 32]={
 
 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 
	0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
	2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,
 
	1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
	5, 6, 6, 7, 8, 8, 9,10,11,12,13,14,16,16,16,16,
 
	2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
	8 , 8, 9,10,11,12,13,14,16,17,19,20,22,22,22,22
};


 
 
static const UINT8 opn_fktable[16] = {0,0,0,0,0,0,0,1,2,3,3,3,3,3,3,3};


 
 
static const UINT32 lfo_samples_per_step[8] = {108, 77, 71, 67, 62, 44, 8, 5};



                                                          
                               
                                            

                                                                          
                                                                          
                                                                          

                                             

                                                                    
                            
              
                
                
                 
  
static const UINT8 lfo_ams_depth_shift[4] = {8, 3, 1, 0};



                                                          
                                         

                                                                         
                                                 

                                                                   
                                                                 
                     

                                                              
                           

              
                                                           
                                                                 
                                                                      
                                                                     
                                        
  
static const UINT8 lfo_pm_output[7*8][8]={  
 
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   1,   1,   1,   1},

 
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   1,   1,   1,   1},
  {0,   0,   1,   1,   2,   2,   2,   3},

 
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   1},
  {0,   0,   0,   0,   1,   1,   1,   1},
  {0,   0,   1,   1,   2,   2,   2,   3},
  {0,   0,   2,   3,   4,   4,   5,   6},

 
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   0,   0,   1,   1},
  {0,   0,   0,   0,   1,   1,   1,   1},
  {0,   0,   0,   1,   1,   1,   1,   2},
  {0,   0,   1,   1,   2,   2,   2,   3},
  {0,   0,   2,   3,   4,   4,   5,   6},
  {0,   0,   4,   6,   8,   8, 0xa, 0xc},

 
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   1,   1,   1,   1},
  {0,   0,   0,   1,   1,   1,   2,   2},
  {0,   0,   1,   1,   2,   2,   3,   3},
  {0,   0,   1,   2,   2,   2,   3,   4},
  {0,   0,   2,   3,   4,   4,   5,   6},
  {0,   0,   4,   6,   8,   8, 0xa, 0xc},
  {0,   0,   8, 0xc,0x10,0x10,0x14,0x18},

 
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   2,   2,   2,   2},
  {0,   0,   0,   2,   2,   2,   4,   4},
  {0,   0,   2,   2,   4,   4,   6,   6},
  {0,   0,   2,   4,   4,   4,   6,   8},
  {0,   0,   4,   6,   8,   8, 0xa, 0xc},
  {0,   0,   8, 0xc,0x10,0x10,0x14,0x18},
  {0,   0,0x10,0x18,0x20,0x20,0x28,0x30},

 
  {0,   0,   0,   0,   0,   0,   0,   0},
  {0,   0,   0,   0,   4,   4,   4,   4},
  {0,   0,   0,   4,   4,   4,   8,   8},
  {0,   0,   4,   4,   8,   8, 0xc, 0xc},
  {0,   0,   4,   8,   8,   8, 0xc,0x10},
  {0,   0,   8, 0xc,0x10,0x10,0x14,0x18},
  {0,   0,0x10,0x18,0x20,0x20,0x28,0x30},
  {0,   0,0x20,0x30,0x40,0x40,0x50,0x60},

};

 
static INT32 lfo_pm_table[128*8*32];  

 
#define OPN_CHAN(N) (N&3)
#define OPN_SLOT(N) ((N>>2)&3)

 
#define SLOT1 0
#define SLOT2 2
#define SLOT3 1
#define SLOT4 3

 
#define OUTD_RIGHT  1
#define OUTD_LEFT   2
#define OUTD_CENTER 3


 
 

#ifdef SAVE_SAMPLE
static FILE *sample[1];
	#if 1	 
		#define SAVE_ALL_CHANNELS \
		{	signed int pom = lt; \
			fputc((unsigned short)pom&0xff,sample[0]); \
			fputc(((unsigned short)pom>>8)&0xff,sample[0]); \
		}
	#else	 
		#define SAVE_ALL_CHANNELS \
		{	signed int pom = lt; \
			fputc((unsigned short)pom&0xff,sample[0]); \
			fputc(((unsigned short)pom>>8)&0xff,sample[0]); \
			pom = rt; \
			fputc((unsigned short)pom&0xff,sample[0]); \
			fputc(((unsigned short)pom>>8)&0xff,sample[0]); \
		}
	#endif
#endif


 
typedef struct
{
	INT32	*DT;		 
	UINT8	KSR;		 
	UINT32	ar;			 
	UINT32	d1r;		 
	UINT32	d2r;		 
	UINT32	rr;			 
	UINT8	ksr;		 
	UINT32	mul;		 

	 
	UINT32	phase;		 
	INT32	Incr;		 

	 
	UINT8	state;		 
	UINT32	tl;			 
	INT32	volume;		 
	UINT32	sl;			 
	UINT32	vol_out;	 

	UINT8	eg_sh_ar;	 
	UINT8	eg_sel_ar;	 
	UINT8	eg_sh_d1r;	 
	UINT8	eg_sel_d1r;	 
	UINT8	eg_sh_d2r;	 
	UINT8	eg_sel_d2r;	 
	UINT8	eg_sh_rr;	 
	UINT8	eg_sel_rr;	 

	UINT8	ssg;		 
	UINT8	ssgn;		 

	UINT8	key;		 

	 
	UINT32	AMmask;		 

} FM_SLOT;

typedef struct
{
	FM_SLOT	SLOT[4];	 

	UINT8	ALGO;		 
	UINT8	FB;			 
	INT32	op1_out[2];	 

	INT32	*connect1;	 
	INT32	*connect3;	 
	INT32	*connect2;	 
	INT32	*connect4;	 

	INT32	*mem_connect; 
	INT32	mem_value;	 

	INT32	pms;		 
	UINT8	ams;		 

	UINT32	fc;			 
	UINT8	kcode;		 
	UINT32	block_fnum;	 
	UINT8	Muted;
} FM_CH;


typedef struct
{
	 
	void *		param;				 
	double		freqbase;			 
	int			timer_prescaler;	 
	UINT8		irq;				 
	UINT8		irqmask;			 
#if FM_BUSY_FLAG_SUPPORT
	TIME_TYPE	busy_expiry_time;	 
#endif
	UINT32		clock;				 
	UINT32		rate;				 
#if RSM_ENABLE
	INT32		rateratio;			 
	INT32		framecnt;			 
	FMSAMPLE	cur_sample[2];		 
	FMSAMPLE	prev_sample[2];		 
#endif
	UINT8		address;			 
	UINT8		status;				 
	UINT32		mode;				 
	UINT8		fn_h;				 
	UINT8		prescaler_sel;		 
	INT32		TA;					 
	INT32		TAC;				 
	UINT8		TB;					 
	INT32		TBC;				 
	 
	INT32		dt_tab[8][32];		 
	 
	FM_TIMERHANDLER	timer_handler;
	FM_IRQHANDLER	IRQ_Handler;
	const ssg_callbacks *SSG;
} FM_ST;



 
 
 

 
typedef struct
{
	UINT32  fc[3];			 
	UINT8	fn_h;			 
	UINT8	kcode[3];		 
	UINT32	block_fnum[3];	 
	UINT8   key_csm;         
} FM_3SLOT;

 
typedef struct
{
	UINT8	type;			 
	FM_ST	ST;				 
	FM_3SLOT SL3;			 
	FM_CH	*P_CH;			 
	unsigned int pan[6*2];	 

	UINT32	eg_cnt;			 
	UINT32	eg_timer;		 
	UINT32	eg_timer_add;	 
	UINT32	eg_timer_overflow; 


	                                                                      
                                                                                   
	UINT32  fn_table[4096];  
	UINT32 fn_max;     

	 
	UINT8   lfo_cnt;             
	UINT32  lfo_timer;           
	UINT32  lfo_timer_add;       
	UINT32  lfo_timer_overflow;  
	UINT32  LFO_AM;              
	UINT32  LFO_PM;              

	INT32	m2,c1,c2;		 
	INT32	mem;			 
	INT32	out_fm[6];		 

} FM_OPN;

 
typedef struct
{
	UINT8		REGS[512];			 
	FM_OPN		OPN;				 
	FM_CH		CH[6];				 
	UINT8		addr_A1;			 

	 
	 
	UINT8		dacen;
	UINT8		dac_test;
	INT32		dacout;
	UINT8		MuteDAC;

	UINT8		WaveOutMode;
	INT32		WaveL;
	INT32		WaveR;
} YM2612;

 
#define LOG_ERR  3       
#define LOG_WAR  2       
#define LOG_INF  1       
#define LOG_LEVEL LOG_INF

#ifndef __RAINE__
#define LOG(n,x) do { if( (n)>=LOG_LEVEL ) logerror x; } while (0)
#endif

 
#define Limit(val, max,min) { \
	if ( val > max )      val = max; \
	else if ( val < min ) val = min; \
}

#if 0
#define USE_VGM_INIT_SWITCH
static UINT8 IsVGMInit = 0;
#endif
static UINT8 PseudoSt = 0x00;
                    
                   
                           

 
INLINE void FM_STATUS_SET(FM_ST *ST,int flag)
{
	 
	ST->status |= flag;
	if ( !(ST->irq) && (ST->status & ST->irqmask) )
	{
		ST->irq = 1;
		 
		if(ST->IRQ_Handler) (ST->IRQ_Handler)(ST->param,1);
	}
}

 
INLINE void FM_STATUS_RESET(FM_ST *ST,int flag)
{
	 
	ST->status &=~flag;
	if ( (ST->irq) && !(ST->status & ST->irqmask) )
	{
		ST->irq = 0;
		 
		if(ST->IRQ_Handler) (ST->IRQ_Handler)(ST->param,0);
	}
}

 
INLINE void FM_IRQMASK_SET(FM_ST *ST,int flag)
{
	ST->irqmask = flag;
	 
	FM_STATUS_SET(ST,0);
	FM_STATUS_RESET(ST,0);
}

INLINE void FM_KEYON(FM_OPN *OPN, FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];

	                       
                                                        
                                                     
	if( !SLOT->key && (!OPN->SL3.key_csm || CH == &OPN->P_CH[3]))
	{
		 
		SLOT->phase = 0;

		 
		SLOT->ssgn = 0;

		if ((SLOT->ar + SLOT->ksr) < 94  )
		{
			SLOT->state = (SLOT->volume <= MIN_ATT_INDEX) ? ((SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC) : EG_ATT;
		}
		else
		{
			 
			SLOT->volume = MIN_ATT_INDEX;

			 
			SLOT->state = (SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC;
		}

		 
		if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)))
			SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
		else
			SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
	}

	SLOT->key = 1;
}

INLINE void FM_KEYOFF(FM_OPN *OPN, FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];

	if (SLOT->key && (!OPN->SL3.key_csm || CH == &OPN->P_CH[3]))
	{
#ifdef USE_VGM_INIT_SWITCH
		if (IsVGMInit)	 
		{
			SLOT->state = EG_OFF;
			SLOT->volume = MAX_ATT_INDEX;
			SLOT->vol_out= MAX_ATT_INDEX;
		}
		else
#endif
		if (SLOT->state>EG_REL)
		{
			SLOT->state = EG_REL;  

			 
			if (SLOT->ssg&0x08)
			{
				 
				if (SLOT->ssgn ^ (SLOT->ssg&0x04))
						SLOT->volume = (0x200 - SLOT->volume);

				 
				if (SLOT->volume >= 0x200)
				{
					SLOT->volume = MAX_ATT_INDEX;
					SLOT->state  = EG_OFF;
				}

				 
				SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
			}
		}
	}

	SLOT->key = 0;
}

INLINE void FM_KEYON_CSM(FM_OPN *OPN, FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];

	if( !SLOT->key && !OPN->SL3.key_csm)
	{
		 
		SLOT->phase = 0;

		 
		SLOT->ssgn = 0;

		if ((SLOT->ar + SLOT->ksr) < 94  )
		{
			SLOT->state = (SLOT->volume <= MIN_ATT_INDEX) ? ((SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC) : EG_ATT;
		}
		else
		{
			 
			SLOT->volume = MIN_ATT_INDEX;

			 
			SLOT->state = (SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC;
		}

		 
		if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)))
			SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
		else
			SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
	}
}

INLINE void FM_KEYOFF_CSM(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if (!SLOT->key)
	{
#ifdef USE_VGM_INIT_SWITCH
		if (IsVGMInit)
		{
			SLOT->state = EG_OFF;
			SLOT->volume = MAX_ATT_INDEX;
			SLOT->vol_out= MAX_ATT_INDEX;
		}
		else
#endif
		if (SLOT->state>EG_REL)
		{
			SLOT->state = EG_REL;  

			 
			if (SLOT->ssg&0x08)
			{
				 
				if (SLOT->ssgn ^ (SLOT->ssg&0x04))
					SLOT->volume = (0x200 - SLOT->volume);

				 
				if (SLOT->volume >= 0x200)
				{
					SLOT->volume = MAX_ATT_INDEX;
					SLOT->state  = EG_OFF;
				}

				 
				SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
			}
		}
	}
}

 
INLINE void set_timers( FM_OPN *OPN, FM_ST *ST, void *n, int v )
{
	 
	 
	 
	 
	 
	 
	 
	 

	if ((OPN->ST.mode ^ v) & 0xC0)
	{
		 
		OPN->P_CH[2].SLOT[SLOT1].Incr=-1;

		 
		if (((v & 0xC0) != 0x80) && OPN->SL3.key_csm)
		{
			 
			FM_KEYOFF_CSM(&OPN->P_CH[2],SLOT1);
			FM_KEYOFF_CSM(&OPN->P_CH[2],SLOT2);
			FM_KEYOFF_CSM(&OPN->P_CH[2],SLOT3);
			FM_KEYOFF_CSM(&OPN->P_CH[2],SLOT4);
			OPN->SL3.key_csm = 0;
		}
	}

	 
	if( v & 0x20 )
		FM_STATUS_RESET(ST,0x02);
	 
	if( v & 0x10 )
		FM_STATUS_RESET(ST,0x01);
	 
	if ((v&2) && !(ST->mode&2))
	{
		ST->TBC = ( 256-ST->TB)<<4;
		 
		if (ST->timer_handler) (ST->timer_handler)(n,1,ST->TBC * ST->timer_prescaler,(int)ST->clock);
	}
	 
	if ((v&1) && !(ST->mode&1))
	{
		ST->TAC = (1024-ST->TA);
		 
		if (ST->timer_handler) (ST->timer_handler)(n,0,ST->TAC * ST->timer_prescaler,(int)ST->clock);
		ST->TAC *= 4096;
	}

	ST->mode = (UINT32)v;
}


 
INLINE void TimerAOver(FM_ST *ST)
{
	 
	if(ST->mode & 0x04) FM_STATUS_SET(ST,0x01);
	 
	ST->TAC = (1024-ST->TA);
	if (ST->timer_handler) (ST->timer_handler)(ST->param,0,ST->TAC * ST->timer_prescaler,(int)ST->clock);
	ST->TAC *= 4096;
}
 
INLINE void TimerBOver(FM_ST *ST)
{
	 
	if(ST->mode & 0x08) FM_STATUS_SET(ST,0x02);
	 
	ST->TBC = ( 256-ST->TB)<<4;
	if (ST->timer_handler) (ST->timer_handler)(ST->param,1,ST->TBC * ST->timer_prescaler,(int)ST->clock);
}


#if FM_INTERNAL_TIMER
 
 

 
	#define INTERNAL_TIMER_A(ST,CSM_CH)					\
	{													\
		if( (ST)->TAC &&  ((ST)->timer_handler==0) )		\
			if( ((ST)->TAC -= (int)((ST)->freqbase*4096)) <= 0 )	\
			{											\
				TimerAOver( ST );						\
				 	\
				if( (ST)->mode & 0x80 )					\
					CSMKeyControll( OPN, CSM_CH );			\
			}											\
	}
 
	#define INTERNAL_TIMER_B(ST,step)						\
	{														\
		if( (ST)->TBC && ((ST)->timer_handler==0) )				\
			if( ((ST)->TBC -= (int)((ST)->freqbase*4096*step)) <= 0 )	\
				TimerBOver( ST );							\
	}
#else  
 
#define INTERNAL_TIMER_A(ST,CSM_CH)
#define INTERNAL_TIMER_B(ST,step)
#endif  



#if FM_BUSY_FLAG_SUPPORT
#define FM_BUSY_CLEAR(ST) ((ST)->busy_expiry_time = UNDEFINED_TIME)
INLINE UINT8 FM_STATUS_FLAG(FM_ST *ST)
{
	if( COMPARE_TIMES(ST->busy_expiry_time, UNDEFINED_TIME) != 0 )
	{
		if (COMPARE_TIMES(ST->busy_expiry_time, FM_GET_TIME_NOW(ST->device->machine)) > 0)
			return ST->status | 0x80;	 
		 
		FM_BUSY_CLEAR(ST);
	}
	return ST->status;
}
INLINE void FM_BUSY_SET(FM_ST *ST,int busyclock )
{
	TIME_TYPE expiry_period = MULTIPLY_TIME_BY_INT(ATTOTIME_IN_HZ(ST->clock), busyclock * ST->timer_prescaler);
	ST->busy_expiry_time = ADD_TIMES(FM_GET_TIME_NOW(ST->device->machine), expiry_period);
}
#else
#define FM_STATUS_FLAG(ST) ((ST)->status)
#define FM_BUSY_SET(ST,bclock) {}
#define FM_BUSY_CLEAR(ST) {}
#endif


 
INLINE void setup_connection( FM_OPN *OPN, FM_CH *CH, int ch )
{
	INT32 *carrier = &OPN->out_fm[ch];

	INT32 **om1 = &CH->connect1;
	INT32 **om2 = &CH->connect3;
	INT32 **oc1 = &CH->connect2;

	INT32 **memc = &CH->mem_connect;

	switch( CH->ALGO )
	{
	case 0:
		 
		*om1 = &OPN->c1;
		*oc1 = &OPN->mem;
		*om2 = &OPN->c2;
		*memc= &OPN->m2;
		break;
	case 1:
		 
		 
		*om1 = &OPN->mem;
		*oc1 = &OPN->mem;
		*om2 = &OPN->c2;
		*memc= &OPN->m2;
		break;
	case 2:
		 
		 
		*om1 = &OPN->c2;
		*oc1 = &OPN->mem;
		*om2 = &OPN->c2;
		*memc= &OPN->m2;
		break;
	case 3:
		 
		 
		*om1 = &OPN->c1;
		*oc1 = &OPN->mem;
		*om2 = &OPN->c2;
		*memc= &OPN->c2;
		break;
	case 4:
		 
		 
		 
		*om1 = &OPN->c1;
		*oc1 = carrier;
		*om2 = &OPN->c2;
		*memc= &OPN->mem;	 
		break;
	case 5:
		 
		 
		 
		*om1 = 0;	 
		*oc1 = carrier;
		*om2 = carrier;
		*memc= &OPN->m2;
		break;
	case 6:
		 
		 
		 
		 
		*om1 = &OPN->c1;
		*oc1 = carrier;
		*om2 = carrier;
		*memc= &OPN->mem;	 
		break;
	case 7:
		 
		 
		 
		 
		 
		*om1 = carrier;
		*oc1 = carrier;
		*om2 = carrier;
		*memc= &OPN->mem;	 
		break;
	}

	CH->connect4 = carrier;
}

 
INLINE void set_det_mul(FM_ST *ST,FM_CH *CH,FM_SLOT *SLOT,int v)
{
	SLOT->mul = (v&0x0f)? (v&0x0f)*2 : 1;
	SLOT->DT  = ST->dt_tab[(v>>4)&7];
	CH->SLOT[SLOT1].Incr=-1;
}

 
INLINE void set_tl(FM_CH *CH,FM_SLOT *SLOT , int v)
{
	SLOT->tl = (v&0x7f)<<(ENV_BITS-7);  
	(void)CH;

	 
	if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)) && (SLOT->state > EG_REL))
		SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
	else
		SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
}

 
INLINE void set_ar_ksr(UINT8 type, FM_CH *CH,FM_SLOT *SLOT,int v)
{
	UINT8 old_KSR = SLOT->KSR;
	(void)type;

	SLOT->ar = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->KSR = 3-(v>>6);
	if (SLOT->KSR != old_KSR)
	{
		CH->SLOT[SLOT1].Incr=-1;
	}

	 
	 
	 
	 
	if ((SLOT->ar + SLOT->ksr) < 94  )
	{
		SLOT->eg_sh_ar  = eg_rate_shift [SLOT->ar  + SLOT->ksr ];
		SLOT->eg_sel_ar = eg_rate_select2612[SLOT->ar  + SLOT->ksr ];
	}
	else
	{
		SLOT->eg_sh_ar  = 0;
		SLOT->eg_sel_ar = 18*RATE_STEPS;	 
	}
}

 
INLINE void set_dr(UINT8 type, FM_SLOT *SLOT,int v)
{
	(void)type;
	SLOT->d1r = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->eg_sh_d1r = eg_rate_shift [SLOT->d1r + SLOT->ksr];
	SLOT->eg_sel_d1r= eg_rate_select2612[SLOT->d1r + SLOT->ksr];
}

 
INLINE void set_sr(UINT8 type, FM_SLOT *SLOT,int v)
{
	(void)type;
	SLOT->d2r = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->eg_sh_d2r = eg_rate_shift [SLOT->d2r + SLOT->ksr];
	SLOT->eg_sel_d2r= eg_rate_select2612[SLOT->d2r + SLOT->ksr];
}

 
INLINE void set_sl_rr(UINT8 type, FM_SLOT *SLOT,int v)
{
	(void)type;
	SLOT->sl = sl_table[ v>>4 ];

	 
	if ((SLOT->state == EG_DEC) && (SLOT->volume >= (INT32)(SLOT->sl)))
		SLOT->state = EG_SUS;

	SLOT->rr  = 34 + ((v&0x0f)<<2);

	SLOT->eg_sh_rr  = eg_rate_shift [SLOT->rr  + SLOT->ksr];
	SLOT->eg_sel_rr = eg_rate_select2612[SLOT->rr  + SLOT->ksr];
}

 
INLINE void advance_lfo(FM_OPN *OPN)
{
	if (OPN->lfo_timer_overflow)    
	{
		 
		OPN->lfo_timer +=  OPN->lfo_timer_add;

		 
		while (OPN->lfo_timer >= OPN->lfo_timer_overflow)
		{
			OPN->lfo_timer -= OPN->lfo_timer_overflow;

			 
			OPN->lfo_cnt = ( OPN->lfo_cnt + 1 ) & 127;

			                                                              
                                           
			 
			 
			if (OPN->lfo_cnt<64)
				OPN->LFO_AM = (UINT32)(OPN->lfo_cnt ^ 63) << 1;
			else
				OPN->LFO_AM = (UINT32)(OPN->lfo_cnt & 63) << 1;

			 
			OPN->LFO_PM = OPN->lfo_cnt >> 2;
		}
	}
}

INLINE void advance_eg_channel(FM_OPN *OPN, FM_SLOT *SLOT)
{
	 
	unsigned int i = 4;  

	do
	{
		switch(SLOT->state)
		{
			case EG_ATT:     
			if (!(OPN->eg_cnt & ((1<<SLOT->eg_sh_ar)-1)))
			{
				 
				SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN->eg_cnt>>SLOT->eg_sh_ar)&7)]))>>4;

				 
				if (SLOT->volume <= MIN_ATT_INDEX)
				{
					SLOT->volume = MIN_ATT_INDEX;
					SLOT->state = (SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC;  
				}

				 
				if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)))   
				SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
				else
					SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
			}
			break;

			case EG_DEC:   
			if (!(OPN->eg_cnt & ((1<<SLOT->eg_sh_d1r)-1)))
			{
				 
				if (SLOT->ssg&0x08)
				{
					 
					if (SLOT->volume < 0x200)
				{
					SLOT->volume += 4 * eg_inc[SLOT->eg_sel_d1r + ((OPN->eg_cnt>>SLOT->eg_sh_d1r)&7)];

					 
					if (SLOT->ssgn ^ (SLOT->ssg&0x04))    
						SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
					else
						SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
				}

				}
				else
				{
				 
				SLOT->volume += eg_inc[SLOT->eg_sel_d1r + ((OPN->eg_cnt>>SLOT->eg_sh_d1r)&7)];

				 
				SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
				}

				 
				if (SLOT->volume >= (INT32)(SLOT->sl))
					SLOT->state = EG_SUS;
			}
			break;

			case EG_SUS:   
			if (!(OPN->eg_cnt & ((1<<SLOT->eg_sh_d2r)-1)))
			{
				 
				if (SLOT->ssg&0x08)
				{
				 
				if (SLOT->volume < 0x200)
				{
					SLOT->volume += 4 * eg_inc[SLOT->eg_sel_d2r + ((OPN->eg_cnt>>SLOT->eg_sh_d2r)&7)];

					 
					if (SLOT->ssgn ^ (SLOT->ssg&0x04))    
						SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
					else
						SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
				}
				}
				else
				{
					 
					SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN->eg_cnt>>SLOT->eg_sh_d2r)&7)];

					 
					if ( SLOT->volume >= MAX_ATT_INDEX )
						SLOT->volume = MAX_ATT_INDEX;
					 

					 
					SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
				}
			}
			break;

			case EG_REL:   
			if (!(OPN->eg_cnt & ((1<<SLOT->eg_sh_rr)-1)))
			{
				 
				if (SLOT->ssg&0x08)
				{
					 
					if (SLOT->volume < 0x200)
						SLOT->volume += 4 * eg_inc[SLOT->eg_sel_rr + ((OPN->eg_cnt>>SLOT->eg_sh_rr)&7)];
				 
				if (SLOT->volume >= 0x200)
				{
					SLOT->volume = MAX_ATT_INDEX;
					SLOT->state = EG_OFF;
				}
				}
				else
				{
					 
					SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN->eg_cnt>>SLOT->eg_sh_rr)&7)];

					 
					if (SLOT->volume >= MAX_ATT_INDEX)
					{
						SLOT->volume = MAX_ATT_INDEX;
						SLOT->state = EG_OFF;
					}
				}

				 
				SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;

			}
			break;
		}

		                                                                                 
                                   
                                                                                           
                        
		#if 0
		out = ((UINT32)SLOT->volume);

		 
		if ((SLOT->ssg&0x08) && (SLOT->ssgn&2) && (SLOT->state > EG_REL))
			out ^= MAX_ATT_INDEX;

		                                                                       
                         
		SLOT->vol_out = out + SLOT->tl;
		#endif

		SLOT++;
		i--;
	} while (i);

}

 
 
 
INLINE void update_ssg_eg_channel(FM_SLOT *SLOT)
{
	unsigned int i = 4;  

	do
	{
		 
		 
		 
		if ((SLOT->ssg & 0x08) && (SLOT->volume >= 0x200) && (SLOT->state > EG_REL))
		{
			if (SLOT->ssg & 0x01)   
			{
				 
				if (SLOT->ssg & 0x02)
					SLOT->ssgn = 4;

				 
				if ((SLOT->state != EG_ATT) && !(SLOT->ssgn ^ (SLOT->ssg & 0x04)))
					SLOT->volume  = MAX_ATT_INDEX;
			}
			else   
			{
				 
					if (SLOT->ssg & 0x02)
						SLOT->ssgn ^= 4;
					else
						SLOT->phase = 0;

				 
				if (SLOT->state != EG_ATT)
				{
					if ((SLOT->ar + SLOT->ksr) < 94  )
					{
						SLOT->state = (SLOT->volume <= MIN_ATT_INDEX) ? ((SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC) : EG_ATT;
					}
					else
					{
						 
						SLOT->volume = MIN_ATT_INDEX;
						SLOT->state = (SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC;
					}
				}
			}

			 
			if (SLOT->ssgn ^ (SLOT->ssg&0x04))
				SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
			else
				SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
		}

		 
		SLOT++;
		i--;
	} while (i);
}


INLINE void update_phase_lfo_slot(FM_OPN *OPN, FM_SLOT *SLOT, INT32 pms, UINT32 block_fnum)
{
	UINT32 fnum_lfo   = ((block_fnum & 0x7f0) >> 4) * 32 * 8;
	INT32  lfo_fn_table_index_offset = lfo_pm_table[ fnum_lfo + pms + OPN->LFO_PM ];

	block_fnum = block_fnum*2 + lfo_fn_table_index_offset;

	if (lfo_fn_table_index_offset)     
	{
		UINT8 blk = (block_fnum&0x7000) >> 12;
		UINT32 fn  = block_fnum & 0xfff;

		 
		 
		                                                      
                                                 
                                                  
                                                                     
                                            
		 
		int kc = (blk<<2) | opn_fktable[fn >> 8];
		 

		 
		int fc = (OPN->fn_table[fn]>>(7-blk)) + SLOT->DT[kc];

		 
		if (fc < 0) fc += OPN->fn_max;

		 
		SLOT->phase += (fc * SLOT->mul) >> 1;
	}
	else     
	{
		SLOT->phase += SLOT->Incr;
	}
}

INLINE void update_phase_lfo_channel(FM_OPN *OPN, FM_CH *CH)
{
	UINT32 block_fnum = CH->block_fnum;

	UINT32 fnum_lfo  = ((block_fnum & 0x7f0) >> 4) * 32 * 8;
	INT32  lfo_fn_table_index_offset = lfo_pm_table[ fnum_lfo + CH->pms + OPN->LFO_PM ];

	block_fnum = block_fnum*2 + lfo_fn_table_index_offset;

	if (lfo_fn_table_index_offset)     
	{
		UINT8 blk = (block_fnum&0x7000) >> 12;
		UINT32 fn  = block_fnum & 0xfff;

		 
		 
		 
		int kc = (blk<<2) | opn_fktable[fn >> 8];

		 
		int fc = (OPN->fn_table[fn]>>(7-blk));

		 
		int finc = fc + CH->SLOT[SLOT1].DT[kc];
		if (finc < 0) finc += OPN->fn_max;
		CH->SLOT[SLOT1].phase += (finc*CH->SLOT[SLOT1].mul) >> 1;

		finc = fc + CH->SLOT[SLOT2].DT[kc];
		if (finc < 0) finc += OPN->fn_max;
		CH->SLOT[SLOT2].phase += (finc*CH->SLOT[SLOT2].mul) >> 1;

		finc = fc + CH->SLOT[SLOT3].DT[kc];
		if (finc < 0) finc += OPN->fn_max;
		CH->SLOT[SLOT3].phase += (finc*CH->SLOT[SLOT3].mul) >> 1;

		finc = fc + CH->SLOT[SLOT4].DT[kc];
		if (finc < 0) finc += OPN->fn_max;
		CH->SLOT[SLOT4].phase += (finc*CH->SLOT[SLOT4].mul) >> 1;
	}
	else     
	{
		CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
		CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
		CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
		CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
	}
}

 
INLINE void refresh_fc_eg_slot(FM_OPN *OPN, FM_SLOT *SLOT , int fc , int kc )
{
	int ksr = kc >> SLOT->KSR;

	fc += SLOT->DT[kc];

	 
	if (fc < 0) fc += OPN->fn_max;

	 
	SLOT->Incr = (fc * SLOT->mul) >> 1;

	if( SLOT->ksr != ksr )
	{
		SLOT->ksr = ksr;

		 
		if ((SLOT->ar + SLOT->ksr) < 32+62)
		{
			SLOT->eg_sh_ar  = eg_rate_shift [SLOT->ar  + SLOT->ksr ];
			SLOT->eg_sel_ar = eg_rate_select2612[SLOT->ar  + SLOT->ksr ];
		}
		else
		{
			SLOT->eg_sh_ar  = 0;
			SLOT->eg_sel_ar = 18*RATE_STEPS;  
		}

		SLOT->eg_sh_d1r = eg_rate_shift [SLOT->d1r + SLOT->ksr];
		SLOT->eg_sh_d2r = eg_rate_shift [SLOT->d2r + SLOT->ksr];
		SLOT->eg_sh_rr  = eg_rate_shift [SLOT->rr  + SLOT->ksr];

		SLOT->eg_sel_d1r= eg_rate_select2612[SLOT->d1r + SLOT->ksr];
		SLOT->eg_sel_d2r= eg_rate_select2612[SLOT->d2r + SLOT->ksr];
		SLOT->eg_sel_rr = eg_rate_select2612[SLOT->rr  + SLOT->ksr];
	}
}

 
INLINE void refresh_fc_eg_chan(FM_OPN *OPN, FM_CH *CH )
{
	if( CH->SLOT[SLOT1].Incr==-1)
	{
		int fc = CH->fc;
		int kc = CH->kcode;
		refresh_fc_eg_slot(OPN, &CH->SLOT[SLOT1] , fc , kc );
		refresh_fc_eg_slot(OPN, &CH->SLOT[SLOT2] , fc , kc );
		refresh_fc_eg_slot(OPN, &CH->SLOT[SLOT3] , fc , kc );
		refresh_fc_eg_slot(OPN, &CH->SLOT[SLOT4] , fc , kc );
	}
}

#define volume_calc(OP) ((OP)->vol_out + (AM & (OP)->AMmask))

INLINE signed int op_calc(UINT32 phase, unsigned int env, signed int pm)
{
  UINT32 p;

  p = (env<<3) + sin_tab[ ( ((signed int)((phase & ~FREQ_MASK) + (pm<<15))) >> FREQ_SH ) & SIN_MASK ];

  if (p >= TL_TAB_LEN)
    return 0;
  return tl_tab[p];
}

INLINE signed int op_calc1(UINT32 phase, unsigned int env, signed int pm)
{
  UINT32 p = (env<<3) + sin_tab[ ( ((signed int)((phase & ~FREQ_MASK) + pm      )) >> FREQ_SH ) & SIN_MASK ];
  if (p >= TL_TAB_LEN)
    return 0;
  return tl_tab[p];
}

INLINE void chan_calc(YM2612 *F2612, FM_OPN *OPN, FM_CH *CH)
{
  UINT32 AM = OPN->LFO_AM >> CH->ams;
  unsigned int eg_out;

  if (CH->Muted)
    return;

  OPN->m2 = OPN->c1 = OPN->c2 = OPN->mem = 0;

  *CH->mem_connect = CH->mem_value;   

  eg_out = volume_calc(&CH->SLOT[SLOT1]);
  {
    INT32 out = CH->op1_out[0] + CH->op1_out[1];
    CH->op1_out[0] = CH->op1_out[1];

    if( !CH->connect1 )
    {
       
      OPN->mem = OPN->c1 = OPN->c2 = CH->op1_out[0];
    }
    else
    {
       
      *CH->connect1 += CH->op1_out[0];
    }


    CH->op1_out[1] = 0;
    if( eg_out < ENV_QUIET )   
    {
      if (!CH->FB)
        out=0;

      CH->op1_out[1] = op_calc1(CH->SLOT[SLOT1].phase, eg_out, (out<<CH->FB) );
    }
  }

  eg_out = volume_calc(&CH->SLOT[SLOT3]);
  if( eg_out < ENV_QUIET )     
    *CH->connect3 += op_calc(CH->SLOT[SLOT3].phase, eg_out, OPN->m2);

  eg_out = volume_calc(&CH->SLOT[SLOT2]);
  if( eg_out < ENV_QUIET )     
    *CH->connect2 += op_calc(CH->SLOT[SLOT2].phase, eg_out, OPN->c1);

  eg_out = volume_calc(&CH->SLOT[SLOT4]);
  if( eg_out < ENV_QUIET )     
    *CH->connect4 += op_calc(CH->SLOT[SLOT4].phase, eg_out, OPN->c2);


   
  CH->mem_value = OPN->mem;

   
  if(CH->pms)
  {
     
    if ((OPN->ST.mode & 0xC0) && (CH == &F2612->CH[2]))
    {
      update_phase_lfo_slot(OPN, &CH->SLOT[SLOT1], CH->pms, OPN->SL3.block_fnum[1]);
      update_phase_lfo_slot(OPN, &CH->SLOT[SLOT2], CH->pms, OPN->SL3.block_fnum[2]);
      update_phase_lfo_slot(OPN, &CH->SLOT[SLOT3], CH->pms, OPN->SL3.block_fnum[0]);
      update_phase_lfo_slot(OPN, &CH->SLOT[SLOT4], CH->pms, CH->block_fnum);
    }
    else update_phase_lfo_channel(OPN, CH);
  }
  else   
  {
    CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
    CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
    CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
    CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
  }
}

static void FMCloseTable( void )
{
#ifdef SAVE_SAMPLE
	fclose(sample[0]);
#endif
	return;
}


 
INLINE void CSMKeyControll(FM_OPN *OPN, FM_CH *CH)
{
	 
	FM_KEYON_CSM(OPN,CH,SLOT1);
	FM_KEYON_CSM(OPN,CH,SLOT2);
	FM_KEYON_CSM(OPN,CH,SLOT3);
	FM_KEYON_CSM(OPN,CH,SLOT4);
	OPN->SL3.key_csm = 1;
}

#ifdef __STATE_H__
 
static void FMsave_state_channel(running_device *device,FM_CH *CH,int num_ch)
{
	int slot , ch;

	for(ch=0;ch<num_ch;ch++,CH++)
	{
		 
		state_save_register_device_item_array(device, ch, CH->op1_out);
		state_save_register_device_item(device, ch, CH->fc);
		 
		for(slot=0;slot<4;slot++)
		{
			FM_SLOT *SLOT = &CH->SLOT[slot];
			state_save_register_device_item(device, ch * 4 + slot, SLOT->phase);
			state_save_register_device_item(device, ch * 4 + slot, SLOT->state);
			state_save_register_device_item(device, ch * 4 + slot, SLOT->volume);
		}
	}
}

static void FMsave_state_st(running_device *device,FM_ST *ST)
{
#if FM_BUSY_FLAG_SUPPORT
	state_save_register_device_item(device, 0, ST->busy_expiry_time.seconds );
	state_save_register_device_item(device, 0, ST->busy_expiry_time.attoseconds );
#endif
	state_save_register_device_item(device, 0, ST->address );
	state_save_register_device_item(device, 0, ST->irq     );
	state_save_register_device_item(device, 0, ST->irqmask );
	state_save_register_device_item(device, 0, ST->status  );
	state_save_register_device_item(device, 0, ST->mode    );
	state_save_register_device_item(device, 0, ST->prescaler_sel );
	state_save_register_device_item(device, 0, ST->fn_h );
	state_save_register_device_item(device, 0, ST->TA   );
	state_save_register_device_item(device, 0, ST->TAC  );
	state_save_register_device_item(device, 0, ST->TB  );
	state_save_register_device_item(device, 0, ST->TBC  );
}
#endif  

#if BUILD_OPN
 
static void OPNWriteMode(FM_OPN *OPN, int r, int v)
{
	UINT8 c;
	FM_CH *CH;

	switch(r)
	{
	case 0x21:	 
		break;
	case 0x22:	 
		if (v&8)  
		{
			#if 0
			if (!OPN->lfo_timer_overflow)
			{
				 
				OPN->lfo_cnt   = 0;
				OPN->lfo_timer = 0;
				OPN->LFO_AM    = 0;
				OPN->LFO_PM    = 0;
			}
			#endif

			OPN->lfo_timer_overflow = lfo_samples_per_step[v&7] << LFO_SH;
		}
		else
		{
			                                                
                                       
			OPN->lfo_timer_overflow = 0;
			OPN->lfo_timer = 0;
			OPN->lfo_cnt = 0;


			OPN->LFO_PM = 0;
			OPN->LFO_AM = 126;
			 
		}
		break;
	case 0x24:	 
		OPN->ST.TA = (OPN->ST.TA & 0x03)|(((int)v)<<2);
		break;
	case 0x25:	 
		OPN->ST.TA = (OPN->ST.TA & 0x3fc)|(v&3);
		break;
	case 0x26:	 
		OPN->ST.TB = (UINT8)v;
		break;
	case 0x27:	 
		set_timers( OPN, &(OPN->ST),OPN->ST.param,v );
		break;
	case 0x28:	 
		c = v & 0x03;
		if( c == 3 ) break;
		if( (v&0x04) && (OPN->type & TYPE_6CH) ) c+=3;
		CH = OPN->P_CH;
		CH = &CH[c];
		if(v&0x10) FM_KEYON(OPN,CH,SLOT1); else FM_KEYOFF(OPN,CH,SLOT1);
		if(v&0x20) FM_KEYON(OPN,CH,SLOT2); else FM_KEYOFF(OPN,CH,SLOT2);
		if(v&0x40) FM_KEYON(OPN,CH,SLOT3); else FM_KEYOFF(OPN,CH,SLOT3);
		if(v&0x80) FM_KEYON(OPN,CH,SLOT4); else FM_KEYOFF(OPN,CH,SLOT4);
		break;
	}
}

 
static void OPNWriteReg(FM_OPN *OPN, int r, int v)
{
	FM_CH *CH;
	FM_SLOT *SLOT;

	UINT8 c = OPN_CHAN(r);

	if (c == 3) return;  

	if (r >= 0x100) c+=3;

	CH = OPN->P_CH;
	CH = &CH[c];

	SLOT = &(CH->SLOT[OPN_SLOT(r)]);

	switch( r & 0xf0 ) {
	case 0x30:	 
		set_det_mul(&OPN->ST,CH,SLOT,v);
		break;

	case 0x40:	 
		set_tl(CH,SLOT,v);
		break;

	case 0x50:	 
		set_ar_ksr(OPN->type,CH,SLOT,v);
		break;

	case 0x60:	 
		set_dr(OPN->type, SLOT,v);

		if(OPN->type & TYPE_LFOPAN)  
		{
			SLOT->AMmask = (v&0x80) ? ~0 : 0;
		}
		break;

	case 0x70:	 
		set_sr(OPN->type,SLOT,v);
		break;

	case 0x80:	 
		set_sl_rr(OPN->type,SLOT,v);
		break;

	case 0x90:	 
		SLOT->ssg  =  v&0x0f;

		   
		if (SLOT->state > EG_REL)
		{
			if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)))
				SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
			else
				SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
		}

		                           

               
                    \

                     

                     
                     
                 \

                     
                     
                  

                    \

                     


                         


                                                                        

                                                                         
                 

                                                                          
                                                   

                                                                        
                                                                       
                                                    

                                                        
                                                                        
                                                    

                                                                         
                                                                             

                                                                               
                                                                                 
                                                                                      

                                                                           
                                                                         
                                                              
                       
                                                                  
                                                                               

                                                                   


                  
                                                                            
                                                                         

                                                                   
                                                                                
                                
                                                 
                                              
                                                                           
                                           


                                                                           
                                                                           

          


		break;

	case 0xa0:
		switch( OPN_SLOT(r) )
		{
		case 0:		 
#ifdef USE_VGM_INIT_SWITCH
			if (IsVGMInit)
				OPN->ST.fn_h = CH->block_fnum >> 8;
#endif
			{
				UINT32 fn = (((UINT32)( (OPN->ST.fn_h)&7))<<8) + v;
				UINT8 blk = OPN->ST.fn_h>>3;
				 
				CH->kcode = (blk<<2) | opn_fktable[fn >> 7];
				 
				CH->fc = OPN->fn_table[fn*2]>>(7-blk);

				 
				CH->block_fnum = (blk<<11) | fn;

				CH->SLOT[SLOT1].Incr=-1;
			}
			break;
		case 1:		 
			OPN->ST.fn_h = v&0x3f;
#ifdef USE_VGM_INIT_SWITCH
			if (IsVGMInit)	 
				CH->block_fnum = (OPN->ST.fn_h << 8) | (CH->block_fnum & 0xFF);
#endif
			break;
		case 2:		 
#ifdef USE_VGM_INIT_SWITCH
			if (IsVGMInit)
				OPN->SL3.fn_h = OPN->SL3.block_fnum[c] >> 8;
#endif
			if(r < 0x100)
			{
				UINT32 fn = (((UINT32)(OPN->SL3.fn_h&7))<<8) + v;
				UINT8 blk = OPN->SL3.fn_h>>3;
				 
				OPN->SL3.kcode[c]= (blk<<2) | opn_fktable[fn >> 7];
				 
				OPN->SL3.fc[c] = OPN->fn_table[fn*2]>>(7-blk);
				OPN->SL3.block_fnum[c] = (blk<<11) | fn;
				(OPN->P_CH)[2].SLOT[SLOT1].Incr=-1;
			}
			break;
		case 3:		 
			if(r < 0x100)
			{
				OPN->SL3.fn_h = v&0x3f;
#ifdef USE_VGM_INIT_SWITCH
				if (IsVGMInit)
					OPN->SL3.block_fnum[c] = (OPN->SL3.fn_h << 8) | (OPN->SL3.block_fnum[c] & 0xFF);
#endif
			}
			break;
		}
		break;

	case 0xb0:
		switch( OPN_SLOT(r) )
		{
		case 0:		 
			{
				unsigned char feedback = ((v>>3)&7);
				CH->ALGO = v&7;
				CH->FB   = feedback ? feedback + 6 : 0;
				setup_connection( OPN, CH, c );
			}
			break;
		case 1:		 
			if( OPN->type & TYPE_LFOPAN)
			{
				 
				CH->pms = (v & 7) * 32;  

				 
				CH->ams = lfo_ams_depth_shift[(v>>4) & 0x03];

				 
				OPN->pan[ c*2   ] = (v & 0x80) ? ~0 : 0;
				OPN->pan[ c*2+1 ] = (v & 0x40) ? ~0 : 0;

			}
			break;
		}
		break;
	}
}

 
static void init_timetables(FM_OPN *OPN, double freqbase)
{
	int i,d;
	double rate;

	 
	for (d = 0;d <= 3;d++)
	{
		for (i = 0;i <= 31;i++)
		{
			rate = ((double)dt_tab[d*32 + i]) * freqbase * (1<<(FREQ_SH-10));  
			OPN->ST.dt_tab[d][i]   = (INT32) rate;
			OPN->ST.dt_tab[d+4][i] = -OPN->ST.dt_tab[d][i];
		}
	}

	                                                                      
                                                                                  
	 
	for(i = 0; i < 4096; i++)
	{
		 
		 
		 
		 
		 
		 
		OPN->fn_table[i] = (UINT32)( (double)i * 32 * freqbase * (1<<(FREQ_SH-10)) );  
	}

	 
	OPN->fn_max = (UINT32)( (double)0x20000 * freqbase * (1<<(FREQ_SH-10)) );
}

 
static void OPNSetPres(FM_OPN *OPN, int pres, int timer_prescaler, int SSGpres)
{
	 
	OPN->ST.freqbase = (OPN->ST.rate) ? ((double)OPN->ST.clock / OPN->ST.rate) / pres : 0;

	 
	OPN->eg_timer_add  = (UINT32)((1<<EG_SH) * OPN->ST.freqbase);
	OPN->eg_timer_overflow = ( 3 ) * (1<<EG_SH);

	 
	OPN->lfo_timer_add  = (UINT32)((1<<LFO_SH) * OPN->ST.freqbase);

	 
	OPN->ST.timer_prescaler = timer_prescaler;

	 
	if( SSGpres ) (*OPN->ST.SSG->set_clock)( OPN->ST.param, OPN->ST.clock * 2 / SSGpres );

	 
	init_timetables(OPN, OPN->ST.freqbase);
}

static void reset_channels( FM_ST *ST , FM_CH *CH , int num )
{
    int c,s;
    (void)ST;

	for( c = 0 ; c < num ; c++ )
	{
		 
		CH[c].mem_value = 0;
		CH[c].op1_out[0] = 0;
		CH[c].op1_out[1] = 0;
		CH[c].fc = 0;
		for(s = 0 ; s < 4 ; s++ )
		{
			 
			CH[c].SLOT[s].Incr = -1;
			CH[c].SLOT[s].key = 0;
			CH[c].SLOT[s].phase = 0;
			CH[c].SLOT[s].ssg = 0;
			CH[c].SLOT[s].ssgn = 0;
			CH[c].SLOT[s].state= EG_OFF;
			CH[c].SLOT[s].volume = MAX_ATT_INDEX;
			CH[c].SLOT[s].vol_out= MAX_ATT_INDEX;
		}
	}
}

 
static void init_tables(void)
{
	signed int i,x;
	signed int n;
	double o,m;

	 
	for (x=0; x<TL_RES_LEN; x++)
	{
		m = (1<<16) / pow(2, (x+1) * (ENV_STEP/4.0) / 8.0);
		m = floor(m);

		 
		 

		n = (int)m;		 
		n >>= 4;		 
		if (n&1)		 
			n = (n>>1)+1;
		else
			n = n>>1;
						 
		n <<= 2;		 


		 
		tl_tab[ x*2 + 0 ] = n;
		tl_tab[ x*2 + 1 ] = -tl_tab[ x*2 + 0 ];

		 
		 
		 
		 
		 
		for (i=1; i<13; i++)
		{
			tl_tab[ x*2+0 + i*2*TL_RES_LEN ] =  tl_tab[ x*2+0 ]>>i;
			tl_tab[ x*2+1 + i*2*TL_RES_LEN ] = -tl_tab[ x*2+0 + i*2*TL_RES_LEN ];
		}
	}

	 
	for (i=0; i<SIN_LEN; i++)
	{
		 
		m = sin( ((i*2)+1) * M_PI / SIN_LEN );  
		 

		if (m>0.0)
			o = 8*log(1.0/m)/log(2.0);	 
		else
			o = 8*log(-1.0/m)/log(2.0);	 

		o = o / (ENV_STEP/4);

		n = (int)(2.0*o);
		if (n&1)    		 
			n = (n>>1)+1;
		else
			n = n>>1;

		 
		sin_tab[ i ] = n*2 + (m>=0.0? 0: 1 );
	}

	 
	for(i = 0; i < 8; i++)  
	{
		UINT8 fnum;
		for (fnum=0; fnum<128; fnum++)  
		{
			UINT8 value;
			UINT8 step;
			UINT32 offset_depth = i;
			UINT32 offset_fnum_bit;
			UINT32 bit_tmp;

			for (step=0; step<8; step++)
			{
				value = 0;
				for (bit_tmp=0; bit_tmp<7; bit_tmp++)  
				{
					if (fnum & (1<<bit_tmp))  
					{
						offset_fnum_bit = bit_tmp * 8;
						value += lfo_pm_output[offset_fnum_bit + offset_depth][step];
					}
				}
				 
				lfo_pm_table[(fnum*32*8) + (i*32) + step   + 0] = value;
				lfo_pm_table[(fnum*32*8) + (i*32) +(step^7)+ 8] = value;
				lfo_pm_table[(fnum*32*8) + (i*32) + step   +16] = -value;
				lfo_pm_table[(fnum*32*8) + (i*32) +(step^7)+24] = -value;
			}

		}
	}

#ifdef SAVE_SAMPLE
	sample[0]=fopen("sampsum.pcm","wb");
#endif
}

#endif  


 
 
 

static void ym2612_generate(void *chip, FMSAMPLE *buffer, int frames, int mix)
{
	YM2612 *F2612 = (YM2612 *)chip;
	FM_CH  *cch = F2612->CH;
	FMSAMPLE  *bufOut = buffer;
	int i;
#if !RSM_ENABLE
	FMSAMPLE bufTmp[2];
#endif

	ym2612_pre_generate(chip);

	if (!frames)
	{
		update_ssg_eg_channel(&cch[0].SLOT[SLOT1]);
		update_ssg_eg_channel(&cch[1].SLOT[SLOT1]);
		update_ssg_eg_channel(&cch[2].SLOT[SLOT1]);
		update_ssg_eg_channel(&cch[3].SLOT[SLOT1]);
		update_ssg_eg_channel(&cch[4].SLOT[SLOT1]);
		update_ssg_eg_channel(&cch[5].SLOT[SLOT1]);
	}

	 
	for(i=0 ; i < frames ; i++)
	{
#if RSM_ENABLE
		while(F2612->OPN.ST.framecnt >= F2612->OPN.ST.rateratio) 
		{
			 
			F2612->OPN.ST.prev_sample[0] = F2612->OPN.ST.cur_sample[0];
			F2612->OPN.ST.prev_sample[1] = F2612->OPN.ST.cur_sample[1];
			ym2612_generate_one_native(chip, F2612->OPN.ST.cur_sample);
			F2612->OPN.ST.framecnt -= F2612->OPN.ST.rateratio;
			 
		}
		if (mix)
		{
			*bufOut++ += (FMSAMPLE)((F2612->OPN.ST.prev_sample[0] * (F2612->OPN.ST.rateratio - F2612->OPN.ST.framecnt)
								  + F2612->OPN.ST.cur_sample[0] * F2612->OPN.ST.framecnt) / F2612->OPN.ST.rateratio);
			*bufOut++ += (FMSAMPLE)((F2612->OPN.ST.prev_sample[1] * (F2612->OPN.ST.rateratio - F2612->OPN.ST.framecnt)
								  + F2612->OPN.ST.cur_sample[1] * F2612->OPN.ST.framecnt) / F2612->OPN.ST.rateratio);
		} else {
			*bufOut++ = (FMSAMPLE)((F2612->OPN.ST.prev_sample[0] * (F2612->OPN.ST.rateratio - F2612->OPN.ST.framecnt)
								  + F2612->OPN.ST.cur_sample[0] * F2612->OPN.ST.framecnt) / F2612->OPN.ST.rateratio);
			*bufOut++ = (FMSAMPLE)((F2612->OPN.ST.prev_sample[1] * (F2612->OPN.ST.rateratio - F2612->OPN.ST.framecnt)
								  + F2612->OPN.ST.cur_sample[1] * F2612->OPN.ST.framecnt) / F2612->OPN.ST.rateratio);
		}
		F2612->OPN.ST.framecnt += 1 << RSM_FRAC;
#else
		if (mix)
		{
			ym2612_generate_one_native(chip, bufTmp);
			bufOut[0] += bufTmp[0];
			bufOut[1] += bufTmp[1];
		}
		else
		{
			ym2612_generate_one_native(chip, bufOut);
		}
		bufOut += 2;
#endif
	}
	 
}

void ym2612_pre_generate(void *chip)
{
	YM2612 *F2612 = (YM2612 *)chip;
	FM_OPN *OPN   = &F2612->OPN;
	FM_CH  *cch = F2612->CH;

	 
	refresh_fc_eg_chan( OPN, &cch[0] );
	refresh_fc_eg_chan( OPN, &cch[1] );
	if( (OPN->ST.mode & 0xc0) )
	{
		 
		if( cch[2].SLOT[SLOT1].Incr==-1)
		{
			refresh_fc_eg_slot(OPN, &cch[2].SLOT[SLOT1] , OPN->SL3.fc[1] , OPN->SL3.kcode[1] );
			refresh_fc_eg_slot(OPN, &cch[2].SLOT[SLOT2] , OPN->SL3.fc[2] , OPN->SL3.kcode[2] );
			refresh_fc_eg_slot(OPN, &cch[2].SLOT[SLOT3] , OPN->SL3.fc[0] , OPN->SL3.kcode[0] );
			refresh_fc_eg_slot(OPN, &cch[2].SLOT[SLOT4] , cch[2].fc , cch[2].kcode );
		}
	} else
		refresh_fc_eg_chan( OPN, &cch[2] );
	refresh_fc_eg_chan( OPN, &cch[3] );
	refresh_fc_eg_chan( OPN, &cch[4] );
	refresh_fc_eg_chan( OPN, &cch[5] );
}

void ym2612_generate_one_native(void *chip, FMSAMPLE buffer[])
{
	YM2612 *F2612 = (YM2612 *)chip;
	FM_OPN *OPN   = &F2612->OPN;
	INT32  *out_fm = OPN->out_fm;
	FM_CH  *cch = F2612->CH;
	INT32 dacout;
	int lt,rt;

	if (! F2612->MuteDAC)
		dacout = F2612->dacout;
	else
		dacout = 0;

	 
	out_fm[0] = 0;
	out_fm[1] = 0;
	out_fm[2] = 0;
	out_fm[3] = 0;
	out_fm[4] = 0;
	out_fm[5] = 0;

	 
	update_ssg_eg_channel(&cch[0].SLOT[SLOT1]);
	update_ssg_eg_channel(&cch[1].SLOT[SLOT1]);
	update_ssg_eg_channel(&cch[2].SLOT[SLOT1]);
	update_ssg_eg_channel(&cch[3].SLOT[SLOT1]);
	update_ssg_eg_channel(&cch[4].SLOT[SLOT1]);
	update_ssg_eg_channel(&cch[5].SLOT[SLOT1]);

	 
	if (! F2612->dac_test)
	{
		chan_calc(F2612, OPN, &cch[0]);
		chan_calc(F2612, OPN, &cch[1]);
		chan_calc(F2612, OPN, &cch[2]);
		chan_calc(F2612, OPN, &cch[3]);
		chan_calc(F2612, OPN, &cch[4]);
		if( F2612->dacen )
			cch[5].connect4 += dacout;
		else
			chan_calc(F2612, OPN, &cch[5]);
	}
	else
	{
		out_fm[0] = out_fm[1] = dacout;
		out_fm[2] = out_fm[3] = dacout;
		out_fm[5] = dacout;
	}

	 
	advance_lfo(OPN);

	 
	OPN->eg_timer += OPN->eg_timer_add;
	while (OPN->eg_timer >= OPN->eg_timer_overflow)
	{
		 
		OPN->eg_timer -= OPN->eg_timer_overflow;
		 
		OPN->eg_cnt++;
		 
		if (OPN->eg_cnt == 4096)
			OPN->eg_cnt = 1;

		 
		advance_eg_channel(OPN, &cch[0].SLOT[SLOT1]);
		advance_eg_channel(OPN, &cch[1].SLOT[SLOT1]);
		advance_eg_channel(OPN, &cch[2].SLOT[SLOT1]);
		advance_eg_channel(OPN, &cch[3].SLOT[SLOT1]);
		advance_eg_channel(OPN, &cch[4].SLOT[SLOT1]);
		advance_eg_channel(OPN, &cch[5].SLOT[SLOT1]);
	}

	                                              
                            
                                     
                      
                 

	if (out_fm[0] > 8192) out_fm[0] = 8192;
	else if (out_fm[0] < -8192) out_fm[0] = -8192;
	if (out_fm[1] > 8192) out_fm[1] = 8192;
	else if (out_fm[1] < -8192) out_fm[1] = -8192;
	if (out_fm[2] > 8192) out_fm[2] = 8192;
	else if (out_fm[2] < -8192) out_fm[2] = -8192;
	if (out_fm[3] > 8192) out_fm[3] = 8192;
	else if (out_fm[3] < -8192) out_fm[3] = -8192;
	if (out_fm[4] > 8192) out_fm[4] = 8192;
	else if (out_fm[4] < -8192) out_fm[4] = -8192;
	if (out_fm[5] > 8192) out_fm[5] = 8192;
	else if (out_fm[5] < -8192) out_fm[5] = -8192;

	 
	lt  = ((out_fm[0]>>0) & OPN->pan[0]);
	rt  = ((out_fm[0]>>0) & OPN->pan[1]);
	lt += ((out_fm[1]>>0) & OPN->pan[2]);
	rt += ((out_fm[1]>>0) & OPN->pan[3]);
	lt += ((out_fm[2]>>0) & OPN->pan[4]);
	rt += ((out_fm[2]>>0) & OPN->pan[5]);
	lt += ((out_fm[3]>>0) & OPN->pan[6]);
	rt += ((out_fm[3]>>0) & OPN->pan[7]);
	if (! F2612->dac_test)
	{
		lt += ((out_fm[4]>>0) & OPN->pan[8]);
		rt += ((out_fm[4]>>0) & OPN->pan[9]);
	}
	else
	{
		lt += dacout;
		lt += dacout;
	}
	lt += ((out_fm[5]>>0) & OPN->pan[10]);
	rt += ((out_fm[5]>>0) & OPN->pan[11]);

	 
	 

	#ifdef SAVE_SAMPLE
		SAVE_ALL_CHANNELS
	#endif

	 
	if (F2612->WaveOutMode & 0x01)
		F2612->WaveL = lt;
	if (F2612->WaveOutMode & 0x02)
		F2612->WaveR = rt;
	if (F2612->WaveOutMode ^ 0x03)
		F2612->WaveOutMode ^= 0x03;

	buffer[0] = (FMSAMPLE)(F2612->WaveL / 2);
	buffer[1] = (FMSAMPLE)(F2612->WaveR / 2);

	 
	 
	OPN->SL3.key_csm <<= 1;

	 
	 
	{
		if( OPN->ST.TAC &&  (OPN->ST.timer_handler==0) )
			if( (OPN->ST.TAC -= (int)(OPN->ST.freqbase*4096)) <= 0 )
			{
				TimerAOver( &OPN->ST );
				 
				if( OPN->ST.mode & 0x80 )
					CSMKeyControll( OPN, &cch[2] );
			}
	}

	 
	if (OPN->SL3.key_csm & 2)
	{
		 
		FM_KEYOFF_CSM(&cch[2],SLOT1);
		FM_KEYOFF_CSM(&cch[2],SLOT2);
		FM_KEYOFF_CSM(&cch[2],SLOT3);
		FM_KEYOFF_CSM(&cch[2],SLOT4);
		OPN->SL3.key_csm = 0;
	}
}

#if 0
void ym2612_post_generate(void *chip, int length)
{
	YM2612 *F2612 = (YM2612 *)chip;
	 
	INTERNAL_TIMER_B(&F2612->OPN.ST, length);
}
#endif

#ifdef __STATE_H__
void ym2612_postload(void *chip)
{
	if (chip)
	{
		YM2612 *F2612 = (YM2612 *)chip;
		int r;

		 
		F2612->dacout = ((int)F2612->REGS[0x2a] - 0x80) << 6;	 
		F2612->dacen  = F2612->REGS[0x2d] & 0x80;
		 
		 
		for(r=0x30;r<0x9e;r++)
			if((r&3) != 3)
			{
				OPNWriteReg(&F2612->OPN,r,F2612->REGS[r]);
				OPNWriteReg(&F2612->OPN,r|0x100,F2612->REGS[r|0x100]);
			}
		 
		for(r=0xb0;r<0xb6;r++)
			if((r&3) != 3)
			{
				OPNWriteReg(&F2612->OPN,r,F2612->REGS[r]);
				OPNWriteReg(&F2612->OPN,r|0x100,F2612->REGS[r|0x100]);
			}
		 
		 
	}
}

static void YM2612_save_state(YM2612 *F2612, running_device *device)
{
	state_save_register_device_item_array(device, 0, F2612->REGS);
	FMsave_state_st(device,&F2612->OPN.ST);
	FMsave_state_channel(device,F2612->CH,6);
	 
	state_save_register_device_item_array(device, 0, F2612->OPN.SL3.fc);
	state_save_register_device_item(device, 0, F2612->OPN.SL3.fn_h);
	state_save_register_device_item_array(device, 0, F2612->OPN.SL3.kcode);
	 
	state_save_register_device_item(device, 0, F2612->addr_A1);
}
#endif  

 
static void * ym2612_init(void *param, int clock, int rate,
				FM_TIMERHANDLER timer_handler,FM_IRQHANDLER IRQHandler)
{
	YM2612 *F2612;

	if (clock <= 0 || rate <= 0)
		return NULL;  

	 
	 
	F2612 = (YM2612 *)malloc(sizeof(YM2612));
	if (F2612 == NULL)
		return NULL;
	memset(F2612, 0x00, sizeof(YM2612));
	 
	init_tables();

	F2612->OPN.ST.param = param;
	F2612->OPN.type = TYPE_YM2612;
	F2612->OPN.P_CH = F2612->CH;
	 
	F2612->OPN.ST.clock = clock;
#if RSM_ENABLE
	F2612->OPN.ST.rate = 53267;
	F2612->OPN.ST.rateratio = (INT32)(UINT32)((((UINT64)144 * rate) << RSM_FRAC) / clock);
	F2612->OPN.ST.framecnt = 1 << RSM_FRAC;
	memset(&(F2612->OPN.ST.cur_sample), 0x00, sizeof(FMSAMPLE) * 2);
	memset(&(F2612->OPN.ST.prev_sample), 0x00, sizeof(FMSAMPLE) * 2);
#else
	F2612->OPN.ST.rate = rate;
#endif
	 
	 
	 
	F2612->OPN.ST.timer_handler = timer_handler;
	F2612->OPN.ST.IRQ_Handler   = IRQHandler;

	if (PseudoSt)
		F2612->WaveOutMode = 0x01;
	else
		F2612->WaveOutMode = 0x03;
	                                    
                                                             
                                                                
                  

#ifdef __STATE_H__
	YM2612_save_state(F2612, device);
#endif
	return F2612;
}

 
static void ym2612_shutdown(void *chip)
{
	YM2612 *F2612 = (YM2612 *)chip;
	 

	FMCloseTable();
	 
	free(F2612);
}

 
static void ym2612_reset_chip(void *chip)
{
	int i;
	YM2612 *F2612 = (YM2612 *)chip;
	FM_OPN *OPN   = &F2612->OPN;

	OPNSetPres( OPN, 6*24, 6*24, 0);
	 
	FM_IRQMASK_SET(&OPN->ST,0x03);
	FM_BUSY_CLEAR(&OPN->ST);
	   

#if RSM_ENABLE
	 
	F2612->OPN.ST.framecnt = 1 << RSM_FRAC;
	memset(&(F2612->OPN.ST.cur_sample), 0x00, sizeof(FMSAMPLE) * 2);
	memset(&(F2612->OPN.ST.prev_sample), 0x00, sizeof(FMSAMPLE) * 2);
#endif

	OPN->eg_timer = 0;
	OPN->eg_cnt   = 0;

	OPN->lfo_timer = 0;
	OPN->lfo_cnt   = 0;
	OPN->LFO_AM    = 126;
	OPN->LFO_PM    = 0;

	OPN->ST.TAC    = 0;
	OPN->ST.TBC    = 0;

	OPN->SL3.key_csm = 0;

	OPN->ST.status = 0;
	OPN->ST.mode = 0;

	memset(F2612->REGS, 0x00, sizeof(UINT8) * 512);

	OPNWriteMode(OPN,0x22,0x00);

	OPNWriteMode(OPN,0x27,0x30);
	OPNWriteMode(OPN,0x26,0x00);
	OPNWriteMode(OPN,0x25,0x00);
	OPNWriteMode(OPN,0x24,0x00);

	reset_channels( &OPN->ST , &F2612->CH[0] , 6 );

	for(i = 0xb6 ; i >= 0xb4 ; i-- )
	{
		OPNWriteReg(OPN,i      ,0xc0);
		OPNWriteReg(OPN,i|0x100,0xc0);
	}
	for(i = 0xb2 ; i >= 0x30 ; i-- )
	{
		OPNWriteReg(OPN,i      ,0);
		OPNWriteReg(OPN,i|0x100,0);
	}

	 
	F2612->dacen = 0;
	F2612->dac_test = 0;
	F2612->dacout = 0;

	if (F2612->WaveOutMode == 0x02)
		F2612->WaveOutMode >>= 1;
}

 
 
 
 
static int ym2612_write(void *chip, int a, UINT8 v)
{
	YM2612 *F2612 = (YM2612 *)chip;
	int addr;

	v &= 0xff;	 

	switch( a&3)
	{
	case 0:	 
		F2612->OPN.ST.address = v;
		F2612->addr_A1 = 0;
		break;

	case 1:	 
		if (F2612->addr_A1 != 0)
			break;	 

		addr = F2612->OPN.ST.address;
		F2612->REGS[addr] = v;
		switch( addr & 0xf0 )
		{
		case 0x20:	 
			switch( addr )
			{
			case 0x2a:	 
				ym2612_update_one(chip, DUMMYBUF, 0);
				F2612->dacout = ((int)v - 0x80) << 6;	 
				break;
			case 0x2b:	 
				 
				F2612->dacen = v & 0x80;
				break;
			case 0x2C:	 
				 
				F2612->dac_test = v & 0x20;
				break;
			default:	 
				 
				ym2612_update_one(chip, DUMMYBUF, 0);
				 
				OPNWriteMode(&(F2612->OPN),addr,v);
			}
			break;
		default:	 
			ym2612_update_one(chip, DUMMYBUF, 0);
			 
			OPNWriteReg(&(F2612->OPN),addr,v);
		}
		break;

	case 2:	 
		F2612->OPN.ST.address = v;
		F2612->addr_A1 = 1;
		break;

	case 3:	 
		if (F2612->addr_A1 != 1)
			break;	 

		addr = F2612->OPN.ST.address;
		F2612->REGS[addr | 0x100] = v;
		ym2612_update_one(chip, DUMMYBUF, 0);
		OPNWriteReg(&(F2612->OPN),addr | 0x100,v);
		break;
	}
	return F2612->OPN.ST.irq;
}

#if 0
static UINT8 ym2612_read(void *chip,int a)
{
	YM2612 *F2612 = (YM2612 *)chip;

	switch( a&3)
	{
	case 0:	 
		return FM_STATUS_FLAG(&F2612->OPN.ST);
	case 1:
	case 2:
	case 3:
		 
		return FM_STATUS_FLAG(&F2612->OPN.ST);
	}
	return 0;
}

static int ym2612_timer_over(void *chip,int c)
{
	YM2612 *F2612 = (YM2612 *)chip;

	if( c )
	{	 
		TimerBOver( &(F2612->OPN.ST) );
	}
	else
	{	 
		ym2612_update_one(chip, DUMMYBUF, 0);
		 
		TimerAOver( &(F2612->OPN.ST) );
		 
		if ((F2612->OPN.ST.mode & 0xc0) == 0x80)
		{	 
			CSMKeyControll( &F2612->OPN, &(F2612->CH[2]) );
		}
	}
	return F2612->OPN.ST.irq;
}
#endif

static void ym2612_set_mutemask(void *chip, UINT32 MuteMask)
{
	YM2612 *F2612 = (YM2612 *)chip;
	UINT8 CurChn;

	for (CurChn = 0; CurChn < 6; CurChn ++)
		F2612->CH[CurChn].Muted = (MuteMask >> CurChn) & 0x01;
	F2612->MuteDAC = (MuteMask >> 6) & 0x01;

	return;
}
#if 0
static void ym2612_setoptions(UINT8 Flags)
{
	PseudoSt = (Flags >> 2) & 0x01;

	return;
}
#endif

}  


Ym2612_MAME_Emu::Ym2612_MAME_Emu() { impl = 0; }

Ym2612_MAME_Emu::~Ym2612_MAME_Emu()
{
	if ( impl ) Ym2612_MameImpl::ym2612_shutdown( impl );
}

const char *Ym2612_MAME_Emu::set_rate(double sample_rate, double clock_rate)
{
	if ( impl ) Ym2612_MameImpl::ym2612_shutdown( impl );
	impl = Ym2612_MameImpl::ym2612_init( NULL, static_cast<int>(clock_rate), static_cast<int>(sample_rate), NULL, NULL );
	if ( !impl )
		return "Out of memory";
	return 0;
}

void Ym2612_MAME_Emu::reset()
{
	if ( impl ) Ym2612_MameImpl::ym2612_reset_chip( impl );
}

void Ym2612_MAME_Emu::mute_voices(int mask)
{
	if ( impl ) Ym2612_MameImpl::ym2612_set_mutemask( impl, mask );
}

void Ym2612_MAME_Emu::write0(int addr, int data)
{
	if ( !impl ) return;
	Ym2612_MameImpl::ym2612_write( impl, 0, static_cast<uint8_t>(addr) );
	Ym2612_MameImpl::ym2612_write( impl, 1, static_cast<uint8_t>(data) );
}

void Ym2612_MAME_Emu::write1(int addr, int data)
{
	if ( !impl ) return;
	Ym2612_MameImpl::ym2612_write( impl, 0 + 2, static_cast<uint8_t>(addr) );
	Ym2612_MameImpl::ym2612_write( impl, 1 + 2, static_cast<uint8_t>(data) );
}

void Ym2612_MAME_Emu::run(int pair_count, Ym2612_MAME_Emu::sample_t *out)
{
	if ( impl ) Ym2612_MameImpl::ym2612_generate( impl, out, pair_count, 1);
}
