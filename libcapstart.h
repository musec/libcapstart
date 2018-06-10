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

#if !defined(LIBCAPSTART_H_)
#define LIBCAPSTART_H_

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdlib.h>

__BEGIN_DECLS

/* see libcapstart(3) for documentation */

struct cs_sandbox;

/*
 * Lifecycle bootstrapping
 */
struct cs_sandbox*	capstart_create(void);
const char * const *	capstart_errors(struct cs_sandbox *, size_t *);
const char *		capstart_last_error(struct cs_sandbox *);
void			capstart_release(struct cs_sandbox *);

/*
 * Sandbox setup
 */
bool	capstart_add_file(struct cs_sandbox *, const char *path,
		int csflags, cap_rights_t *rights);
bool	capstart_add_path(struct cs_sandbox *, const char *path,
		int flags, cap_rights_t *rights);
bool	capstart_copy_env(struct cs_sandbox *, const char *whitelist,
		int flags);
bool	capstart_set_env(struct cs_sandbox *, const char *varname,
		const char *value);
int	capstart_set_target(struct cs_sandbox *, int binary);

/*
 * Sandbox execution
 */
int	capstart_exec(struct cs_sandbox *, char *const argv[]);

__END_DECLS

#endif /* !defined(LIBCAPSTART_H_) */
