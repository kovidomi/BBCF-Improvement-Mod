#pragma once
#define IMPL_PALNAME_LENGTH 32
#define IMPL_DESC_LENGTH 64
#define IMPL_CREATOR_LENGTH 32
#define IMPL_PALETTE_DATALEN 1024 // .hpl datalen

struct IMPL_header_t
{
	const char filesig[8] = "IMPL";
	int headerlen;
	int datalen;
	short charindex;
	short RESERVED = 0; //padding
};

struct IMPL_data_t
{
	char palname[IMPL_PALNAME_LENGTH];
	char creator[IMPL_CREATOR_LENGTH];
	char desc[IMPL_DESC_LENGTH];
	char file0[IMPL_PALETTE_DATALEN];
	char file1[IMPL_PALETTE_DATALEN];
	char file2[IMPL_PALETTE_DATALEN];
	char file3[IMPL_PALETTE_DATALEN];
	char file4[IMPL_PALETTE_DATALEN];
	char file5[IMPL_PALETTE_DATALEN];
	char file6[IMPL_PALETTE_DATALEN];
	char file7[IMPL_PALETTE_DATALEN];
};

struct IMPL_t
{
	IMPL_header_t header;
	IMPL_data_t paldata;
};