/*-
 * Copyright (c) 2018 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed at Memorial University under the
 * NSERC Discovery (RGPIN-2015-06048) and RDC Ignite (#5404.1822.101) programs.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <malloc_np.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "internal.h"


struct cs_sandbox*
capstart_create()
{
	struct cs_sandbox *csp;

	csp = calloc(1, sizeof(*csp));
	csp->binary = -1;
	csp->linker = -1;

	csp->errcap = 4;
	csp->errors = calloc(csp->errcap, sizeof(csp->errors[0]));

	return (csp);
}

const char * const *
capstart_errors(struct cs_sandbox *csp, size_t *countp)
{
	if ((csp == NULL) || (countp == NULL)) {
		errno = EINVAL;
		return (NULL);
	}

	*countp = csp->errlen;
	return (const char * const *) csp->errors;
}

const char *
capstart_last_error(struct cs_sandbox *csp)
{
	if (csp == NULL) {
		errno = EINVAL;
		return (NULL);
	}

	if (csp->errlen == 0) {
		return (0);
	}

	return (csp->errors[csp->errlen - 1]);
}

void
capstart_release(struct cs_sandbox *csp)
{
	int i;

	close(csp->binary);
	close(csp->linker);

	for (i = 0; i < csp->libdir_count; i++) {
		close(csp->libdirs[i]);
	}

	free(csp->libdirs);
	free(csp->errors);
	free(csp);
}

bool
error_append(struct cs_sandbox *csp, const char *format, ...)
{
	va_list args;
	char **newerrs;
	size_t newcap;

	if (csp == NULL) {
		return (false);
	}

	// Enlarge error buffer if necessary
	if (csp->errlen == csp->errcap) {
		newcap = 2 * csp->errcap;
		newerrs = rallocx(csp->errors, newcap * sizeof(csp->errors[0]),
		                  MALLOCX_ZERO);

		if (newerrs == NULL) {
			return (false);
		}

		csp->errors = newerrs;
		csp->errcap = newcap;
	}

	va_start(args, format);

	char **error_slot = csp->errors + csp->errlen;
	vasprintf(error_slot, format, args);
	if (*error_slot == NULL) {
		return (false);
	}

	csp->errlen += 1;

	va_end(args);

	return (true);
}

void
print_all_errors(struct cs_sandbox *csp)
{
	const char * const *errors;
	size_t len;

	errors = capstart_errors(csp, &len);
	if (errors == NULL) {
		return;
	}

	fprintf(stderr, "%ld errors\n", len);

	for (size_t i = 0; i < len; i++) {
		fprintf(stderr, " - %s\n", errors[i]);
	}
}
