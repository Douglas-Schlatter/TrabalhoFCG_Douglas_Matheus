#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int object_id;
uniform vec2 pos_luz;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 position_world;
out vec4 normal;
out vec3 cor_v;

#define CAPIVARA2 4

void main()
{
    // A variável gl_Position define a posição final de cada vértice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estará entre -1 e 1 após divisão por w.
    // Veja {+NDC2+}.
    //
    // O código em "main.cpp" define os vértices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operações de modelagem, definição da câmera, e projeção, para computar
    // as coordenadas finais em NDC (variável gl_Position). Após a execução
    // deste Vertex Shader, a placa de vídeo (GPU) fará a divisão por W. Veja
    // slides 41-67 e 69-86 do documento Aula_09_Projecoes.pdf.

    gl_Position = projection * view * model * model_coefficients;

    // Como as variáveis acima  (tipo vec4) são vetores com 4 coeficientes,
    // também é possível acessar e modificar cada coeficiente de maneira
    // independente. Esses são indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto é, 'x' é o primeiro coeficiente, 'y' é o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    // Agora definimos outros atributos dos vértices que serão interpolados pelo
    // rasterizador para gerar atributos únicos para cada fragmento gerado.

    // Posição do vértice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;

    // Normal do vértice atual no sistema de coordenadas global (World).
    // Veja slides 123-151 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    if (object_id == CAPIVARA2) {

        #define M_PI 3.141592

        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
        vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
        vec4 n = normalize(normal);

    // Posição da fonte de luz
        vec4 posL = vec4(pos_luz.x, 2.0, pos_luz.y, 1.0);

    // Direção da luz
        vec4 vecL = normalize(vec4(0.0, -1.0, 0.0, 0.0));

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    //Tarefa 1
    //vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));
    //Tarefa 2 parte 1
    //vec4 l = normalize(camera_position - p);
        vec4 l = normalize(posL - p);



    // Vetor que define o sentido da câmera em relação ao ponto atual.
        vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
        vec4 r = -l + 2 * n * (dot(n,l)); // PREENCHA AQUI o vetor de reflexão especular ideal

    // Parâmetros que definem as propriedades espectrais da superfície
        vec3 Kd = vec3(0.08,0.4,0.8); // Refletância difusa
        vec3 Ks = vec3(0.8,0.8,0.8); // Refletância especular
        vec3 Ka = vec3(0.04,0.2,0.4); // Refletância ambiente
        float q = 32.0; // Expoente especular para o modelo de iluminação de Phong

    // Espectro da fonte de iluminação
        vec3 I;

    // Luz spotlight, se fora do angulo, fica sem luz
        if (dot(vecL, -l) > cos(M_PI/12.0)) {
            I = vec3(1.0,1.0,1.0); // PREENCH AQUI o espectro da fonte de luz
        } else {
            I = vec3(0.0, 0.0, 0.0);

        }

        // Espectro da luz ambiente
        vec3 Ia = vec3(0.2,0.2,0.2); // PREENCHA AQUI o espectro da luz ambiente

    // Termo difuso utilizando a lei dos cossenos de Lambert
        vec3 lambert_diffuse_term = Kd * I * max(0,dot(n,l)); // PREENCHA AQUI o termo difuso de Lambert

    // Termo ambiente
        vec3 ambient_term = Ka * Ia; // PREENCHA AQUI o termo ambiente

    // Termo especular utilizando o modelo de iluminação de Phong
        vec3 phong_specular_term  = Ks * I * pow(max(0,dot(r,v)),q); // PREENCH AQUI o termo especular de Phong

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.
        cor_v = lambert_diffuse_term + ambient_term + phong_specular_term;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    }
}

