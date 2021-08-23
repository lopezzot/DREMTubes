#ifndef dream_md5_h
#define dream_md5_h

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long uint32;

struct MD5Context {
    uint32 buf[4];
    uint32 bits[2];
    unsigned char in[64];
};

void MD5Init(struct MD5Context *ctx);
void MD5Update(struct MD5Context *ctx,
               unsigned char *buf, unsigned len);
void MD5Final(unsigned char digest[16], struct MD5Context *ctx);
void MD5Transform(uint32 buf[4], uint32 in[16]);

/* The following function returns 0 on success */
int md5_file_digest(const char *filename, unsigned char digest[16]);

#ifdef __cplusplus
}
#endif

#endif /* !dream_md5_h */
