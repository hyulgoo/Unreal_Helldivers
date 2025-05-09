#pragma once

#define CONDITION_CHECK(condition) \
if(condition) \
{ \
    if (WITH_EDITOR == false) \
    { \
         UE_LOG(LogTemp, Error, TEXT("[%s]!"), TEXT(#condition)); \
    } \
    return; \
} \

#define CONDITION_CHECK_WITH_RETURNTYPE(condition, ret) \
if(condition) \
{ \
    if (WITH_EDITOR == false) \
    { \
         UE_LOG(LogTemp, Error, TEXT("[%s]!"), TEXT(#condition)); \
    } \
    return ret; \
} \

#define NULL_CHECK(ptr) \
if(ptr == nullptr) \
{ \
    if (WITH_EDITOR == false) \
    { \
         UE_LOG(LogTemp, Error, TEXT("pointer [%s] is nullptr!"), TEXT(#ptr)); \
    } \
    return; \
} \

#define NULL_CHECK_WITH_RETURNTYPE(ptr, ret) \
if(ptr == nullptr) \
{ \
    if (WITH_EDITOR == false) \
    { \
        UE_LOG(LogTemp, Error, TEXT("pointer [%s] is nullptr!"), TEXT(#ptr)); \
    } \
    return ret; \
} \

#define VALID_CHECK(ptr) \
if (IsValid(ptr) == false) { \
    if (WITH_EDITOR == false) \
    { \
        UE_LOG(LogTemp, Error, TEXT("UObject [%s] is Invalid!"), TEXT(#ptr)); \
    } \
    return; \
} \

#define VALID_CHECK_WITH_RETURNTYPE(ptr, ret) \
if (IsValid(ptr) == false) \
{ \
    if (WITH_EDITOR == false) \
    { \
        UE_LOG(LogTemp, Error, TEXT("UObject [%s] is Invalid!"), TEXT(#ptr)); \
    } \
    return ret; \
} \
