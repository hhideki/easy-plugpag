#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libPPPagSeguro/PPPagSeguro.h"
#include "easy-plugpag.h"

static void printResult(const int resultCode, const stPPPSTransactionResult result);

int main(int argc, char *argv[])
{
    printf("Starting payment\n");

    int ret = -99999;
    stPPPSTransactionResult result;
    memset(&result, 0, sizeof(stPPPSTransactionResult));

    struct PlugPagApplicationData *appData = PlugPagApplicationData("easy-plugpag", "1.0.0");
    struct PlugPagPaymentData *paymentData = PlugPagPaymentData(PPPAGSEGURO_CREDIT, PPPAGSEGURO_A_VISTA, 1, "1234578", "easypp");
    struct PlugPag *pp = PlugPag("COM0", appData);
    ret = pp->pay(pp, paymentData, &result);
    printResult(ret, result);

    if (appData != NULL)
    {
        free(appData);
    }

    if (paymentData != NULL)
    {
        free(paymentData);
    }

    if (pp != NULL)
    {
        free(pp);
    }

    printf("Finished\n");
    return 0;
}


static void printResult(const int resultCode, const stPPPSTransactionResult result)
{
    printf("==================================================\n");
    printf("Result code: %d\n", resultCode);
    printf("Message:     %s\n", result.message);

    if (resultCode == PPPS_RET_OK)
    {
        printf("==================================================\n");
        printf("Date and time:          %s %s\n", result.date, result.time);
        printf("Transaction code:       %s\n", result.transactionCode);
        printf("Host NSU:               %s\n", result.hostNsu);
        printf("Card brand:             %s\n", result.cardBrand);
        printf("BIN:                    %s\n", result.bin);
        printf("Holder:                 %s\n", result.holder);
        printf("User reference:         %s\n", result.userReference);
        printf("Terminal serial number: %s\n", result.terminalSerialNumber);
    }

    printf("==================================================\n");
}
