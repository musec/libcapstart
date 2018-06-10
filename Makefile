LIB=	capstart
SHLIB_MAJOR=	0

CFLAGS=-O0 -g

SRCS=	\
	binary.c \
	core.c \
	libraries.c \

INCS=	libcapstart.h

LDADD+=	-l elf

SUBDIR=	tests

test: all
.for dir in ${SUBDIR}
	${MAKE} -C ${.CURDIR}/${dir} test
.endfor

.include <bsd.lib.mk>
