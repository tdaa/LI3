#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "struct.h"
#include <omp.h>
//function which initiates the qs struct
TAD_istruct init2(TAD_istruct qs){
  int i;
  //runs through all the cube's and author's block inside the hash table andpercorre os arrays cube e author e inicializa-os.
  for(i=0; i < qs->hashsize; i++){
    qs->articles[i].id = -1;//-1 is the deafult id of an article
    qs->articles[i].isOccupied = 0;
    qs->articles[i].nrevisions = 0;
    qs->authors[i].id = -1;//-1 is the default id of an author
    qs->authors[i].isOccupied = 0;
    qs->authors[i].ncontributions = 0;
  }
  return qs;
}
//function which counts how many articles exist in the document
int parseCount (xmlDocPtr doc){
  xmlNodePtr cur, aux;
  int p=0;

  if (doc == NULL ) {
    fprintf(stderr,"Document not parsed successfully. \n");
    return 0;
  }

  cur = xmlDocGetRootElement(doc);

  if (cur == NULL) {
    fprintf(stderr,"empty document\n");
    return 0;
  }

  aux = cur->xmlChildrenNode;
  while (aux != NULL ){
    if(!(xmlStrcmp(aux->name, (const xmlChar *) "page"))) p++;//everytime there is a page node we increment the p variable
    aux = aux->next;
  }
  return p;
}

//function which parse all the document and insert the information on the struct
void parseDocID (xmlNodePtr cur, TAD_istruct qs, xmlDocPtr doc){
  xmlNodePtr aux, cont,user, texto;
  char username[256];
  char  *text, *con, *t, *u;
  int textflag = 0;
  int id, revisionid, usernameid=-1;
  while (cur != NULL){
    if(!(xmlStrcmp(cur->name, (const xmlChar *) "page"))){
      aux= cur->xmlChildrenNode;//everytime the function finds a page node, it equals the aux to the children node
      while(aux!=NULL){
        if(!(xmlStrcmp(aux->name, (const xmlChar *) "title"))){
          con = (char*) xmlNodeGetContent(aux);
          //strcpy(title, con);
          //xmlFree(con);
        }
        if(!(xmlStrcmp(aux->name, (const xmlChar *) "id"))){
          char* i = (char*) xmlNodeGetContent(aux);
          id = atoi(i);
          xmlFree(i);
        }
        if(!(xmlStrcmp(aux->name, (const xmlChar *) "revision"))){
          cont=aux->xmlChildrenNode;
          while(cont!=NULL){
            if(!(xmlStrcmp(cont->name, (const xmlChar *) "id"))){
              char *ri = (char*) xmlNodeGetContent(cont);
              revisionid = atoi(ri);
              xmlFree(ri);
            }
            if(!(xmlStrcmp(cont->name, (const xmlChar *) "timestamp"))){
              t = (char*) xmlNodeGetContent(cont);
              //strcpy(timestamp, t);
              //xmlFree(t);
            }
            if(!(xmlStrcmp(cont->name, (const xmlChar *) "contributor"))){
              user= cont->xmlChildrenNode;
              while(user!=NULL){
                if(!(xmlStrcmp(user->name, (const xmlChar *) "username"))){
                  u = (char*) xmlNodeGetContent(user);
                  strcpy(username, u);
                  xmlFree(u);
                }
                if(!(xmlStrcmp(user->name, (const xmlChar *) "id"))){
                  char* uid = (char*) xmlNodeGetContent(user);
                  usernameid = atoi(uid);
                  xmlFree(uid);
                }
                user=user->next;
              }
            }
            if(!(xmlStrcmp(cont->name, (const xmlChar *) "text")) && !textflag){
              texto = cont->xmlChildrenNode;
              if (texto != NULL){
               textflag = 1;
               text = (char*) xmlNodeGetContent(texto);
              }
            }
            cont=cont->next;
          }
        }
        aux=aux->next;
      }
      insertID(id, revisionid, usernameid, con, username, t, text, qs);
      xmlFree(text);
    }
    usernameid=-1;
    textflag = 0;
    cur = cur->next;
  }

}



/*
parseDoc - parse all doc info
*/
void parseDoc(xmlDocPtr doc, TAD_istruct qs) {

	xmlNodePtr cur, aux;

  if (doc == NULL ) {
    fprintf(stderr,"Document not parsed successfully. \n");
    return;
  }

  cur = xmlDocGetRootElement(doc);

  if (cur == NULL) {
    fprintf(stderr,"empty document\n");
    return;
  }

  aux = cur->xmlChildrenNode;
  parseDocID(aux, qs, doc);
}


void parse(int nsnaps, char **snaps_paths, TAD_istruct qs) {
  int i, size = 0;
  if (nsnaps <= 0) {
    printf("Usage: ./program docname\n");
    return;
  }

  xmlDocPtr doc[nsnaps];
#pragma omp parallel for num_threads(8) ordered schedule(dynamic)
  for(i=0; i<nsnaps; i++){
    doc[i] = xmlParseFile(snaps_paths[i]);
    size+=parseCount(doc[i]);
  }


  size = (size *2);
  qs->hashsize = size;
  qs->articles = malloc(size*sizeof(Article));
  qs->authors = malloc(size*sizeof(Contributor));
  qs = init2(qs);
  
  for(i=0; i<nsnaps; i++){
    parseDoc(doc[i], qs);
    xmlFreeDoc(doc[i]);
  }

  xmlCleanupParser();
}
