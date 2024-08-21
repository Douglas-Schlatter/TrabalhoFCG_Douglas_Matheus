## TEMPLATE DE READ ME PARA ALTERAR OS TITULOS E CONTEUDO  

Projeto de Regressão Linear e TensorFlow/Keras

## Integrantes do Grupo:

- **Douglas Ardenghi Schlatter** - Cartão: 00332849 - Turma A
- **João Paulo Vasquez Dias** - Cartão: 00333827 - Turma A
- **Matheus Rodrigues Fonseca** - Cartão: 00332800 - Turma A




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