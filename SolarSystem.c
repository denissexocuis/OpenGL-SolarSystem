/*
 *  'SolarSystem'
 *  GL/freeglut
 *  Graficación por computadora
 *  25/10/24
*/

#include <GL/freeglut.h>
#include <stdlib.h>
#include <time.h>
//texturas ;)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//se usó stb_image.h en vez de SOIL por su facilidad de compilación ;)

#define N_STARS 500
#define N_PLANETAS 13
#define N_ASTEROIDES 100

/* FUENTES
    OpenGL SuperBible: Comprehensive Tutorial and Reference (4th Edition), Richard Wright, Benjamin Lipchak, Nicholas Haemel

    https://astronomia.udp.cl/es/espanol-construccion-sistema-solar-a-escala-y-como-utilizarlo-taller-astroudp-para-profesores/

    https://www.solarsystemscope.com/textures/

    https://www.color-hex.com/color-palettes/

    texturas:
    Learn OpenGL - GRAPHICS PROGRAMMING, Joey de Vries
 */

/* INFORMATION...
* (4 coordenadas, la última es la opacidad)
* AMBIENT: ilumina todo de manera uniforme en todos los objetos (debe ser valor bajo para mantener contraste)
* DIFFUSE: proviene de un foco cuyo inicio 
* podemos determinar (como cuando el sol ilumina un objeto)
* SPECULAR: parecida a la difusa, genera 
* "brillos" en el objeto (luz que se refleja en superficies brillantes)
* POSITION: donde está la fuente de luz
* DIRECTION: hacia donde apunta
* COLOR: define el color de la luz
* 'SPOTLIGHT'-> ilumina en un área especifica
* CUTOFF: ángulo en el que la luz se proyecta
* EXPONENT: intensidad de luz
* DIRECTION: hacia donde apunta
* MAT_AMBIENT: color que refleja uniformemente (valor bajo para que no se vea plano)
* MAT_SPECULAR: color del brillo en la superficie
* MAT_DIFFUSE: color que el material refleja 
  cuando la luz lo ilumina directamente.
* MAT_SHINN = cuán brillante es el material
* MAT_COLOR = color base del objeto
* MAT_EMISSION: color que el material "emite" como 
* si fuera una fuente de luz
* FOCUS_EMISSION: color de emisión para un objeto
* especial.
*/

enum planetas
{
    SOL, MERCURIO, VENUS, TIERRA, MARTE, JUPITER, SATURNO, URANO, NEPTUNO, PLUTON, CERES, HAUMEA, MAKEMAKE
};

typedef struct star
{
    float star_x, star_y, star_z,
          star2_x, star2_y, star2_z;
}STAR;

typedef struct ast
{
    float x,y,z;
}ASTEROIDES;

STAR star[N_STARS];
ASTEROIDES asteroid[N_ASTEROIDES];
unsigned int textureID[N_PLANETAS];

float light_ambient [] = {0.12,0.07,0.19,1.0},
      light_diffuse[] = {1.0,1.0, 1.0, 1.0},
      light_specular[] = {0.968,0.909,0.952,1.0},
      light_position[]={0.0,0.0,0.0,1.0},
      m_a []= {1.0,0.0, 0.0, 1.0},
      m_d[] = {0.0, 0.0, 0.0, 1.0},
      m_s[] = {0.0, 0.3, 0.4, 1.0},
      sh []= {7},
      fog [] = {0.0,0.0,0.16,1.0},
      R = 0.0, R2=0.0, R3=0.0, r=0.0, r2=0.0,r3=0.0,
      rotate[N_PLANETAS];

const char *TEXTURAS[N_PLANETAS] =
{
    "sol.jpg",
    "mercurio.jpg",
    "venus.jpg",
    "tierra.jpg",
    "marte.jpg",
    "jupiter.jpg",
    "saturno.jpg",
    "urano.jpg",
    "neptuno.jpg",
    "pluton.jpg",
    "ceres.jpg",
    "haumea.jpg",
    "makemake.jpg"
};

float velocidades[N_PLANETAS] = 
{
    0.0,        // solecito n.n
    0.1017,     // MERCURIO (aprox. 6.14°/día)
    0.04,      // VENUS (aprox. 1.48°/día)
    0.3495,     // TIERRA (360°/día)
    0.2567,     // MARTE (aprox. 349.51°/día)
    0.8457,     // JUPITER (aprox. 878.05°/día)
    1.0,        // SATURNO (aprox. 800°/día)
    0.3495,     // URANO (aprox. 500°/día)
    0.5243,     // NEPTUNO (aprox. 537.31°/día)
    0.05,       // PLUTÓN (aprox. 2.34°/día)
    0.02,       // CERES (aprox. 40°/día)
    0.07,       // HAUMEA (aprox. 92.3°/día)
    0.06        // MAKEMAKE (aprox. 16.1°/día)
};

