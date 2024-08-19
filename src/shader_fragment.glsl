#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;
in vec3 cor_v;


// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 pos_luz;
uniform int game_ID;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define WALL  1
#define PLANE  2
#define CAPIVARA  3
#define CAPIVARA2  4
#define HUD  5
#define VITORIA 6
#define DERROTA 7
#define M_PI 3.141592

#define CAPIVARAIMPOSTORA 1
#define DESVIECAPIVARA 2
#define ANGRYCAP 3
uniform int object_id;
uniform float tempo;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

void Jogo1();
void Jogo2();
void Jogo3();

void main()
{
    if (object_id == VITORIA) {
        color = vec4(0,1,0,1);
    }
    if (object_id == DERROTA) {
        color = vec4(1,0,0,1);
    }
    if (game_ID == CAPIVARAIMPOSTORA) {
        Jogo1();
    } else if (game_ID == DESVIECAPIVARA) {
        Jogo2();
    } else if (game_ID == ANGRYCAP) {
        Jogo3();
    }
}

void Jogo1() {
// Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
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
    vec4 l = normalize(posL - p);



    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2 * n * (dot(n,l));

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    vec3 Kd0 = vec3(1,1,1);


    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI
        // Propriedades espectrais da esfera
        Kd = vec3(0.8,0.4,0.08);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.4,0.2,0.04);
        q = 1.0;
    }
    else if ( object_id == PLANE )
    {
        Kd = vec3(0.2,0.2,0.2);
        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(0.1,0.1,0.1);
        q = 20.0;

        U = (position_model.x - bbox_min.x)/(bbox_max.x - bbox_min.x);
        V = (position_model.z - bbox_min.z)/(bbox_max.z - bbox_min.z);

        Kd0 = texture(TextureImage1, vec2(U,V)).rgb;
    }
    else if ( object_id == CAPIVARA)
    {
        Kd = vec3(0.9,0.5,0.5);
        Ks = vec3(0.8,0.8,0.8);
        Ka = vec3(0.5,0.5,0.5);
        q = 32.0;

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 p2 = (position_model - bbox_center)/length(position_model - bbox_center);

        float theta = atan(p2.z, p2.x)+M_PI;
        float phi = asin(p2.x);

        //V = (theta + M_PI)/(2 * M_PI);
        //U = (phi + M_PI_2)/M_PI;

        V = (position_model.y - bbox_min.y)/(bbox_max.y - bbox_min.y);
        U = theta/(2*M_PI);

        Kd0 = texture(TextureImage0, vec2(U,V)).rgb;

    }
    else if ( object_id == CAPIVARA2)
    {
        color.rgb = cor_v;
    }
    else if (object_id == HUD) {
        //Cor linear que vai de (0,1,0) para (1,1,0) e depois para (1,0,0)
        if (tempo >= 0.5) {
            Ka = vec3(-2*tempo + 2,1.0,0);
        } else {
            Ka = vec3(1,2*tempo,0);
        }
        color.rgb = Ka;
    }
    else // Objeto desconhecido = preto
    {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I;

    // Luz spotlight, se fora do angulo, fica sem luz
    if (dot(vecL, -l) > cos(M_PI/12.0)) {
        I = vec3(1.0,1.0,1.0);
    } else {
        I = vec3(0.0, 0.0, 0.0);
    }

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0,dot(n,l));
    // Termo ambiente
    vec3 ambient_term = Ka * Ia;

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term  = Ks * I * pow(max(0,dot(r,v)),q);

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
    color.a = 1;

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.
    if (object_id != CAPIVARA2 && object_id != HUD )
        color.rgb = Kd0 * (lambert_diffuse_term + ambient_term + phong_specular_term);


    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}

