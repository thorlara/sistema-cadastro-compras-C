#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ID_PRODUTO 101
#define ID_QTD 102
#define ID_PRECO 103
#define ID_SALVAR 104
#define ID_MOSTRAR 105
#define ID_LIMPAR 106
#define ID_LISTA 107
#define ID_TOTAL 108

#define ARQUIVO "aval1234.txt"

HWND campoProduto, campoQtd, campoPreco, campoLista, textoTotal;
double acumulado = 0.0;
int contador = 0;

int lerInteiroPositivo(HWND campo, int *valor) {
    char texto[64], *fim;
    long n;
    GetWindowTextA(campo, texto, sizeof(texto));
    if (texto[0] == '\0') return 0;
    errno = 0;
    n = strtol(texto, &fim, 10);
    if (fim == texto || *fim != '\0' || errno == ERANGE || n <= 0 || n > 1000000)
        return 0;
    *valor = (int)n;
    return 1;
}

int lerPrecoPositivo(HWND campo, double *valor) {
    char texto[64], *fim;
    double n;
    GetWindowTextA(campo, texto, sizeof(texto));
    if (texto[0] == '\0') return 0;
    /* Aceita vírgula ou ponto como separador decimal. */
    for (int i = 0; texto[i]; i++) if (texto[i] == ',') texto[i] = '.';
    errno = 0;
    n = strtod(texto, &fim);
    if (fim == texto || *fim != '\0' || errno == ERANGE || n <= 0.0 || n > 1000000.0)
        return 0;
    *valor = n;
    return 1;
}

void limparCampos(void) {
    SetWindowTextA(campoProduto, "");
    SetWindowTextA(campoQtd, "");
    SetWindowTextA(campoPreco, "");
    SetFocus(campoProduto);
}

void atualizarTotal(void) {
    char texto[128];
    snprintf(texto, sizeof(texto), "Total acumulado: R$ %.2f | Compras: %d", acumulado, contador);
    SetWindowTextA(textoTotal, texto);
}

void salvarCompra(HWND janela) {
    char produto[100];
    int quantidade;
    double preco, total;
    FILE *arquivo;

    GetWindowTextA(campoProduto, produto, sizeof(produto));
    if (produto[0] == '\0') {
        MessageBoxA(janela, "Informe o nome do produto.", "Dados inválidos", MB_OK | MB_ICONWARNING);
        return;
    }
    if (strchr(produto, ';') || strchr(produto, '\n') || strchr(produto, '\r')) {
        MessageBoxA(janela, "O nome do produto não pode conter ponto e vírgula ou quebra de linha.", "Dados inválidos", MB_OK | MB_ICONWARNING);
        return;
    }
    if (!lerInteiroPositivo(campoQtd, &quantidade)) {
        MessageBoxA(janela, "Informe uma quantidade inteira entre 1 e 1000000.", "Dados inválidos", MB_OK | MB_ICONWARNING);
        return;
    }
    if (!lerPrecoPositivo(campoPreco, &preco)) {
        MessageBoxA(janela, "Informe um preço maior que zero e até R$ 1.000.000,00.", "Dados inválidos", MB_OK | MB_ICONWARNING);
        return;
    }

    total = quantidade * preco;
    arquivo = fopen(ARQUIVO, "a");
    if (!arquivo) {
        MessageBoxA(janela, "Não foi possível abrir o arquivo para gravação.", "Erro", MB_OK | MB_ICONERROR);
        return;
    }
    fprintf(arquivo, "%s;%d;%.2f;%.2f\n", produto, quantidade, preco, total);
    fclose(arquivo);

    MessageBoxA(janela, "Compra salva com sucesso!", "Confirmação", MB_OK | MB_ICONINFORMATION);
    limparCampos();
}