void ENABLE_fog();
void ASTEROIDS();
void STARS(int,int);


void cargarTexturas()
{
    for(int i=0; i<N_PLANETAS; i++)
    {
        int width, height, nrChannels;
        unsigned char *data = stbi_load(TEXTURAS[i], &width, &height,
        &nrChannels, 0);

        glGenTextures(1, &textureID[i]);
        glBindTexture(GL_TEXTURE_2D, textureID[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), width, height, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
}

static void init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glDepthFunc(GL_LESS);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    cargarTexturas();
    ASTEROIDS();

    //antialiasing, para suavizar los bordes y no tener estrellitas pixeladas D:
    glEnable(GL_BLEND); //<-- para activar blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //<<- determina como se van a mezclar los colorsitos
    glEnable(GL_POINT_SMOOTH); //para los puntos de las estrellas
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); //para que se vea pipirisnais
}

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(60.0,1.0,1.0,100.0);  
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    glTranslatef(0.0, 0.0, -30.0);
    glMatrixMode(GL_MODELVIEW);

    //inicializar estrellas y dibujarlas de acuerdo a la ventana
    STARS(width, height);
}

void STARS(int width, int height)
{   srand(time(NULL));
    for(int i=0; i<=N_STARS; i++)
    {
        star[i].star_x = (float) (rand() % (width / 20)) - (width / 40);
        star[i].star_y = (float) (rand() % (height / 20)) - (height / 40);
        star[i].star_z = (float) (rand() % (height / 20)) - (height / 40);

        star[i].star2_x = (float) (rand() % (width / 20)) - (width / 40);
        star[i].star2_y = (float) (rand() % (height / 20)) - (height / 40);
        star[i].star2_z = (float) (rand() % (height / 20)) - (height / 40);
    }
}

void ASTEROIDS()
{
    srand(time(NULL));
    for (int i = 0; i < N_ASTEROIDES; i++)
    {
        /* descomposición de componentes rectangulares  (vectores) 
            Vx = Vcos0
            Vy = Vsen0

            V es la distancia vv
            V = (Vx, Vy)
         */
        float angle = (float)(rand() % 360),
              d = 9.5 + (rand() % 3);

        asteroid[i].x = d * cos(angle);
        asteroid[i].y = d * sin(angle);
        asteroid[i].z = 3.0;  // Profundidad
    }
}

void drawS()
{
    glPointSize(4);
    glBegin(GL_POINTS);    
        for(int i=0; i<=N_STARS; i++)
        {
                glVertex3f(star[i].star_x,star[i].star_y, star[i].star_z);
        }
    glEnd();

    glPointSize(5);
    glBegin(GL_POINTS);   
        for(int i=0; i<=N_STARS; i++)
        {
                glVertex3f(star[i].star2_x,star[i].star2_y, star[i].star2_z);
        }
    glEnd();
}

void drawAst()
{
    ENABLE_fog();
    for (int i = 0; i < N_ASTEROIDES; i++) 
    {
        glPushMatrix();
            glTranslatef(asteroid[i].x, asteroid[i].y, asteroid[i].z);
            glColor3f(0.5,0.5,0.5);
            glutSolidSphere(0.05, 10, 10);
        glPopMatrix();
    }
}

void animation()
{

    for (int i =0; i < N_PLANETAS; i++)
        rotate[i] += velocidades[i];

    glutPostRedisplay();
}

void ENABLE_fog()
{
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, fog);
    glFogf(GL_FOG_DENSITY, 0.2f);
    //glFogf(GL_FOG_START,0.0);
    glFogf(GL_FOG_END, 40.0f);
    glFogi(GL_FOG_MODE, GL_LINEAR);
}

