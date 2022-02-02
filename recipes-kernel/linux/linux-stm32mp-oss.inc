SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit kernel
include linux-stm32mp.inc
include linux-stm32mp-sign-kernel.inc

#SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.tar.xz;name=kernel"

SRC_URI += " \
    file://0001-ARM-STM32MP15-support-of-Preemptible-Kernel.patch \
           file://0001-tty-rpmsg-Fix-race-condition-releasing-tty-port.patch \
           file://0002-rpmsg-char-Fix-race-between-the-release-of-rpmsg_ctr.patch \
           file://0003-rpmsg-char-Fix-race-between-the-release-of-rpmsg_ept.patch \
           file://0004-rpmsg-char-Export-eptdev-create-and-destroy-function.patch \
           file://0005-rpmsg-Create-the-rpmsg-class-in-core-instead-of-in-r.patch \
           file://0006-rpmsg-Move-the-rpmsg-control-device-from-rpmsg_char-.patch \
           file://0007-arm-configs-Configs-that-had-RPMSG_CHAR-now-get-RPMS.patch \
           file://0008-RISC-V-configs-Configs-that-had-RPMSG_CHAR-now-get-R.patch \
           file://0009-arm64-defconfig-Config-that-had-RPMSG_CHAR-now-gets-.patch \
           file://0010-rpmsg-Update-rpmsg_chrdev_register_device-function.patch \
           file://0011-rpmsg-char-Refactor-rpmsg_chrdev_eptdev_create-funct.patch \
           file://0012-rpmsg-char-Add-possibility-to-use-default-endpoint-o.patch \
           file://0013-rpmsg-char-Introduce-the-rpmsg-raw-channel.patch \
           file://0014-rpmsg-ctrl-Introduce-new-RPMSG_CREATE-RELEASE_DEV_IO.patch \
           file://0015-usb-dwc2-drd-fix-soft-connect-when-gadget-is-unconfi.patch \
           file://0016-rpmsg-virtio-overwrite-dst-add-on-ept-callback-if-se.patch \
          "

PV = "${LINUX_VERSION}"

S = "${WORKDIR}/linux-${ST_KERNEL_VERSION}"

# -------------------------------------------------------------
# Defconfig
#
KERNEL_DEFCONFIG        = "defconfig"
KERNEL_CONFIG_FRAGMENTS = "${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${WORKDIR}/fragments/5.x/fragment-01-multiv7_cleanup.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${WORKDIR}/fragments/5.x/fragment-02-multiv7_addons.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '${WORKDIR}/fragments/5.x/fragment-03-systemd.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('COMBINED_FEATURES', 'optee', '${WORKDIR}/fragments/5.x/fragment-04-optee.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${WORKDIR}/fragments/5.x/fragment-05-modules.config"
KERNEL_CONFIG_FRAGMENTS += "${@oe.utils.ifelse(d.getVar('KERNEL_SIGN_ENABLE') == '1', '${WORKDIR}/fragments/5.x/fragment-06-signature.config','')} "
KERNEL_CONFIG_FRAGMENTS += "${WORKDIR}/fragments/5.x/fragment-09-efi.config"

SRC_URI += "file://5.x/fragment-01-multiv7_cleanup.config;subdir=fragments"
SRC_URI += "file://5.x/fragment-02-multiv7_addons.config;subdir=fragments"
SRC_URI += "file://5.x/fragment-03-systemd.config;subdir=fragments"
SRC_URI += "file://5.x/fragment-04-optee.config;subdir=fragments"
SRC_URI += "file://5.x/fragment-05-modules.config;subdir=fragments"
SRC_URI += "file://5.x/fragment-06-signature.config;subdir=fragments"
SRC_URI += "file://5.x/fragment-09-efi.config;subdir=fragments"

# -------------------------------------------------------------
# Kernel Args
#
KERNEL_EXTRA_ARGS += "LOADADDR=${ST_KERNEL_LOADADDR}"

deltask shared_workdir
addtask shared_workdir after do_compile_kernelmodules before do_install
do_shared_workdir[depends] +="virtual/kernel:do_compile_kernelmodules"

# -------------------------------------------------------------
# RT Management
include ${@bb.utils.contains('RT_KERNEL', '1', 'linux-stm32mp-oss-rt.inc', '', d)}
# -------------------------------------------------------------
# modprobe action
#
do_install_append() {
    install -d ${D}/${sysconfdir}/modprobe.d/
    echo "softdep snd-soc-cs42l51 pre: snd-soc-cs42l51-i2c" > ${D}/${sysconfdir}/modprobe.d/stm32mp1-snd.conf
}
FILES_${KERNEL_PACKAGE_NAME}-modules += "${sysconfdir}/modprobe.d"
