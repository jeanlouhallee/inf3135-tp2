/**   
* INF3135  
*  
* Travail pratique 2  
*  
* Nom:             Levasseur  
* Prenom:          Olivier  
* Code Permanent:  LEVO19109301  
* Email:           levasseuro.ol@gmail.com  
* Nom:			   Hallee
* Prenom:          Jean-Lou
* Code Permanent:  HALJ05129309
* Email:           
*/

#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "graphviz.h"
#include "countries.h"
#include "util.h"

// Implementation
// --------------


// Fonctions privees
// -----------------

/**
 * Cette fonction ecrit dans un fichier les informations voulues sur un pays en 
 * respectant le format graphviz.
 * 
 * @param langues, 1 si on veut afficher les langues, 0 sinon
 * @param capitale, 1 si on veut afficher la capitale, 0 sinon
 * @param frontieres, 1 si on veut afficher les frontieres, 0 sinon
 * @param flag, 1 si on veut afficher l'image du drapeau, 0 sinon
 * @param pays, le pays dont on veut afficher les informations
 * @param graphviz, un pointeur vers le fichier ouvert ou on veut ecrire
*/
void graphviz_ecrireUnPays(int langues, int capitale, int frontieres, int flag, json_t *pays, FILE * graphviz) {
    
    const char * codePays = countries_getCode(pays); 
    fprintf(graphviz, "%s%s%s", DEBUTPAYS, codePays, DEBUTPAYS2);
    fprintf(graphviz, "%s%s", SHAPE, LABEL);
    if (flag == 1) {
        char codeMin[4];
        util_chaineEnMinuscules(codePays, codeMin);
        fprintf(graphviz, "%s%s%s%s%s", TRTDIMAGE, TRTDIMAGE2, codeMin, TRTDIMAGE3, TRTDIMAGE4);
    }
    fprintf(graphviz, "%s%s%s", TRTDNAME, countries_getNomPays(pays), FINTRTD);
    fprintf(graphviz, "%s%s%s", TRTDCODE, countries_getCode(pays), FINTRTD);
    if (capitale == 1) {
        fprintf(graphviz, "%s%s%s", TRTDCAPITAL, countries_getCapitale(pays), FINTRTD); 
    }
    if (langues == 1) {
        json_t * langues = countries_langues(pays);
        fprintf(graphviz, "%s", TRTDLANGUAGE);
        void * iter = json_object_iter(langues);
        json_t *langueTemp = json_object_iter_value(iter);
        fprintf(graphviz, "%s", json_string_value(langueTemp));
        iter = json_object_iter_next(langues, iter);
        while (iter) {
            langueTemp = json_object_iter_value(iter);
            fprintf(graphviz, ", %s", json_string_value(langueTemp));
            iter = json_object_iter_next(langues, iter);
        }
        fprintf(graphviz, "%s", FINTRTD);
    }
    if (frontieres == 1) {
        fprintf(graphviz, "%s", TRTDBORDERS);
        json_t *frontieres = countries_frontieres(pays);
        if (json_array_size(frontieres) > 0) {
            fprintf(graphviz, "%s", json_string_value(json_array_get(frontieres,0)));
        }
        int i=1;
        while (i<json_array_size(frontieres)) {
            fprintf(graphviz, ", %s", json_string_value(json_array_get(frontieres,i)));
            i++;
        }
        fprintf(graphviz, "%s", FINTRTD);

    }
    fprintf(graphviz, "%s%s", LABELFIN, FINPAYS);
}

/**
 * Cette fonction ecrit dans un fichier les informations voulues sur tous 
 * les pays d'un tableau passe en parametres. De plus, il definit les 
 * liens entre les pays qui ont des frontieres communes pour que ces 
 * pays soient relies dans graphviz en appelant la fonction 
 * graphviz_ecrirePlusieursPays
 *
 * @param langues, 1 si on veut afficher les langues, 0 sinon
 * @param capitale, 1 si on veut afficher la capitale, 0 sinon
 * @param frontieres, 1 si on veut afficher les frontieres, 0 sinon
 * @param flag, 1 si on veut afficher l'image du drapeau, 0 sinon
 * @param pays, le pays dont on veut afficher les informations
 * @param graphviz, un pointeur vers le fichier ouvert ou on veut ecrire
*/
void graphviz_ecrirePaysVoisins(json_t *tabPays, FILE * graphviz) {
    int i;
    for (i=0; i< json_array_size(tabPays); i++) {
        json_t *pays = json_array_get(tabPays, i);
        const char * codePays = countries_getCode(pays);
        json_t *frontieres = countries_frontieres(pays);
        if (json_is_array(frontieres)) {
            if (json_array_size(frontieres) > 0) {
                int j;
                for (j=0; j< json_array_size(frontieres); j++) {
                    const char * codePaysTemp = json_string_value(json_array_get(frontieres, j));
                    json_t * paysTemp = countries_getJsonObjectFromCountry(codePaysTemp, tabPays);
                    if (paysTemp != NULL) { //donc si codePaysTemp fait partie de la même région
                        if (strcmp(codePays, codePaysTemp)<0) {
                            fprintf(graphviz, "    %s -- %s;\n", codePays, codePaysTemp);
                        }
                    }
                }
            }
        }
    }
}

// Fonctions publiques
// -------------------

void graphviz_ecrireUnSeulPays(int langues, int capitale, int frontieres, int flag, json_t *pays, const char * nomFichier) {
    FILE * graphviz = fopen(nomFichier, "w");
    fprintf(graphviz, "%s", DEBUTGRAPH);
    graphviz_ecrireUnPays(langues, capitale, frontieres, flag, pays, graphviz);    
    fprintf(graphviz, "%s", FINGRAPH);
    fclose(graphviz);
}

void graphviz_ecrirePlusieursPays(int langues, int capitale, int frontieres, int flag, json_t *tabPays, const char * nomFichier) {
    FILE * graphviz = fopen(nomFichier, "w");
    fprintf(graphviz, "%s", DEBUTGRAPH);
    int i;
    for (i=0; i< json_array_size(tabPays); i++) {
        graphviz_ecrireUnPays(langues, capitale, frontieres, flag, json_array_get(tabPays, i), graphviz);
    }
    graphviz_ecrirePaysVoisins(tabPays, graphviz);
    fprintf(graphviz, "%s", FINGRAPH);

    fclose(graphviz);
}

//pourquoi ne pas seulement passer le pays en paramètres?
//
//A FAIRE: 
//--prendre le code de ecrireUnPays et le séparer en sous-fonctions
//--mettre les noms de code en lettre minuscules
