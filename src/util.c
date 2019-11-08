#include "util.h"

#include <efilib.h>

const CHAR16* TmpStr(CHAR8 *src, int length) {
	static CHAR16 arr[4][16];
	static int j;
	CHAR16* dest = arr[j = (j+1) % 4];
	int i;
	for (i = 0; i < length && i < 16-1 && src[i]; ++i) {
		dest[i] = src[i];
	}
	dest[i] = 0;
	return dest;
}

void* LoadFileWithPadding(EFI_FILE_HANDLE dir, const CHAR16* path, UINTN* size_ptr, UINTN padding) {
	EFI_STATUS e;
	EFI_FILE_HANDLE handle;

	e = dir->Open(dir, &handle, (CHAR16*) path, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(e)) {
		return 0;
	}

	EFI_FILE_INFO *info = LibFileInfo(handle);
	UINTN size = info->FileSize;
	FreePool(info);

	void* data = 0;
	e = BS->AllocatePool(EfiBootServicesData, size + padding, &data);
	if (EFI_ERROR(e)) {
		handle->Close(handle);
		return 0;
	}
	e = handle->Read(handle, &size, data);
	for (int i = 0; i < padding; ++i) {
		*((char*)data + size + i) = 0;
	}
	handle->Close(handle);
	if (EFI_ERROR(e)) {
		FreePool(data);
		return 0;
	}
	if (size_ptr) {
		*size_ptr = size;
	}
	return data;
}
