CONTIKI_PROJECT = sky-webserver
all: sky-webserver-1 sky-webserver-2 sky-webserver-3

UIP_CONF_IPV6=1
#CFLAGS+= -DUIP_CONF_IPV6_RPL

SMALL=1

DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE=16
# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE=16
# DEFINES=NETSTACK_CONF_RDC=cxmac_driver,NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE=8
# DEFINES=NETSTACK_CONF_RDC=nullrdc_driver

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PLATFORM_BUILD=1 # This is needed to avoid the shell to include the httpd-cfs version of the webserver

# WITH_WEBSERVER=1
WITH_WEBSERVER=webserver
ifeq ($(WITH_WEBSERVER),1)
CFLAGS += -DWEBSERVER=1
PROJECT_SOURCEFILES += httpd-simple.c
else ifneq ($(WITH_WEBSERVER), 0)
APPS += $(WITH_WEBSERVER)
CFLAGS += -DWEBSERVER=2
endif

# The webserver application normally contains a built-in file system and support
# for server-side includes.
#
# This webserver example supports building the alternative webserver application
# which serves files from an cfs file system. To build the alternative webserver
# run make with the parameter HTTPD-CFS=1.

# ifeq ($(HTTPD-CFS),1)
# override webserver_src = webserver-nogui.c http-strings.c psock.c memb.c \
#                          httpd-cfs.c
# endif

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include


$(CONTIKI)/tools/tunslip6:	$(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 aaaa::1/64


#sky-webserver.$(TARGET): $(OBJECTDIR)/ajax-cgi.o

# Intentionally httpd.c and httpd-cfs.c implement the same interface. When
# switching from one webserver alternative to the other with an existent
# Contiki library then both files end up in the library making the linker
# use whichever it finds first :-(
#
# The most straightforward way to make sure this doesn't happen is to delete
# the Contiki library. But it would be undesirable to do that on every build
# so the existence of the "wrong" object file is used to detect a switch and
# trigger deletion of the Contiki library - and the trigger file of course.

# ifeq ($(HTTPD-CFS),1)
#   ifneq (${wildcard $(OBJECTDIR)/httpd.o},)
#   DUMMY := ${shell rm -f contiki-$(TARGET).a $(OBJECTDIR)/httpd.o}
#   endif
# else
#   ifneq (${wildcard $(OBJECTDIR)/httpd-cfs.o},)
#   DUMMY := ${shell rm -f contiki-$(TARGET).a $(OBJECTDIR)/httpd-cfs.o}
#   endif
# endif

