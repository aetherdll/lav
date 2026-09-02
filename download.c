#include <ntddk.h>

NTSTATUS Download_HandleCleanEvent(HANDLE ProcessId, PUNICODE_STRING ImageFileName) {
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(ImageFileName);
    return STATUS_SUCCESS;
}