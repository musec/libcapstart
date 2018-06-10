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

#include <stdbool.h>

#include "libcapstart.h"

struct cs_sandbox
{
	int	binary;
	int	linker;
	int	*libdirs;
	int	libdir_count;

	/* Human-readable descriptions of errors (NULL-terminated) */
	char **errors;
	size_t	errcap;
	size_t	errlen;
};

/**
 * Append an error message to a sandbox's list of errors.
 */
bool	error_append(struct cs_sandbox *, const char *format, ...)
		__printflike(2, 3);

#ifndef NDEBUG
void	print_all_errors(struct cs_sandbox *);
#endif

/*
 * Set up library paths and the run-time linker for a sandbox. There
 * are a few ways of doing this, depending on what sort of binary we're
 * going to run (native, 32b compat or Linux compat).
 */
bool	setup_compat32_libraries(struct cs_sandbox *);
bool	setup_linux_compat_libraries(struct cs_sandbox *);
bool	setup_native_libraries(struct cs_sandbox *);
