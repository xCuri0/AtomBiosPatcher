# AtomBiosPatcher
Load a custom AtomBios to unlock graphics card features. Currently this tool only supports loading to the ACPI VFCT table. Probably will only work on pure-UEFI APU only systems

# Windows
If it appears that the ATOMBIOS hasn't changed, uninstall the GPU from device manager (keep software), reboot and it should automatically install back. During all these steps make sure you boot with AtomBiosPatcher (and in the future too so it doesn't get overwritten)

# Credit
HackBGRT (alot of code reused from it)
drivers/gpu/drm/radeon/radeon_bios.c
