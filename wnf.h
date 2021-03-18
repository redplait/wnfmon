extern "C"
{

typedef struct _WNF_STATE_NAME
{
    ULONG Data[2];
} WNF_STATE_NAME, *PWNF_STATE_NAME;

typedef const WNF_STATE_NAME *PCWNF_STATE_NAME;

typedef struct _WNF_TYPE_ID
{
    GUID TypeId;
} WNF_TYPE_ID, *PWNF_TYPE_ID;

typedef const WNF_TYPE_ID *PCWNF_TYPE_ID;

// rev
typedef ULONG WNF_CHANGE_STAMP, *PWNF_CHANGE_STAMP;

typedef enum _WNF_STATE_NAME_LIFETIME
{
    WnfWellKnownStateName,
    WnfPermanentStateName,
    WnfPersistentStateName,
    WnfTemporaryStateName
} WNF_STATE_NAME_LIFETIME;

typedef enum _WNF_STATE_NAME_INFORMATION
{
    WnfInfoStateNameExist,
    WnfInfoSubscribersPresent,
    WnfInfoIsQuiescent
} WNF_STATE_NAME_INFORMATION;

typedef enum _WNF_DATA_SCOPE
{
    WnfDataScopeSystem,
    WnfDataScopeSession,
    WnfDataScopeUser,
    WnfDataScopeProcess,
    WnfDataScopeMachine // REDSTONE3
} WNF_DATA_SCOPE;

// from https://movaxbx.ru/tag/injection/
typedef NTSTATUS (NTAPI *PWNF_USER_CALLBACK) (
    _In_     WNF_STATE_NAME   StateName,
    _In_     WNF_CHANGE_STAMP ChangeStamp,
    _In_opt_ PWNF_TYPE_ID     TypeId,
    _In_opt_ PVOID            CallbackContext,
    _In_     PVOID            Buffer,
    _In_     ULONG            BufferSize);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryWnfStateData(
    _In_ PCWNF_STATE_NAME StateName,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_opt_ const VOID *ExplicitScope,
    _Out_ PWNF_CHANGE_STAMP ChangeStamp,
    _Out_writes_bytes_to_opt_(*BufferSize, *BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
// wnf 2 dword, 0, size, buffer, 0
RtlPublishWnfStateData(WNF_STATE_NAME StateName, size_t, PVOID Buffer, ULONG BufferSize, size_t);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSubscribeWnfStateChange(
    _In_ PCWNF_STATE_NAME StateName,
    _In_opt_ WNF_CHANGE_STAMP ChangeStamp,
    _In_ ULONG EventMask,
    _Out_opt_ PULONG64 SubscriptionId
    );

NTSYSCALLAPI
NTSTATUS NTAPI
RtlSubscribeWnfStateChangeNotification(void*,
                                         WNF_STATE_NAME StateName,
                                         uint32_t,
                                         PWNF_USER_CALLBACK,
                                         PVOID ctx,
                                         size_t,
                                         size_t,
                                         size_t);

};
