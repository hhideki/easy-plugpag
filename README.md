# easy-plugpag

Biblioteca de suporte para facilitar o uso da biblioteca PlugPag oficial.

A biblioteca PlugPag oficial ainda é necessária para compilar seu código final.


## Preparação

Antes de executar o programa de teste, você deve garantir que pelo menos um
terminal do PagSeguro está pareado com seu computador (ou sua Raspberry Pi) e
que o terminal está pronto para ser utilizado.

Para ajudar na preparação do terminal, eu criei o script `bind-bluetooth.sh`.

Antes de executar esse script, é necessário que o terminal esteja pareado
corretamente.

Depois que o terminal estiver pareado, execute o seguinte comando:

`sudo bind-bluetooh.sh <device-id>`

Onde `<device-id>` é o nome do seu terminal, por exemplo *PRO-12345678* ou
*PLUS-12345678*.

É necessário executar esse script com permissões elevadas pois ele executa o
comando `rfcomm bind`.


## Compilando

Para rodar um programa de teste, basta executar o seguinte comando:

`make all`

Esse comando irá limpar arquivos de cache, compilar o código, linkar o código,
exportar uma biblioteca de linkagen dinâmica e executar o programa de teste.


## Por que usar uma biblioteca a mais?

Para utilizar a biblioteca *easy-plugpag* é necessário incluir também as
bibliotecas oficiais do PlugPag.

A vantagem do *easy-plugpag* é apenas do ponto de vista de organização do
código.

O exemplo abaixo foi extraído do programa de teste:

```
    stPPPSTransactionResult result;
    memset(&result, 0, sizeof(stPPPSTransactionResult));

    struct PlugPagApplicationData *appData = PlugPagApplicationData("easy-plugpag", "1.0.0");
    struct PlugPagPaymentData *paymentData = PlugPagPaymentData(PPPAGSEGURO_CREDIT, PPPAGSEGURO_A_VISTA, 1, "1234578", "easypp");
    struct PlugPag *pp = PlugPag("COM0", appData);
    pp->pay(pp, paymentData, &result);
```

Dessa forma, as informações ficam bem separadas, tornando fácil a visualização
de quais dados são refentes à aplicação, ao pagamento ou ao terminal que será
utilizado.


## Como utilizar

1. Execute o comando `make clean export`

2. Copie os arquivos do diretório *exported* para o seu projeto

4. Altere o script de compilação do seu projeto para incluir a biblioteca
*easy-plugpag.so* e para utilizar o diretório no qual o *easy-plugpag.h* foi
copiado.

Um exemplo de como incluir utilizar bibliotecas e header files de outras
bibliotecas pode ser extraído do *Makefile* desse projeto.

Se estiver utilizando o *gcc*, adicione a flag `-I<inc_dir>` no momento de
compilar o código e a flag `-L<lib_dir>` no momento de linkar os objetos
intermediários.

`<inc_dir>` é o diretório no qual o *easy-plugpag.h* foi copiado.

`<lib_dir>` é o diretório no qual o *easy-plugpag.so* foi copiado.


## Exemplos

### Pagamento

```
    stPPPSTransactionResult result;
    memset(&result, 0, sizeof(stPPPSTransactionResult));

    struct PlugPagApplicationData *appData = PlugPagApplicationData("easy-plugpag", "1.0.0");
    struct PlugPag *pp = PlugPag("COM0", appData);
    struct PlugPagPaymentData *paymentData = PlugPagPaymentData(PPPAGSEGURO_CREDIT, PPPAGSEGURO_A_VISTA, 1, "1234578", "easypp");
    pp->pay(pp, paymentData, &result);
```

### Estorno

```
    stPPPSTransactionResult result;
    memset(&result, 0, sizeof(stPPPSTransactionResult));

    struct PlugPagApplicationData *appData = PlugPagApplicationData("easy-plugpag", "1.0.0");
    struct PlugPag *pp = PlugPag("COM0", appData);
    pp->voidPayment(pp, &result);
```

### Consulta da última transação efetuada com sucesso

```
    stPPPSTransactionResult result;
    memset(&result, 0, sizeof(stPPPSTransactionResult));

    struct PlugPagApplicationData *appData = PlugPagApplicationData("easy-plugpag", "1.0.0");
    struct PlugPag *pp = PlugPag("COM0", appData);
    pp->getLastApprovedTransaction(pp, &result);
```

### Verificando qual é a versão do PlugPag que está sendo utilizada

```
    struct PlugPagApplicationData *appData = PlugPagApplicationData("easy-plugpag", "1.0.0");
    struct PlugPag *pp = PlugPag("COM0", appData);

    char plugpagVersion[10];
    pp->getVersion(pp, plugpagVersion);
```

### Liberando memória

```
    struct PlugPagApplicationData *appData = PlugPagApplicationData("easy-plugpag", "1.0.0");
    struct PlugPag *pp = PlugPag("COM0", appData);

    pp->release(pp);
    free(pp);
    free(appData);
```
