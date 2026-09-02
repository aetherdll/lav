#include <ntddk.h>

NTSTATUS Download_HandleCleanEvent(HANDLE ProcessId, PUNICODE_STRING ImageFileName);
VOID Uninstall_HandleMaliciousEvent(HANDLE ProcessId, PUNICODE_STRING ImageFileName);
NTSTATUS Communication_Initialize(PDRIVER_OBJECT DriverObject);
VOID Communication_Uninitialize(VOID);
NTSTATUS Registry_Initialize(VOID);
VOID Registry_Uninitialize(VOID);
NTSTATUS FileTracker_Initialize(PDRIVER_OBJECT DriverObject);
VOID FileTracker_Uninitialize(VOID);
NTSTATUS SelfProtection_Initialize(VOID);
VOID SelfProtection_Uninitialize(VOID);

VOID LavProcessNotifyRoutineEx(
    PEPROCESS Process,
    HANDLE ProcessId,
    PPS_CREATE_NOTIFY_INFO CreateInfo
) {
    UNREFERENCED_PARAMETER(Process);

    if (CreateInfo == NULL) {
        return;
    }

    if (CreateInfo->ImageFileName != NULL) {
        BOOLEAN isMalicious = FALSE; 

        if (isMalicious) {
            Uninstall_HandleMaliciousEvent(ProcessId, CreateInfo->ImageFileName);
            CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
        } else {
            Download_HandleCleanEvent(ProcessId, CreateInfo->ImageFileName);
        }
    }
}

NTSTATUS Manager_Initialize(PDRIVER_OBJECT DriverObject) {
    NTSTATUS status;

    KdPrint(("LAV [Manager]: Initializing advanced EDR subsystems...\n"));

    status = PsSetCreateProcessNotifyRoutineEx(LavProcessNotifyRoutineEx, FALSE);
    if (!NT_SUCCESS(status)) return status;

    status = Registry_Initialize();
    if (!NT_SUCCESS(status)) KdPrint(("LAV [Manager]: Registry protection warning.\n"));

    status = Communication_Initialize(DriverObject);
    if (!NT_SUCCESS(status)) KdPrint(("LAV [Manager]: Communication interface warning.\n"));

    status = FileTracker_Initialize(DriverObject);
    if (!NT_SUCCESS(status)) KdPrint(("LAV [Manager]: FileTracker warning (MiniFilter).\n"));

    status = SelfProtection_Initialize();
    if (!NT_SUCCESS(status)) KdPrint(("LAV [Manager]: Self-Protection warning.\n"));

    KdPrint(("LAV [Manager]: All subsystems initialized successfully.\n"));
    return STATUS_SUCCESS;
}

VOID Manager_Uninitialize(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    KdPrint(("LAV [Manager]: Shutting down security subsystems...\n"));

    SelfProtection_Uninitialize();
    FileTracker_Uninitialize();
    Communication_Uninitialize();
    Registry_Uninitialize();
    PsSetCreateProcessNotifyRoutineEx(LavProcessNotifyRoutineEx, TRUE);

    KdPrint(("LAV [Manager]: Core manager uninitialized.\n"));
}