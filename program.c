#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "interface.h"
#include <time.h>

int main(int argc, char **argv){
  int i;
  long result;
  TAD_istruct qs;
  clock_t t;
  t = clock();
  qs = init();
  t = clock() - t;
  double time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("init finished in %f seconds. Starting load...\n\n", time_taken);

  t = clock();
  qs = load(qs, argc-1, argv+1);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Load successful in %f seconds.\n\n", time_taken);

  t = clock();
  result = all_articles(qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Demorou %f segundos em calcular o número de artigos: %ld\n\n", time_taken, result);

  t = clock();
  result = unique_articles(qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Demorou %f segundos em calcular o número de unique articles: %ld\n\n", time_taken, result);

  t = clock();
  result = all_revisions(qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Demorou %f segundos em calcular o número de revisions: %ld\n\n", time_taken, result);

  t = clock();
  long *l = top_10_contributors(qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Demorou %f segundos a calcular os top 10 contributors\n", time_taken);
  for(i = 0; i<10; i++) printf("ID nº %d: %ld\n",i+1, l[i]);
  printf("\n");

  t = clock();
  char *res = contributor_name(910180, qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Contributor name em %f segundos: %s\n\n", time_taken, res);

  t = clock();
  long* art = top_20_largest_articles(qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Demorou %f segundos em calcular os top 20 articles\n", time_taken);
  for(i=0; i<20; i++) printf("Artigo nº %d: %ld\n", i, art[i]);
  printf("\n");

  t = clock();
  res = article_title(334, qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Article title em %f segundos: %s\n\n", time_taken, res);

  t = clock();
  l = top_N_articles_with_more_words(15, qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Demorou %f segundos a calcular os top N articles with more words\n", time_taken);
  for(i = 0; i<15; i++) printf("ID nº %d: %ld\n", i+1, l[i]);
  printf("\n");

  t = clock();
  char** whatever = titles_with_prefix("Anax", qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Demorou %f segundos em calcular os titulos com prefixo Anax\n", time_taken);
  for(i=0; whatever[i]; i++) printf("Título com prefixo nº %d: %s\n", i, whatever[i]);
  printf("\n");

  t = clock();
  res = article_timestamp(10, 631144794, qs);
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Article timestamp em %f segundos: %s\n\n", time_taken, res);

  t=clock();
  qs = clean(qs);
  t=clock()-t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Demorou %f segundos a fazer clear à estrutura.\n\n", time_taken);
  return 0;
}
