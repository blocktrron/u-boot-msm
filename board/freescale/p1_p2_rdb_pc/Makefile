#
# Copyright 2010-2011 Freescale Semiconductor, Inc.
#
# SPDX-License-Identifier:	GPL-2.0+
#

MINIMAL=

ifdef CONFIG_SPL_BUILD
ifdef CONFIG_SPL_INIT_MINIMAL
MINIMAL=y
endif
endif

ifdef MINIMAL

obj-y	+= spl_minimal.o tlb.o law.o

else
ifdef CONFIG_SPL_BUILD
obj-y += spl.o
endif

obj-y        += p1_p2_rdb_pc.o
obj-y        += ddr.o
obj-y        += law.o
obj-y        += tlb.o

ifndef CONFIG_SPL_BUILD
obj-y        += msm460.o
endif

endif
