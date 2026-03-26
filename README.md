# Projeto 1 – Processamento de Imagens

**Disciplina:** Computação Visual  
**Universidade:** Universidade Presbiteriana Mackenzie  
**Professor:** André Kishimoto

---

## Descrição

Software de processamento de imagens desenvolvido em **linguagem C (C99)**, utilizando as bibliotecas **SDL3**, **SDL3\_image** e **SDL3\_ttf**.

O programa carrega uma imagem fornecida via linha de comando, converte-a para escala de cinza, exibe seu histograma e permite equalizá-lo interativamente por meio de uma interface gráfica com duas janelas.

---

## Funcionalidades

| # | Funcionalidade | Descrição |
|---|---------------|-----------|
| 1 | Carregamento de imagem | Suporta PNG, JPG e BMP; trata erros de arquivo inválido ou não encontrado |
| 2 | Conversão para escala de cinza | Detecta automaticamente imagens coloridas e aplica `Y = 0.2125·R + 0.7154·G + 0.0721·B` |
| 3 | Interface com duas janelas | Janela principal (imagem) + janela secundária (histograma e botão) |
| 4 | Histograma | Exibição proporcional com análise de brilho (clara/média/escura) e contraste (alto/médio/baixo) |
| 5 | Equalização do histograma | Botão toggle que equaliza/reverte a imagem sem recarregá-la |
| 6 | Salvar imagem | Tecla **S** salva a imagem atual como `output_image.png` |

---

## Estrutura do projeto

```
Proj1/
├── Makefile
├── README.md
└── src/
    ├── main.c              # Ponto de entrada; inicialização e fluxo principal
    ├── types.h             # Tipos e constantes compartilhados (AppState)
    ├── image_processing.h  # Cabeçalho: carregamento, conversão, equalização, salvar
    ├── image_processing.c  # Implementação das operações de imagem
    ├── histogram.h         # Cabeçalho: cálculo e análise de histograma
    ├── histogram.c         # Implementação do histograma
    ├── gui.h               # Cabeçalho: criação de janelas, loop de eventos, renderização
    ├── gui.c               # Implementação da GUI (SDL3)
    └── utils.h             # Utilitários inline (clamp, max)
```

---

## Como compilar

### Pré-requisitos

- **gcc** ≥ 15.1.0 (ou compatível com C99)
- **SDL3** instalado no sistema
- **SDL3\_image** instalado no sistema
- **SDL3\_ttf** instalado no sistema

#### Instalação das dependências (Ubuntu/Debian)

As bibliotecas SDL3 podem precisar ser instaladas manualmente (ainda não disponíveis nos repositórios padrão em algumas distribuições). Consulte [https://github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL) e os repositórios correspondentes de SDL\_image e SDL\_ttf.

```bash
# Exemplo genérico após instalar SDL3 via build/install:
sudo apt-get install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev
```

### Compilação

```bash
make
```

O executável `programa` será gerado na raiz do projeto. Os objetos intermediários ficam em `build/` (**não versionados**).

Para limpar:

```bash
make clean
```

---

## Como executar

```bash
./programa caminho_da_imagem.ext
```

**Exemplos:**

```bash
./programa foto.jpg
./programa imagens/paisagem.png
./programa teste.bmp
```

---

## Uso interativo

| Ação | Efeito |
|------|--------|
| Botão **"Equalizar"** (janela secundária) | Equaliza o histograma; imagem principal é atualizada |
| Botão **"Ver original"** (mesmo botão) | Reverte para a imagem em escala de cinza sem recarregar |
| Tecla **S** | Salva a imagem exibida no momento como `output_image.png` |
| Tecla **ESC** / fechar janela | Encerra o programa |

### Estados visuais do botão

| Estado | Cor |
|--------|-----|
| Normal | Azul |
| Mouse sobre o botão | Azul claro |
| Clicado (pressionado) | Azul escuro |

---

## Detalhes técnicos

### Conversão para escala de cinza

```
Y = 0.2125 * R + 0.7154 * G + 0.0721 * B
```

O programa verifica previamente se a imagem já está em escala de cinza (R == G == B em todos os pixels) e, nesse caso, apenas copia os valores sem reaplicar a fórmula.

### Equalização do histograma

Implementada via **CDF (Cumulative Distribution Function)**:

```
lut[i] = round( (CDF[i] - CDF_min) / (1 - CDF_min) * 255 )
```

A reversão é feita mantendo a superfície original em cinza na memória; nenhum recarregamento de arquivo é necessário.

### Análise do histograma

| Métrica | Classificação |
|---------|--------------|
| Média < 85 | Escura |
| 85 ≤ Média < 170 | Média |
| Média ≥ 170 | Clara |
| Desvio padrão < 40 | Contraste baixo |
| 40 ≤ DP < 80 | Contraste médio |
| DP ≥ 80 | Contraste alto |

---

## Contribuições dos integrantes

| Integrante | RA | Contribuições |
|------------|-----|---------------|
| Integrante 1 | XXXXXXX | Carregamento de imagem, conversão para escala de cinza, equalização do histograma |
| Integrante 2 | XXXXXXX | Interface gráfica (GUI), loop de eventos, renderização do histograma, botão |

> **Atenção:** Preencha a tabela acima com os nomes, RAs e contribuições reais do grupo antes da entrega.

---

## Observações

- Os arquivos intermediários de compilação (pasta `build/`) **não são versionados** (ver `.gitignore`).
- O arquivo `output_image.png` gerado pelo programa também **não é versionado**.
- A fonte utilizada para texto na janela secundária é a **DejaVu Sans**, normalmente presente em sistemas Linux. Caso não esteja disponível, o programa continua funcionando sem exibir textos analíticos.
