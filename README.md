# Trabalho de Fundamentos de Computação Gráfica - Uma Capicatastrofe
Este relatório documenta a progressão do trabalho final de computação grafica. Nosso projeto é o jogo "Uma Capicatastrofe", composto por diversos minigames, tendo como propósito sobreviver ao maior número de minigames possível. O jogo possui inspiração no aplicativo mobile “Dumb Ways to die”. Sendo assim, o intuito é que sejam minigames curtos com limite de tempo que caso o jogador falhe seu placar será reiniciado. 


## Integrantes do Grupo:

- **Douglas Ardenghi Schlatter**
- **Email: douglas.ardenghi18@gmail.com**

- **Matheus Rodrigues Fonseca** 
- **Email: mat.rodf@gmail.com**






    ## Minigames e seus Conteúdos de Computação Gráfica relacionados

    Essa aplicação consiste em 3 jogos curtos com temática de capivaras, cada um com um critério de vitória e derrota. Ao terminar um dos jogos, é iniciado outro jogo e seu score é aumentado:

    O integrante comum e estrela do jogo são as nossas queridas capivaras, elas são um arquivo .obj de malha de triangulos que  utilizam um sistema de iluminação Blinn-Phong com textura calculada com coordenadas cilíndricas.

    Todos os jogos tem uma UI mostrando o tempo restante do jogo, esse UI foi desenhada em coordenadas NDC e aplicado uma matriz Model para escalar e posicionar ele acima da tela, o tempo muda de cor dinamicamente conforme o tempo passa, por meio de uma interpolação de cores, indo de verde para amarelo e vermelho.

    ### Desvie da capivara

    ![Alt text](/images%20app/Desvie.jpg)
    **Tutorial**

    Você e uma capivara estão em uma sala, ela vai te perseguir, o objetivo é você fugir dela até o tempo acabar. Se ela te encostar você perde.
    Você pode se movimentar com as teclas **W**,**A**,**S**,**D** e olhar ao redor ao clicar com o botão esquerdo e mover o mouse.

    Nesse jogo, tanto a colisão entre o jogador e as paredes, quanto colisão entre a capivara e a parede foram feitas por meio de uma Check Box - Plane. Já a colisão entre a capivara e o jogador é feita por meio de uma Check Box - Box.

    Para o desenvolvimento desse minijogo foi programado uma máquina de estados para servir de inteligência artificial da capivara, ela constantemente ciclará entre os estados Espera, Atenção e Corre. No estado de Espera a capivara ficará idle, já no estado de atenção ela olha em direção ao jogador e segue seus movimentos, posteriormente ela entrará no modo Corre que avançará na direção do jogador utilizando uma função de easing para calcular sua velocidade de avanço. 
    Foi utilizada transformações geométricas na capivara em reação aos inputs do player, para fazer com que a capivara mova e olhe para o jogador
    O jogador possui uma área de colisão Box Collider e controla o jogo por meio de uma câmera livre.
    
    É importante ressaltar que ao longo do jogo utilizamos diversos tipos de aplicação de texturas, nesse caso textura das paredes foi feita repetindo a imagem no eixo X para que ela não estique e sua iluminação é uma **iluminação difusa (Lambert)**.

    ### Capivara impostora

    ![Alt text](/images%20app/sus.jpg)
    **Tutorial**

    4 capivaras estão girando na tela, mas uma está diferente. Utilize as teclas **W**,**A**,**S**,**D** para mover a luz e observar mais atentamente as capivaras. Clique com a **barra de espaço** com a luz sobre a capivara que você acha que é a impostora para selecionar ela. Se você selecionar a capivara certa você ganha. Se você selecionar a errada ou o tempo acabar, você perde.

    Esse jogo possui uma câmera look-at e uma luz tipo spotlight para iluminar os objetos. Também possui múltiplas instâncias da capivara desenhadas em lugares diferentes.

    A "capivara falsa" é desenhada com modelo de iluminação de Gouraud, onde a cor é calculada por vértice, enquanto as outras são desenhadas com modelo de Phong, onde a cor é avaliada para cada pixel. Capivaras com o modelo de Gouraud aparentam mais "borradas" ao serem iluminadas e a luz spot-light fica quadriculada nos limites da capivara impostora, enquanto capivaras com o modelo Phong aparentam menos borradas e não quadriculam a spot-light.
    
    Para realizar o selecionamento da capivara impostora foi utilizado o método de detecção de colisão Esfera-Esfera. Junto à luz existe uma esfera de colisão que ao entrar em contato com a esfera de colisão das capivaras torna possível seu selecionamento.

    ### Angry Capivara
    
    ![Alt text](/images%20app/atirar.jpg) 

    Existem 4 alvos no mapa se mexendo pelo mapa, você pode usar as teclas **W**,**A**,**S**,**D** para andar pelo mapa, apertar a **barra de espaço** para arremessar uma capivara para frente e olhar ao redor ao clicar com o botão esquerdo e mover o mouse.  O seu objetivo é acertar todos os alvos antes do tempo acabar. Se o tempo acabar você perde.
    
    Para a implementação desse minigame foi necessário calcular uma curva de Bézier cúbica e seus pontos de referência. O cálculo dos pontos da curva usa como referência o vetor view e v da câmera, fazendo com que a curva siga a direção que o jogador aponta a free-camera. Após isso outra função utiliza esses pontos como input e realiza uma interpolação cúbica com entrada do tempo para obter o ponto em que a capivara deve estar naquele momento da curva.

    Além disso, para fazer com que os alvos ciclem entre suas posições de maneira suave foi utilizada uma função cosseno e múltiplas Box To Box colliders para calcular suas colisões com a capivara que esta sendo jogada.

    Também é importante mencionar que a textura do chão foi implementada com repetição no eixo X e Z para prevenir esticamento das texturas.

    

## Créditos:
Ambos os participantes foram ativos no momento de planejamento dos minigames, estruturas comuns de controle do jogo e criação dos conteúdos relacionados (vídeos no youtube, documentos de anotação e relatório final).

Matheus foi responsável pelos minigames **Desvie da Capivara** e **Capivara Impostora**, além de todas as funções e texturas relacionadas aos mesmos. Também atuou no desenvolvimento da lógica de troca de jogos e o mapeamento de texturas.

Douglas foi responsável pelos minigame **Angry Capivara**, além de todas as funções e texturas  relacionadas ao mesmo. Também atuou no versionamento do GitHub criando o diretorio, resolvendo conflitos de codigo e integração com o software CodeBlocks.

## Utilização de IA para desenvolvimento do projeto

Não tivemos necessidade de utilizar nenhuma ferramenta de apoio, ChatGPT ou qualquer recurso de inteligência artificial para esse projeto.

**Lembre-se! Nenhuma capivara foi machucada ao longo do desenvolvimento desse projeto!**

Para executar esse projeto no Windows:
===================================
Essas instruções foram retiradas do moodle da disciplina:
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


=== Soluções de Problemas
===================================

Caso você tenha problemas em executar o código deste projeto, tente atualizar o
driver da sua placa de vídeo.



