# Trabalho de Fundamentos de Computação Gráfica  

Projeto de Regressão Linear e TensorFlow/Keras

## Integrantes do Grupo:

- **Douglas Ardenghi Schlatter**
- **Matheus Rodrigues Fonseca** 



    ## Parágrafo listando as contribuição de cada membro da dupla para o trabalho;

    Matheus foi responsável por fazer os jogos **Desvie da Capivara** e **Capivara Impostora**, também fez a lógica de troca de jogos e 

    Douglas foi respo 

    ## Parágrafo curto indicando se a dupla fez uso do ChatGPT (ou alguma outra ferramenta similar, como Github Copilot, OpenAI Codex, etc.) para desenvolvimento do trabalho, descrevendo como a ferramenta foi utilizada e para quais partes do trabalho. O parágrafo deve também incluir uma análise crítica descrevendo quão útil a dupla achou a ferramenta, onde ela auxiliou e onde ela não auxiliou adequadamente;

    Não foi utilizado ChatGPT ou qualquer recurso de inteligência artificial para esse projeto.

    ## Descrição do processo de desenvolvimento e do uso em sua aplicação dos conceitos de Computação Gráfica estudados e listados nos requisitos acima;

    Essa aplicação consiste em 3 jogos curtos com temática de capivaras, cada um com um critério de vitória e derrota. Ao terminar um dos jogos, é iniciado outro jogo:

    ### Desvie da capivara

    ![Alt text](/images%20app/Desvie.jpg)

    Você e uma capivara estão em uma sala, ela vai te perseguir, o objetivo é você fugir dela até o tempo acabar. Se ela te encostar você perde.
    Você pode se movimentar com as teclas W,A,S,D.

    Nesse jogo, a colisão entre o jogador e as paredes e colisão entre a capivara e a parede é feita por um check Box - Plane. A colisão entre a capivara e o jogador é feita com um check Box - Box.
    A capivara utiliza um sistema de iluminação Blinn-Phong e as paredes possuem iluminação difusa.

    ### Capivara impostora

    ![Alt text](/images%20app/sus.jpg)

    4 capivaras estão girando na tela, mas uma está diferente. Utilize as teclas W,A,S,D para mover a luz e observar mais atentamente as capivaras. Clique com a barra de espaço com a luz sobre a capivara que você acha que é a impostora para selecionar ela. Se você selecionar a capivara certa você ganha. Se você selecionar a errada ou o tempo acabar, você perde.

    ### Angry Capivara
    
    ![Alt text](/images%20app/atirar.jpg) 

    Existem 4 alvos no mapa, você pode usar as teclas W,A,S,D para andar pelo mapa e clicar a barra de espaço para arremessar uma capivara para frente. O seu objetivo é acertar todos os alvos antes do tempo acabar. Se o tempo acabar você perde.


    ## No mínimo duas imagens mostrando o funcionamento da aplicação;
    
    ## Um manual descrevendo a utilização da aplicação (atalhos de teclado, etc.);

    ## Explicação de todos os passos necessários para compilação e execução da aplicação;

=== Windows
===================================
Para compilar e executar este projeto no Windows, você possui duas
opções para compilação:

--- Windows com Code::Blocks
-------------------------------------------
Baixe a IDE Code::Blocks em http://codeblocks.org/ e abra o arquivo
"Laboratorio_4.cbp".

**ATENÇÃO**: os "Build targets" padrões (Debug e Release) estão configurados
para Code::Blocks versão 20.03 ou superior, que utiliza MinGW 64-bits. Se você
estiver utilizando versões mais antigas do Code::Blocks (17.12 ou anteriores)
você precisa alterar o "Build target" para "Debug (CBlocks 17.12 32-bit)" ou
"Release (CBlocks 17.12 32-bit)" antes de compilar o projeto.

--- Windows com VSCode (Visual Studio Code)
-------------------------------------------
1) Instale o VSCode seguindo as instruções em https://code.visualstudio.com/ .

2) Instale o compilador GCC no Windows seguindo as instruções em
https://code.visualstudio.com/docs/cpp/config-mingw#_installing-the-mingww64-toolchain .

