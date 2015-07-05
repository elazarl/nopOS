
STRIP = strip
quiet = $(if $V, $1, @echo " $2"; $1)
very-quiet = $(if $V, $1, @$1)

#objects += isa-serial.o console-driver.o

all: loader.img loader.bin

#usr.img

image-size = $(shell stat --printf %s lzloader.elf)

boot.bin: boot16.ld boot16.o
	$(call quiet, $(LD) -o $@ -T $^, LD $@)

loader-stripped.elf: loader.elf
	$(call very-quiet, cp loader.elf loader-stripped.elf)
	$(call quiet, $(STRIP) loader-stripped.elf, STRIP loader.elf)

loader.img: boot.bin lzloader.elf
	$(call quiet, dd if=boot.bin of=$@ > /dev/null 2>&1, DD $@ boot.bin)
	$(call quiet, cat lzloader.elf /dev/zero|dd count=`python -c 'print max(192, ($(image-size)-1)/512+1)'` of=$@ conv=notrunc seek=128 > /dev/null 2>&1, \
		DD $@ lzloader.elf)
	$(call quiet, ./scripts/imgedit.py setsize $@ $(image-size), IMGEDIT $@)
	$(call quiet, ./scripts/imgedit.py setargs $@ $(cmdline), IMGEDIT $@)

loader.bin: boot32.o loader32.ld
	$(call quiet, $(LD) -nostartfiles -static -nodefaultlibs -o $@ \
	                $(filter-out %.bin, $(^:%.ld=-T %.ld)), LD $@)

fastlz/fastlz.o:
	$(makedir)
	$(call quiet, $(CXX) $(CXXFLAGS) -O2 -m32 -o $@ -c ./fastlz/fastlz.cc, CXX $@)

fastlz/lz: fastlz/fastlz.cc fastlz/lz.cc
	$(makedir)
	$(call quiet, $(CXX) $(CXXFLASG) -O2 -o $@ $(filter %.cc, $^), CXX $@)

loader-stripped.elf.lz.o: loader-stripped.elf fastlz/lz
	$(call quiet, fastlz/lz loader-stripped.elf, LZ $@)
	$(call quiet, objcopy -B i386 -I binary -O elf32-i386 loader-stripped.elf.lz $@, OBJCOPY $@)

fastlz/lzloader.o: fastlz/lzloader.cc
	$(call quiet, $(CXX) $(CXXFLAGS) -O2 -m32 -o $@ -c fastlz/lzloader.cc, CXX $@)

lzloader.elf: loader-stripped.elf.lz.o fastlz/lzloader.o lzloader.ld \
	fastlz/fastlz.o
	$(call quiet, scripts/check-image-size.sh loader-stripped.elf 23068672)
	$(call quiet, $(LD) -o $@ \
		-Bdynamic --export-dynamic --eh-frame-hdr --enable-new-dtags \
	-T lzloader.ld \
	$(filter %.o, $^), LD $@)

loader.elf: boot.o loader.ld loader.o $(drivers) \
	$(objects)
	$(call quiet, $(LD) -o $@ \
		-Bdynamic --export-dynamic --eh-frame-hdr --enable-new-dtags \
	    $(filter-out %.bin, $(^:%.ld=-T %.ld)) \
	    --whole-archive \
	      $(libstdc++.a) $(libgcc_s.a) $(libgcc_eh.a) \
	      $(boost-libs) \
	    --no-whole-archive, \
		LD $@)

# rules

autodepend = -MD -MT $@ -MP

makedir = $(call very-quiet, mkdir -p $(dir $@))
build-cxx = $(CXX) $(CXXFLAGS) -c -o $@ $<
q-build-cxx = $(call quiet, $(build-cxx), CXX $@)
build-c = $(CC) $(CFLAGS) -c -o $@ $<
q-build-c = $(call quiet, $(build-c), CC $@)
build-s = $(CXX) $(CXXFLAGS) $(ASFLAGS) -c -o $@ $<
q-build-s = $(call quiet, $(build-s), AS $@)
build-so = $(CC) $(CFLAGS) -o $@ $^
q-build-so = $(call quiet, $(build-so), CC $@)
adjust-deps = sed -i 's! $(subst .,\.,$<)\b! !g' $(@:.o=.d)
q-adjust-deps = $(call very-quiet, $(adjust-deps))

%.o: %.cc
	$(makedir)
	$(q-build-cxx)
	$(q-adjust-deps)

%.o: %.c
	$(makedir)
	$(q-build-c)
	$(q-adjust-deps)

%.o: %.S
	$(makedir)
	$(q-build-s)

%.o: %.s
	$(makedir)
	$(q-build-s)

INCLUDES = $(local-includes) -Iarch/$(arch) -I. -Iinclude  -Iarch/common
INCLUDES += -isystem include/glibc-compat
EXTRA_FLAGS = -D__OSV_CORE__ -DOSV_KERNEL_BASE=$(kernel_base)
EXTRA_LIBS =
COMMON = $(autodepend) -g -Wall -Wno-pointer-arith $(CFLAGS_WERROR) -Wformat=0 -Wno-format-security \
	-D __BSD_VISIBLE=1 -U _FORTIFY_SOURCE -fno-stack-protector $(INCLUDES) \
	$(kernel-defines) \
	-fno-omit-frame-pointer $(compiler-specific) \
	-include compiler/include/intrinsics.hh \
	$(do-sys-includes) \
	$(arch-cflags) $(conf-opt) $(acpi-defines) $(tracing-flags) $(gcc-sysroot) \
	$(configuration) -D__OSV__ -D__XEN_INTERFACE_VERSION__="0x00030207" -DARCH_STRING=$(ARCH_STR) $(EXTRA_FLAGS)

CXXFLAGS = -std=gnu++11 $(COMMON)
CFLAGS = -std=gnu99 $(COMMON)
