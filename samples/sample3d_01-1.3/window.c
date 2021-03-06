/*!\file window.c
 *
 * \brief introduction aux textures (plus ce qui a �t� vu avant) 
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr 
 * \date February 15 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
/* pour les g�om�tries GL4Dummies */
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fen�tre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant de la g�om�trie quadrilat�re de GL4Dummies */
static GLuint _quad = 0;
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant d'une texture */
static GLuint _texId = 0;

/*!\brief cr�� la fen�tre d'affichage, initialise GL et les donn�es,
 * affecte les fonctions d'�v�nements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les param�tres OpenGL et les donn�es. 
 *
 * Exercice (corrig� en 1.2) : remplacer la texture ci-dessous par un
 * damier noir et blanc de 50x50 cases en utilisant tr�s peu de
 * donn�es. Puis (corrig� en 1.3) remplacer le vao cr�� manuellement
 * par le quadrilat�re g�n�rable par gl4dgGenQuadf et modifier le
 * shader pour n'afficher que la texture car les couleurs n'existent
 * pas dans ce qui est g�n�r� par les fonctions gl4dgGenXXXXX o� : (1)
 * la premi�re donn�e est un vec3 coordonn�e 3D ; (2) la seconde est
 * un vec3 vecteur normal 3D ; (3) la troisi�me est un vec2 coordonn�e
 * de texture 2D normalis�e. Ce dernier point ne permet plus de
 * r�p�ter la texture, corriger le probl�me en trouvant une solution
 * (corrig� en 1.4). Enfin (corrig� en 1.5), placer le quadrilat�re
 * horizontalement et agrandisser-le d'un facteur au moins 50. Tenter
 * de faire du MipMapping ou de l'anisotropic filtering.
 */
static void init(void) {
  const GLuint B = RGB(255, 255, 255), N = 0;
  GLuint tex[] = { B, N, N, B };
  /* G�n�ration du quadrilat�re GL4Dummies */
  _quad = gl4dgGenQuadf();
  /* g�n�rer un identifiant de texture */
  glGenTextures(1, &_texId);
  /* lier l'identifiant de texture comme texture 2D (1D ou 3D
   * possibles) */
  glBindTexture(GL_TEXTURE_2D, _texId);
  /* param�trer la texture, voir la doc de la fonction glTexParameter
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  /* envoi de la donn�e texture depuis la RAM CPU vers la RAM GPU voir
   * la doc de glTexImage2D (voir aussi glTexImage1D et glTexImage3D)
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
  /* d�-lier la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Cr�ation du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation de la texture 2D */
  glEnable(GL_TEXTURE_2D);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derri�re. */
  glEnable(GL_DEPTH_TEST);
  /* Cr�ation des matrices GL4Dummies, une pour la projection, une
   * pour la mod�lisation et une pour la vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
  /* on active la matrice de projection cr��e pr�c�demment */
  gl4duBindMatrix("projectionMatrix");
  /* la matrice en cours re�oit identit� (matrice avec des 1 sur la
   * diagonale et que des 0 sur le reste) */
  gl4duLoadIdentityf();
  /* on multiplie la matrice en cours par une matrice de projection
   * orthographique ou perspective */
  /* d�commenter pour orthographique gl4duOrthof(-1, 1, -1, 1, 0, 100); */
  gl4duFrustumf(-1, 1, -1, 1, 2, 100);
  /* dans quelle partie de l'�cran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  /* une variable d'angle, maintenant elle passe ne degr�s */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur, nous rajoutons aussi le buffer
   * de profondeur afin de bien rendre les fragments qui sont devant
   * au dessus de ceux qui sont derri�re. Commenter le
   * "|GL_DEPTH_BUFFER_BIT" pour voir la diff�rence. Nous avons ajout�
   * l'activation du test de profondeur dans la fonction init via
   * l'appel glEnable(GL_DEPTH_TEST). */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant ou "courante") la matrice vue cr��e dans
   * init */
  gl4duBindMatrix("viewMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* Composer la matrice vue courante en simulant une "cam�ra" �
   * l'aide de la fonction LookAt(xyz_position_cam,
   * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
  gl4duLookAtf(0, 0, 3, 0, 0, 0, 0, 1, 0);
  /* lier (mettre en avant ou "courante") la matrice mod�le cr��e dans
   * init */
  gl4duBindMatrix("modelMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* on transmet la variable d'angle en l'int�grant dans la matrice
   * mod�le. Soit composer la matrice courante avec une rotation
   * "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 0, 1, 0);
  /* on incr�mente angle d'un 1/60 de 1/4 de tour soit (360� x 1/60). Tester
   * l'application en activant/d�sactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que r�sultat soit toujours le m�me. */
  angle += 0.25f * (1.0f / 60.0f) * 360.0f;
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
  /* activer l'�tage de textures 0, plusieurs �tages sont disponibles,
   * nous pouvons lier une texture par type et par �tage */
  glActiveTexture(GL_TEXTURE0);
  /* lier la texture _texId comme texture 2D */
  glBindTexture(GL_TEXTURE_2D, _texId);
  /* envoyer une info au program shader indiquant que tex est une
   * texture d'�tage 0, voir le type (sampler2D) de la variable tex
   * dans le shader */
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  /* envoi d'un bool�en pour inverser l'axe y des coordonn�es de
   * textures (plus efficace � faire dans le vertex shader */
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1); 
  /* Dessiner une g�om�trie g�n�r�e par GL4Dummies */
  gl4dgDraw(_quad);
  /* d�sactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appel�e au moment de sortir du programme (atexit), elle
 *  lib�re les �l�ments OpenGL utilis�s.*/
static void quit(void) {
  /* suppression de la texture _texId en GPU */
  if(_texId)
    glDeleteTextures(1, &_texId);
  /* nettoyage des �l�ments utilis�s et g�n�r�s par la biblioth�que
   * GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
