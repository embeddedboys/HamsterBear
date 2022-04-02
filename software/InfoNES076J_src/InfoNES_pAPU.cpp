/*===================================================================*/
/*                                                                   */
/*  InfoNES_pAPU.cpp : InfoNES Sound Emulation Function              */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on DarcNES and NesterJ )     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/
#include "K6502.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

/*-------------------------------------------------------------------*/
/*  pAPU Event resources                                             */
/*-------------------------------------------------------------------*/

struct ApuEvent_t ApuEventQueue[ APU_EVENT_MAX ];
int  cur_event;
WORD entertime;

/*-------------------------------------------------------------------*/
/*  pAPU Register Write Functions                                    */
/*-------------------------------------------------------------------*/

#define APU_WRITEFUNC(name, evtype) \
void ApuWrite##name(WORD addr, BYTE value) \
{ \
  ApuEventQueue[cur_event].time = entertime - g_wPassedClocks; \
  ApuEventQueue[cur_event].type = APUET_W_##evtype; \
  ApuEventQueue[cur_event].data = value; \
  cur_event++; \
}

APU_WRITEFUNC(C1a, C1A);
APU_WRITEFUNC(C1b, C1B);
APU_WRITEFUNC(C1c, C1C);
APU_WRITEFUNC(C1d, C1D);

APU_WRITEFUNC(C2a, C2A);
APU_WRITEFUNC(C2b, C2B);
APU_WRITEFUNC(C2c, C2C);
APU_WRITEFUNC(C2d, C2D);

APU_WRITEFUNC(C3a, C3A);
APU_WRITEFUNC(C3b, C3B);
APU_WRITEFUNC(C3c, C3C);
APU_WRITEFUNC(C3d, C3D);

APU_WRITEFUNC(C4a, C4A);
APU_WRITEFUNC(C4b, C4B);
APU_WRITEFUNC(C4c, C4C);
APU_WRITEFUNC(C4d, C4D);

APU_WRITEFUNC(Control, CTRL);

ApuWritefunc pAPUSoundRegs[16] = 
{
  ApuWriteC1a,
  ApuWriteC1b,
  ApuWriteC1c,
  ApuWriteC1d,
  ApuWriteC2a,
  ApuWriteC2b,
  ApuWriteC2c,
  ApuWriteC2d,
  ApuWriteC3a,
  ApuWriteC3b,
  ApuWriteC3c,
  ApuWriteC3d,
  ApuWriteC4a,
  ApuWriteC4b,
  ApuWriteC4c,
  ApuWriteC4d,
};

/*-------------------------------------------------------------------*/
/*  pAPU resources                                                   */
/*-------------------------------------------------------------------*/

BYTE wave_buffers[4][735];      /* 44100 / 60 = 735 samples per sync */

BYTE ApuCtrl;
BYTE ApuCtrlNew;

/*-------------------------------------------------------------------*/
/*  pAPU Quality resources                                           */
/*-------------------------------------------------------------------*/

int ApuQuality;

DWORD ApuPulseMagic;
DWORD ApuTriangleMagic;
DWORD ApuNoiseMagic;
unsigned int ApuSamplesPerSync;
unsigned int ApuCyclesPerSample;
unsigned int ApuSampleRate;

struct ApuQualityData_t 
{
  DWORD pulse_magic;
  DWORD triangle_magic;
  DWORD noise_magic;
  unsigned int samples_per_sync;
  unsigned int cycles_per_sample;
  unsigned int sample_rate;
} ApuQual[] = {
  { 0xa2567000, 0x512b3800, 0x512b3800, 367, 82, 22050 },
  { 0x512b3800, 0x289d9c00, 0x289d9c00, 735, 41, 44100 },
};

/*-------------------------------------------------------------------*/
/*  Rectangle Wave #0 resources                                      */
/*-------------------------------------------------------------------*/
BYTE ApuC1a; BYTE ApuC1b;
BYTE ApuC1c; BYTE ApuC1d;

BYTE* ApuC1Wave;
DWORD ApuC1Skip;
DWORD ApuC1Index;
DWORD ApuC1EnvPhase;
BYTE  ApuC1EnvVol;
BYTE  ApuC1Atl;
DWORD ApuC1SweepPhase;
DWORD ApuC1Freq;   

/*-------------------------------------------------------------------*/
/*  Rectangle Wave #1 resources                                      */
/*-------------------------------------------------------------------*/
BYTE ApuC2a; BYTE ApuC2b;
BYTE ApuC2c; BYTE ApuC2d;

