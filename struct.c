#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include "struct.h"
#include "parsing.h"

void freeREV(RevList *list){
  RevList *aux = list;
  while(list){
    list=list->next;
    xmlFree(aux->date);
    xmlFree(aux->title);
    free(aux);
    aux=list;
  }
}

void count (char* content, RevList* rev){
  int i,c=0, prev=0;
  for(i=0; content[i]!='\0'; i++){
    if(content[i]==' ' || content[i]=='\t' || content[i]=='\n') {
      if(prev!=0){
        c++;
        prev=0;
      }
    }
    else prev++;
  }
  rev->nwords = c;
  rev->nchars = i;
}


int increaseHashedID(int hashedID, int hashSize){
  hashedID++;
  if(hashedID > hashSize) hashedID = 0;
  return hashedID;
}

int hash(int id, TAD_istruct qs){
  int hashedID;
  hashedID = id%(qs->hashsize);
  return hashedID;
}

int contains(RevList *list, RevList *new){
  RevList *aux;
  aux = list;
  int found = 0;
  while(aux && !found){
    if(aux->id == new->id){
      found = 1;
    }
    aux=aux->next;
  }
  return found;
}

void newContributor(char* authorName, int authorID, Contributor *author){
  strcpy(author->name, authorName);
  author->id = authorID;
  author->ncontributions++;
  author->isOccupied = 1;
}

void insertCON(int authorID, char* authorName, Contributor author[], TAD_istruct qs){
  int hashedID = hash(authorID, qs), found = 0;
    while(author[hashedID].isOccupied && !found){
      if(author[hashedID].id == authorID){
        author[hashedID].ncontributions++;
        found = 1;
      }
      else{
        hashedID = increaseHashedID(hashedID, qs->hashsize);
      }
    }
    if(!found) newContributor(authorName, authorID, &author[hashedID]);
}

void createREV(int articleid,struct RevisionList *new, int revisionid, char* title, char* date){
  new->id = revisionid;
  new->title =title;
  new->date = date;
  new->next = NULL;
}

RevList *LastRevison(RevList *list){
  RevList *aux = list, *prev;
  while(aux){
    prev = aux;
    aux=aux->next;
  }
  return prev;
}

void fillNewArticle(int articleID, RevList *newRevision, Article *newArticle){
  newArticle->id = articleID;
  newArticle->revisions = newRevision;
  newArticle->isOccupied = 1;
  newArticle->nrevisions++;
}

void insertID(int articleID, int revisionID, int contributorID, char* revtitle, char* contrname, char* revdate, char* text, TAD_istruct qs){
  int hashedID = hash(articleID, qs), found = 0;
  RevList *newRevision;
  Article *currentArticle;
  while(qs->articles[hashedID].isOccupied && !found){
    currentArticle = &(qs->articles[hashedID]);
    if(currentArticle->id == articleID){
      newRevision = malloc(sizeof(struct RevisionList));
      createREV(articleID, newRevision, revisionID, revtitle, revdate);
      if(!contains(currentArticle->revisions, newRevision)){
        LastRevison(currentArticle->revisions)->next = newRevision;
        count(text, newRevision);
        if(contributorID != -1) insertCON(contributorID, contrname, qs->authors, qs);
        currentArticle->nrevisions++;
      }
      else{
        freeREV(newRevision);
      }
      found = 1;
    }
    else{
      hashedID=increaseHashedID(hashedID, qs->hashsize);
    }
  }
  if(!found){
    newRevision = malloc(sizeof(struct RevisionList));
    createREV(articleID, newRevision, revisionID, revtitle, revdate);
    count(text, newRevision);
    fillNewArticle(articleID, newRevision, &(qs->articles[hashedID]));
    if(contributorID != -1) insertCON(contributorID, contrname, qs->authors, qs);
    qs->occupied_articles++;
  }
}

TAD_istruct init(){
  TAD_istruct qs = (TAD_istruct) malloc(sizeof(TCD_istruct));
  qs->occupied_articles = 0;
  qs->hashsize = -1;
  return qs;
}

TAD_istruct load(TAD_istruct qs, int nsnaps, char* snaps_paths[]){
  parse(nsnaps, snaps_paths, qs);
  return qs;
}

TAD_istruct clean(TAD_istruct qs){
  int i;
  for(i=0; i< qs->hashsize; i++){
    if(qs->articles[i].revisions)
      freeREV(qs->articles[i].revisions);
  }
  free(qs->articles);
  free(qs->authors);
  free(qs);
  return qs;
}
