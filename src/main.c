/***
 *  TOTP: Time-Based One-Time Password Algorithm
 *  Copyright (c) 2017, fmount <fmount9@autistici.org>
 *
 *  Test Vector is composed by:
 *  key: ORSXG5A=
 *  msg: 1234
 *
 *  In order to test the hmac returned value from the openssl implementation
 *  check with bash launching:
 *
 *  $ echo -n "1234" | openssl dgst -sha1 -hmac "test"
 */
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>
#include "rfc4226.h"
#include "rfc6238.h"
#include "utils.h"

#define T0 0
#define DIGITS 6
#define VALIDITY 30
#define TIME 2
#define VERSION 1.0

uint32_t
totp(uint8_t *k, size_t keylen)
{
    time_t t = floor((time(NULL) - T0) / VALIDITY);

    return TOTP(k, keylen, t, DIGITS);
}

void
usage(char *arg)
{
    fprintf(stderr, "Usage %s [-f fname] | [-b b32_secretkey] | [-m mode] [-s] | [-v]\n", arg);
}

int
main(int argc, char *argv[])
{
	size_t pos;
	size_t len;
	size_t keylen;
	uint8_t *k;
	char *fname = NULL;
	char *key;
	int mode = 0;
	int opt;
	uint32_t result;

	if (argc <= 1) {
		fprintf(stderr, "Provide at least one argument\n");
		return -1;
	}

	/* Processing cli parameters and make a few checks on the input */
	while ((opt = getopt(argc, argv, "b:f:m:vs")) != -1) {
		switch(opt) {
		case 'b':
			key = optarg;
			break;
		case 'f':
			fname = optarg;
			break;
		case 'v':
			printf("%s %.1f\n", argv[0], VERSION);
			return 0;
		default:
			usage(argv[0]);
			return -1;
		}
	}

	len = strlen(key);
	if (validate_b32key(key, len, pos) == 1) {
		fprintf(stderr, "%s: invalid base32 secret\n", key);
		return -1;
	}
	k = (uint8_t *)key;
	keylen = decode_b32key(&k, len);
	result = totp(k, keylen);
	printf("The resulting OTP value is: %06u\n", result);

	return 0;
}