void mostrarCompras(HWND janela) {
    FILE *arquivo = fopen(ARQUIVO, "r");
    char linha[256], produto[100], exibicao[256];
    int quantidade, lidos;
    double preco, total;

    SendMessageA(campoLista, LB_RESETCONTENT, 0, 0);
    acumulado = 0.0;
    contador = 0;

    if (!arquivo) {
        MessageBoxA(janela, "Ainda não existe arquivo de compras. Cadastre uma compra primeiro.", "Aviso", MB_OK | MB_ICONWARNING);
        atualizarTotal();
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        lidos = sscanf(linha, "%99[^;];%d;%lf;%lf", produto, &quantidade, &preco, &total);
        if (lidos == 4) {
            snprintf(exibicao, sizeof(exibicao),
                     "%s | Quantidade: %d | Unitário: R$ %.2f | Total: R$ %.2f",
                     produto, quantidade, preco, total);
            SendMessageA(campoLista, LB_ADDSTRING, 0, (LPARAM)exibicao);
            acumulado += total;
            contador++;
        }
    }
    fclose(arquivo);
    atualizarTotal();
}

LRESULT CALLBACK procedimentoJanela(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam) {
    switch (mensagem) {
    case WM_CREATE:
        CreateWindowA("STATIC", "Produto:", WS_VISIBLE | WS_CHILD, 20, 22, 90, 24, janela, NULL, NULL, NULL);
        campoProduto = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                     110, 18, 300, 26, janela, (HMENU)ID_PRODUTO, NULL, NULL);

        CreateWindowA("STATIC", "Quantidade:", WS_VISIBLE | WS_CHILD, 20, 62, 90, 24, janela, NULL, NULL, NULL);
        campoQtd = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                 110, 58, 120, 26, janela, (HMENU)ID_QTD, NULL, NULL);

        CreateWindowA("STATIC", "Preço (R$):", WS_VISIBLE | WS_CHILD, 250, 62, 90, 24, janela, NULL, NULL, NULL);
        campoPreco = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                   340, 58, 120, 26, janela, (HMENU)ID_PRECO, NULL, NULL);

        CreateWindowA("BUTTON", "Salvar compra", WS_VISIBLE | WS_CHILD,
                      20, 105, 130, 32, janela, (HMENU)ID_SALVAR, NULL, NULL);
        CreateWindowA("BUTTON", "Mostrar compras", WS_VISIBLE | WS_CHILD,
                      165, 105, 145, 32, janela, (HMENU)ID_MOSTRAR, NULL, NULL);
        CreateWindowA("BUTTON", "Limpar", WS_VISIBLE | WS_CHILD,
                      325, 105, 90, 32, janela, (HMENU)ID_LIMPAR, NULL, NULL);

        campoLista = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER |
                                   LBS_NOTIFY | WS_VSCROLL,
                                   20, 155, 440, 210, janela, (HMENU)ID_LISTA, NULL, NULL);
        textoTotal = CreateWindowA("STATIC", "Total acumulado: R$ 0.00 | Compras: 0",
                                   WS_VISIBLE | WS_CHILD, 20, 380, 440, 28, janela,
                                   (HMENU)ID_TOTAL, NULL, NULL);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SALVAR:
            salvarCompra(janela);
            return 0;
        case ID_MOSTRAR:
            mostrarCompras(janela);
            return 0;
        case ID_LIMPAR:
            limparCampos();
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(janela, mensagem, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE anterior, LPSTR linhaComando, int mostrar) {
    const char CLASSE[] = "JanelaControleCompras";
    WNDCLASSA wc = {0};
    HWND janela;
    MSG msg;

    wc.lpfnWndProc = procedimentoJanela;
    wc.hInstance = instancia;
    wc.lpszClassName = CLASSE;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Falha ao registrar a classe da janela.", "Erro", MB_OK | MB_ICONERROR);
        return 1;
    }

    janela = CreateWindowA(CLASSE, "Sistema de Cadastro de Compras",
                           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                           510, 470, NULL, NULL, instancia, NULL);
    if (!janela) {
        MessageBoxA(NULL, "Falha ao criar a janela.", "Erro", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(janela, mostrar);
    UpdateWindow(janela);

    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}