void Jogo2() {
// Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
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

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(1,1,1,0));


    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    vec4 h = normalize(l+v);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2 * n * (dot(n,l));

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    vec3 Kd0 = vec3(1,1,1);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;


    if ( object_id == SPHERE )
    {
        // Propriedades espectrais da esfera
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.8,0.8,0.8);
        Ka = vec3(1.0,1.0,1.0);
        q = 1.0;

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 p2 = (position_model - bbox_center)/length(position_model - bbox_center);

        float theta = atan(p2.x, p2.z);
        float phi = asin(p2.y);

        U = (theta + M_PI)/(2 * M_PI);
        V = (phi + M_PI/2)/M_PI;

        Kd0 = texture(TextureImage3, vec2(U,V)).rgb;
    }
    else if ( object_id == WALL )
    {
        // Propriedades espectrais do coelho
        Kd = vec3(0.8,0.8,0.8);
        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(1,1,1);
        q = 20.0;

        U = position_model.x - floor(position_model.x);
        V = (position_model.z - bbox_min.z)/(bbox_max.z - bbox_min.z);

        Kd0 = texture(TextureImage2, vec2(U,V)).rgb;
    }
    else if ( object_id == PLANE )
    {
        // Propriedades espectrais do plano
        Kd = vec3(0.2,0.2,0.2);
        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(0.5,0.5,0.5);
        q = 20.0;

        U = (position_model.x - bbox_min.x)/(bbox_max.x - bbox_min.x);
        V = (position_model.z - bbox_min.z)/(bbox_max.z - bbox_min.z);

        Kd0 = texture(TextureImage1, vec2(U,V)).rgb;
    }
    else if ( object_id == CAPIVARA)
    {
        Kd = vec3(0.9,0.5,0.5);
        Ks = vec3(0.8,0.8,0.8);
        Ka = vec3(0.5,0.5,0.5);
        q = 32.0;

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 p2 = (position_model - bbox_center)/length(position_model - bbox_center);

        float theta = atan(p2.z, p2.x)+M_PI;
        float phi = asin(p2.x);

        //V = (theta + M_PI)/(2 * M_PI);
        //U = (phi + M_PI_2)/M_PI;

        V = (position_model.y - bbox_min.y)/(bbox_max.y - bbox_min.y);
        U = theta/(2*M_PI);

        Kd0 = texture(TextureImage0, vec2(U,V)).rgb;

    }
    else if ( object_id == CAPIVARA2)
    {
        color.rgb = cor_v;
    }
    else if (object_id == HUD) {
        //Cor linear que vai de (0,1,0) para (1,1,0) e depois para (1,0,0)
        if (tempo >= 0.5) {
            Ka = vec3(-2*tempo + 2,1.0,0);
        } else {
            Ka = vec3(1,2*tempo,0);
        }
        color.rgb = Ka;
    }
    else // Objeto desconhecido = preto
    {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I = vec3(1,1,1);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2);
    //vec3 Ia = vec3(1,1,1);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0,dot(n,l));

    // Termo ambiente
    vec3 ambient_term = Ka * Ia;

    // Termo especular utilizando o modelo de iluminação de Phong
    //vec3 phong_specular_term  = Ks * I * pow(max(0,dot(r,v)),q);
    vec3 blinn_phong_specular_term  = Ks * I * pow(max(0,dot(normal,h)),q);

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
    color.a = 1;

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.
    if (object_id != CAPIVARA2 && object_id != HUD)
        color.rgb = Kd0 * (lambert_diffuse_term + ambient_term + blinn_phong_specular_term);


    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}
void Jogo3() {
// Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
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

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(0,1,0,0));



    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2 * n * (dot(n,l)); // PREENCHA AQUI o vetor de reflexão especular ideal

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    vec3 Kd0 = vec3(1,1,1);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;


    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI
        // Propriedades espectrais da esfera
        Kd = vec3(0.8,0.4,0.08);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.4,0.2,0.04);
        q = 1.0;
    }
    else if ( object_id == WALL )
    {
        // PREENCHA AQUI
        // Propriedades espectrais do coelho
        Kd = vec3(0.8,0.8,0.8);
        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(1,1,1);
        q = 20.0;

        U = position_model.x - floor(position_model.x);
        V = (position_model.z - bbox_min.z)/(bbox_max.z - bbox_min.z);

        Kd0 = texture(TextureImage2, vec2(U,V)).rgb;
    }
    else if ( object_id == PLANE )
    {
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        Kd = vec3(0.2,0.2,0.2);
        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(0.5,0.5,0.5);
        q = 20.0;

        U = (position_model.x - bbox_min.x)/(bbox_max.x - bbox_min.x);
        V = (position_model.z - bbox_min.z)/(bbox_max.z - bbox_min.z);

        Kd0 = texture(TextureImage1, vec2(U,V)).rgb;
    }
    else if ( object_id == CAPIVARA)
    {
        Kd = vec3(0.9,0.5,0.5);
        Ks = vec3(0.8,0.8,0.8);
        Ka = vec3(0.5,0.5,0.5);
        q = 32.0;

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 p2 = (position_model - bbox_center)/length(position_model - bbox_center);

        float theta = atan(p2.z, p2.x)+M_PI;
        float phi = asin(p2.x);

        //V = (theta + M_PI)/(2 * M_PI);
        //U = (phi + M_PI_2)/M_PI;

        V = (position_model.y - bbox_min.y)/(bbox_max.y - bbox_min.y);
        U = theta/(2*M_PI);

        Kd0 = texture(TextureImage0, vec2(U,V)).rgb;

    }
    else if ( object_id == CAPIVARA2)
    {
        color.rgb = cor_v;
    }
    else if (object_id == HUD) {
        //Cor linear que vai de (0,1,0) para (1,1,0) e depois para (1,0,0)
        if (tempo >= 0.5) {
            Ka = vec3(-2*tempo + 2,1.0,0);
        } else {
            Ka = vec3(1,2*tempo,0);
        }
        color.rgb = Ka;
    }
    else // Objeto desconhecido = preto
    {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I = vec3(1,1,1);

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
    color.a = 1;

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.
    if (object_id != CAPIVARA2 && object_id != HUD)
        color.rgb = Kd0 * (lambert_diffuse_term + ambient_term + phong_specular_term);


    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}
