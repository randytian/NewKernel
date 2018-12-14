
#ifndef __EVB_PIRT_H__
#define __EVB_PIRT_H__

#include <Protocol/LegacyBiosPlatform.h>
#include "PlatformDxe.h"


#define FIXED_ENTRY_NUM 11

//LPC[45].[7:4]  INTH#
//LPC[45].[3:0]  INTG#
//LPC[44].[7:4]  INTF#
//LPC[44].[3:0]  INTE#
//LPC[55].[7:4]  INTA#
//LPC[56].[7:4]  INTC#
//LPC[56].[3:0]  INTB#
//LPC[57].[7:4]  INTD#

//MTN_20161114 Modify -s
#define PIRQA_LINK         0x55
#define PIRQB_LINK         0x56
#define PIRQC_LINK         0x56
#define PIRQD_LINK         0x57
#define PIRQE_LINK         0x44
#define PIRQF_LINK         0x44
#define PIRQG_LINK         0x45
#define PIRQH_LINK         0x45
/*
#define PIRQA_LINK         1
#define PIRQB_LINK         2
#define PIRQC_LINK         3
#define PIRQD_LINK         4
#define PIRQE_LINK         5
#define PIRQF_LINK         6
#define PIRQG_LINK         7
#define PIRQH_LINK         8
*/
//MTN_20161114 Modify -e


#define PIRQ_MASK          0xDCF8

#if defined(HX002EA0_03)
    #define IRQ_ROUTING_COUNT  20	
#elif defined(HX002EB0_00)
    #define IRQ_ROUTING_COUNT  18	
#elif defined(HX002EB0_11)
    #define IRQ_ROUTING_COUNT	 16
#elif defined(HX002ED0_02)   
    #define IRQ_ROUTING_COUNT	 18
#elif defined(HX002ED0_10)
    #define IRQ_ROUTING_COUNT	 17
#elif defined(HX002EE0_04)	
     #define IRQ_ROUTING_COUNT 20 	
#elif defined(HX002EE0_05)  
    #define IRQ_ROUTING_COUNT	21	   
#elif defined(HX002EC0_01)
    #define IRQ_ROUTING_COUNT	35	   
#elif defined(HX002EC0_10)
    #define IRQ_ROUTING_COUNT	34	   
#elif defined(HX002EH0_01_IOE_03)
    #define IRQ_ROUTING_COUNT	30	 
#elif defined(HX002EH0_01_IOE_23)
    #define IRQ_ROUTING_COUNT	34	 	
#elif defined(HX002EK0_03)
    #define IRQ_ROUTING_COUNT  20
#elif defined(HX002EL0_05)  
    #define IRQ_ROUTING_COUNT	21
#else
     #define IRQ_ROUTING_COUNT 37 // temp
#endif







//YKN_20160309 +S
typedef  struct {
    UINT8 Seg;
    UINT8 Bus;
	UINT8 Dev;
	UINT8 Fun;
	UINT8 PriBus;
	UINT8 SecBus;
	UINT8 Enabled;
	UINT8 SlotNum;
} STRUCT_PCI_BRIDGE;

typedef struct {
  EFI_LEGACY_PIRQ_TABLE_HEADER  Hdr;
  EFI_LEGACY_IRQ_ROUTING_ENTRY  Entry[IRQ_ROUTING_COUNT];
} PIR_DATA;

VOID CollectPCIBusInfo (VOID);
VOID UpdatePIRTable (VOID);

#if defined(HX002EA0_03) || defined(HX002EK0_03)


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
			{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
			{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///D3F0/F1/F2/F3
			{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
			{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
			{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0,0},{0,0}}, 0 ,0}, ///D8F0
			{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
			{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
			{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
			{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
			{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
			{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
			{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
			{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0
			
			{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE0
			{0x02, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 2 ,0},///PE2
			{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 3 ,0},///PE3
			{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 4 ,0},///ITE PCI2PCI Bridge
			{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 5 ,0},///PE4
			{0x06, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 6 ,0},///PE6
			{0x07, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 7 ,0},///PE7
   }

};


#elif defined(HX002EB0_00)


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0
		
		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE0
		{0x02, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 2 ,0},///PE2
		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 3 ,0},///PE3
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 4 ,0},///PE4
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 5 ,0},///PE5
   }
};


#elif defined(HX002EB0_11)


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0
		
		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE0
		{0x02, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 2 ,0},///PE4
		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 3 ,0},///PE6


   }
};


#elif defined(HX002ED0_02)


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0
		
		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE0
		{0x02, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 2 ,0},///PE4
		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 3 ,0},///PE5
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 4 ,0},///PE6
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 5 ,0},///PE7


   }
};



