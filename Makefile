# Delete the builtin make rules, as if "make -r" was used.
.SUFFIXES:

# Ask make to not delete "intermediate" results, such as the .o in the chain
# .cc -> .o -> .so. Otherwise, during the first build, make considers the .o
# to be intermediate, and deletes it, but the newly-created ".d" files lists
# the ".o" as a target - so it needs to be created again on the second make.
# See commit fac05c95 for a longer explanation.
.SECONDARY:

# Deleting partially-build targets on error should be the default, but it
# isn't, for historical reasons, so we need to turn it on explicitly...
.DELETE_ON_ERROR:

OUT=out
STRIP = strip
quiet = $(if $V, $1, @echo " $2"; $1)
very-quiet = $(if $V, $1, @$1)

include acpi.mk
acpi = $(patsubst %.c, %.o, $(acpi-source))
acpi-defines = -DACPI_MACHINE_WIDTH=64 -DACPI_USE_LOCAL_CACHE
acpi-objects = $(acpi:%=$(OUT)/%)
$(acpi-objects): CFLAGS += -I acpica/source/include -fno-strict-aliasing -Wno-strict-aliasing

objects += $(acpi-objects)

_objects += print.o console.o arch-setup.o printf.o entry.o exceptions.o arch-cpu.o memory.o cpuid.o \
	   xen.o entry-xen.o pci.o clock.o runtime.o cruntime.o acpi.o __ctype_b_loc.o smp.o apic.o \
	   pagetable.o logger.o main.o

objects += $(_objects:%=$(OUT)/%)

all: $(OUT)/loader.img $(OUT)/loader.bin $(OUT)/boot16.elf

clean:
	rm -rf $(OUT)

block-size = $(shell expr 512 \* 64)
image-size = $(shell stat --printf %s $(OUT)/lzloader.elf)
image-block-size = $(shell expr \( $(image-size) - 1 \) / $(block-size) + 1)

$(OUT)/acpi.o $(OUT)/smp.o: CXXFLAGS += -I ./acpica/source/include
$(OUT)/acpi.o: $(acpi-objects)

$(OUT)/boot16.elf: boot16.ld $(OUT)/boot16.o
	$(call quiet, $(LD) $(OUT)/boot16.o boot16.ld -o $@, LD $@)

$(OUT)/boot16.bin: boot16.ld $(OUT)/boot16.o
	$(call quiet, $(LD) -o $@ -T $^, LD $@)

$(OUT)/loader-stripped.elf: $(OUT)/loader.elf
	$(call very-quiet, cp $(OUT)/loader.elf $(OUT)/loader-stripped.elf)
	$(call quiet, $(STRIP) $(OUT)/loader-stripped.elf, STRIP loader.elf)

$(OUT)/loader.img: $(OUT)/boot16.bin $(OUT)/lzloader.elf
	$(call quiet, dd if=$(OUT)/boot16.bin of=$@ > /dev/null 2>&1, DD $@ boot16.bin)
	$(call quiet, cat $(OUT)/lzloader.elf /dev/zero|dd count=$(image-block-size) ibs=$(block-size) of=$@ conv=notrunc seek=128 > /dev/null 2>&1, \
		DD $@ lzloader.elf)
	$(call quiet, python setsize.py $@ $(OUT)/lzloader.elf, DD $@ boot16.bin)

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
INCLUDES = $(local-includes) -I. -Iinclude 
INCLUDES += -isystem include/glibc-compat
INCLUDES += -isystem include/api
INCLUDES += -isystem include/api/x64
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

CXXFLAGS = -fno-rtti -fno-exceptions -std=gnu++11 $(COMMON)
CFLAGS += -std=gnu99 $(COMMON)

-include $(OUT)/loader.d $(objects:.o=.d)
