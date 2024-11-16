#ifndef _MBASE64_H
#define _MBASE64_H

extern const char b64_alphabet[];
int base64_encode(char *output, char *input, int inputLen);
int base64_decode(char *output, char *input, int inputLen);
int base64_enc_len(int inputLen);
int base64_dec_len(char *input, int inputLen);

#endif