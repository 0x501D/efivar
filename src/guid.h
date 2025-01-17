// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * libefivar - library for the manipulation of EFI variables
 * Copyright 2012-2013 Red Hat, Inc.
 */
#ifndef LIBEFIVAR_GUID_H
#define LIBEFIVAR_GUID_H 1

#include <endian.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "efivar_endian.h"

#define GUID_FORMAT "%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x"

static inline int
real_isspace(char c)
{
	char spaces[] = " \f\n\r\t\v";
	for (int i = 0; spaces[i] != '\0'; i++)
		if (c == spaces[i])
			return 1;
	return 0;
}

static inline int
check_sanity(const char *text, size_t len)
{
	size_t sl = strlen("84be9c3e-8a32-42c0-891c-4cd3b072becc");

	errno = EINVAL;
	if (len < sl)
		return -1;
	else if (len > sl && !real_isspace(text[sl]))
		return -1;

	if (text[8] != '-' || text[13] != '-' || text[18] != '-' ||
			text[23] != '-')
		return -1;

	errno = 0;
	return 0;
}

static inline int
check_segment_sanity(const char *text, size_t len)
{
	for(unsigned int i = 0; i < len; i++) {
		if (text[i] >= '0' && text[i] <= '9')
			continue;
		/* "| 0x20" is tolower() without having to worry about
		 * locale concerns, since we know everything here must
		 * be within traditional ascii space. */
		if ((text[i] | 0x20) >= 'a' && (text[i] | 0x20) <= 'f')
			continue;
		errno = EINVAL;
		return -1;
	}
	return 0;
}

static inline int UNUSED
text_to_guid(const char *text, efi_guid_t *guid)
{
	/* these variables represent the length of the /string/ they hold,
	 * not the interpreted length of the value from them.  Mostly the
	 * names make it more obvious to verify that my bounds checking is
	 * correct. */
	char eightbytes[9] = "";
	char fourbytes[5] = "";
	char twobytes[3] = "";
	size_t textlen = strlen(text);
	size_t guidlen = strlen("84be9c3e-8a32-42c0-891c-4cd3b072becc");

	if (textlen == guidlen + 2) {
		if (text[0] != '{' || text[textlen - 1] != '}') {
			errno = EINVAL;
			return -1;
		}
		text++;
		textlen -= 2;
	}

	if (check_sanity(text, textlen) < 0)
		return -1;

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 * ^ */
	memcpy(eightbytes, text, 8);
	if (check_segment_sanity(eightbytes, 8) < 0)
		return -1;
	guid->a = (uint32_t)strtoul(eightbytes, NULL, 16);
	guid->a = cpu_to_le32(guid->a);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *	  ^ */
	memcpy(fourbytes, text+9, 4);
	if (check_segment_sanity(fourbytes, 4) < 0)
		return -1;
	guid->b = (uint16_t)strtoul(fourbytes, NULL, 16);
	guid->b = cpu_to_le16(guid->b);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *	       ^ */
	memcpy(fourbytes, text+14, 4);
	if (check_segment_sanity(fourbytes, 4) < 0)
		return -1;
	guid->c = (uint16_t)strtoul(fourbytes, NULL, 16);
	guid->c = cpu_to_le16(guid->c);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *		    ^ */
	memcpy(fourbytes, text+19, 4);
	if (check_segment_sanity(fourbytes, 4) < 0)
		return -1;
	guid->d = (uint16_t)strtoul(fourbytes, NULL, 16);
	guid->d = cpu_to_be16(guid->d);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *			 ^ */
	memcpy(twobytes, text+24, 2);
	if (check_segment_sanity(twobytes, 2) < 0)
		return -1;
	guid->e[0] = (uint8_t)strtoul(twobytes, NULL, 16);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *			   ^ */
	memcpy(twobytes, text+26, 2);
	if (check_segment_sanity(twobytes, 2) < 0)
		return -1;
	guid->e[1] = (uint8_t)strtoul(twobytes, NULL, 16);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *			     ^ */
	memcpy(twobytes, text+28, 2);
	if (check_segment_sanity(twobytes, 2) < 0)
		return -1;
	guid->e[2] = (uint8_t)strtoul(twobytes, NULL, 16);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *			       ^ */
	memcpy(twobytes, text+30, 2);
	if (check_segment_sanity(twobytes, 2) < 0)
		return -1;
	guid->e[3] = (uint8_t)strtoul(twobytes, NULL, 16);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *				 ^ */
	memcpy(twobytes, text+32, 2);
	if (check_segment_sanity(twobytes, 2) < 0)
		return -1;
	guid->e[4] = (uint8_t)strtoul(twobytes, NULL, 16);

	/* 84be9c3e-8a32-42c0-891c-4cd3b072becc
	 *				   ^ */
	memcpy(twobytes, text+34, 2);
	if (check_segment_sanity(twobytes, 2) < 0)
		return -1;
	guid->e[5] = (uint8_t)strtoul(twobytes, NULL, 16);

	return 0;
}

struct guidname {
	efi_guid_t guid;
	char symbol[256];
	char name[256];
};

#endif /* LIBEFIVAR_GUID */

// vim:fenc=utf-8:tw=75:noet
