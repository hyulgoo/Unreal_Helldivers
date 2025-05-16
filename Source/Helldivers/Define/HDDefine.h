#pragma once

#if PLATFORM_WINDOWS
#define SHORT_FILE_NAME \
        (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define SHORT_FILE_NAME \
        (strrchr(__FILE__, '/')  ? strrchr(__FILE__, '/')  + 1 : __FILE__)
#endif

#define CONDITION_CHECK(condition) \
if(condition) \
{ \
    if (WITH_EDITOR && GIsEditor && GWorld && GWorld->IsPlayInEditor()) \
    { \
        UE_LOG(LogTemp, Error, TEXT("[%s]!, At Function: %s (%s:%d)"), \
            TEXT(#condition), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
    } \
    return; \
} \

#define CONDITION_CHECK_WITH_RETURNTYPE(condition, ret) \
if(condition) \
{ \
    if (WITH_EDITOR && GIsEditor && GWorld && GWorld->IsPlayInEditor()) \
    { \
        UE_LOG(LogTemp, Error, TEXT("[%s]!, At Function: %s (%s:%d)"), \
            TEXT(#condition), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
    } \
    return ret; \
} \

#define NULL_CHECK(ptr) \
if(ptr == nullptr) \
{ \
    if (WITH_EDITOR && GIsEditor && GWorld && GWorld->IsPlayInEditor()) \
    { \
        UE_LOG(LogTemp, Error, TEXT("[%s] is null, At Function: %s (%s:%d)"), \
            TEXT(#ptr), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
    } \
    return; \
} \

#define NULL_CHECK_WITH_RETURNTYPE(ptr, ret) \
if(ptr == nullptr) \
{ \
    if (WITH_EDITOR && GIsEditor && GWorld && GWorld->IsPlayInEditor()) \
    { \
        UE_LOG(LogTemp, Error, TEXT("[%s] is null, At Function: %s (%s:%d)"), \
            TEXT(#ptr), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
    } \
    return ret; \
} \

#define VALID_CHECK(ptr) \
if(IsValid(ptr) == false) \
{ \
    if (WITH_EDITOR && GIsEditor && GWorld && GWorld->IsPlayInEditor()) \
        { \
        const UObject* CheckedObject = static_cast<const UObject*>(ptr); \
        FString ObjectName = CheckedObject ? CheckedObject->GetName() : TEXT("null"); \
        UE_LOG(LogTemp, Error, TEXT("Invalid : [%s], Name: [%s], At Function: %s (%s:%d)"), \
        TEXT(#ptr), *ObjectName, ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
        } \
    return; \
} \

#define VALID_CHECK_WITH_RETURNTYPE(ptr, ret) \
if(IsValid(ptr) == false) \
{ \
    if (WITH_EDITOR && GIsEditor && GWorld && GWorld->IsPlayInEditor()) \
    { \
        const UObject* CheckedObject = static_cast<const UObject*>(ptr); \
        FString ObjectName = CheckedObject ? CheckedObject->GetName() : TEXT("null"); \
        UE_LOG(LogTemp, Error, TEXT("Invalid : [%s], Name: [%s], At Function: %s (%s:%d)"), \
        TEXT(#ptr), *ObjectName, ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
    } \
    return ret; \
} \
