#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "interface.h"
#include "struct.h"
#include "parsing.h"

int cstring_cmp(const void *a, const void *b){
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
	/* strcmp functions works exactly as expected from
	comparison function */
}

long all_articles(TAD_istruct qs){
  return (qs->hashsize)/2;
}

long unique_articles(TAD_istruct qs){
  return (long) qs->occupied_articles;//occupied_articles is the number of unique id's inside the hash table.
}

long all_revisions(TAD_istruct qs){
  int i;
  long l=0;
  //runs through all the revisions inside the hash table and as long as doesn't finish, increase the l variable.
  for(i=0; i<qs->hashsize; i++) l += qs->articles[i].nrevisions;
  return l;
}

//'inserts' is an auxiliar function to the top_10_contributors, top_20_largest_articles and
//top_N_articles_with_more_words. It inserts the ncontributions, the nc, the nw, and the id's.
void inserts(long id, int n, int aux[], long* final, int tam, int flag){
  int i, j, k;
  for(j=0; j<tam && !flag; j++){
    if(n > aux[j] || (n == aux[j] && id < final[j]) ){
      flag = 1;
      for(i=(tam-1); i>j; i--) final[i] = final[i-1];
      final[i] = id;
      for(k=(tam-1); k>j; k--) aux[k] = aux[k-1];//when inserting the id, pushes all the others(after the new id's position) to the right.
      aux[k] = n;
    }
  }
}

long* top_10_contributors(TAD_istruct qs){
  int i, flag;
  int contributions[10] = {-1};//creates an array with the top ten contributions.
  long *top_ten = malloc(10*sizeof(long));//creates the final array.
  for(i=0; i<qs->hashsize; i++){
    flag = 0;
    if(qs->authors[i].isOccupied){
      inserts(qs->authors[i].id, qs->authors[i].ncontributions, contributions, top_ten, 10, flag);
    }
  }
  return top_ten;
}

char* contributor_name(long contributor_id, TAD_istruct qs){
  int i = hash(contributor_id, qs), found = 0;
  char *name;//final pointer to the contributor name.
  while(qs->authors[i].isOccupied && !found){//runs through all the author's block inside the hash table.
      if(qs->authors[i].id == contributor_id) {//check if the given id equals to the present idcontributor.
        found = 1;
        name = strdup(qs->authors[i].name);//saves the author's name.
      }
      i++;
  }
  if(!found) return NULL;
  return name;
}

long* top_20_largest_articles(TAD_istruct qs){
  long *top_twenty = malloc(20*sizeof(long));
  int i, flag, nc;
  int numberOfChars[20] = {-1};
  RevList* aux;
  for(i=0; i<qs->hashsize; i++){
    flag = 0;
    if(qs->articles[i].revisions){
      nc = qs->articles[i].revisions->nchars;
      aux = qs->articles[i].revisions->next;
      while(aux){
        if(nc < aux->nchars) nc = aux->nchars;
        aux = aux->next;
      }
      inserts(qs->articles[i].id, nc, numberOfChars, top_twenty, 20, flag);
    }
  }
  return top_twenty;
}

//mostRecent gives us the most recent title of the article.
char* mostRecentTitle(RevList* aux, char* timestamp, char* title){
  title = aux->title;
  aux = aux->next;
  while(aux){
    if(strcmp(timestamp, aux->date) == -1){
      timestamp = aux->date;
      title = aux->title;
    }
    aux=aux->next;
  }
  return title;
}

char* article_title(long article_id, TAD_istruct qs){
  char *title, *timestamp;
  int i = hash(article_id, qs), flag = 0;
  RevList *aux;
  while (qs->articles[i].isOccupied && !flag){//runs through all the author's block inside the hash table.
    if(qs->articles[i].id == article_id){//checks if the given id equals to the present article's id.
      flag = 1;
      timestamp = qs->articles[i].revisions->date;//saves the date.
      aux = qs->articles[i].revisions;
      title = strdup(mostRecentTitle(aux, timestamp, title));
    }
    i++;
  }
  return title;
}

long* top_N_articles_with_more_words(int n, TAD_istruct qs){
  long* top_N = malloc(n*sizeof(long));
  int i, l, flag, nw;
  int numberOfWords[n];
  RevList *aux;
  for(l=0; l<n; l++) numberOfWords[l] = -1;
  for(l=0; l<n; l++) top_N[l] = 9999999;
  for(i=0; i<qs->hashsize; i++){
    flag=0;
    if(qs->articles[i].revisions){
      nw = qs->articles[i].revisions->nwords;
      aux = qs->articles[i].revisions->next;
      while(aux){
        if(nw < aux->nwords) nw = aux->nwords;
        aux = aux->next;
      }
      inserts(qs->articles[i].id, nw, numberOfWords, top_N, n, flag);
  }
}
  return top_N;
}

char** titles_with_prefix(char* prefix, TAD_istruct qs){
  int i, inserted=0, size = 1, length = strlen(prefix);
  char** final = (char**) malloc(size*sizeof(char*));//the final pointer that the function will return.
  char *timestamp, *title;//timestamp will point to the date, title to the article's title.
  RevList* aux;
  for(i=0; i<qs->hashsize; i++){
    if(qs->articles[i].revisions){//it only checks the title and inserts it if there are revisions.
      timestamp = qs->articles[i].revisions->date;
      aux = qs->articles[i].revisions;
      title = mostRecentTitle(aux, timestamp, title);//'title' saves the most recent title;
      if(!strncmp(prefix, title, length)){
        final = realloc(final, size*sizeof(char*)+sizeof(char*));//increases the final's size.
        size = size + 1;
        final[inserted++] = strdup(title);
      }
    }
  }
  final[inserted] = NULL;
  for(i=0;final[i];i++);
  qsort(final, i, sizeof(char*), cstring_cmp);
  return final;
}


char* article_timestamp(long article_id, long revision_id, TAD_istruct qs){
  int i=hash(article_id, qs), flag=0;//'i' variable is to run through the hash table. The flag is to get out of the for cycle without a break.
  char *timestamp;
  RevList *aux;
  while(qs->articles[i].isOccupied && !flag){
    if(article_id == qs->articles[i].id){//first we search for the article's id.
      aux = qs->articles[i].revisions;
      for(; aux && !flag; aux = aux->next){
        if(revision_id == aux->id){//searches for the revision's id. If it finds it, then we point the timestamp to the date.
          timestamp = strdup(aux->date);
          flag = 1;
        }
      }
    }
    i++;
  }
  return timestamp;
}
