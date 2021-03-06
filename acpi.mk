acpi-source := \
acpica/source/components/debugger/dbcmds.c \
acpica/source/components/debugger/dbexec.c \
acpica/source/components/debugger/dbinput.c \
acpica/source/components/debugger/dbmethod.c \
acpica/source/components/debugger/dbdisply.c \
acpica/source/components/debugger/dbconvert.c \
acpica/source/components/debugger/dbxface.c \
acpica/source/components/debugger/dbfileio.c \
acpica/source/components/debugger/dbhistry.c \
acpica/source/components/debugger/dbutils.c \
acpica/source/components/debugger/dbobject.c \
acpica/source/components/debugger/dbstats.c \
acpica/source/components/debugger/dbnames.c \
acpica/source/components/debugger/dbtest.c \
acpica/source/components/events/evxfregn.c \
acpica/source/components/events/evxface.c \
acpica/source/components/events/evgpeinit.c \
acpica/source/components/events/evhandler.c \
acpica/source/components/events/evgpeutil.c \
acpica/source/components/events/evglock.c \
acpica/source/components/events/evxfgpe.c \
acpica/source/components/events/evmisc.c \
acpica/source/components/events/evgpe.c \
acpica/source/components/events/evgpeblk.c \
acpica/source/components/events/evevent.c \
acpica/source/components/events/evregion.c \
acpica/source/components/events/evxfevnt.c \
acpica/source/components/events/evsci.c \
acpica/source/components/events/evrgnini.c \
acpica/source/components/utilities/utxfinit.c \
acpica/source/components/utilities/utstring.c \
acpica/source/components/utilities/utobject.c \
acpica/source/components/utilities/utmath.c \
acpica/source/components/utilities/utclib.c \
acpica/source/components/utilities/utprint.c \
acpica/source/components/utilities/utstate.c \
acpica/source/components/utilities/utxfmutex.c \
acpica/source/components/utilities/utbuffer.c \
acpica/source/components/utilities/uteval.c \
acpica/source/components/utilities/utxferror.c \
acpica/source/components/utilities/utaddress.c \
acpica/source/components/utilities/utownerid.c \
acpica/source/components/utilities/utxface.c \
acpica/source/components/utilities/utdecode.c \
acpica/source/components/utilities/utosi.c \
acpica/source/components/utilities/utdelete.c \
acpica/source/components/utilities/utglobal.c \
acpica/source/components/utilities/utcache.c \
acpica/source/components/utilities/utmisc.c \
acpica/source/components/utilities/utlock.c \
acpica/source/components/utilities/uterror.c \
acpica/source/components/utilities/utfileio.c \
acpica/source/components/utilities/utcopy.c \
acpica/source/components/utilities/utuuid.c \
acpica/source/components/utilities/utexcep.c \
acpica/source/components/utilities/utinit.c \
acpica/source/components/utilities/utalloc.c \
acpica/source/components/utilities/utnonansi.c \
acpica/source/components/utilities/uttrack.c \
acpica/source/components/utilities/utmutex.c \
acpica/source/components/utilities/utpredef.c \
acpica/source/components/utilities/utids.c \
acpica/source/components/utilities/uthex.c \
acpica/source/components/utilities/utdebug.c \
acpica/source/components/utilities/utresrc.c \
acpica/source/components/hardware/hwxfsleep.c \
acpica/source/components/hardware/hwxface.c \
acpica/source/components/hardware/hwregs.c \
acpica/source/components/hardware/hwesleep.c \
acpica/source/components/hardware/hwvalid.c \
acpica/source/components/hardware/hwtimer.c \
acpica/source/components/hardware/hwacpi.c \
acpica/source/components/hardware/hwpci.c \
acpica/source/components/hardware/hwsleep.c \
acpica/source/components/hardware/hwgpe.c \
acpica/source/components/namespace/nsaccess.c \
acpica/source/components/namespace/nsdump.c \
acpica/source/components/namespace/nsarguments.c \
acpica/source/components/namespace/nswalk.c \
acpica/source/components/namespace/nsprepkg.c \
acpica/source/components/namespace/nsnames.c \
acpica/source/components/namespace/nsxfname.c \
acpica/source/components/namespace/nsload.c \
acpica/source/components/namespace/nseval.c \
acpica/source/components/namespace/nssearch.c \
acpica/source/components/namespace/nsobject.c \
acpica/source/components/namespace/nsdumpdv.c \
acpica/source/components/namespace/nsinit.c \
acpica/source/components/namespace/nsrepair.c \
acpica/source/components/namespace/nsxfobj.c \
acpica/source/components/namespace/nspredef.c \
acpica/source/components/namespace/nsalloc.c \
acpica/source/components/namespace/nsconvert.c \
acpica/source/components/namespace/nsparse.c \
acpica/source/components/namespace/nsxfeval.c \
acpica/source/components/namespace/nsutils.c \
acpica/source/components/namespace/nsrepair2.c \
acpica/source/components/parser/psopcode.c \
acpica/source/components/parser/psobject.c \
acpica/source/components/parser/pswalk.c \
acpica/source/components/parser/psopinfo.c \
acpica/source/components/parser/psutils.c \
acpica/source/components/parser/psloop.c \
acpica/source/components/parser/psparse.c \
acpica/source/components/parser/psargs.c \
acpica/source/components/parser/psxface.c \
acpica/source/components/parser/pstree.c \
acpica/source/components/parser/psscope.c \
acpica/source/components/dispatcher/dswstate.c \
acpica/source/components/dispatcher/dswload.c \
acpica/source/components/dispatcher/dswexec.c \
acpica/source/components/dispatcher/dsutils.c \
acpica/source/components/dispatcher/dsfield.c \
acpica/source/components/dispatcher/dsdebug.c \
acpica/source/components/dispatcher/dsargs.c \
acpica/source/components/dispatcher/dscontrol.c \
acpica/source/components/dispatcher/dsmethod.c \
acpica/source/components/dispatcher/dsinit.c \
acpica/source/components/dispatcher/dswload2.c \
acpica/source/components/dispatcher/dsmthdat.c \
acpica/source/components/dispatcher/dsobject.c \
acpica/source/components/dispatcher/dswscope.c \
acpica/source/components/dispatcher/dsopcode.c \
acpica/source/components/tables/tbdata.c \
acpica/source/components/tables/tbxfload.c \
acpica/source/components/tables/tbxfroot.c \
acpica/source/components/tables/tbfind.c \
acpica/source/components/tables/tbfadt.c \
acpica/source/components/tables/tbinstal.c \
acpica/source/components/tables/tbutils.c \
acpica/source/components/tables/tbxface.c \
acpica/source/components/tables/tbprint.c \
acpica/source/components/disassembler/dmutils.c \
acpica/source/components/disassembler/dmcstyle.c \
acpica/source/components/disassembler/dmbuffer.c \
acpica/source/components/disassembler/dmresrcs.c \
acpica/source/components/disassembler/dmopcode.c \
acpica/source/components/disassembler/dmresrcl2.c \
acpica/source/components/disassembler/dmdeferred.c \
acpica/source/components/disassembler/dmnames.c \
acpica/source/components/disassembler/dmresrc.c \
acpica/source/components/disassembler/dmwalk.c \
acpica/source/components/disassembler/dmresrcl.c \
acpica/source/components/executer/exsystem.c \
acpica/source/components/executer/exresnte.c \
acpica/source/components/executer/exprep.c \
acpica/source/components/executer/exoparg6.c \
acpica/source/components/executer/exdebug.c \
acpica/source/components/executer/exmisc.c \
acpica/source/components/executer/exutils.c \
acpica/source/components/executer/exstoren.c \
acpica/source/components/executer/exoparg1.c \
acpica/source/components/executer/exstore.c \
acpica/source/components/executer/exnames.c \
acpica/source/components/executer/exfldio.c \
acpica/source/components/executer/exoparg2.c \
acpica/source/components/executer/exconfig.c \
acpica/source/components/executer/exoparg3.c \
acpica/source/components/executer/exfield.c \
acpica/source/components/executer/exstorob.c \
acpica/source/components/executer/exconvrt.c \
acpica/source/components/executer/exregion.c \
acpica/source/components/executer/exmutex.c \
acpica/source/components/executer/exresop.c \
acpica/source/components/executer/exdump.c \
acpica/source/components/executer/excreate.c \
acpica/source/components/executer/exresolv.c \
acpica/source/components/resources/rscalc.c \
acpica/source/components/resources/rsdump.c \
acpica/source/components/resources/rsutils.c \
acpica/source/components/resources/rscreate.c \
acpica/source/components/resources/rsio.c \
acpica/source/components/resources/rsmemory.c \
acpica/source/components/resources/rslist.c \
acpica/source/components/resources/rsinfo.c \
acpica/source/components/resources/rsirq.c \
acpica/source/components/resources/rsserial.c \
acpica/source/components/resources/rsaddr.c \
acpica/source/components/resources/rsdumpinfo.c \
acpica/source/components/resources/rsxface.c \
acpica/source/components/resources/rsmisc.c
