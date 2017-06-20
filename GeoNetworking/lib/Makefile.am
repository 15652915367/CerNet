INCLUDES = -I$(top_srcdir)/include
METASOURCES = AUTO
lib_LIBRARIES = libnetlink.a libutil.a
libnetlink_a_SOURCES = libnetlink.c ll_map.c
libutil_a_SOURCES = dnet_ntop.c dnet_pton.c inet_proto.c ipx_ntop.c ipx_pton.c \
	ll_addr.c ll_proto.c ll_types.c rt_names.c utils.c
noinst_HEADERS = $(top_builddir)/include/SNAPSHOT.h $(top_builddir)/include/ip6tables.h $(top_builddir)/include/iptables.h $(top_builddir)/include/iptables_common.h \
	$(top_builddir)/include/libnetlink.h $(top_builddir)/include/ll_map.h $(top_builddir)/include/rt_names.h $(top_builddir)/include/rtm_map.h $(top_builddir)/include/utils.h
