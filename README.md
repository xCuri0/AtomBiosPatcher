# AtomBiosPatcher
Load a custom AtomBios to unlock graphics card features. Currently this tool only supports loading to the ACPI VFCT table which in my testing only Linux used. Probably will only work on pure-UEFI APU systems

# TODO
Load to platform BIOS (idk what this is but apparently Windows uses it)

Load to legacy BIOS so CSM will work

# Credit
HackBGRT (alot of code reused from it)
drivers/gpu/drm/radeon/radeon_bios.c
