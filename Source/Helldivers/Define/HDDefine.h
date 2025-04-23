#pragma once

#define NULL_CHECK(ptr) \
if(ptr == nullptr) \
{ \
    UE_LOG(LogTemp, Error, TEXT("pointer [%s] is nullptr!"), TEXT(#ptr)); \
    return; \
} \

#define NULL_CHECK_WITH_RETURNTYPE(ptr, ret) \
if(ptr == nullptr) \
{ \
    UE_LOG(LogTemp, Error, TEXT("pointer [%s] is nullptr!"), TEXT(#ptr)); \
    return ret; \
} \

#define VALID_CHECK(ptr) \
if (IsValid(ptr) == false) { \
    UE_LOG(LogTemp, Error, TEXT("UObject [%s] is Invalid!"), TEXT(#ptr)); \
    return; \
} \

#define VALID_CHECK_WITH_RETURNTYPE(ptr, ret) \
if (IsValid(ptr) == false) \
{ \
    UE_LOG(LogTemp, Error, TEXT("UObject [%s] is Invalid!"), TEXT(#ptr)); \
    return ret; \
} \
