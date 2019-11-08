#include <efi.h>
#include <efilib.h>

#include "types.h"
#include "util.h"

#define ACPI_OFFSET 0x68

/**
 * The Print function signature.
 */
typedef UINTN print_t(IN CONST CHAR16 *fmt, ...);

/**
 * The function for debug printing; either Print or NullPrint.
 */
print_t* Debug = Print;

/**
 * Update the ACPI tables as needed for the desired table change.
 *
 * If action is REMOVE, table entry will be removed.
 * If action is KEEP, the first table entry will be returned.
 * If action is REPLACE, the given table entry will be stored in each XSDT.
 *
 * @param action The intended action.
 * @param table The table, if action is REPLACE.
 * @return Pointer to the table, or 0 if not found (or destroyed).
 */
static ACPI_SDT_HEADER* HandleAcpiTables() {
	ACPI_SDT_HEADER* table = 0;
	for (int i = 0; i < ST->NumberOfTableEntries; i++) {
		EFI_GUID Acpi20TableGuid = ACPI_20_TABLE_GUID;
		EFI_GUID* vendor_guid = &ST->ConfigurationTable[i].VendorGuid;
		if (!CompareGuid(vendor_guid, &AcpiTableGuid) && !CompareGuid(vendor_guid, &Acpi20TableGuid)) {
			continue;
		}
		ACPI_20_RSDP* rsdp = (ACPI_20_RSDP *) ST->ConfigurationTable[i].VendorTable;
		if (CompareMem(rsdp->signature, "RSD PTR ", 8) != 0 || rsdp->revision < 2 || !VerifyAcpiRsdp2Checksums(rsdp)) {
			continue;
		}
		Debug(L"RSDP: revision = %d, OEM ID = %s\n", rsdp->revision, TmpStr(rsdp->oem_id, 6));

		ACPI_SDT_HEADER* xsdt = (ACPI_SDT_HEADER *) (UINTN) rsdp->xsdt_address;
		if (!xsdt || CompareMem(xsdt->signature, "XSDT", 4) != 0 || !VerifyAcpiSdtChecksum(xsdt)) {
			Debug(L"* XSDT: missing or invalid\n");
			continue;
		}
		UINT64* entry_arr = (UINT64*)&xsdt[1];
		UINT32 entry_arr_length = (xsdt->length - sizeof(*xsdt)) / sizeof(UINT64);

		Debug(L"* XSDT: OEM ID = %s, entry count = %d\n", TmpStr(xsdt->oem_id, 6), entry_arr_length);

		int table_count = 0;
		for (int j = 0; j < entry_arr_length; j++) {
			ACPI_SDT_HEADER *entry = (ACPI_SDT_HEADER *)((UINTN)entry_arr[j]);
			if (CompareMem(entry->signature, "VFCT", 4) != 0) {
				continue;
			}
			Debug(L" - ACPI table: %s, revision = %d, OEM ID = %s\n", TmpStr(entry->signature, 4), entry->revision, TmpStr(entry->oem_id, 6));
            Debug(L" -> Returning this one for later use.\n");
            table = (ACPI_SDT_HEADER*) entry;
			table_count += 1;
		}
		SetAcpiSdtChecksum(xsdt);
	}
	return table;
}

void ReplaceACPI(void* atom, UINTN s) {
    ACPI_SDT_HEADER* table = HandleAcpiTables(0);

	Print(L"Patching VFCT ATOMBIOS at %x\n", table);

    CopyMem(table, atom, s);
	SetAcpiSdtChecksum(table);
}

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *ST_) {
	UINTN size_ptr = 0;
	EFI_LOADED_IMAGE* image;

	InitializeLib(image_handle, ST_);
    if (EFI_ERROR(BS->HandleProtocol(image_handle, &LoadedImageProtocol, (void**) &image))) {
		Debug(L"HackACPI: LOADED_IMAGE_PROTOCOL failed.\n");
        return 1;
	}
	EFI_FILE_HANDLE root_dir = LibOpenRoot(image->DeviceHandle);

	Print(L"AtomBiosPatcher\n\n");

	Print(L"Loading ATOM.bin\n");

	void* atom = LoadFile(root_dir, L"ATOM.bin", &size_ptr);
	if (!atom) {
		Print(L"Failed to load ATOM.bin\n");
		return 1;
	}
	ReplaceACPI(atom, size_ptr);

	/*
	TODO: Replace Platform ROM
	*/
	return 0;
}