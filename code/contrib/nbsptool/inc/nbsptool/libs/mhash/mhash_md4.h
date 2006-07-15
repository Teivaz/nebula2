#ifndef MD4_H
#define MD4_H

struct MD4Context {
    word32 buf[4];
    word32 bits[2];
    unsigned char in[64];
};

void MD4Init(struct MD4Context *context);
void MD4Update(struct MD4Context *context, unsigned char const *buf,
           unsigned len);
void MD4Final( struct MD4Context *context, unsigned char *digest);
void MD4Transform(word32 buf[4], word32 const in[16]);

typedef struct MD4Context MD4_CTX;

#endif /* !MD4_H */