BYTE* ApuC2Wave;
DWORD ApuC2Skip;
DWORD ApuC2Index;
DWORD ApuC2EnvPhase;
BYTE  ApuC2EnvVol;
BYTE  ApuC2Atl;   
DWORD ApuC2SweepPhase;
DWORD ApuC2Freq;   

/*-------------------------------------------------------------------*/
/*  Triangle Wave resources                                          */
/*-------------------------------------------------------------------*/
BYTE ApuC3a; BYTE ApuC3b;
BYTE ApuC3c; BYTE ApuC3d;

DWORD ApuC3Skip;
DWORD ApuC3Index;
BYTE  ApuC3Atl;
DWORD ApuC3Llc;                             /* Linear Length Counter */
BYTE  ApuC3WriteLatency;
BYTE  ApuC3CounterStarted;

/*-------------------------------------------------------------------*/
/*  Noise resources                                                  */
/*-------------------------------------------------------------------*/
BYTE ApuC4a; BYTE ApuC4b;
BYTE ApuC4c; BYTE ApuC4d;

DWORD ApuC4Sr;                                     /* Shift register */
DWORD ApuC4Fdc;                          /* Frequency divide counter */
DWORD ApuC4Skip;
DWORD ApuC4Index;
BYTE  ApuC4Atl;
BYTE  ApuC4EnvVol;
DWORD ApuC4EnvPhase;

