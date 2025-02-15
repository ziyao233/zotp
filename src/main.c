/***
 *  TOTP: Time-Based One-Time Password Algorithm
 *  Copyright (c) 2023-2024 Yao Zi <ziyao@disroot.org>
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

#ifndef GIT_HASH
	#define GIT_HASH_VERSION "(No Git Version)"
#else
	#define GIT_HASH_VERSION GIT_HASH
#endif

#define T0 0
#define DIGITS 6
#define VALIDITY 30
#define TIME 2

static uint32_t
totp(uint8_t *k, size_t keylen)
{
	time_t t = floor((time(NULL) - T0) / VALIDITY);

	return TOTP(k, keylen, t, DIGITS);
}

static void
usage(void)
{
	puts("zotp: Ziyao's TOTP Token Generator");
	puts("Generate TOTP token from secret key and local time\n");
	puts("Usage:");
	puts("\tzotp <OPTIONS\n");
	puts("Options:");
	puts("\t-k\t\tTOTP secret");
	puts("\t-f <file>\tLoad TOTP from <file> (- for stdin)");
	puts("\t-v\t\tPrint zotp version based on git hash");
	puts("\t-h\t\tPrint this help");
	return;
}

static char *
load_key_from_file(char *path)
{
	FILE *fp = NULL;
	if (!strcmp(path, "-")) {
		/*	read from stdin		*/
		fp = stdin;
	} else {
		fp = fopen(path, "r");
		if (!fp) {
			fprintf(stderr, "Cannot open secret key file %s\n",
				path);
			exit(-1);
		}
	}

	size_t size = 0;
	char *key = NULL;
	check(getline(&key, &size, fp) > 0,
	      "Cannot read secret key from file %s\n", path);

	if (key[strlen(key) - 1] == '\n')
		key[strlen(key) - 1] = '\0';

	if (fp != stdin)
		fclose(fp);

	return key;
}

int
main(int argc, char *argv[])
{
	size_t len;
	size_t keylen;
	uint8_t *k;
	char *key = NULL;
	int opt;
	uint32_t result;

	if (argc <= 1) {
		fprintf(stderr, "Provide at least one argument\n");
		return -1;
	}

	/* Processing cli parameters and make a few checks on the input */
	while ((opt = getopt(argc, argv, "k:f:v")) != -1) {
		switch(opt) {
		case 'k':
			key = strdup(optarg);
			check(key, "Cannot allocate memory for secret key\n");
			break;
		case 'f':
			key = load_key_from_file(optarg);
			break;
		case 'v':
			printf("%s\n", GIT_HASH_VERSION);
			return 0;
		case 'h':
			usage();
			return 0;
		default:
			usage();
			return -1;
		}
	}

	len = strlen(key);
	if (validate_b32key(key, len)) {
		fprintf(stderr, "%s: invalid base32 secret\n", key);
		return -1;
	}

	k = (uint8_t *)key;
	keylen = decode_b32key(&k, len);
	result = totp(k, keylen);
	printf("The resulting OTP value is: %06u\n", result);

	free(key);

	return 0;
}
