#include <ntddk.h>

OB_CALLBACK_REGISTRATION g_ObRegistration = { 0 };
PVOID g_RegistrationHandle = NULL;

OB_PREOP_CALLBACK_STATUS ObjectPreCallback(
    PVOID RegistrationContext,
    POB_PRE_OPERATION_INFORMATION OperationInformation
) {
    UNREFERENCED_PARAMETER(RegistrationContext);

    if (OperationInformation->ObjectType == *PsProcessType) {
        PEPROCESS targetProcess = (PEPROCESS)OperationInformation->Object;
        UNREFERENCED_PARAMETER(targetProcess);
    }

    return OB_PREOP_SUCCESS;
}

NTSTATUS SelfProtection_Initialize(VOID) {
    OB_CALLBACK_REGISTRATION obReg;
    OB_OPERATION_REGISTRATION opReg;

    UNICODE_STRING altitudeString;
    RtlInitUnicodeString(&altitudeString, L"321000");

    RtlZeroMemory(&obReg, sizeof(OB_CALLBACK_REGISTRATION));
    obReg.Version = OB_FLT_REGISTRATION_VERSION;
    obReg.OperationRegistrationCount = 1;
    obReg.Altitude = altitudeString;
    
    RtlZeroMemory(&opReg, sizeof(OB_OPERATION_REGISTRATION));
    opReg.ObjectType = PsProcessType;
    opReg.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
    opReg.PreOperation = ObjectPreCallback;
    
    obReg.OperationRegistration = &opReg;

    NTSTATUS status = ObRegisterCallbacks(&obReg, &g_RegistrationHandle);
    if (NT_SUCCESS(status)) {
        KdPrint(("LAV [SelfProtection]: Object callbacks active against tampering.\n"));
    }
    return status;
}

VOID SelfProtection_Uninitialize(VOID) {
    if (g_RegistrationHandle) {
        ObUnRegisterCallbacks(g_RegistrationHandle);
        KdPrint(("LAV [SelfProtection]: Object callbacks removed.\n"));
    }
}