Alternativamente, se você já possui o Code::Blocks instalado no seu PC
(versão que inclui o MinGW), você pode utilizar o GCC que vem com esta
instalação no passo 5.

3) Instale o CMake seguindo as instruções em https://cmake.org/download/ .
Alternativamente, você pode utilizar algum package manager do
Windows para fazer esta instalação, como https://chocolatey.org/ .

4) Instale as extensões "ms-vscode.cpptools" e "ms-vscode.cmake-tools"
no VSCode. Se você abrir o diretório deste projeto no VSCode,
automaticamente será sugerida a instalação destas extensões (pois
estão listadas no arquivo ".vscode/extensions.json").

5) Abra as configurações da extensão cmake-tools (Ctrl-Shift-P e
busque por "CMake: Open CMake Tools Extension Settings"), e adicione o
caminho de instalação do GCC na opção de configuração "additionalCompilerSearchDirs".

Por exemplo, se você quiser utilizar o compilador MinGW que vem junto
com o Code::Blocks, pode preencher o diretório como
"C:\Program Files\CodeBlocks\MinGW\bin" (verifique se este é o local
de instalação do seu Code::Blocks).

6) Clique no botão de "Play" na barra inferior do VSCode para compilar
e executar o projeto. Na primeira compilação, a extensão do CMake para
o VSCode irá perguntar qual compilador você quer utilizar. Selecione
da lista o compilador GCC que você instalou com o MSYS/MinGW.

Veja mais instruções de uso do CMake no VSCode em:

https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md

=== Linux
===================================
Para compilar e executar este projeto no Linux, primeiro você precisa instalar
as bibliotecas necessárias. Para tanto, execute o comando abaixo em um terminal.
Esse é normalmente suficiente em uma instalação de Linux Ubuntu:

    sudo apt-get install build-essential make libx11-dev libxrandr-dev \
                         libxinerama-dev libxcursor-dev libxcb1-dev libxext-dev \
                         libxrender-dev libxfixes-dev libxau-dev libxdmcp-dev

Se você usa Linux Mint, talvez seja necessário instalar mais algumas bibliotecas:

    sudo apt-get install libmesa-dev libxxf86vm-dev

Após a instalação das bibliotecas acima, você possui várias opções para compilação:

--- Linux com Makefile
-------------------------------------------
Abra um terminal, navegue até a pasta "Laboratorio_0X_Codigo_Fonte", e execute
o comando "make" para compilar. Para executar o código compilado, execute o
comando "make run".

--- Linux com CMake
-------------------------------------------
Abra um terminal, navegue até a pasta "Laboratorio_0X_Codigo_Fonte", e execute
os seguintes comandos:

    mkdir build  # Cria diretório de build
    cd build     # Entra no diretório
    cmake ..     # Realiza a configuração do projeto com o CMake
    make         # Realiza a compilação
    make run     # Executa o código compilado

--- Linux com VSCode
-------------------------------------------

1) Instale o VSCode seguindo as instruções em https://code.visualstudio.com/ .

2) Instale as extensões "ms-vscode.cpptools" e "ms-vscode.cmake-tools"
no VSCode. Se você abrir o diretório deste projeto no VSCode,
automaticamente será sugerida a instalação destas extensões (pois
estão listadas no arquivo ".vscode/extensions.json").

3) Clique no botão de "Play" na barra inferior do VSCode para compilar
e executar o projeto. Na primeira compilação, a extensão do CMake para
o VSCode irá perguntar qual compilador você quer utilizar. Selecione
da lista o compilador que você deseja utilizar.

Veja mais instruções de uso do CMake no VSCode em:

https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md

