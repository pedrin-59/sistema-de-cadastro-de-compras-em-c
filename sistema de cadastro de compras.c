#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>

/* Apenas pra rodar no VS Code*/
#pragma comment(lib, "user32.lib")


#define ID_PRODUTO 101
#define ID_QTD     102
#define ID_PRECO   103
#define ID_SALVAR  104
#define ID_MOSTRAR 105
#define ID_LIMPAR  106
#define ARQUIVO "aval1234.txt"

HWND campoProduto;
HWND campoQtd;
HWND campoPreco;
HWND campoResultado;
HWND campoLista;

// Ler numeros decimais
int lerDecimal(HWND campo, double *valor) {
  char texto[100];
  char *fim;

  GetWindowTextA(campo, texto, sizeof(texto));

  // Aceitar virgula ou ponto decimal
  for (int i = 0; texto[i]; i++) {
    if (texto[i] == ',')
      texto[i] = '.';
  }

  errno = 0;
  *valor = strtod(texto, &fim);
  if (texto == fim || errno == ERANGE ||
      !isfinite(*valor))
    return 0;
  while (isspace((unsigned char)*fim))
    fim++;
  return *fim == '\0';
}

// Ler quantidade inteira
int lerQuantidade(int *quantidade) {
  char texto[100];
  char *fim;
  long numero;

  GetWindowTextA(campoQtd, texto, sizeof(texto));
  errno = 0;
  numero = strtol(texto, &fim, 10);
  if (texto == fim || errno == ERANGE ||
      numero <= 0 || numero > 1000000)
    return 0;
  while (isspace((unsigned char)*fim))
    fim++;
  if (*fim != '\0')
    return 0;

  *quantidade = (int)numero;
  return 1;
}

// Salvar uma compra
void salvarCompra() {
  char produto[100];
  char mensagem[200];
  int quantidade;
  double preco, total;

  GetWindowTextA(campoProduto, produto, sizeof(produto));
  if (strlen(produto) == 0 ||
      strchr(produto, ';') != NULL ||
      strchr(produto, '\n') != NULL ||
      strchr(produto, '\r') != NULL) {
    MessageBoxA(NULL,
      "Informe um produto valido, sem ponto e virgula.",
      "Erro", MB_OK | MB_ICONERROR);
    return;
  }

  if (!lerQuantidade(&quantidade)) {
    MessageBoxA(NULL,
      "Digite uma quantidade inteira positiva!",
      "Erro", MB_OK | MB_ICONERROR);
    return;
  }

  if (!lerDecimal(campoPreco, &preco) ||
      preco < 0) {
    MessageBoxA(NULL,
      "Digite um preco valido!",
      "Erro", MB_OK | MB_ICONERROR);
    return;
  }

  total = quantidade * preco;
  if (!isfinite(total)) {
    MessageBoxA(NULL,
      "Total fora do intervalo permitido.",
      "Erro", MB_OK | MB_ICONERROR);
    return;
  }

  FILE *arquivo = fopen(ARQUIVO, "a");
  if (arquivo == NULL) {
    MessageBoxA(NULL,
      "Nao foi possivel abrir o arquivo!",
      "Erro", MB_OK | MB_ICONERROR);
    return;
  }

  // Gravar os dados separados por ;
  int gravou = fprintf(arquivo,
    "%s;%d;%.2f;%.2f\n",
    produto, quantidade, preco, total);

  int erro = ferror(arquivo);
  int fechou = fclose(arquivo);
  if (gravou < 0 || erro || fechou != 0) {
    MessageBoxA(NULL,
      "Erro ao gravar a compra!",
      "Erro", MB_OK | MB_ICONERROR);
    return;
  }

  snprintf(mensagem, sizeof(mensagem),
    "Total da compra: R$ %.2f", total);
  SetWindowTextA(campoResultado, mensagem);
  MessageBoxA(NULL,
    "Compra cadastrada com sucesso!",
    "Sucesso", MB_OK | MB_ICONINFORMATION);
}

