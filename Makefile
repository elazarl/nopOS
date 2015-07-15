
OUT=out
STRIP = strip
quiet = $(if $V, $1, @echo " $2"; $1)
very-quiet = $(if $V, $1, @$1)

objects += $(OUT)/console.o $(OUT)/runtime.o $(OUT)/libc.o

all: $(OUT)/loader.img $(OUT)/loader.bin

clean:
	rm -rf $(OUT)

image-size = $(shell stat --printf %s $(OUT)/lzloader.elf)

$(OUT)/boot.bin: boot16.ld $(OUT)/boot16.o
	$(call quiet, $(LD) -o $@ -T $^, LD $@)

$(OUT)/loader-stripped.elf: $(OUT)/loader.elf
	$(call very-quiet, cp $(OUT)/loader.elf $(OUT)/loader-stripped.elf)
	$(call quiet, $(STRIP) $(OUT)/loader-stripped.elf, STRIP loader.elf)

$(OUT)/loader.img: $(OUT)/boot.bin $(OUT)/lzloader.elf
	$(call quiet, dd if=$(OUT)/boot.bin of=$@ > /dev/null 2>&1, DD $@ boot.bin)
	$(call quiet, cat $(OUT)/lzloader.elf /dev/zero|dd count=`python -c 'print max(192, ($(image-size)-1)/512+1)'` of=$@ conv=notrunc seek=128 > /dev/null 2>&1, \
		DD $@ lzloader.elf)

$(OUT)/loader.bin: $(OUT)/boot32.o loader32.ld
	$(call quiet, $(LD) -nostartfiles -static -nodefaultlibs -o $@ \
	                $(filter-out %.bin, $(^:%.ld=-T %.ld)), LD $@)

$(OUT)/fastlz.o: fastlz/fastlz.cc
	$(makedir)
	$(call quiet, $(CXX) $(CXXFLAGS) -O2 -m32 -o $@ -c fastlz/fastlz.cc, CXX $@)

$(OUT)/lz: fastlz/fastlz.cc fastlz/lz.cc
	$(makedir)
	$(call quiet, $(CXX) $(CXXFLASG) -O2 -o $@ $(filter %.cc, $^), CXX $@)

$(OUT)/loader-stripped.elf.lz.o: $(OUT)/loader-stripped.elf $(OUT)/lz
	$(call quiet, $(OUT)/lz $(OUT)/loader-stripped.elf, LZ $@)
	$(call quiet, cd $(OUT)&&objcopy -B i386 -I binary -O elf32-i386 loader-stripped.elf.lz loader-stripped.elf.lz.o, OBJCOPY $@)

$(OUT)/lzloader.o: fastlz/lzloader.cc
	$(call quiet, $(CXX) $(CXXFLAGS) -O2 -m32 -o $@ -c $^, CXX $@)

$(OUT)/lzloader.elf: $(OUT)/loader-stripped.elf.lz.o $(OUT)/lzloader.o lzloader.ld \
	$(OUT)/fastlz.o
	$(call quiet, scripts/check-image-size.sh $(OUT)/loader-stripped.elf 23068672)
	$(call quiet, $(LD) -o $@ \
		-Bdynamic --export-dynamic --eh-frame-hdr --enable-new-dtags \
	-T lzloader.ld \
	$(filter %.o, $^), LD $@)

$(OUT)/loader.elf: $(OUT)/boot.o loader.ld $(OUT)/loader.o $(drivers) \
	$(objects)
	$(call quiet, $(LD) -o $@ \
		-Bdynamic --export-dynamic --eh-frame-hdr --enable-new-dtags \
	    $(filter-out %.bin, $(^:%.ld=-T %.ld)) \
	    --whole-archive \
	      $(libstdc++.a) $(libgcc.a) $(libgcc_eh.a) \
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

$(OUT)/%.o: %.cc
	$(makedir)
	$(q-build-cxx)
	$(q-adjust-deps)

$(OUT)/%.o: %.c
	$(makedir)
	$(q-build-c)
	$(q-adjust-deps)

$(OUT)/%.o: %.S
	$(makedir)
	$(q-build-s)

$(OUT)/%.o: %.s
	$(makedir)
	$(q-build-s)

ASFLAGS += -I$(OUT)
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

# libs
libgcc.a := $(shell $(CC) -print-libgcc-file-name)
ifeq ($(filter /%,$(libgcc.a)),)
	$(error Error: libgcc.a needs to be installed.)
endif

libgcc_eh.a := $(shell $(CC) -print-file-name=libgcc_eh.a)
ifeq ($(filter /%,$(libgcc_eh.a)),)
	$(error Error: libgcc_eh.a needs to be installed.)
endif

libstdc++.a := $(shell $(CXX) -print-file-name=libstdc++.a)
ifeq ($(filter /%,$(libstdc++.a)),)
	$(error Error: libstdc++.a needs to be installed.)
endif

libsupc++.a := $(shell $(CXX) -print-file-name=libsupc++.a)
ifeq ($(filter /%,$(libsupc++.a)),)
	$(error Error: libsupc++.a needs to be installed.)
endif

# link with -mt if present, else the base version (and hope it is multithreaded)
boost-mt := -mt
boost-lib-dir := $(dir $(shell $(CC) --print-file-name libboost_system$(boost-mt).a))
ifeq ($(filter /%,$(boost-lib-dir)),)
	boost-mt :=
	boost-lib-dir := $(dir $(shell $(CC) --print-file-name libboost_system$(boost-mt).a))
	ifeq ($(filter /%,$(boost-lib-dir)),)
		$(error Error: libboost_system.a needs to be installed.)
	endif
endif
# When boost_env=host, we won't use "-nostdinc", so the build machine's
# header files will be used normally. So we don't need to add anything
# special for Boost.
boost-includes =

boost-libs := $(boost-lib-dir)/libboost_program_options$(boost-mt).a \
              $(boost-lib-dir)/libboost_system$(boost-mt).a

