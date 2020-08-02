MODULE = cpu

DIRS = $(RIOTCPU)/cortexm_common periph stmclk vectors

ifneq (,$(filter bootloader_stm32,$(USEMODULE)))
  ifeq ($(CPU_FAM), mp1)
    $(error STM32MP1 family has no flash and thus no bootloader can be used)
  endif
  DIRS += bootloader
endif

include $(RIOTBASE)/Makefile.base
