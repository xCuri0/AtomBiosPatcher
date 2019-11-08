#pragma once

#include <efi.h>

/**
 * Convert a short ASCII string to UCS2, store in a static array.
 *
 * @param src The ASCII string. Will be truncated to 15 characters + null.
 * @param length The maximum length, if the string is not null-terminated.
 * @return The UCS2 string, statically allocated, null-terminated.
 */
extern const CHAR16* TmpStr(CHAR8 *src, int length);

/**
 * Load a file, allocate some extra bytes as well.
 */
extern void* LoadFileWithPadding(EFI_FILE_HANDLE dir, const CHAR16* path, UINTN* size_ptr, UINTN padding);

/**
 * Load a file.
 */
static inline void* LoadFile(EFI_FILE_HANDLE dir, const CHAR16* path, UINTN* size_ptr) {
	return LoadFileWithPadding(dir, path, size_ptr, 0);
}
