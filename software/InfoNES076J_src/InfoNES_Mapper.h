/*===================================================================*/
/*                                                                   */
/*  InfoNES_Mapper.h : InfoNES Mapper Function                       */
/*                                                                   */
/*  2000/05/16  InfoNES Project ( based on NesterJ and pNesX )       */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_MAPPER_H_INCLUDED
#define InfoNES_MAPPER_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "InfoNES_Types.h"

/*-------------------------------------------------------------------*/
/*  Macros                                                           */
/*-------------------------------------------------------------------*/

/* The address of 8Kbytes unit of the ROM */
#define ROMPAGE(a)     &ROM[ (a) * 0x2000 ]
/* From behind the ROM, the address of 8kbytes unit */
#define ROMLASTPAGE(a) &ROM[ NesHeader.byRomSize * 0x4000 - ( (a) + 1 ) * 0x2000 ]
/* The address of 1Kbytes unit of the VROM */
#define VROMPAGE(a)    &VROM[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the VRAM */
#define VRAMPAGE(a)    &PPURAM[ 0x2000 + (a) * 0x400 ]
/* The address of 1Kbytes unit of the VRAM */
#define VRAMPAGE0(a)   &PPURAM[ 0x0000 + (a) * 0x400 ]
/* The address of 1Kbytes unit of the VRAM */
#define VRAMPAGE1(a)   &PPURAM[ 0x1000 + (a) * 0x400 ]
/* Translate the pointer to ChrBuf into the address of Pattern Table */ 
#define PATTBL(a)      ( ( (a) - ChrBuf ) >> 2 )

/*-------------------------------------------------------------------*/
/*  Macros ( Mapper specific )                                       */
/*-------------------------------------------------------------------*/

/* The address of 8Kbytes unit of the Map5 ROM */
#define Map5_ROMPAGE(a)     &Map5_Wram[ ( (a) & 0x07 ) * 0x2000 ]
/* The address of 1Kbytes unit of the Map6 Chr RAM */
#define Map6_VROMPAGE(a)    &Map6_Chr_Ram[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the Map19 Chr RAM */
#define Map19_VROMPAGE(a)   &Map19_Chr_Ram[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the Map85 Chr RAM */
#define Map85_VROMPAGE(a)   &Map85_Chr_Ram[ (a) * 0x400 ]

/*-------------------------------------------------------------------*/
/*  Table of Mapper initialize function                              */
/*-------------------------------------------------------------------*/

struct MapperTable_tag
{
  int nMapperNo;
  void (*pMapperInit)();
};

extern struct MapperTable_tag MapperTable[];

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

void Map0_Init();
void Map0_Write( WORD wAddr, BYTE byData );
void Map0_Sram( WORD wAddr, BYTE byData );
void Map0_Apu( WORD wAddr, BYTE byData );
BYTE Map0_ReadApu( WORD wAddr );
void Map0_VSync();
void Map0_HSync();
void Map0_PPU( WORD wAddr );
void Map0_RenderScreen( BYTE byMode );

void Map1_Init();
void Map1_Write( WORD wAddr, BYTE byData );
void Map1_set_ROM_banks();

void Map2_Init();
void Map2_Write( WORD wAddr, BYTE byData );

void Map3_Init();
void Map3_Write( WORD wAddr, BYTE byData );

void Map4_Init();
void Map4_Write( WORD wAddr, BYTE byData );
void Map4_HSync();
void Map4_Set_CPU_Banks();
void Map4_Set_PPU_Banks();

void Map5_Init();
void Map5_Write( WORD wAddr, BYTE byData );
void Map5_Apu( WORD wAddr, BYTE byData );
BYTE Map5_ReadApu( WORD wAddr );
void Map5_HSync();
void Map5_RenderScreen( BYTE byMode );
void Map5_Sync_Prg_Banks( void );

void Map6_Init();
void Map6_Write( WORD wAddr, BYTE byData );
void Map6_Apu( WORD wAddr, BYTE byData );
void Map6_HSync();

void Map7_Init();
void Map7_Write( WORD wAddr, BYTE byData );

void Map8_Init();
void Map8_Write( WORD wAddr, BYTE byData );

void Map9_Init();
void Map9_Write( WORD wAddr, BYTE byData );
void Map9_PPU( WORD wAddr );

