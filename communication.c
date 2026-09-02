#include <ntddk.h>

#define SIOCTL_DEVICE_TYPE 0x00008305
#define IOCTL_LAV_COMM_MESSAGE CTL_CODE(SIOCTL_DEVICE_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

PDEVICE_OBJECT g_LavDeviceObject = NULL;

NTSTATUS Communication_CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS Communication_IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_LAV_COMM_MESSAGE:
            KdPrint(("LAV [Communication]: Signal received from lavservice.exe.\n"));
            break;
        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS Communication_Initialize(PDRIVER_OBJECT DriverObject) {
    NTSTATUS status;
    UNICODE_STRING devName, symLink;

    RtlInitUnicodeString(&devName, L"\\Device\\LavSysDevice");
    RtlInitUnicodeString(&symLink, L"\\DosDevices\\LavSysLink");

    status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &g_LavDeviceObject);
    if (!NT_SUCCESS(status)) return status;

    status = IoCreateSymbolicLink(&symLink, &devName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(g_LavDeviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = Communication_CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = Communication_CreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Communication_IoControl;

    KdPrint(("LAV [Communication]: IOCTL channel established.\n"));
    return STATUS_SUCCESS;
}

VOID Communication_Uninitialize(VOID) {
    UNICODE_STRING symLink;
    RtlInitUnicodeString(&symLink, L"\\DosDevices\\LavSysLink");
    IoDeleteSymbolicLink(&symLink);
    if (g_LavDeviceObject) IoDeleteDevice(g_LavDeviceObject);
    KdPrint(("LAV [Communication]: Channel closed.\n"));
}