// Mostrar todas as compras do arquivo
void mostrarCompras() {
  FILE *arquivo = fopen(ARQUIVO, "r");
  if (arquivo == NULL) {
    MessageBoxA(NULL,
      "Nenhuma compra cadastrada!",
      "Aviso", MB_OK | MB_ICONINFORMATION);
    return;
  }

  char linha[300];
  char produto[100];
  char texto[500];
  int quantidade;
  double preco, total;
  double acumulado = 0;
  int contador = 0;

  // Limpar a lista
  SendMessageA(campoLista, LB_RESETCONTENT, 0, 0);

  while (fgets(linha, sizeof(linha), arquivo)) {
    if (sscanf(linha,
        "%99[^;];%d;%lf;%lf",
        produto, &quantidade,
        &preco, &total) == 4) {

      snprintf(texto, sizeof(texto),
        "%s | Qtd: %d | Unit: %.2f | Total: %.2f",
        produto, quantidade, preco, total);

      SendMessageA(campoLista,
        LB_ADDSTRING, 0, (LPARAM)texto);
      acumulado += total;
      contador++;
    }
  }
  fclose(arquivo);

  snprintf(texto, sizeof(texto),
    "TOTAL GERAL: R$ %.2f | %d compras",
    acumulado, contador);
  SetWindowTextA(campoResultado, texto);
}

// Limpar campos
void limparCampos() {
  SetWindowTextA(campoProduto, "");
  SetWindowTextA(campoQtd, "");
  SetWindowTextA(campoPreco, "");
  SetWindowTextA(campoResultado, "Total: R$ 0,00");
  SendMessageA(campoLista, LB_RESETCONTENT, 0, 0);
}

// Funcao principal da janela
LRESULT CALLBACK Janela(
  HWND hwnd, UINT msg,
  WPARAM wParam, LPARAM lParam) {

  switch (msg) {
  case WM_CREATE:
    CreateWindowA("STATIC", "Produto:",
      WS_VISIBLE | WS_CHILD,
      20, 20, 100, 25,
      hwnd, NULL, NULL, NULL);
    campoProduto = CreateWindowA("EDIT", "",
      WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
      130, 20, 300, 25,
      hwnd, (HMENU)ID_PRODUTO, NULL, NULL);

    CreateWindowA("STATIC", "Quantidade:",
      WS_VISIBLE | WS_CHILD,
      20, 60, 100, 25,
      hwnd, NULL, NULL, NULL);
    campoQtd = CreateWindowA("EDIT", "",
      WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
      130, 60, 120, 25,
      hwnd, (HMENU)ID_QTD, NULL, NULL);

    CreateWindowA("STATIC", "Preco unitario:",
      WS_VISIBLE | WS_CHILD,
      20, 100, 110, 25,
      hwnd, NULL, NULL, NULL);
    campoPreco = CreateWindowA("EDIT", "",
      WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
      130, 100, 120, 25,
      hwnd, (HMENU)ID_PRECO, NULL, NULL);

    CreateWindowA("BUTTON", "Salvar compra",
      WS_VISIBLE | WS_CHILD,
      20, 150, 130, 35,
      hwnd, (HMENU)ID_SALVAR, NULL, NULL);
    CreateWindowA("BUTTON", "Mostrar compras",
      WS_VISIBLE | WS_CHILD,
      160, 150, 140, 35,
      hwnd, (HMENU)ID_MOSTRAR, NULL, NULL);
    CreateWindowA("BUTTON", "Limpar",
      WS_VISIBLE | WS_CHILD,
      310, 150, 120, 35,
      hwnd, (HMENU)ID_LIMPAR, NULL, NULL);

    campoResultado = CreateWindowA(
      "STATIC", "Total: R$ 0,00",
      WS_VISIBLE | WS_CHILD,
      20, 205, 430, 30,
      hwnd, NULL, NULL, NULL);

    CreateWindowA("STATIC",
      "Compras cadastradas no arquivo:",
      WS_VISIBLE | WS_CHILD,
      20, 245, 400, 25,
      hwnd, NULL, NULL, NULL);

    campoLista = CreateWindowA("LISTBOX", "",
      WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL |
      LBS_NOTIFY,
      20, 275, 430, 200,
      hwnd, NULL, NULL, NULL);
    break;

  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case ID_SALVAR:
      salvarCompra();
      break;
    case ID_MOSTRAR:
      mostrarCompras();
      break;
    case ID_LIMPAR:
      limparCampos();
      break;
    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProcA(hwnd, msg, wParam, lParam);
  }

  return 0;
}

// Funcao principal do Windows
int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow) {

  WNDCLASSA wc = {0};
  wc.lpfnWndProc   = Janela;
  wc.hInstance     = hInstance;
  wc.lpszClassName = "CadastroCompras";
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

  if (!RegisterClassA(&wc))
    return 1;

  HWND hwnd = CreateWindowA(
    "CadastroCompras",
    "Sistema de Cadastro de Compras em C",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    500, 550,
    NULL, NULL, hInstance, NULL);
  if (!hwnd)
    return 1;

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}