#include <ntddk.h>

LARGE_INTEGER g_RegistryCookie = { 0 };

NTSTATUS RegistryCallback(PVOID CallbackContext, PVOID Argument1, PVOID Argument2) {
    UNREFERENCED_PARAMETER(CallbackContext);
    UNREFERENCED_PARAMETER(Argument2);

    REG_NOTIFY_CLASS notifyClass = (REG_NOTIFY_CLASS)(ULONG_PTR)Argument1;
    if (notifyClass == RegNtPreSetValueKey) {
        // Intercept persistence modifications
    }
    return STATUS_SUCCESS;
}

NTSTATUS Registry_Initialize(VOID) {
    UNICODE_STRING altitude = RTL_CONSTANT_STRING(L"320000");
    NTSTATUS status = CmRegisterCallbackEx(RegistryCallback, &altitude, NULL, NULL, &g_RegistryCookie, NULL);
    if (NT_SUCCESS(status)) KdPrint(("LAV [Registry]: Protection callback active.\n"));
    return status;
}

VOID Registry_Uninitialize(VOID) {
    if (g_RegistryCookie.QuadPart != 0) {
        CmUnRegisterCallback(g_RegistryCookie);
        KdPrint(("LAV [Registry]: Callback removed.\n"));
    }
}