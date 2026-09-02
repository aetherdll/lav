#include <ntddk.h>
#include <fltKernel.h>

PFLT_FILTER g_FilterHandle = NULL;

FLT_PREOP_CALLBACK_STATUS FilePreCreate(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS FltObjects,
    PVOID *CompletionContext
) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    if (Data->RequestorMode == UserMode) {
        // Monitor file creation/opening attempts from user space
    }

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_CREATE, 0, FilePreCreate, NULL },
    { IRP_MJ_OPERATION_REGISTRATION_END }
};

CONST FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),
    FLT_REGISTRATION_VERSION,
    0,
    NULL,
    Callbacks,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

NTSTATUS FileTracker_Initialize(PDRIVER_OBJECT DriverObject) {
    NTSTATUS status = FltRegisterFilter(DriverObject, &FilterRegistration, &g_FilterHandle);
    if (NT_SUCCESS(status)) {
        FltStartFiltering(g_FilterHandle);
        KdPrint(("LAV [FileTracker]: MiniFilter file monitor started.\n"));
    }
    return status;
}

VOID FileTracker_Uninitialize(VOID) {
    if (g_FilterHandle) {
        FltUnregisterFilter(g_FilterHandle);
        KdPrint(("LAV [FileTracker]: MiniFilter unregistered.\n"));
    }
}