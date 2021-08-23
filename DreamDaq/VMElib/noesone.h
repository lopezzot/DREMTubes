static uint32_t* VMEBASE;
#define MAPCAMAC(ptr) { VMEBASE = (uint32_t*)\
                               ((uint64_t)ptr+0x800000);\
                        camac24= (BCNAF24*)VMEBASE;\
                        camac16= (BCNAF16*)VMEBASE;}
#ifndef VCC
/* here for CBD 8210 + standard CCA2 */
/* Q and X response have now 2 arguments (crate is dummy) .Done for
compatibility with VCC case. 7.2.92 */

typedef struct cam24 { uint32_t bcnaf24 [8] [8] [32] [16] [32] ; } BCNAF24;

typedef struct cam16 { uint16_t bcnaf16 [8] [8] [32] [16] [32] [2] ; } BCNAF16;

volatile int16_t noeso_dummy_var;
volatile BCNAF24* camac24 = (BCNAF24*)0xf0800000;
volatile BCNAF16* camac16 = (BCNAF16*)0xf0800000;

#define CAM_ACC24(b,c,n,a,f) camac24->bcnaf24[b] [c] [n] [a] [f]
#define CAM_ACC16(b,c,n,a,f) camac16->bcnaf16[b] [c] [n] [a] [f] [1]
/* "no wait" here is identical to "wait" of course */
#define CAM_ACC24_NW(b,c,n,a,f) camac24->bcnaf24[b] [c] [n] [a] [f]
#define CAM_ACC16_NW(b,c,n,a,f) camac16->bcnaf16[b] [c] [n] [a] [f] [1]

#define Q_RESP(b,c) (noeso_dummy_var=CAM_ACC16(b,0,29,0,0)&0x8000)
#define X_RESP(b,c) (noeso_dummy_var=CAM_ACC16(b,0,29,0,0)&0x4000)

#else

#ifndef V8251

/* here for VIC 8250 + VCC 2117 */
/* CPU is FIC 8232 */
/* vicini has already been run to initialize the VIC;
   vcc_mapping has already been used to map all 14 crate: space used by
   CAMAC is 0x10100000 to 0x10E00000;
   vcc_init has to be called as first action in order to set all the
   crates which are to be used to tranparent/online (calling vcc_init
   with crate=0 sets all the crates which are present) */
/* branch dummy for the moment, kept for compatibility with CBD case */

typedef struct cam24
                  /* Cr  Bm  Ca   0   W    F    N    A */
 { uint32_t bcnaf24 [16] [2] [2] [2] [2] [32] [32] [16] ; } BCNAF24;

typedef struct cam16
                  /* Cr  Bm  Ca   0   W    F    N    A  Si=l */
 { uint16_t bcnaf16 [16] [2] [2] [2] [2] [32] [32] [16] [2] ; } BCNAF16;

volatile int16_t noeso_dummy_var;
volatile BCNAF24* camac24 = (BCNAF24*)0x90000000;
volatile BCNAF16* camac16 = (BCNAF16*)0x90000000;

#define CAM_ACC24(b,c,n,a,f) camac24->bcnaf24 [c] [0] [0] [0] [0] [f] [n] [a]
#define CAM_ACC16(b,c,n,a,f) camac16->bcnaf16 [c] [0] [0] [0] [0] [f] [n] [a] [1]
#define CAM_ACC24_NW(b,c,n,a,f) camac24->bcnaf24 [c] [0] [0] [0] [1] [f] [n] [a]
#define CAM_ACC16_NW(b,c,n,a,f) camac16->bcnaf16 [c] [0] [0] [0] [1] [f] [n] [a] [1]

/* Q and X in internal register ==> bit 18 =1 */
/* When reading in NW mode the result of the last reading action is to
   be found in internal register; this is the reason for the following
   define that is not there for the moment in the CBD section */

#define INT_ACC24(b,c) (camac24->bcnaf24[c] [0] [1] [0] [0] [0] [30] [0])
/* Shift Q and X bit to keep compatibility with CBD case (short destination);
   please note that if destination is long Q is ffff8000 this way ! */
#define Q_RESP(b,c) (noeso_dummy_var=(INT_ACC24(b,c)&0x80000000»>16)
#define X_RESP(b,c) (noeso_dummy_var=(INT_ACC24(b,c)&0x40000000»>16)

#else
/* here for VIC 8251 + VCC 2117 */
/* CPU is FIC 8232 */ 
/* vic51ini has already been run to initialize the VIC;
   vcc_map8251 has already been used to map all 15 crate: space used by
   CAMAC is 0x10400000 to 0x13c00000;
   vcc_51init has to be called as first action in order to set all the
   crates which are to be used to tranparent/online (calling vcc_51init 
   with crate=0 sets all the crates which are present) */
/* branch dummy for the moment, kept for compatibility with CBD case */

typedef struct cam24
                  /* Cr   0  Bm  Ca   0   W    F    N    A */
 { uint32_t bcnaf24 [16] [4] [2] [2] [2] [2] [32] [32] [16] ; } BCNAF24;

typedef struct cam16
                  /* Cr   0  Bm  Ca   0   W    F    N    A  Si=l */
 { uint16_t bcnaf16 [16] [4] [2] [2] [2] [2] [32] [32] [16] [2] ; } BCNAF16;

volatile int16_t noeso_dummy_var;
volatile BCNAF24* camac24 = (BCNAF24*)0x90000000;
volatile BCNAF16* camac16 = (BCNAF16*)0x90000000;

#define CAM_ACC24(b,c,n,a,f) camac24->bcnaf24 [c] [0] [0] [0] [0] [0] [f] [n] [a]
#define CAM_ACC16(b,c,n,a,f) camac16->bcnaf16 [c] [0] [0] [0] [0] [0] [f] [n] [a] [1]

#define CAM_ACC24_NW(b,c,n,a,f) camac24->bcnaf24 [c] [0] [0] [0] [0] [1] [f] [n] [a]
#define CAM_ACC16_NW(b,c,n,a,f) camac16->bcnaf16 [c] [0] [0] [0] [0] [1] [f] [n] [a]

/* Q and X in internal register ==> bit 18 =1 */
/* When reading in NW mode the result of the last reading action is to
   be found in internal register; this is the reason for the following
   define that is not there for the moment in the CBD section */

#define INT_ACC24 (b, c) (camac24->bcnaf24 [c] [0] [0] [1] [0] [0] [0] [30] [0] )
/* Shift Q and X bit to keep compatibility with CBD case (short destination);
   please note that if destination is long Q is ffff8000 this way ! */
#define Q_RESP(b,c) (noeso_dummy_var=(INT_ACC24(b,c)&0x80000000»>16)
#define X_RESP(b,c) (noeso_dummy_var=(INT_ACC24(b,c)&0x40000000»>16)

#endif

#endif

/* same for CBD and VIC case */
#define CLRINH(b,c) (noeso_dummy_var=CAM_ACC16(b,c,30,9,24)) 
#define SETINH(b,c) (noeso_dummy_var=CAM_ACC16(b,c,30,9,26)) 

#define CAMACZ(b,c) (noeso_dummy_var=CAM_ACC16(b,c,28,8,26))
#define CAMACC(b,c) (noeso_dummy_var=CAM_ACC16(b,c,28,9,26))
