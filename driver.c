#include <ntddk.h>

NTSTATUS Manager_Initialize(PDRIVER_OBJECT DriverObject);
VOID Manager_Uninitialize(PDRIVER_OBJECT DriverObject);

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    Manager_Uninitialize(DriverObject);
    KdPrint(("LAV [Driver]: lavsys.sys unloaded successfully.\n"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;

    KdPrint(("LAV [Driver]: Loading Lower Anti Virus System Driver...\n"));

    DriverObject->DriverUnload = DriverUnload;

    status = Manager_Initialize(DriverObject);
    if (!NT_SUCCESS(status)) {
        KdPrint(("LAV [Driver]: Initialization failed! Status: 0x%X\n", status));
        return status;
    }

    KdPrint(("LAV [Driver]: All EDR core and security modules active.\n"));
    return STATUS_SUCCESS;
}