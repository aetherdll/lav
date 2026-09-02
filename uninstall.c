#include <ntddk.h>

VOID Uninstall_HandleMaliciousEvent(HANDLE ProcessId, PUNICODE_STRING ImageFileName) {
    if (ImageFileName != NULL && ImageFileName->Buffer != NULL) {
        KdPrint(("LAV [Uninstall]: [THREAT BLOCKED] PID: %p | Target: %wZ\n", 
            ProcessId, ImageFileName));
    }
}