void sol()
{
    //textura
    glBindTexture(GL_TEXTURE_2D, textureID[SOL]);
    glEnable(GL_TEXTURE_2D);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_a);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_d);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_s);
    glMaterialfv(GL_FRONT, GL_SHININESS, sh);

    /* SOLECITO - ESFERA */
    glPushMatrix();
        glColor3f(0.9, 0.4, 0.0);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric, 3, 50, 50);
        gluDeleteQuadric(quadric); // lo limpia
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void mercurio()
{
    glBindTexture(GL_TEXTURE_2D, textureID[MERCURIO]);
    glEnable(GL_TEXTURE_2D);

    /* MERCURIO ESCALA
        TAMAÑO: 1.2 cm
        DISTANCIA AL SOL: 4 cm 
     */
    glPushMatrix();
        glRotatef(rotate[MERCURIO],0,1,0);
        glTranslatef(12,0,0); // tamaño sol * distancia
        glColor3f(0.4,0.4,0.4);
        
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric, 1.2, 50, 50);
        gluDeleteQuadric(quadric); // lo limpia
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void venus()
{
    glBindTexture(GL_TEXTURE_2D, textureID[VENUS]);
    glEnable(GL_TEXTURE_2D);
    
    /* VENUS ESCALA
        TAMAÑO: 1.50cm
        DISTANCIA AL SOL: 6.9cm
     */
    glPushMatrix();
        glRotatef(rotate[VENUS],0,1,0);
        glTranslatef(20.7,0,0);
        glColor3f(0.65,0.50,0.27);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric, 1.5, 50, 50);
        gluDeleteQuadric(quadric);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void tierra()
{
    glBindTexture(GL_TEXTURE_2D, textureID[TIERRA]);
    glEnable(GL_TEXTURE_2D);

    /* TIERRA
        TAMAÑO: 1.5 cm
        DISTANCIA AL SOL: 9.2cm
     */

    glPushMatrix();
        glRotatef(rotate[TIERRA],0,1,0);
        glTranslatef(27.6,0,0); //tamaño tierra * distancia al sol ;)
        glColor3f(0.41,0.57,0.83);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric, 1.5, 50, 50); //tamaño
        gluDeleteQuadric(quadric);
        //glutSolidSphere(0.6,50,50);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void marte()
{
    glBindTexture(GL_TEXTURE_2D, textureID[MARTE]);
    glEnable(GL_TEXTURE_2D);

    /* MARTE
        TAMAÑO:  0.8 cm
        DISTANCIA AL SOL: 14cm
     */

    glPushMatrix();
        glRotatef(rotate[MARTE],0,1,0);
        glTranslatef(42,0,0); //tamaño tierra * distancia al sol ;)
        glColor3f(0.75,0.26,0.5);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,  0.8, 50, 50); //tamaño
        gluDeleteQuadric(quadric);
        //glutSolidSphere(0.6,50,50);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void jupiter()
{
    glBindTexture(GL_TEXTURE_2D, textureID[JUPITER]);
    glEnable(GL_TEXTURE_2D);

    /* JUPITER
        TAMAÑO: 5.26
        DISTANCIA AL SOL: 22cm
     */
    glPushMatrix();
        glRotatef(rotate[JUPITER],0,1,0);
        glRotatef(90,1,0,0);
        glTranslatef(66,0,0); //tamaño tierra * distancia al sol ;)
        glColor3f(0.89,0.86,0.79);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,5.26, 50, 50); //tamaño
        gluDeleteQuadric(quadric);
        //glutSolidSphere(0.6,50,50);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void saturno()
{
    glBindTexture(GL_TEXTURE_2D, textureID[SATURNO]);
    glEnable(GL_TEXTURE_2D);

    /* SATURNO
        TAMAÑO: 4.46
        DISTANCIA AL SOL: 28cm
     */
    glPushMatrix();
        glRotatef(rotate[SATURNO],0,1,0);
        glRotatef(90,1,0,0);
        glTranslatef(84,0,0); //tamaño tierra * distancia al sol ;)
        glColor3f(0.8,0.74,0.4);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,4.46, 50, 50); //tamaño
        //glutSolidSphere(0.6,50,50);
        gluDeleteQuadric(quadric);

        //anillo
        glPushMatrix();
            glRotatef(10,0, 1, 0);
            glColor3f(0.84,0.68,0.42);
            glutSolidTorus(0.5, 7, 30, 30);
        glPopMatrix();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void urano()
{
    glBindTexture(GL_TEXTURE_2D, textureID[URANO]);
    glEnable(GL_TEXTURE_2D);

    /* URANO 
        TAMAÑO: 1.96
        DISTANCIA AL SOL: 32.5 cm
     */
    glPushMatrix();
        glRotatef(rotate[URANO],0,1,0);
        glRotatef(90,1,0,0);
        glTranslatef(97.5,0,0); //tamaño tierra * distancia al sol ;)
        glColor3f(0.7,0.82,0.89);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,1.96, 50, 50); //tamaño
        //glutSolidSphere(0.6,50,50);
        gluDeleteQuadric(quadric);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void neptuno()
{
    glBindTexture(GL_TEXTURE_2D, textureID[NEPTUNO]);
    glEnable(GL_TEXTURE_2D);

    /* NEPTUNO
        TAMAÑO: 1.83cm
        DISTANCIA AL SOL: 101.4 cm
     */
    glPushMatrix();
        glRotatef(rotate[NEPTUNO],0,1,0);
        glRotatef(90,1,0,0);
        glTranslatef(101.4,0,0);
        glColor3f(0.7,0.82,0.89);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,1.83, 50, 50); //tamaño
        //glutSolidSphere(0.6,50,50);
        gluDeleteQuadric(quadric);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void pluton()
{
    glBindTexture(GL_TEXTURE_2D, textureID[PLUTON]);
    glEnable(GL_TEXTURE_2D);

    /* Plutón
        TAMAÑO: 0.4 cm
        DISTANCIA AL SOL: 59.1 cm
     */
    glPushMatrix();
        glRotatef(rotate[PLUTON],0,1,0);
        glRotatef(90,1,0,0);
        glTranslatef(177.3,0,0);
        glColor3f(0.7,0.82,0.89);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,0.4, 50, 50); //tamaño
        //glutSolidSphere(0.6,50,50);
        gluDeleteQuadric(quadric);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void ceres()
{
    glBindTexture(GL_TEXTURE_2D, textureID[CERES]);
    glEnable(GL_TEXTURE_2D);

    /* CERES
        TAMAÑO: 0.16 cm
        DISTANCIA AL SOL: 8.31 cm
     */
    glPushMatrix();
        glRotatef(rotate[CERES],0,1,0);
        glRotatef(90,1,0,0);
        glTranslatef(8.31,0,0);
        glColor3f(0.7,0.82,0.89);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,0.16, 50, 50); //tamaño
        //glutSolidSphere(0.6,50,50);
        gluDeleteQuadric(quadric);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void haumea()
{
    glBindTexture(GL_TEXTURE_2D, textureID[HAUMEA]);
    glEnable(GL_TEXTURE_2D);

    /* HAUMEA
        TAMAÑO: 0.5 cm
        DISTANCIA AL SOL: 129cm
     */
    glPushMatrix();
        glRotatef(rotate[HAUMEA],0,1,0);
        glRotatef(90,1,0,0);
        glTranslatef(129,0,0);
        glColor3f(0.7,0.82,0.89);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,0.5, 50, 50); //tamaño
        //glutSolidSphere(0.6,50,50);
        gluDeleteQuadric(quadric);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void makemake()
{
    glBindTexture(GL_TEXTURE_2D, textureID[MAKEMAKE]);
    glEnable(GL_TEXTURE_2D);

    /* MAKEMAKE
        TAMAÑO: 0.47 cm
        DISTANCIA AL SOL: 136.8 cm
     */
    glPushMatrix();
        glRotatef(rotate[NEPTUNO],0,1,0);
        glRotatef(90,1,0,0);
        glTranslatef(136.8,0,0);
        glColor3f(0.7,0.82,0.89);
        GLUquadric* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric,0.47, 50, 50); //tamaño
        //glutSolidSphere(0.6,50,50);
        gluDeleteQuadric(quadric);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