#elif defined(HX002ED0_10)


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0
		
		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE0
		{0x02, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 2 ,0},///PE4
		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 3 ,0},///PE6
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 4 ,0},///PE7

   }
};



#elif defined(HX002EE0_04) 


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0
		
		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE0
		{0x02, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 2 ,0},///PE1
		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 3 ,0},///PE2
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 4 ,0},///PE3
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 5 ,0},///PE4
		{0x06, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 6 ,0},///PE6
		{0x07, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 7 ,0},///PE7

   }
};


#elif defined(HX002EE0_05) || defined(HX002EL0_05)


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0
		
		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE0
		{0x02, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 2 ,0},///PE1
		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 3 ,0},///PE2
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 4 ,0},///PE3
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 5 ,0},///PE4
		{0x06, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 6 ,0},///PE5
		{0x07, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 7 ,0},///PE6
		{0x08, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 7 ,0},///PE7

   }
};

#elif defined(HX002EC0_01) 


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{PIRQH_LINK, PIRQ_MASK},{0,0}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0


		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},/// IOE SwitchUpBridge B1D0F0
			
		{0x02, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D1F0 		
		{0x02, 0x02 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D2F0		
		{0x02, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D3F0		
		{0x02, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D4F0	
		{0x02, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D5F0		
		{0x02, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D8F0		

		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC  B2D1F0 Slot		
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D2F0 Slot		
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D3F0 Slot		
		{0x06, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D4F0 Slot		
		{0x07, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D5F0 Slot		
		{0x08, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}},0 ,0},/// IOE RC B8D0F0 Below device  Busxp2 D0F0 PCIEIF

		{0x09, 0x0D << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DDF0	
		{0x09, 0x0E << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DEF0	
		{0x09, 0x10 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///  IOE PCIE2PCI Bridge B10D10F0	
		{0x09, 0x0F << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DFF0	
		{0x09, 0x12 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10D12F0
			
		{0x0A, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE2
		{0x0B, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///PE4
		{0x0C, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///PE6
		{0x0D, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///PE7

   }
};

#elif defined(HX002EC0_10) 


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0


		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},/// IOE SwitchUpBridge B1D0F0
			
		{0x02, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D1F0 		
		{0x02, 0x02 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D2F0		
		{0x02, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D3F0		
		{0x02, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D4F0	
		{0x02, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D5F0		
		{0x02, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D8F0		

		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC  B2D1F0 Slot		
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D2F0 Slot		
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D3F0 Slot		
		{0x06, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D4F0 Slot		
		{0x07, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D5F0 Slot		
		{0x08, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}},0 ,0},/// IOE RC B8D0F0 Below device  Busxp2 D0F0 PCIEIF

		{0x09, 0x0D << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DDF0	
		{0x09, 0x0E << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DEF0	
		{0x09, 0x10 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///  IOE PCIE2PCI Bridge B10D10F0	
		{0x09, 0x0F << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DFF0	
		{0x09, 0x12 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10D12F0
			
		{0x0A, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE4
		{0x0B, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///PE6
		{0x0C, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///PE7

   }
};

#elif defined(HX002EH0_01_IOE_03) 


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{PIRQH_LINK, PIRQ_MASK},{0,0}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0

		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},/// IOE SwitchUpBridge B1D0F0
			
		{0x02, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D1F0 		
		{0x02, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D5F0		
		{0x02, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D8F0		

		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC  B2D1F0 Slot		
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},/// IOE RC B2D5F0 Slot		
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// ITE8993 PCI2PCI Bridge Slot		
		{0x06, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},/// IOE RC B2D8F0 Slot		

		{0x07, 0x0D << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///
		{0x07, 0x0E << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// 
		{0x07, 0x10 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///  
		{0x07, 0x0F << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},/// 
		{0x07, 0x12 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///
			
		{0x08, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///PE2
		{0x09, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///PE4
		{0x0A, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///PE6
		{0x0B, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///PE7

   }
};


#elif defined(HX002EH0_01_IOE_23) 


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, ///D1F0 D1F1
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{PIRQH_LINK, PIRQ_MASK},{0,0}}, 0 ,0},///D3F0/F1/F2/F3
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///D4F0/F1
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0},///D5F0/F1
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D8F0
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D9F0
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{0, 0},{0, 0}}, 0 ,0},///D10F0/F1
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D11F0
		{0x00, 0x0E << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D14F0/F7
		{0x00, 0x0F << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D15F0
		{0x00, 0x10 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK},{0,0},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0}, ///D16F0/F1/F7
		{0x00, 0x12 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D18F0
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, ///D20F0

		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},/// IOE SwitchUpBridge B1D0F0
			
		{0x02, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D1F0 		
		{0x02, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D3F0		
		{0x02, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D4F0		
		{0x02, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D5F0		
		{0x02, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D8F0		

		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC  B2D1F0 Slot		
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC  B2D3F0 Slot		
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC  B2D4F0 Slot		

		{0x06, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},/// IOE RC B2D5F0 Slot		
		{0x07, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// ITE8993 PCI2PCI Bridge Slot		
		{0x08, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},/// IOE RC B2D8F0 Slot		

		{0x09, 0x0D << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},
		{0x09, 0x0E << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0}, 
		{0x09, 0x10 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},  
		{0x09, 0x0F << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0}, 
		{0x09, 0x12 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},
			
		{0x0A, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},
		{0x0B, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},
		{0x0C, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},
		{0x0D, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},

   }
};



#else


STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,
    0x1106,
    0x300A,
    0,
    {0},
    0               // checksum
  },
  {
		{0x00, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, 				
		{0x00, 0x02 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},
		{0x00, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},
		{0x00, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},
		{0x00, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0}}, 0 ,0}, 		
		{0x00, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0,0},{0,0}}, 0 ,0}, 
		{0x00, 0x09 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},			
		{0x00, 0x0A << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK, PIRQ_MASK},{PIRQD_LINK, PIRQ_MASK}}, 0 ,0},
		{0x00, 0x0B << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, 
		{0x00, 0x0C << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0}, 
		{0x00, 0x14 << 3, {{PIRQA_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},
		
		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},/// IOE SwitchUpBridge B1D0F0
		
		{0x02, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},/// IOE XBCTL B2D0F0			
		{0x02, 0x01 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D1F0		
		{0x02, 0x02 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D2F0		
		{0x02, 0x03 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D3F0		
		{0x02, 0x04 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D4F0	
		{0x02, 0x05 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D5F0		
		///{0x02, 0x06 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D6F0	
		///{0x02, 0x07 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D7F0	
		{0x02, 0x08 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///IOE BUSX+1 D8F0		
		
		{0x03, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC  B2D1F0 Slot		
		{0x04, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D2F0 Slot		
		{0x05, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D3F0 Slot		
		{0x06, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D4F0 Slot		
		{0x07, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D5F0 Slot		
		{0x08, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},/// IOE RC B2D8F0 Slot
		
		{0x09, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}},0 ,0},/// IOE RC B8D0F0 Slot
		
		{0x0A, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},/// IOE PCIE2PCI Bridge B10D0F0 
		{0x0A, 0x0D << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DDF0	
		{0x0A, 0x0E << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DEF0	
		{0x0A, 0x10 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 0 ,0},///  IOE PCIE2PCI Bridge B10D10F0 
		{0x0A, 0x0F << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10DFF0	
		{0x0A, 0x12 << 3, {{PIRQH_LINK, PIRQ_MASK},{0, 0},{0, 0},{0, 0}}, 0 ,0},///  IOE PCIE2PCI Bridge B10D12F0	
		
		{0x0C, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 1 ,0},///SOC PEG0
		{0x0B, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK}}, 2 ,0},///SOC PE0		
		{0x01, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{PIRQH_LINK,PIRQH_LINK},{PIRQH_LINK,PIRQH_LINK},{PIRQH_LINK,PIRQH_LINK}}, 0 ,0},///SOC PEG Internal connect IOE RP
		{0x0D, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///SOC PEG2
		{0x0E, 0x00 << 3, {{PIRQH_LINK, PIRQ_MASK},{0,0},{0,0},{0,0}}, 0 ,0},///SOC PEG3

   }
};


#endif

#endif
  

  
  
  
  
  
  
  
  