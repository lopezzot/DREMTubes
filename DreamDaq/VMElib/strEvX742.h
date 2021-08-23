#ifndef _STR_EV_X742_H_
#define _STR_EV_X742_H_

typedef struct {
	uint32_t sz : 28;
	uint32_t mk : 4;
} v1742_h1;

typedef struct {
	uint32_t mk : 8;
	// uint32_t mk : 4;
	// uint32_t nu2 : 4;
	uint32_t ptt : 19;
	// uint32_t ptt : 14;
	// uint32_t nu1 : 5;
	uint32_t id : 5;
} v1742_h2;

typedef struct {
	uint32_t ec : 32;
	// uint32_t ec : 22;
	// uint32_t nu1 : 10;
} v1742_h3;

typedef struct {
	uint32_t ett : 32;
} v1742_h4;

typedef struct {
	uint32_t csz : 12;
	uint32_t trig : 4;
        uint32_t frq : 4;
	uint32_t sid : 12;
} v1742_gdw;

typedef struct {
	uint32_t ttt : 30;
	uint32_t boh : 2;
} v1742_ttt;

typedef struct {
	uint32_t dtL : 12;
	uint32_t dtM : 12;
	uint32_t dtH : 8;
} v1742_dt1;

typedef struct {
	uint32_t dtL : 4;
	uint32_t dtK : 12;
	uint32_t dtI : 12;
	uint32_t dtH : 4;
} v1742_dt2;

typedef struct {
	uint32_t dtL : 8;
	uint32_t dtM : 12;
	uint32_t dtH : 12;
} v1742_dt3;

typedef union {
	uint32_t v;
	v1742_h1 h1;
	v1742_h2 h2;
	v1742_h3 h3;
	v1742_h4 h4;
	v1742_gdw gdw;
	v1742_ttt ttt;
	v1742_dt1 dt1;
	v1742_dt2 dt2;
	v1742_dt3 dt3;
} v1742_raw;

#endif // _STR_EV_X742_H_
