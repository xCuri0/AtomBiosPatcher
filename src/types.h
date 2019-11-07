#pragma once

#include <efi.h>

#pragma pack(push, 1)

/** RSDP (Root System Description Pointer) */
typedef struct {
	CHAR8 signature[8];
	UINT8 checksum;
	CHAR8 oem_id[6];
	UINT8 revision;
	UINT32 rsdt_address;
	UINT32 length;
	UINT64 xsdt_address;
	UINT8 extended_checksum;
	UINT8 reserved[3];
} ACPI_20_RSDP;

/** SDT (System Description Table) entry header */
typedef struct {
	CHAR8 signature[4];
	UINT32 length;
	UINT8 revision;
	UINT8 checksum;
	CHAR8 oem_id[6];
	CHAR8 oem_table_id[8];
	UINT32 oem_revision;
	UINT32 asl_compiler_id;
	UINT32 asl_compiler_revision;
} ACPI_SDT_HEADER;

/**
 * Verify the checksums of an ACPI RSDP version 2.
 *
 * @param data Pointer to the table.
 * @return 1 if the checksum is correct, 0 otherwise.
 */
extern int VerifyAcpiRsdp2Checksums(const void* data);

/**
 * Set the correct checksums of an ACPI RSDP version 2.
 *
 * @param data Pointer to the table.
 */
extern void SetAcpiRsdp2Checksums(void* data);

/**
 * Verify the checksum of an ACPI SDT.
 *
 * @param data Pointer to the table.
 * @return 1 if the checksum is correct, 0 otherwise.
 */
extern int VerifyAcpiSdtChecksum(const void* data);

/**
 * Set the correct checksum for an ACPI SDT.
 *
 * @param data Pointer to the table.
 */
extern void SetAcpiSdtChecksum(void* data);

#pragma pack(pop)
