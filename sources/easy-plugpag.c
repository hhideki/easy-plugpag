#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "libPPPagSeguro/PPPagSeguro.h"

#include "easy-plugpag.h"


#define ASSERT(condition, falseConditionBlock) \
    do \
    { \
        if (condition) \
        { \
            falseConditionBlock; \
        } \
    } while(0);


struct PlugPagInternalData
{
    struct PlugPagApplicationData appData;
    tyComPort portNumber;
    unsigned char isReleased;
};


static int pay(struct PlugPag *plugpag, struct PlugPagPaymentData *paymentData, stPPPSTransactionResult *outResult);
static int voidPayment(struct PlugPag *plugpag, stPPPSTransactionResult *outResult);
static int getLastApprovedTransaction(struct PlugPag *plugpag, stPPPSTransactionResult *outResult);
static int getVersion(struct PlugPag *plugpag, char *outVersion);
static int release(struct PlugPag *plugpag);


struct PlugPag *PlugPag(tyComPort portNumber, struct PlugPagApplicationData *appData)
{
    ASSERT(portNumber == NULL, { return NULL; });
    ASSERT(appData == NULL, { return NULL; });
    ASSERT(appData->appName == NULL || strlen(appData->appName) == 0, { return NULL; });
    ASSERT(appData->appVersion == NULL || strlen(appData->appVersion) == 0, { return NULL; });

    struct PlugPag *pp = (struct PlugPag *) malloc(sizeof(struct PlugPag));
    ASSERT(pp == NULL, { return NULL; });

    struct PlugPagInternalData *internal = (struct PlugPagInternalData *) malloc(sizeof(struct PlugPagInternalData));
    ASSERT(internal == NULL, { free(pp); return NULL; });

    strncpy(internal->appData.appName, appData->appName, sizeof(tyAppName));
    strncpy(internal->appData.appVersion, appData->appVersion, sizeof(tyAppVersion));
    strncpy(internal->portNumber, portNumber, sizeof(tyComPort));
    internal->isReleased = (unsigned char) 0;

    SetVersionName(internal->appData.appName, internal->appData.appVersion);
    InitBTConnection((const tyComPort *) internal->portNumber);

    pp->__internal = internal;
    pp->pay = pay;
    pp->voidPayment = voidPayment;
    pp->getLastApprovedTransaction = getLastApprovedTransaction;
    pp->getVersion = getVersion;
    pp->release = release;

    return pp;
}


static int pay(struct PlugPag *plugpag, struct PlugPagPaymentData *paymentData, stPPPSTransactionResult *outResult)
{
    ASSERT(plugpag == NULL, { return EASY_PLUGPAG_ERROR_NULL_PLUGPAG_INSTANCE; });
    ASSERT(paymentData == NULL, { return EASY_PLUGPAG_ERROR_NULL_PAYMENT_DATA; });
    ASSERT(outResult == NULL, { return EASY_PLUGPAG_ERROR_NULL_RESULT_OUTPUT; });
    ASSERT(paymentData->installments < 0 ||
           paymentData->amount == NULL || strlen(paymentData->amount) == 0 ||
           paymentData->userReference == NULL || strlen(paymentData->userReference) == 0,
           { return EASY_PLUGPAG_ERROR_MALFORMED_PAYMENT_DATA; });

    struct PlugPagInternalData *internal = (struct PlugPagInternalData *) plugpag->__internal;
    ASSERT(internal == NULL || internal->isReleased != 0, { return EASY_PLUGPAG_ERROR_RELEASED; });

    int result = SimplePaymentTransaction(
        paymentData->paymentMethod,
        paymentData->installmentType,
        paymentData->installments,
        (const tyAmount *) &paymentData->amount,
        (const tyUserReference *) &paymentData->userReference,
        outResult);

    return result;
}


static int voidPayment(struct PlugPag *plugpag, stPPPSTransactionResult *outResult)
{
    ASSERT(plugpag == NULL, { return EASY_PLUGPAG_ERROR_NULL_PLUGPAG_INSTANCE; });
    ASSERT(outResult == NULL, { return EASY_PLUGPAG_ERROR_NULL_RESULT_OUTPUT; });

    struct PlugPagInternalData *internal = (struct PlugPagInternalData *) plugpag->__internal;
    ASSERT(internal == NULL || internal->isReleased != 0, { return EASY_PLUGPAG_ERROR_RELEASED; });

    return CancelTransaction(outResult);
}


static int getLastApprovedTransaction(struct PlugPag *plugpag, stPPPSTransactionResult *outResult)
{
    ASSERT(plugpag == NULL, { return EASY_PLUGPAG_ERROR_NULL_PLUGPAG_INSTANCE; });
    ASSERT(outResult == NULL, { return EASY_PLUGPAG_ERROR_NULL_RESULT_OUTPUT; });

    struct PlugPagInternalData *internal = (struct PlugPagInternalData *) plugpag->__internal;
    ASSERT(internal == NULL || internal->isReleased != 0, { return EASY_PLUGPAG_ERROR_RELEASED; });

    return GetLastApprovedTransactionStatus(outResult);
}


static int getVersion(struct PlugPag *plugpag, char *outVersion)
{
    ASSERT(plugpag == NULL, { return EASY_PLUGPAG_ERROR_NULL_PLUGPAG_INSTANCE; });
    ASSERT(outVersion == NULL, { return EASY_PLUGPAG_ERROR_NULL_OUTPUT; });

    const char *version = GetVersionLib();
    memset(outVersion, 0, strlen(version) + 1);
    memmove(outVersion, version, strlen(version));

    return EASY_PLUGPAG_OK;
}


struct PlugPagApplicationData *PlugPagApplicationData(tyAppName appName, tyAppVersion appVersion)
{
    ASSERT(appName == NULL || strlen(appName) == 0, { return NULL; });
    ASSERT(appVersion == NULL || strlen(appVersion) == 0, { return NULL; });

    struct PlugPagApplicationData *data = (struct PlugPagApplicationData *) malloc(sizeof(struct PlugPagApplicationData));
    ASSERT(data == NULL, { return NULL; });

    memmove(data->appName, appName, sizeof(tyAppName));
    memmove(data->appVersion, appVersion, sizeof(tyAppVersion));

    return data;
}


struct PlugPagPaymentData *PlugPagPaymentData(enPPPSPaymentMethod paymentMethod, enPPPSInstallmentType installmentType, unsigned int installments, tyAmount amount, tyUserReference userReference)
{
    ASSERT(installments < 0, { return NULL; });
    ASSERT(amount == NULL || strlen(amount) == 0, { return NULL; });
    ASSERT(userReference == NULL || strlen(userReference) == 0, { return NULL; });

    struct PlugPagPaymentData *data = (struct PlugPagPaymentData *) malloc(sizeof(struct PlugPagPaymentData));
    ASSERT(data == NULL, { return NULL; });

    data->paymentMethod = paymentMethod;
    data->installmentType = installmentType;
    data->installments = installments;
    memmove(data->amount, amount, sizeof(tyAmount));
    memmove(data->userReference, userReference, sizeof(tyUserReference));

    return data;
}


static int release(struct PlugPag *plugpag)
{
    ASSERT(plugpag == NULL, { return EASY_PLUGPAG_OK; });

    struct PlugPagInternalData *internal = (struct PlugPagInternalData *) plugpag->__internal;

    ASSERT(internal == NULL || internal->isReleased != 0, { return EASY_PLUGPAG_OK; });

    free(internal);
    plugpag->__internal = NULL;

    return EASY_PLUGPAG_OK;
}