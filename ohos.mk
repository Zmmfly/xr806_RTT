#
# Rules for ohos library
#

OHOSSOURCE = -Wl,--whole-archive
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libapp_console.a
OHOSSOURCE += -Wl,--no-whole-archive

OHOSSOURCE += -Wl,--start-group
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhievent_lite.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libnative_file.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libliteos_glue.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libdump_static.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libsamgr.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libkernel.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libsec.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libohos_bt_gatt.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libarch.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libbootstrap.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhiview_lite.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libbacktrace.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libposix.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libcpup.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libsamgr_source.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libbroadcast.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhal_iothardware.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhal_token_static.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libcmsis.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libutils.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libexchook.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libwifiservice.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhal_file_static.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libxr_wifi_adapter.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhal_sysparam.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libkal.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libsysparam.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhichainsdk.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libtoken_static.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libutils_kv_store.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhilog_lite_command.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libsamgr_adapter.a
OHOSSOURCE += $(ROOT_PATH)/lib/ohos/libhilog_lite.a
OHOSSOURCE += -Wl,--end-group
#
# end of file
#

