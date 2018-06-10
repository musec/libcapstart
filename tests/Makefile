PACKAGE=	tests

ATF_TESTS_C=	\
		binary \
		core \

CFLAGS=		-I ${.CURDIR}/..
LDFLAGS=	../libcapstart.so.0.full    # manually link against debug lib

.include <bsd.test.mk>

test: all
	@env ${KYUA} test -k ${.OBJDIR}/Kyuafile
