#ifndef _STR_EV_X742_H_
#define _STR_EV_X742_H_

typedef struct {
	uint sz : 28;
	uint mk : 4;
} v1742_h1;

typedef struct {
	uint mk : 8;
	// uint mk : 4;
	// uint nu2 : 4;
	uint ptt : 19;
	// uint ptt : 14;
	// uint nu1 : 5;
	uint id : 5;
} v1742_h2;

typedef struct {
	uint ec : 32;
	// uint ec : 22;
	// uint nu1 : 10;
} v1742_h3;

typedef struct {
	uint ett : 32;
} v1742_h4;

typedef struct {
	uint csz : 12;
	uint tsz : 9;
	uint sid : 11;
} v1742_gdw;

typedef struct {
	uint ttt : 32;
} v1742_ttt;

typedef struct {
	uint dtL : 12;
	uint dtM : 12;
	uint dtH : 8;
} v1742_dt1;

typedef struct {
	uint dtL : 4;
	uint dtK : 12;
	uint dtI : 12;
	uint dtH : 4;
} v1742_dt2;

typedef struct {
	uint dtL : 8;
	uint dtM : 12;
	uint dtH : 12;
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
