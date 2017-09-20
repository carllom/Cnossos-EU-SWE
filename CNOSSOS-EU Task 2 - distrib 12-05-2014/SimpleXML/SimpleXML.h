#pragma once
/* 
 * ------------------------------------------------------------------------------------------------
 * file:		SimpleXML.h
 * version:		1.001
 * author:		dirk.van-maercke@cstb.fr
 * copyright:	see file licence.CSTB.txt
 * description: SimpleXML defines a light-weight XML-to-DOM parser developed by CSTB.  
 * dependency:  SimpleXML builds on top of the EXPAT event-driven XMLparser, an open-source library 
 *			    available under LGPL from http://expat.sourceforge.net/
 * changes:
 *
 *	16/10/2013	this notice included
 *
 *	15/11/2013	support added to convert numerical error codes to human-readable text
 *
 * ------------------------------------------------------------------------------------------------- 
 */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif
#include <string.h>
#include "expat.h"
/*
 * special error codes in case the file cannot be found or accessed 
 */
#define XML_FILE_NOT_FOUND 100
#define XML_FILE_NO_ACCESS 101

// ------------------------------------------------------------------------------------------------
// classe abstraite, impl�mente le parseur EXPAT et la lecture du fichier XML
//
// m�thodes virtuelles, � d�finir dans une classe d'impl�mentation
//
//  startEntity : callback appel� par le parseur EXPAT lorsqu'il rencontre un nouveau tag de d�but
//                d'entit�.
//  endEntity :   callback appel� par EXPAT lorsqu'il rencontre un TAG de fin d'entit�. A noter:
//                EXPAT appelle endENtity pour les tags sans contenu de type <TAG />
//  addText:      callback appel� par EXPAT pour tout texte contenu dans les entit�s.
//
// m�thode concr�te:
//
//  ParseFile:    cr�ation d'un parseur EXPAT, lecture d'un fichier XML et envoi du contenu du
//                fichier � travers le parseur.
//
// ------------------------------------------------------------------------------------------------

class XMLFileParser
{
 public:  
 
   virtual void startEntity (const char *element_name, const char **attr) { } ;
   virtual void endEntity (const char *element_name) { } ;
   virtual void addText (const XML_Char* text, int len_text) { } ;

   virtual ~XMLFileParser() { } ;
   
   bool ParseFile (const char* filename) ;

   unsigned int last_error ;
   unsigned int last_line ;
   unsigned int last_char ;

   static const char* GetErrorText (unsigned int code) ;
} ;

// ------------------------------------------------------------------------------------------------
// classe utilitaire : afficher le contenu d'un fichier XML sous forme d'une arboresence.
//
// la fonction ParseFile est h�rit�e de la classe de base XMLFileParser
//
// cette classe impl�mente ses propres versions des fonctions virtuelles startEntity, endEntity 
// et addText
//
// ------------------------------------------------------------------------------------------------

class XMLDumpFile : public XMLFileParser
{
   char depth[128];
   
 public:  
 
   void startEntity (const char *element_name, const char **attr) ;
   void endEntity (const char *element_name) ;
   void addText (const XML_Char* text, int len_text) ;
   
   XMLDumpFile(void)
   {
       depth[0] = 0 ;
   }
} ;

// ------------------------------------------------------------------------------------------------
// clsse XMLNode ; repr�sentation d'un document XML sous forme d'une arborescence 
//
// A FAIRE: destruction et lib�ration de la m�moire allou�e.
// ------------------------------------------------------------------------------------------------

class XMLNode 
{
    XMLNode *parent ;
    XMLNode *next ;
    XMLNode *first_child ;
    XMLNode *last_child ;
    char    *name ;
    char    *text ;
    char   **attrib ;
 
 public:   
 
 // cr�ation
 
    XMLNode (XMLNode *_parent, const char* _name, const char** _attrib) ;
 
 // destruction
 
    ~XMLNode (void) ;
    
 // chaque noeud a un nom, un texte et une liste d'attributs
 
    const char* GetName (void) { return name ? name : "" ; } 
    const char* GetText (void) { return text ? text : "" ; } 
    
    const char* GetAttribName (int i) { return attrib[2*i] ; } 
    const char* GetAttribValue (int i) { return attrib[2*i+1] ; } 

 // chaque noeud est enfant d'un noeud parent et peut � son tour �tre le parent
 // d'une liste d'enfants. Les enfants d'un noeud sont g�r�s comme une liste
 // cha�n� et peuvent �tre visit�s par des appels � GetFirstChild / GetNextENtity
 
    XMLNode* GetParent (void) { return parent ; } ;
    XMLNode* GetFirstChild (void) { return first_child ; } ;
    XMLNode* GetNextEntity (void) { return next ; } ;
 
 // recherche d'un enfant par nom d'entit�
 
    XMLNode* GetChild (const char *entityName) ;
 
 // recherche de la valeur d'un attribut 
 
    const char* GetAttribute (const char *attributeName) ;

 // recheche d'un enfant par la valeur d'un de des attributs
 
    XMLNode* GetChildByAttribute (const char *attributeName, const char *attributeValue) ;    

 // ajoute un texte au contenu du noeud
 
    void addText (const char* new_text, int len_text = -1) ;    

 // affiche l'arborescence � l'�cran
 
    void dump (int level) ;
} ;

// ------------------------------------------------------------------------------------------------
// classe XMLFileLoader :  chargement d'un fichier XML en m�moire sous la forme 
//                         d'une arborescence de XMLNode's.
//
// la fonction ParseFile est h�rit�e de la classe de base XMLFileParser
//
// cette classe impl�mente ses propres versions des fonctions virtuelles startEntity, endEntity 
// et addText
//
// A FAIRE: destruction et lib�ration de la m�moire...
//
// ------------------------------------------------------------------------------------------------

class XMLFileLoader : public XMLFileParser
{
    XMLNode *root ;
    XMLNode *current ;
    
 public:  
 
   void startEntity (const char *element_name, const char **attr) ;
   void endEntity (const char *element_name) ;
   void addText (const XML_Char* text, int len_text) ;
   
   XMLFileLoader() { root = current = NULL ; } 

   ~XMLFileLoader() { Cleanup() ; }
   
// acc�s � la racine de l'arborescence

   XMLNode* GetRoot (void) { return root ; } 

// afficher l'arborescence � l'�cran

   void dump (void)
   {
       if (root) root->dump(0) ;
   }
   
   void Cleanup (void)
   {
       if (root) delete root ;
       root = current = NULL ;
   }
} ;