void Map10_Init();
void Map10_Write( WORD wAddr, BYTE byData );
void Map10_PPU( WORD wAddr );

void Map11_Init();
void Map11_Write( WORD wAddr, BYTE byData );

void Map15_Init();
void Map15_Write( WORD wAddr, BYTE byData );

void Map16_Init();
void Map16_Write( WORD wAddr, BYTE byData );
void Map16_HSync();

void Map17_Init();
void Map17_Apu( WORD wAddr, BYTE byData );
void Map17_HSync();

void Map18_Init();
void Map18_Write( WORD wAddr, BYTE byData );
void Map18_HSync();

void Map19_Init();
void Map19_Write( WORD wAddr, BYTE byData );
void Map19_Apu( WORD wAddr, BYTE byData );
BYTE Map19_ReadApu( WORD wAddr );
void Map19_HSync();

void Map21_Init();
void Map21_Write( WORD wAddr, BYTE byData );
void Map21_HSync();

void Map22_Init();
void Map22_Write( WORD wAddr, BYTE byData );

void Map23_Init();
void Map23_Write( WORD wAddr, BYTE byData );
void Map23_HSync();

void Map24_Init();
void Map24_Write( WORD wAddr, BYTE byData );
void Map24_HSync();

void Map25_Init();
void Map25_Write( WORD wAddr, BYTE byData );
void Map25_Sync_Vrom( int nBank );
void Map25_HSync();

void Map26_Init();
void Map26_Write( WORD wAddr, BYTE byData );
void Map26_HSync();

void Map32_Init();
void Map32_Write( WORD wAddr, BYTE byData );

void Map33_Init();
void Map33_Write( WORD wAddr, BYTE byData );
void Map33_HSync();

void Map34_Init();
void Map34_Write( WORD wAddr, BYTE byData );
void Map34_Sram( WORD wAddr, BYTE byData );

void Map40_Init();
void Map40_Write( WORD wAddr, BYTE byData );
void Map40_HSync();

void Map41_Init();
void Map41_Write( WORD wAddr, BYTE byData );
void Map41_Sram( WORD wAddr, BYTE byData );

void Map42_Init();
void Map42_Write( WORD wAddr, BYTE byData );
void Map42_HSync();

void Map44_Init();
void Map44_Write( WORD wAddr, BYTE byData );
void Map44_HSync();
void Map44_Set_CPU_Banks();
void Map44_Set_PPU_Banks();

void Map46_Init();
void Map46_Sram( WORD wAddr, BYTE byData );
void Map46_Write( WORD wAddr, BYTE byData );
void Map46_Set_ROM_Banks();

void Map47_Init();
void Map47_Sram( WORD wAddr, BYTE byData );
void Map47_Write( WORD wAddr, BYTE byData );
void Map47_HSync();
void Map47_Set_CPU_Banks();
void Map47_Set_PPU_Banks();

void Map48_Init();
void Map48_Write( WORD wAddr, BYTE byData );
void Map48_HSync();

void Map49_Init();
void Map49_Sram( WORD wAddr, BYTE byData );
void Map49_Write( WORD wAddr, BYTE byData );
void Map49_HSync();
void Map49_Set_CPU_Banks();
void Map49_Set_PPU_Banks();

void Map50_Init();
void Map50_Apu( WORD wAddr, BYTE byData );
void Map50_HSync();

void Map64_Init();
void Map64_Write( WORD wAddr, BYTE byData );

void Map65_Init();
void Map65_Write( WORD wAddr, BYTE byData );
void Map65_HSync();

void Map66_Init();
void Map66_Write( WORD wAddr, BYTE byData );

void Map67_Init();
void Map67_Write( WORD wAddr, BYTE byData );
void Map67_HSync();

void Map68_Init();
void Map68_Write( WORD wAddr, BYTE byData );
void Map68_SyncMirror();

void Map69_Init();
void Map69_Write( WORD wAddr, BYTE byData );
void Map69_HSync();

void Map70_Init();
void Map70_Write( WORD wAddr, BYTE byData );

void Map71_Init();
void Map71_Write( WORD wAddr, BYTE byData );

void Map72_Init();
void Map72_Write( WORD wAddr, BYTE byData );

