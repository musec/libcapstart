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
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"

// Extract ELF header if the given ELF header is valid
static bool	get_exec_header(Elf *, GElf_Ehdr *);

// Is this binary for a 32-bit version of the current architecture?
static bool	is_compat32(GElf_Ehdr *);

// Is this binary for the currently-running architecture?
static bool	is_native(GElf_Ehdr *);

// What is the (user-readable) name of the currently-running architecture?
static const char*	native_arch_name(void);


int
capstart_set_target(struct cs_sandbox *csp, int binary)
{
	GElf_Ehdr header;
	Elf *elf;
	const char *linker_name;
	int arch;

	csp->binary = dup(binary);

	// Determine the ABI of the binary:
	if (elf_version(EV_CURRENT) == EV_NONE) {
		error_append(csp, "ELF library version mismatch: %s",
			elf_errmsg(elf_errno()));

		errno = EPROGMISMATCH;
		return (-1);
	}

	elf = elf_begin(csp->binary, ELF_C_READ, NULL);
	if (!get_exec_header(elf, &header)) {
		error_append(csp, "Failed to read ELF file in FD %d: %s",
			csp->binary, elf_errmsg(elf_errno()));

		elf_end(elf);
		errno = EFTYPE;
		return (-1);
	}

	arch = header.e_machine;
	linker_name = NULL;

	switch (header.e_ident[EI_OSABI]) {
	case ELFOSABI_LINUX:
		linker_name = "/compat/linux/usr/lib/ld-linux.so.2";
		break;

	case ELFOSABI_FREEBSD:
		if (is_native(&header)) {
			linker_name = "/libexec/ld-elf.so.1";
		} else if (is_compat32(&header)) {
			linker_name = "/libexec/ld-elf32.so.1";
		} else {
			error_append(csp, "FreeBSD binary is neither the same"
				" as nor a 32b-compat relative of"
				" the currently-running architecture (%s)",
				native_arch_name());
		}
		break;

	default:
		error_append(csp, "Unsupported OS ABI: %d",
			header.e_ident[EI_OSABI]);
	}

	elf_end(elf);

	if (linker_name == NULL) {
		errno = EOPNOTSUPP;
		return (-1);
	}

	csp->linker = open(linker_name, O_RDONLY);
	if (csp->linker < 0) {
		error_append(csp, "Failed to open run-time linker '%s': %s",
			linker_name, strerror(errno));

		errno = ENOEXEC;
		return (-1);
	}

	return (0);
}

static bool
get_exec_header(Elf *elf, GElf_Ehdr *header)
{

	if ((elf == NULL) || (elf_kind(elf) != ELF_K_ELF)) {
		return (false);
	}

	if (gelf_getehdr(elf, header) == NULL) {
		return (false);
	}

	return (header->e_type == ET_EXEC);
}

static bool
is_compat32(GElf_Ehdr *header)
{
#if defined(__amd64__)
	return (header->e_machine == EM_386);
#else
	// TODO: other architectures (e.g., MIPS32)
	return (false);
#endif

}

static bool
is_native(GElf_Ehdr *header)
{
#if defined(__amd64__)
	return (header->e_machine == EM_X86_64);
#elif defined(__arm__)
	return (header->e_machine == EM_ARM);
#elif defined(__aarch64__)
	return (header->e_machine == EM_AARCH64);
#elif defined(__i386__)
	return (header->e_machine == EM_386);
#elif defined(__mips_o32)
	// TODO: ???
	return (((header->e_machine == EM_MIPS_X)
		&& (header->e_machine == EM_MIPS_RS3_LE)
		&& (header->e_machine == EM_MIPS_RS4_BE))
		&& (header->e_flags & EF_MIPS_ABI_O32));
#elif defined(__mips_o64)
	return (((header->e_machine == EM_MIPS_X)
		&& (header->e_machine == EM_MIPS_RS3_LE)
		&& (header->e_machine == EM_MIPS_RS4_BE))
		&& (header->e_flags & EF_MIPS_ABI_O64));
#elif defined(__powerpc64__)
	return (header->e_machine == EM_PPC64);
#elif defined(__powerpc__)
	return (header->e_machine == EM_PPC);
#elif defined(__riscv)
	return (header->e_machine == EM_RISCV);
#elif defined(__sparc64__)
	return (header->e_machine == EM_SPARCV9);
#else
	return (false);
#endif
}

const char *
native_arch_name()
{
	// TODO: is this already encoded somewhere else?
#if defined(__amd64__)
	return ("amd64");
#elif defined(__arm__)
	return ("arm");
#elif defined(__aarch64__)
	return ("AArch64");
#elif defined(__i386__)
	return ("i386");
#elif defined(__mips_o32)
	return ("mips");
#elif defined(__powerpc64__)
	return ("powerpc64");
#elif defined(__powerpc__)
	return ("powerpc");
#elif defined(__riscv)
	return ("risc-v");
#elif defined(__sparc64__)
	return ("sparc64");
#else
	return ("unknown architecture")
#endif
}