/*-------------------------------------------------------------------*/
/*  Wave Data                                                        */
/*-------------------------------------------------------------------*/
BYTE pulse_25[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

BYTE pulse_50[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

BYTE pulse_75[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

BYTE pulse_87[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
};

BYTE triangle_50[0x20] = {
  0x00, 0x10, 0x20, 0x30,
  0x40, 0x50, 0x60, 0x70,
  0x80, 0x90, 0xa0, 0xb0,
  0xc0, 0xd0, 0xe0, 0xf0,
  0xff, 0xef, 0xdf, 0xcf,
  0xbf, 0xaf, 0x9f, 0x8f,
  0x7f, 0x6f, 0x5f, 0x4f,
  0x3f, 0x2f, 0x1f, 0x0f,
};

BYTE *pulse_waves[4] = {
  pulse_87, pulse_75, pulse_50, pulse_25,
};

/*-------------------------------------------------------------------*/
/*  Active Time Left Data                                            */
/*-------------------------------------------------------------------*/
BYTE ApuAtl[0x20] = 
{
  5, 127, 10, 1, 19,  2, 40,  3, 80,  4, 30,  5, 7,  6, 13,  7,
  6,   8, 12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15,
};

/*-------------------------------------------------------------------*/
/* Frequency Limit of Rectangle Channels                             */
/*-------------------------------------------------------------------*/
WORD ApuFreqLimit[8] = 
{
   0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0
};

/*-------------------------------------------------------------------*/
/* Noise Frequency Lookup Table                                      */
/*-------------------------------------------------------------------*/
DWORD ApuNoiseFreq[ 16 ] =
{
     4,    8,   16,   32,   64,   96,  128,  160,
   202,  254,  380,  508,  762, 1016, 2034, 4068
};

/*===================================================================*/
/*                                                                   */
/*      InfoNES_pApuWave1() : Rendering Rectangular Wave #0          */
/*                                                                   */
/*===================================================================*/

void InfoNES_pApuWave1( void )
{
  int cycles = 0;
  int event = 0;
  BYTE ctrl = ApuCtrl;

  /* note: 41 CPU cycles occur between increments of i */
  for ( unsigned int i = 0; i < ApuSamplesPerSync; i++ ) 
  {
    /* pAPU Reg Write Event */
	  cycles += ApuCyclesPerSample;
	  while ( ( event < cur_event ) && ( ApuEventQueue[event].time < cycles ) ) 
    {
	    if ( ( ApuEventQueue[event].type & APUET_MASK ) == APUET_C1 ) 
      {
		    switch ( ApuEventQueue[event].type & 0x03 ) 
        {
		      case 0:
	          ApuC1a    = ApuEventQueue[event].data;
            ApuC1Wave = pulse_waves[ ApuC1DutyCycle >> 6 ];
		        break;

		      case 1:
		        ApuC1b    = ApuEventQueue[event].data; 
		        break;

		      case 2:
		        ApuC1c = ApuEventQueue[event].data;
		        ApuC1Freq = ( ( ( (WORD)ApuC1d & 0x07 ) << 8 ) + ApuC1c );
		        ApuC1Atl = ApuAtl[ ( ApuC1d & 0xf8 ) >> 3 ];

		        if ( ApuC1Freq ) 
            {
			        ApuC1Skip = ApuPulseMagic / ApuC1Freq;
		        } else {
			        ApuC1Skip = 0;
		        }
		        break;

		      case 3:
		        ApuC1d = ApuEventQueue[event].data;
		        ApuC1Freq = ( ( ( (WORD)ApuC1d & 0x07 ) << 8 ) + ApuC1c );
		        ApuC1Atl = ApuAtl[ ( ApuC1d & 0xf8 ) >> 3 ];

		        if ( ApuC1Freq ) 
            {
			        ApuC1Skip = ApuPulseMagic / ApuC1Freq;
		        } else {
			        ApuC1Skip = 0;
		        }
            break;
		    }
	    } 
      else if ( ApuEventQueue[event].type == APUET_W_CTRL ) 
      {
		    ctrl = ApuEventQueue[event].data;
	    }
	    event++;
	  }

    /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
    ApuC1EnvPhase -= 4;
    while ( ApuC1EnvPhase < 0 )
    {
      ApuC1EnvPhase += ApuC1EnvDelay;

      if ( ApuC1Hold )
      {
        ApuC1EnvVol = ( ApuC1EnvVol + 1 ) & 0x0f;
      } 
      else if ( ApuC1EnvVol < 0x0f )
      {
        ApuC1EnvVol++;
      }
    }

    /*
     * TODO: using a table of max frequencies is not technically
     * clean, but it is fast and (or should be) accurate
     */
    if ( ApuC1Freq < 8 || ( !ApuC1SweepIncDec && ApuC1Freq > ApuC1FreqLimit ) )
    {
      wave_buffers[0][i] = 0;
      break;
    }

    /* Frequency sweeping at a rate of ( Sweep Delay + 1) / 120 secs */
    if ( ApuC1SweepOn && ApuC1SweepShifts )     
    {
      ApuC1SweepPhase -= 2;           /* 120/60 */
      while ( ApuC1SweepPhase < 0)
      {
        ApuC1SweepPhase += ApuC1SweepDelay;

        if ( ApuC1SweepIncDec ) /* ramp up */
        {
          /* Rectangular #0 */
          ApuC1Freq += ~( ApuC1Freq >> ApuC1SweepShifts );
        } else {
          /* ramp down */
          ApuC1Freq += ( ApuC1Freq >> ApuC1SweepShifts );
        }
      }
      ApuC1Skip = ApuPulseMagic / ApuC1Freq;
    }

    /* Wave Rendering */
	  if ( ( ctrl & 0x01 ) && ( ApuC1Atl || ApuC1Hold ) ) 
    {
	    ApuC1Index += ApuC1Skip;
	    ApuC1Index &= 0x1fffffff;
  
      if ( ApuC1Env )
      {
        wave_buffers[0][i] = ApuC1Wave[ApuC1Index >> 24] * ( ApuC1Vol + ApuC1EnvVol );
      } else {
        wave_buffers[0][i] = ApuC1Wave[ApuC1Index >> 24] * ApuC1Vol;
      }
	  } else {
	    wave_buffers[0][i] = 0;
	  }
  }
  if ( ApuC1Atl ) 
  {
	  ApuC1Atl--;
  }
  ApuCtrlNew = ctrl;
}

/*===================================================================*/
/*                                                                   */
/*      InfoNES_pApuWave2() : Rendering Rectangular Wave #1          */
/*                                                                   */
/*===================================================================*/

void InfoNES_pApuWave2( void )
{
  int cycles = 0;
  int event = 0;
  BYTE ctrl = ApuCtrl;
  
  /* note: 41 CPU cycles occur between increments of i */
  for ( unsigned int i = 0; i < ApuSamplesPerSync; i++ ) 
  {
    /* pAPU Reg Write Event */
	  cycles += ApuCyclesPerSample;
	  while ( ( event < cur_event ) && ( ApuEventQueue[event].time < cycles ) ) 
    {
	    if ( ( ApuEventQueue[event].type & APUET_MASK ) == APUET_C2 ) 
      {
		    switch ( ApuEventQueue[event].type & 0x03 ) 
        {
		      case 0:
	          ApuC2a    = ApuEventQueue[event].data;
            ApuC2Wave = pulse_waves[ ApuC2DutyCycle >> 6 ];
		        break;

		      case 1:
		        ApuC2b    = ApuEventQueue[event].data; 
		        break;

		      case 2:
		        ApuC2c = ApuEventQueue[event].data;
		        ApuC2Freq = ( ( ( (WORD)ApuC2d & 0x07 ) << 8 ) + ApuC2c );
		        ApuC2Atl = ApuAtl[ ( ApuC2d & 0xf8 ) >> 3 ];

		        if ( ApuC2Freq ) 
            {
			        ApuC2Skip = ApuPulseMagic / ApuC2Freq;
		        } else {
			        ApuC2Skip = 0;
		        }
		        break;

		      case 3:
		        ApuC2d = ApuEventQueue[event].data;
		        ApuC2Freq = ( ( ( (WORD)ApuC2d & 0x07 ) << 8 ) + ApuC2c );
		        ApuC2Atl = ApuAtl[ ( ApuC2d & 0xf8 ) >> 3 ];

		        if ( ApuC2Freq ) 
            {
			        ApuC2Skip = ApuPulseMagic / ApuC2Freq;
		        } else {
			        ApuC2Skip = 0;
		        }
            break;
		    }
	    } 
      else if ( ApuEventQueue[event].type == APUET_W_CTRL ) 
      {
		    ctrl = ApuEventQueue[event].data;
	    }
	    event++;
	  }

    /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
    ApuC2EnvPhase -= 4;
    while ( ApuC2EnvPhase < 0 )
    {
      ApuC2EnvPhase += ApuC2EnvDelay;

      if ( ApuC2Hold )
      {
        ApuC2EnvVol = ( ApuC2EnvVol + 1 ) & 0x0f;
      } 
      else if ( ApuC2EnvVol < 0x0f )
      {
        ApuC2EnvVol++;
      }
    }

    /*
     * TODO: using a table of max frequencies is not technically
     * clean, but it is fast and (or should be) accurate
     */
    if ( ApuC2Freq < 8 || ( !ApuC2SweepIncDec && ApuC2Freq > ApuC2FreqLimit ) )
    {
      wave_buffers[1][i] = 0;
      break;
    }

    /* Frequency sweeping at a rate of ( Sweep Delay + 1) / 120 secs */
    if ( ApuC2SweepOn && ApuC2SweepShifts )     
    {
      ApuC2SweepPhase -= 2;           /* 120/60 */
      while ( ApuC2SweepPhase < 0)
      {
        ApuC2SweepPhase += ApuC2SweepDelay;

        if ( ApuC2SweepIncDec ) /* ramp up */
        {
          /* Rectangular #1 */
          ApuC2Freq -= ( ApuC2Freq >> ApuC2SweepShifts );
        } else {
          /* ramp down */
          ApuC2Freq += ( ApuC2Freq >> ApuC2SweepShifts );
        }
      }
      ApuC2Skip = ApuPulseMagic / ApuC2Freq;
    }

    /* Wave Rendering */
	  if ( ( ctrl & 0x01 ) && ( ApuC2Atl || ApuC2Hold ) ) 
    {
	    ApuC2Index += ApuC2Skip;
	    ApuC2Index &= 0x1fffffff;
  
      if ( ApuC2Env )
      {
        wave_buffers[1][i] = ApuC2Wave[ApuC2Index >> 24] * ( ApuC2Vol + ApuC2EnvVol );
      } else {
        wave_buffers[1][i] = ApuC2Wave[ApuC2Index >> 24] * ApuC2Vol;
      }
	  } else {
	    wave_buffers[1][i] = 0;
	  }
  }
  if ( ApuC2Atl ) 
  {
	  ApuC2Atl--;
  }
  ApuCtrlNew = ctrl;
}

/*===================================================================*/
/*                                                                   */
/*      InfoNES_pApuWave3() : Rendering Triangle Wave                */
/*                                                                   */
/*===================================================================*/

void InfoNES_pApuWave3( void )
{
  int cycles = 0;
  int event = 0;
  BYTE ctrl = ApuCtrl;
      
  /* note: 41 CPU cycles occur between increments of i */
  for ( unsigned int i = 0; i < ApuSamplesPerSync; i++) 
  {
	  cycles += ApuCyclesPerSample;
	  while (( event < cur_event ) && ( ApuEventQueue[event].time < cycles ) ) 
    {
	    if ( ( ApuEventQueue[event].type & APUET_MASK ) == APUET_C3 ) 
      {
		    switch ( ApuEventQueue[event].type & 3 ) 
        {
		      case 0:
  		      ApuC3a = ApuEventQueue[event].data;
            ApuC3Llc = ApuC3LinearLength;
            break;

		      case 1:
		        ApuC3b = ApuEventQueue[event].data;
		        break;

  		    case 2:
	  	      ApuC3c = ApuEventQueue[event].data;
		        if ( ApuC3Freq ) 
            {
			        ApuC3Skip = ApuTriangleMagic / ApuC3Freq;
  		      } else {
  			      ApuC3Skip = 0;  
  		      }
  		      break;

  		    case 3:
  		      ApuC3d = ApuEventQueue[event].data;
  		      ApuC3Atl = ApuC3LengthCounter;
  		      if ( ApuC3Freq ) 
            {
  			      ApuC3Skip = ApuTriangleMagic / ApuC3Freq;
  		      } else {
  			      ApuC3Skip = 0;
  		      }
	  	  }
	    } else if ( ApuEventQueue[event].type == APUET_W_CTRL ) 
      {
		    ctrl = ApuEventQueue[event].data;
	    }
	    event++;
	  }

    /* Cutting Min Frequency */
    if ( ApuC3Freq < 8 )
    {
      wave_buffers[2][i] = 0;
      break;
    }

    /* Wave Rendering */
    if ( ( ctrl & 4 ) && ( ( ApuC3Atl > 0 || ApuC3Holdnote ) && ApuC3Llc > 0 ) ) 
    {
	    ApuC3Index += ApuC3Skip;
	    ApuC3Index &= 0x1fffffff;
	    wave_buffers[2][i] = triangle_50[ ApuC3Index >> 24 ];
	  } else {
	    wave_buffers[2][i] = 0;
	  }
  }
  
  /* Counter Control */
  if ( ApuC3CounterStarted )
  {
    if ( ApuC3Atl > 0 && !ApuC3Holdnote ) 
    {
	    ApuC3Atl--;
    }
    if ( ApuC3Llc > 0)
    {
      ApuC3Llc--;
    }
  } else if ( !ApuC3Holdnote && ApuC3WriteLatency > 0 )
  {
    if ( --ApuC3WriteLatency == 0 )
    {
      ApuC3CounterStarted = 0x01;
    }
  }
  ApuCtrlNew = ctrl;
}

/*===================================================================*/
/*                                                                   */
/*      InfoNES_pApuWave4() : Rendering Noise                        */
/*                                                                   */
/*===================================================================*/

void InfoNES_pApuWave4(void)
{
  int cycles = 0;
  int event = 0;
  BYTE ctrl = ApuCtrl;
    
  for ( unsigned int i = 0; i < ApuSamplesPerSync; i++ ) 
  {
	  cycles += ApuCyclesPerSample;
	  while ((event < cur_event) && (ApuEventQueue[event].time < cycles)) 
    {
	    if ( ( ApuEventQueue[event].type & APUET_MASK ) == APUET_C4 ) 
      {
		    switch (ApuEventQueue[event].type & 3) {
		      case 0:
		        ApuC4a = ApuEventQueue[event].data;
            break;

		      case 1:
		        ApuC4b = ApuEventQueue[event].data;
		        break;

		      case 2:
		        ApuC4c = ApuEventQueue[event].data;

		        if ( ApuC4Small ) 
            {
			        ApuC4Sr = 0x001f;
		        } else {
			        ApuC4Sr = 0x01ff;
		        }

            /* Frequency */ 
		        if ( ApuC4Freq ) 
            {
			        ApuC4Skip = ApuNoiseMagic / ApuC4Freq;
		        } else {
			        ApuC4Skip = 0;
		        }
		        ApuC4Atl = ApuC4LengthCounter;
		        break;

		      case 3:
		        ApuC4d = ApuEventQueue[event].data;

            /* Frequency */ 
		        if ( ApuC4Freq ) 
            {
			        ApuC4Skip = ApuNoiseMagic / ApuC4Freq;
		        } else {
			        ApuC4Skip = 0;
		        }
		        ApuC4Atl = ApuC4LengthCounter;
		    }
	    } else if (ApuEventQueue[event].type == APUET_W_CTRL) 
      {
		    ctrl = ApuEventQueue[event].data;
	    } 
	    event++;
	  }

    /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
    ApuC4EnvPhase -= 4;
    while ( ApuC4EnvPhase < 0 )
    {
      ApuC4EnvPhase += ApuC4EnvDelay;

      if ( ApuC4Hold )
      {
        ApuC4EnvVol = ( ApuC4EnvVol + 1 ) & 0x0f;
      } 
      else if ( ApuC4EnvVol < 0x0f )
      {
        ApuC4EnvVol++;
      }
    }

    /* Wave Rendering */
	  if ( ctrl & 8 ) 
    {
	    ApuC4Index += ApuC4Skip;
	    if (ApuC4Index > 0x1fffffff) 
      {
		    if ( ApuC4Small )            /* FIXME: may be wrong */
        { 
		      ApuC4Sr |= ((!(ApuC4Sr & 1)) ^ (!(ApuC4Sr & 4))) << 5;
        } else {
		      ApuC4Sr |= ((!(ApuC4Sr & 1)) ^ (!(ApuC4Sr & 16))) << 9;
		    }
		    ApuC4Sr >>= 1;
	    }
	    ApuC4Index &= 0x1fffffff;

	    if ( ApuC4Atl && ( ApuC4Sr & 1 ) ) 
      {
        if ( !ApuC4Env )
        {
		      wave_buffers[3][i] = ApuC4Vol;
        } else {
          wave_buffers[3][i] = ApuC4EnvVol ^ 0x0f;
        }
	    } else {
		    wave_buffers[3][i] = 0;
	    }
    } else {
	    wave_buffers[3][i] = 0;
    }
  }
  if ( ApuC4Atl && !ApuC4Hold ) 
  {
	   ApuC4Atl--;
  }
  ApuCtrlNew = ctrl;
}

/*===================================================================*/
/*                                                                   */
/*     InfoNES_pApuVsync() : Callback Function per Vsync             */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUVsync(void)
{
  InfoNES_pApuWave1();
  InfoNES_pApuWave2();
  InfoNES_pApuWave3();
  InfoNES_pApuWave4();
    
  ApuCtrl = ApuCtrlNew;
    
  InfoNES_SoundOutput4(ApuSamplesPerSync, wave_buffers[0], wave_buffers[1], wave_buffers[2], wave_buffers[3]);

  entertime = g_wPassedClocks;
  cur_event = 0;
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_pApuInit() : Initialize pApu                   */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUInit(void)
{
  /* Sound Hardware Init */
  InfoNES_SoundInit();

  ApuQuality = pAPU_QUALITY;            // 1: 22050, 2: 44100 [samples/sec]

  if ( ApuQuality > 0 ) 
  {
    if ( ApuQuality > 2 ) 
    {
      ApuQuality = 1;
    } else {
      ApuQuality--;
    }
    ApuPulseMagic      = ApuQual[ ApuQuality ].pulse_magic;
    ApuTriangleMagic   = ApuQual[ ApuQuality ].triangle_magic;
    ApuNoiseMagic      = ApuQual[ ApuQuality ].noise_magic;
    ApuSamplesPerSync  = ApuQual[ ApuQuality ].samples_per_sync;
    ApuCyclesPerSample = ApuQual[ ApuQuality ].cycles_per_sample;
    ApuSampleRate      = ApuQual[ ApuQuality ].sample_rate;
	
    InfoNES_SoundOpen( ApuSamplesPerSync, ApuSampleRate );

    ApuC1Wave = pulse_50;
    ApuC2Wave = pulse_50;

    /* Initialize Rectangular Wave's Regs */
    ApuC1EnvPhase = 0;
    ApuC2EnvPhase = 0;
    ApuC4EnvPhase = 0;

    ApuC1EnvVol = 0;
    ApuC2EnvVol = 0;
    ApuC4EnvVol = 0;

    ApuC1SweepPhase = 0;
    ApuC2SweepPhase = 0;

    /* Initialize Triangle Wave's Regs */
    ApuC3WriteLatency = 3;                           /* Magic Number */
    ApuC3CounterStarted = NULL;
  }

  entertime = g_wPassedClocks;
  cur_event = 0;
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_pApuDone() : Finalize pApu                     */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUDone(void)
{
  InfoNES_SoundClose();
}

/*
 * End of InfoNES_pAPU.cpp
 */
