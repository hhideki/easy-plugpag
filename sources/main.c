#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "easy-plugpag.h"

static void printMenu();

static struct PlugPagPaymentData *printPaymentMenu();

static void startPayment(struct PlugPag *pp,
                         struct PlugPagPaymentData *paymentData,
                         stPPPSTransactionResult *result);

static void startVoidPayment(struct PlugPag *pp,
                             stPPPSTransactionResult *result);

static void queryLastSuccessfulTransaction(struct PlugPag *pp,
                                           stPPPSTransactionResult *result);

static void printResult(const int resultCode, const stPPPSTransactionResult result);

static void printGeneralInformation(struct PlugPag *pp);


int main(int argc, char *argv[])
{
    int exit = 0;
    int option = 0;

    struct PlugPagApplicationData *appData = PlugPagApplicationData("easy-plugpag", "1.0.0");
    struct PlugPag *pp = PlugPag("COM0", appData);

    struct PlugPagPaymentData *paymentData = NULL;
    stPPPSTransactionResult result;

    do
    {
        printMenu();
        scanf("%d", &option);

        switch (option)
        {
            case 1:
                paymentData = printPaymentMenu();
                startPayment(pp, paymentData, &result);
                break;

            case 2:
                startVoidPayment(pp, &result);
                break;

            case 3:
                queryLastSuccessfulTransaction(pp, &result);
                break;

            case 4:
                printGeneralInformation(pp);
                break;

            default:
                exit = 1;
                break;
        }
    } while (exit == 0);

    printf("Finalizando aplicacao\n");

    return 0;
}


static void printMenu()
{
    printf("Menu\n");
    printf("\t1. Pagamento\n");
    printf("\t2. Estorno\n");
    printf("\t3. Consultar última transacao\n");
    printf("\t4. Informacoes gerais\n");
    printf("\t0. Sair\n");
}


static struct PlugPagPaymentData *printPaymentMenu()
{
    int paymentMethod = PPPAGSEGURO_CREDIT;
    printf("Pagamento\n");
    printf("\tMétodo\n");
    printf("\t\t1. Credito\n");
    printf("\t\t2. Debito\n");
    printf("\t\t3. Voucher\n");
    scanf("%d", &paymentMethod);

    int installmentType = PPPAGSEGURO_A_VISTA;

    if (paymentMethod == PPPAGSEGURO_CREDIT)
    {
        printf("\tParcelamento\n");
        printf("\t\t1. A vista\n");
        printf("\t\t2. Parcelado vendedor\n");
        scanf("%d", &installmentType);
    }

    int installments = 1;

    if (installmentType != PPPAGSEGURO_A_VISTA)
    {
        printf("\tParcelas: ");
        scanf("%d", &installments);
    }

    printf("\tValor (apenas numeros): ");
    tyAmount amount;
    memset(amount, 0, sizeof(amount));
    scanf("%s", amount);

    printf("\tCodigo de vendas: ");
    tyUserReference userReference;
    memset(userReference, 0, sizeof(userReference));
    scanf("%s", userReference);

    return PlugPagPaymentData(paymentMethod, installmentType, installments, amount, userReference);
}


static void startPayment(struct PlugPag *pp,
                         struct PlugPagPaymentData *paymentData,
                         stPPPSTransactionResult *result)
{
    memset(result, 0, sizeof(stPPPSTransactionResult));

    printf("Starting payment\n");
    int ret = pp->pay(pp, paymentData, result);
    printf("Payment finished\n");
    printResult(ret, *result);
}


static void startVoidPayment(struct PlugPag *pp,
                             stPPPSTransactionResult *result)
{
    memset(result, 0, sizeof(stPPPSTransactionResult));

    printf("Starting void payment\n");
    int ret = pp->voidPayment(pp, result);
    printf("Void payment finished\n");
    printResult(ret, *result);
}

static void queryLastSuccessfulTransaction(struct PlugPag *pp,
                                           stPPPSTransactionResult *result)
{
    memset(result, 0, sizeof(stPPPSTransactionResult));

    printf("Querying last successful transaction\n");
    int ret = pp->getLastApprovedTransaction(pp, result);
    printf("Querying finished\n");
    printResult(ret, *result);
}


static void printResult(const int resultCode, const stPPPSTransactionResult result)
{
    printf("========================================================\n");
    printf("Result code: %d\n", resultCode);
    printf("Message:     %s\n", result.message);

    if (resultCode == PPPS_RET_OK)
    {
        printf("========================================================\n");
        printf("Date and time:          %s %s\n", result.date, result.time);
        printf("Transaction code:       %s\n", result.transactionCode);
        printf("Host NSU:               %s\n", result.hostNsu);
        printf("Card brand:             %s\n", result.cardBrand);
        printf("BIN:                    %s\n", result.bin);
        printf("Holder:                 %s\n", result.holder);
        printf("User reference:         %s\n", result.userReference);
        printf("Terminal serial number: %s\n", result.terminalSerialNumber);
    }

    printf("========================================================\n");
}


static void printGeneralInformation(struct PlugPag *pp)
{
    char version[50];

    pp->getVersion(pp, version);

    printf("Versao do PlugPag: %s\n", version);
}