--- Linux com Code::Blocks
-------------------------------------------
Instale a IDE Code::Blocks (versão Linux em http://codeblocks.org/), abra o
arquivo "Laboratorio_4.cbp", e modifique o "Build target" de "Debug" para "Linux".

=== macOS
===================================
Para compilar e executar esse projeto no macOS, primeiro você precisa instalar o
HOMEBREW, um gerenciador de pacotes para facilitar a instação de bibliotecas. O
HOMEBREW pode ser instalado com o seguinte comando no terminal:

    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

Após a instalação do HOMEBREW, a biblioteca GLFW deve ser instalada. Isso pode
ser feito pelo terminal com o comando:

    brew install glfw

--- macOS com Makefile
-------------------------------------------
Abra um terminal, navegue até a pasta "TrabalhoFCG_Douglas_Matheus", e execute
o comando "make -f Makefile.macOS" para compilar. Para executar o código
compilado, execute o comando "make -f Makefile.macOS run".

Observação: a versão atual da IDE Code::Blocks é bastante desatualizada pra o
macOS. A nota oficial dos desenvolvedores é: "Code::Blocks 17.12 for Mac is
currently not available due to the lack of Mac developers, or developers that
own a Mac. We could use an extra Mac developer (or two) to work on Mac
compatibility issues."

=== Soluções de Problemas
===================================

Caso você tenha problemas em executar o código deste projeto, tente atualizar o
driver da sua placa de vídeo.



# Avaliação Tic-Tac-Toe Misere:
 - **(i) O minimax sempre ganha do randomplayer?**

 - **(ii) O minimax sempre empata consigo mesmo?**
  
 - **(iii) O minimax sempre empata contra as jogadas perfeitas recomendadas pelo
   https://nyc.cs.berkeley.edu/uni/games/ttt/variants/misere ? Para verificar isso, use o
   humanplayer. No link, faça as jogadas do minimax, e no servidor do kit, faça as
   jogadas recomendadas (amarelo ou verde) do link.**

Avaliação Othello:
(i) Represente em uma matriz de 3 X 3 onde as linhas representam o jogador que inicia
(player 1) e as colunas representam o player 2 e em cada célula, indique se a partida
resultou em vitória (1), derrota (-1) ou empate (0) entre os agentes com cada uma das
heurísticas.

Heuristica_count
Heuristica_positon
Heuristica_custom


(ii) Observe e relate qual implementação foi a mais bem-sucedida.



(iii) Reflita sobre o que pode ter tornado cada heurística melhor ou pior, em termos de
performance.


| Dataset       | Tempo (segundos)     | Acurácia  |
|---------------|------------|-----------|
| mnist         | 63.32      | 99.3%     |
| fashion_mnist | 323.33     | 89.8%     |
| cifar10       | 1678.94    | 67.9%     |
| cifar100      | 756.69     | 38.3%     |


# Kit othello
Kit para executar partidas de Othello e Jogo da Velha invertido (Tic-Tac-Toe Misere) e implementar algoritmos de busca com adversário.

## Conteudo
O kit contém os seguintes arquivos (todos os `__init__.py` estao omitidos):

```text
kit_games
├── server.py              <-- servidor de jogos
├── test_minimax_tttm.py        <-- teste da poda alfa-beta no tic-tac-toe misere
├── test_othello_evaluations.py <-- teste das funcoes de avaliacao do othello p/ a poda alfa-beta
├── test_pruning.py             <-- teste da poda alfa-beta em um jogo simplificado
└── advsearch
    ├── othello
    |   ├── board.py       <-- encapsula o tabuleiro do othello
    |   └── gamestate.py   <-- encapsula um estado do othello (config. do tabuleiro e cor que joga)
    ├── tttm
    |   ├── board.py       <-- encapsula o tabuleiro do tic-tac-toe misere
    |   └── gamestate.py   <-- encapsula um estado do tic-tac-toe-misere (config. do tabuleiro e cor que joga)
    ├── randomplayer
    |   └── agent.py       <-- agente que joga aleatoriamente
    ├── humanplayer        
    |   └── agent.py       <-- agente para um humano jogar 
    ├── timer.py           <-- funcoes auxiliares de temporizacao
    └── your_agent         <-- renomeie este diretorio c/ o nome do seu agente 
      ├── minimax.py      <-- implemente a poda alfa-beta aqui
      ├── othello_minimax_count.py  <-- chame seu minimax com a heuristica de contagem 
      ├── othello_minimax_mask.py   <-- chame seu minimax com a heuristica posicional 
      ├── othello_minimax_custom.py <-- chame seu minimax com uma heuristica customizada
      ├── tttm_minimax.py           <-- chame seu minimax sem limite de profundidade aqui
      └── [vc pode adicionar outros arquivos e subdiretorios aqui]
```

## Requisitos 
O servidor foi testado em uma máquina GNU/Linux com o interpretador python 3.9.7.

Outras versões do interpretador python ou sistema operacional podem funcionar, mas não foram testados.

## Instruções

Para iniciar uma partida, digite no terminal:

`python server.py game player1 player 2 [-h] [-d delay] [-p pace]  [-o output-file] [-l log-history]`

Nos parâmetros, game é o jogo a ser jogado (othello ou tttm para tic-tac-toe misere)  'player(1 ou 2)' são o caminhos dentro de `advsearch` onde estão implementados os make_move dos jogadores.

Somente para Othello: Para ver o tabuleiro e as peças com cores, instale a biblioteca `pytermgui` (por exemplo, com `pip install pytermgui`) e execute o `server_tui.py` ao invés do `server.py`.


Os argumentos entre colchetes são opcionais, seu significado é descrito a seguir:
```text
-h, --help            Mensagem de ajuda
-d delay, --delay delay
                    Tempo alocado para os jogadores realizarem a jogada (default=5s)
-p pace, --pace pace
                    Tempo mínimo que o servidor espera para processar a jogada (para poder ver partidas muito 
                    rapidas sem se perder no terminal)
-l log-history, --log-history log-history
                    Arquivo para o log do jogo (default=history.txt)
-o output-file, --output-file output-file
                    Arquivo de saida com os detalhes do jogo (inclui historico)
```

O jogador 'random' se localiza em `advsearch/randomplayer/agent.py`. Para jogar uma partida com ele,
basta substituir player1 ou 2 por esse caminho. Como exemplo, inicie
uma partida random vs. random de othello para ver o servidor funcionando:

`python server.py othello advsearch/randomplayer/agent.py advsearch/randomplayer/agent.py -d 1 -p 0.3`

O delay pode ser de 1 segundo porque o jogador random é muito rápido (e muito incompetente). O passo é de 0.3 segundos para acompanhar o progresso da partida (pode acelerar ou reduzir conforme a necessidade).

O jogador 'human' se localiza em `advsearch/humanplayer/agent.py`. Você pode utilizar este player para jogar você mesmo e testar suas habilidades contra outro agente (inclusive o que você está construindo nesse trabalho). 

Para jogar com ele, utilize o mesmo comando acima, trocando o player1 ou 2 por `advsearch/humanplayer/agent.py`. Você terá o limite de 1 minuto para pensar na sua jogada. Digite as coorenadas da ação na ordem `<coluna> <linha>`.  

## Funcionamento 

Iniciando pelo primeiro jogador, que jogará com as peças pretas, o servidor chama a função `make_move(state)` do seu agente. A função recebe `state`, um objeto da classe `GameState` que contém um tabuleiro (objeto da classe `Board` e o jogador a fazer a jogada (um caractere) (`B` para as pretas ou `W` para as brancas). Para os detalhes, veja `gamestate.py` e `board.py` de cada jogo.

O servidor então espera o delay e recebe a tupla (x,y) com coluna e linha com a jogada do jogador. O servidor processa a jogada, exibe o novo estado no terminal e passa a vez para o próximo jogador, repetindo esse ciclo até o fim do jogo.

No fim do jogo, o servidor exibe a pontuação de cada jogador e cria um arquivo `results.xml`.
com todas as jogadas tentadas pelos jogadores (inclusive as ilegais). Um arquivo `history.txt` também contém as jogadas, e esse é criado mesmo que a partida seja interrompida no meio (e.g. crash de um agente).


## Notas
* O servidor checa a legalidade das jogadas antes de efetivá-las. A vez é devolvida para o jogador que tentou a jogada ilegal
* O jogador 'random' apenas sorteia uma jogada entre as válidas no estado recebido.
* O jogador 'human' verifica a legalidade da jogada antes de enviá-la ao servidor.
* Em caso de problemas com o servidor, reporte via moodle ou email.