void Map73_Init();
void Map73_Write( WORD wAddr, BYTE byData );
void Map73_HSync();

void Map75_Init();
void Map75_Write( WORD wAddr, BYTE byData );

void Map76_Init();
void Map76_Write( WORD wAddr, BYTE byData );

void Map77_Init();
void Map77_Write( WORD wAddr, BYTE byData );

void Map78_Init();
void Map78_Write( WORD wAddr, BYTE byData );

void Map79_Init();
void Map79_Apu( WORD wAddr, BYTE byData );

void Map80_Init();
void Map80_Sram( WORD wAddr, BYTE byData );

void Map82_Init();
void Map82_Sram( WORD wAddr, BYTE byData );

void Map83_Init();
void Map83_Write( WORD wAddr, BYTE byData );
void Map83_Apu( WORD wAddr, BYTE byData );
BYTE Map83_ReadApu( WORD wAddr );
void Map83_HSync();

void Map85_Init();
void Map85_Write( WORD wAddr, BYTE byData );
void Map85_HSync();

void Map86_Init();
void Map86_Sram( WORD wAddr, BYTE byData );

void Map87_Init();
void Map87_Sram( WORD wAddr, BYTE byData );

void Map88_Init();
void Map88_Write( WORD wAddr, BYTE byData );

void Map89_Init();
void Map89_Write( WORD wAddr, BYTE byData );

void Map90_Init();
void Map90_Write( WORD wAddr, BYTE byData );
void Map90_Apu( WORD wAddr, BYTE byData );
BYTE Map90_ReadApu( WORD wAddr );
void Map90_HSync();
void Map90_Sync_Mirror( void );
void Map90_Sync_Prg_Banks( void );
void Map90_Sync_Chr_Banks( void );

void Map91_Init();
void Map91_Sram( WORD wAddr, BYTE byData );

void Map92_Init();
void Map92_Write( WORD wAddr, BYTE byData );

void Map93_Init();
void Map93_Sram( WORD wAddr, BYTE byData );

void Map94_Init();
void Map94_Write( WORD wAddr, BYTE byData );

void Map95_Init();
void Map95_Write( WORD wAddr, BYTE byData );
void Map95_Set_CPU_Banks();
void Map95_Set_PPU_Banks();

void Map97_Init();
void Map97_Write( WORD wAddr, BYTE byData );

void Map101_Init();
void Map101_Write( WORD wAddr, BYTE byData );

void Map112_Init();
void Map112_Write( WORD wAddr, BYTE byData );
void Map112_HSync();
void Map112_Set_CPU_Banks();
void Map112_Set_PPU_Banks();

void Map113_Init();
void Map113_Apu( WORD wAddr, BYTE byData );
void Map113_Write( WORD wAddr, BYTE byData );

void Map114_Init();
void Map114_Sram( WORD wAddr, BYTE byData );
void Map114_Write( WORD wAddr, BYTE byData );
void Map114_HSync();
void Map114_Set_CPU_Banks();
void Map114_Set_PPU_Banks();

void Map117_Init();
void Map117_Write( WORD wAddr, BYTE byData );
void Map117_HSync();

void Map118_Init();
void Map118_Write( WORD wAddr, BYTE byData );
void Map118_HSync();
void Map118_Set_CPU_Banks();
void Map118_Set_PPU_Banks();

void Map122_Init();
void Map122_Sram( WORD wAddr, BYTE byData );

void Map151_Init();
void Map151_Write( WORD wAddr, BYTE byData );

void Map160_Init();
void Map160_Write( WORD wAddr, BYTE byData );
void Map160_HSync();

void Map180_Init();
void Map180_Write( WORD wAddr, BYTE byData );

void Map182_Init();
void Map182_Write( WORD wAddr, BYTE byData );
void Map182_HSync();

void Map185_Init();
void Map185_Write( WORD wAddr, BYTE byData );

void Map188_Init();
void Map188_Write( WORD wAddr, BYTE byData );

void Map189_Init();
void Map189_Apu( WORD wAddr, BYTE byData );
void Map189_Write( WORD wAddr, BYTE byData );
void Map189_HSync();

void Map243_Init();
void Map243_Apu( WORD wAddr, BYTE byData );

#endif /* !InfoNES_MAPPER_H_INCLUDED */