static void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w': R+=0.2;
            break;
        case 's': R-=0.2;
            break;
        case 'a': R2+=0.2;
            break;
        case 'd': R2-=0.2;
            break;
        case 'q': r+=1.0;
            break;
        case 'e': r-=1.0;
            break;
        case 32: R3-=0.3; // espacio
            break;
        case 'm': R3+=0.2;
            break;
        case 't': r2+=0.4;
            break;
        case 'g': r2-=0.4;
            break;
        case 'r': r3+=1.0;
            break;
        case 'y': r3-=1.0;
    }
    glutPostRedisplay();
}

static void draw()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // pal movimiento 100% real no fake ;)
    glLoadIdentity();
    glTranslatef(R2,R3,R);
    glRotatef(r, 0,1,0);
    glRotatef(r2, 1,0,0);
    glRotatef(r3, 0,1,0);

    ENABLE_fog(); //niebla para ponerle mas sombritas y se vea cool
    //planetas
    glPushMatrix();
        glTranslatef(0,-3,0);
        glScalef(0.2,0.2,0.2);
        sol();
        mercurio();
        venus();
        tierra();
        marte();
        jupiter();
        saturno();
        urano();
        neptuno();
        pluton();
        ceres();
        haumea();
        makemake();
        
    glPopMatrix();
    glDisable(GL_FOG);

    /* estrellasss */
    glPushMatrix();
        glColor3f(1,1,1);
        drawS();
    glPopMatrix();

    ENABLE_fog();
    /* asteroidess */
    glPushMatrix();
        glColor3f(0.48,0.45,0.43);
        glRotatef(90,1,0,0);
        glRotatef(-rotate[CERES], 0,0,1);
        drawAst();
    glPopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800,800);
	glutInitWindowPosition(10,50);
	glutCreateWindow("SolarSystem");
	init();
    glutReshapeFunc(resize);
    glutDisplayFunc(draw);
    glutIdleFunc(animation);
    glutKeyboardFunc(keyboard);
    
	glutMainLoop();
	return EXIT_SUCCESS;
}
