#ifndef _HWFACE_H_
#define _HWFACE_H_

struct HWFaceDescriptor {
  u8 isMaster;
  u8 clkPin;                    /* isMaster?OUT:IN */
  u8 lockInPin;                 /* IN */
  u8 lockOutPin;                /* OUT */
  u8 MISODataPin;               /* isMaster?IN:OUT */
  u8 MOSIDataPin;               /* isMaster?OUT:IN */
};

/* Arduino pins for comms resources */   

#define CTA00_SEC04_L   0     /*     OUT  LCKO_SE */
#define CTA01_SEC03_L   1     /*     IN   MOSI_SE */
#define CTA02_SEC02_L   2     /*     OUT  MISO_SE */
#define CTA03_SEC01_L   3     /*     IN   SCKI_SE */
#define CTA04_SEC00_L   4     /*     IN   LCKI_SE */
#define CTA05_ETC10_L   5     /*     IN   RSRV_ET */
#define CTA06_ETC09_L   6     /*     OUT  LCKO_ET */
#define CTA07_ETC08_L   7     /*     IN   LCKI_ET */
#define CTA08_ETC07_U   8     /*     IN   SCKI_ET */
#define CTA09_ETC06_U   9     /*     OUT  MISO_ET */
#define CTA10_ETC05_U  10     /*     IN   MOSI_ET */
#define CTA11_NEC15_U  11     /*     IN   MISO_NE */
#define CTA12_NEC14_U  12     /*     OUT  MOSI_NE */
#define CTA13_NEC13_U  13     /*     IN   LCKI_NE */
#define CTA14_NEC12_U  14     /*     OUT  MCKO_NE */
#define CTA15_NEC11_U  15     /*     OUT  LCKO_NE */
#define CTB00_NTB15_U  16     /*     IN   RSRV_NT */
#define CTB01_NTB14_U  17     /*     OUT  MCKO_NT */
#define CTB02_NTB13_U  18     /*     OUT  MOSI_NT */
#define CTB03_NTB12_U  19     /*     IN   MISO_NT */
#define CTB04_NTB11_U  20     /*     OUT  LCKO_NT */
#define CTB05_NTB10_U  21     /*     IN   LCKI_NT */
#define CTB06_UNC01_U  22     /* */
#define CTB07_UNC02_U  23     /* */
#define CTB08_UNC03_L  24     /* */
#define CTB09_UNC04_L  25     /* */
#define CTB10_STB05_L  26     /*     OUT  LCKO_ST */
#define CTB11_STB04_L  27     /*     IN   LCKI_ST */
#define CTB12_STB03_L  28     /*     OUT  MISO_ST */
#define CTB13_STB02_L  29     /*     IN   MOSI_ST */
#define CTB14_STB01_L  30     /*     IN   SCKI_ST */
#define CTB15_STB00_L  31     /*     IN   RSRV_ST */
#define CTC00_NWA04_U  32     /*     OUT  LCKO_NW */
#define CTC01_NWA03_U  33     /*     OUT  MCKO_NW */
#define CTC02_NWA02_U  34     /*     IN   MISO_NW */
#define CTC03_NWA01_U  35     /*     OUT  MOSI_NW */
#define CTC04_NWA00_U  36     /*     IN   LCKI_NW */
#define CTC05_WTA10_U  37     /*     OUT  MOSI_WT */
#define CTC06_WTA09_U  38     /*     IN   MISO_WT */
#define CTC07_WTA08_U  39     /*     OUT  MCKO_WT */
#define CTC08_WTA07_L  40     /*     OUT  LCKO_WT */
#define CTC09_WTA06_L  41     /*     IN   LCKI_WT */
#define CTC10_WTA05_L  42     /*     IN   RSRV_WT */
#define CTC11_SWA15_L  43     /*     IN   LCKI_SW */
#define CTC12_SWA14_L  44     /*     IN   SCKI_SW */
#define CTC13_SWA13_L  45     /*     OUT  LCKO_SW */
#define CTC14_SWA12_L  46     /*     IN   MOSI_SW */
#define CTC15_SWA11_L  47     /*     OUT  MISO_SW */

#define ET_CKIS  CTA08_ETC07_U /* IN   */
#define ET_LCKI  CTA07_ETC08_L /* IN   */
#define ET_LCKO  CTA06_ETC09_L /* OUT  */
#define ET_MISO  CTA09_ETC06_U /* OUT  */
#define ET_MOSI  CTA10_ETC05_U /* IN   */
#define ET_RSRV  CTA05_ETC10_L /* IN   */

#define NE_CKOM  CTA14_NEC12_U /* OUT  */
#define NE_LCKI  CTA13_NEC13_U /* IN   */
#define NE_LCKO  CTA15_NEC11_U /* OUT  */
#define NE_MISO  CTA11_NEC15_U /* IN   */
#define NE_MOSI  CTA12_NEC14_U /* OUT  */

#define NT_CKOM  CTB01_NTB14_U /* OUT  */
#define NT_LCKI  CTB05_NTB10_U /* IN   */
#define NT_LCKO  CTB04_NTB11_U /* OUT  */
#define NT_MISO  CTB03_NTB12_U /* IN   */
#define NT_MOSI  CTB02_NTB13_U /* OUT  */
#define NT_RSRV  CTB00_NTB15_U /* IN   */

#define NW_CKOM  CTC01_NWA03_U /* OUT  */
#define NW_LCKI  CTC04_NWA00_U /* IN   */
#define NW_LCKO  CTC00_NWA04_U /* OUT  */
#define NW_MISO  CTC02_NWA02_U /* IN   */
#define NW_MOSI  CTC03_NWA01_U /* OUT  */

#define SE_CKIS  CTA03_SEC01_L /* IN   */
#define SE_LCKI  CTA04_SEC00_L /* IN   */
#define SE_LCKO  CTA00_SEC04_L /* OUT  */
#define SE_MISO  CTA02_SEC02_L /* OUT  */
#define SE_MOSI  CTA01_SEC03_L /* IN   */

#define ST_CKIS  CTB14_STB01_L /* IN   */
#define ST_LCKI  CTB11_STB04_L /* IN   */
#define ST_LCKO  CTB10_STB05_L /* OUT  */
#define ST_MISO  CTB12_STB03_L /* OUT  */
#define ST_MOSI  CTB13_STB02_L /* IN   */
#define ST_RSRV  CTB15_STB00_L /* IN   */

#define SW_CKIS  CTC12_SWA14_L /* IN   */
#define SW_LCKI  CTC11_SWA15_L /* IN   */
#define SW_LCKO  CTC13_SWA13_L /* OUT  */
#define SW_MISO  CTC15_SWA11_L /* OUT  */
#define SW_MOSI  CTC14_SWA12_L /* IN   */

#define WT_CKOM  CTC07_WTA08_U /* OUT  */
#define WT_LCKI  CTC09_WTA06_L /* IN   */
#define WT_LCKO  CTC08_WTA07_L /* OUT  */
#define WT_MISO  CTC06_WTA09_U /* IN   */
#define WT_MOSI  CTC05_WTA10_U /* OUT  */
#define WT_RSRV  CTC10_WTA05_L /* IN   */


#endif /* _HWFACE_H_ */
