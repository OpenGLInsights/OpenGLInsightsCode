
/**********************************************************************\
* AUTHOR : HILLAIRE Sébastien
*
* MAIL   : hillaire_sebastien@yahoo.fr
* SITE   : sebastien.hillaire.free.fr
*
*	You are free to totally or partially use this file/code.
* If you do, please credit me in your software or demo and leave this
* note.
*	Share your work and your ideas as much as possible!
\*********************************************************************/



#ifndef GLUT_TIME_COUNTER
#define GLUT_TIME_COUNTER


#include "GL\glut.h"


/**
 *	Classe permettant de gérer le temps écouler depuis le lancement de l'application ou depuis
 * le dernier reset effectué.
 *
 * @author Hillaire Sébastien.
 */
class GLUTTimeCounter
{
	private:

		/**
		 * Le temps auquel nous avons effecteur un reset du compteur.
		 */
		int lastResetTime;

	protected:

	public:

		/**
		 * Constructeur
		 */
		GLUTTimeCounter();

		/**
		 * Destructeur
		 */
		~GLUTTimeCounter();

		/**
		 *	Remet à zéro le conteur de temps.
		 */
		void reset();

		/**
		 *	Renvoie le temps écoulé depuis la dernière mise a zéro du compteur de temps.
		 *
		 * @return le temps écoulé en milliseconde au format int32.
		 */
		int getElapsedTime();

		/**
		 *	Renvoie le temps écoulé depuis la dernière mise a zéro du compteur de temps.
		 *
		 * @return le temps écoulé en milliseconde au format float.(en interne on gere le temps en int32)
		 */
		float getElapsedTimef();

		/**
		 *	Renvoie le temps écoulé depuis le lancement de l'application.
		 *
		 * @return le temps écoulé en milliseconde au format int32.
		 */
		int getElapsedTimeSinceAppStart();

		/**
		 *	Renvoie le temps écoulé depuis le lancement de l'application.
		 *
		 * @return le temps écoulé en milliseconde au format float.(en interne on gere le temps en int32)
		 */
		float getElapsedTimeSinceAppStartf();
};

#endif 
