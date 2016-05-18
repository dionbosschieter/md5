/*
  Dion Bosschieter implementation of MD5, message digest 5
  Compile&run: gcc -Wall md5.c -o md5 ; ./md5
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GREEN  "\x1B[32m"
#define NORMAL "\x1B[0m"

void print_64_bits(unsigned long b)
{
    int i;
    int s = 8 * (sizeof b) - 1;

    for (i = s; i >= 0; i--) {
        unsigned long mask = 1L << i;
        putchar(b & mask ? '1' : '0');
    }
    putchar('\n');
}

void print_32_bits(int b)
{
    int i;
    int s = 8 * (sizeof b) - 1;

    for (i = s; i >= 0; i--) {
        int mask = 1 << i;
        putchar(b & mask ? '1' : '0');
    }
    putchar('\n');
}

void print_hex(unsigned char b[], int bufferLength)
{
    for(int i=0;i<bufferLength;i++) {
        printf("%02X ", b[i]);
    }

    putchar('\n');
}

void print_int_array_hex(unsigned int b[], int bufferLength)
{
    for(int i=0;i<bufferLength;i++) {
        printf("0x%02x\t", b[i]);
    }

    putchar('\n');
}

int shifts[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

int a = 0x67452301; //A
int b = 0xefcdab89; //B
int c = 0x98badcfe; //C
int d = 0x10325476; //D

unsigned int K[64];

void print_header(char *message)
{
    printf("\n%s# %s%s\n", GREEN, message, NORMAL);
}

void generate_k()
{
    // compute K
    for(int i=0;i<64;i++) {
        K[i] = (unsigned int)(fabs(sin(i + 1)) * (1L << 32));
    }
}

// define auxiliary functions
int f(int x, int y, int z)
{
    return (x & y) | ((~x) & z);
}

int g(int x, int y, int z)
{
    return (x & z) | (y & ~(z));
}

int h(int x, int y, int z)
{
    return x ^ y ^ z;
}

int f_i(int x, int y, int z)
{
    return y ^ (x | ~(z));
}

unsigned int rotate_left(unsigned int temp, int times)
{
    return (temp << times) | (temp >> (32 - times));
}

int main()
{
    unsigned char message[64] = "123";
    unsigned long bitlength = strlen((char*)message) * sizeof((char*)message);

    printf("Message(%lu) = %s\n", strlen((char*)message), message);

    print_header("Message with padding");
    message[strlen((char*)message)] = 0x80; // add 1 bit padding
    memcpy(message + 56, &bitlength, 8);    // append bit length to the end
    print_hex(message, 64);

    generate_k();
    print_header("Computed K");
    print_int_array_hex(K, 64);

    // print the default variables
    print_header("Default Variables");
    printf("A = %02X\nB = %02X\nC = %02X\nD = %02X\n", a, b, c, d);

    // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15
    unsigned int chunks[16];
    memcpy(chunks, &message, 64);

    print_header("Message as 16 32-bit Words");
    print_int_array_hex(chunks, 16);

    print_header("Preform 4 rounds with each 16 basic operations");

    /* Let [abcd k s i] denote the operation
     * a = b + ((a + F(b,c,d) + M[k] + K[i]) <<< s)
     * where i == 1..64
     */
    print_header("Before all rounds");

    int A = a;
    int B = b;
    int C = c;
    int D = d;

    printf("A=%02X B=%02X C=%02X D=%02X\n", A, B, C, D);

    int prev_round = 0;

    for (int i=0; i<64; i++) {
        int k = i;
        int function_result = 0;
        int s = shifts[i];
        int round = (i >> 4) + 1;

        if (round != prev_round) {
            prev_round = round;
            print_header("New round applying new filters");
        }

        if (round == 1)
            function_result = f(B, C, D);
        else if (round == 2) {
            k = (i * 5 + 1) & 0x0F; // mod 16
            function_result = g(B, C, D);
        } else if (round == 3) {
            k = (i * 3 + 5) & 0x0F; // mod 16
            function_result = h(B, C, D);
        } else if (round == 4) {
            k = (i * 7) & 0x0F; // mod 16
            function_result = f_i(B, C, D);
        }

        printf("%i %02X ", s, ((A + function_result + chunks[k] + K[i]) << s));

        int temp = B + rotate_left((A + function_result + chunks[k] + K[i]), s);
        A = D;
        D = C;
        C = B;
        B = temp;

        int debug[4] = {A, B, C, D};

        printf("\tA=%02X B=%02X C=%02X D=%02X\n", debug[(i + 1) & 3], debug[(i + 2) & 3], debug[(i + 3) & 3], debug[i & 3]);
    }

    int md[4] = {A + a, B + b, C + c, D + d};

    unsigned char md5[16];
    memcpy(md5, md, 16);

    print_header("MD5 Hash");
    print_hex(md5, 16);

    return 0;
}
