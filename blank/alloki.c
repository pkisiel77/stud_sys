
void *alloc_list(void *(*L), long int size)
 {void *l, *next;
	if((*L)==NULL)
	 {(*L)=(void *)Malloc(size);
		if((*L)==NULL) return NULL;
		*((void **)(*L))=NULL; lml++;
		return (*L);
	 }
	l=(*L); next=*(void **)l;
	while(next!=NULL) {l=(void *)next; next=*(void **)l;}
	next=malloc(size); if(next==NULL) return next;
	(void *)(*(void **)l)=next;
	lml++;
  *(void **)next=NULL;
  return next;
 }

void free_list(void *(*L), void *lf)
 {void *l, *next, *lp=NULL;
	if((*L)==NULL) return;
  l=(*L); next=*(void **)l;
  while(next!=NULL && l!=lf)
   {lp=l; l=next; next=*(void **)l;}
  free(l); lml--;
  if(lp!=NULL) *((void **)lp)=next;
  else *L=next;
  return;
 }

void alloki()
 {void *ll, *next;
  int i=0, sum=0, size=1000;
  char *buf, *buf1;
  struct list *l=NULL;
fprintf(stderr,"\nPrzed alloc");
  do{my_alloc(buf,size);
     i++; sum+=PORC;
     if(i==1000)
      {fprintf(stderr," przepisz");
       PRZEPISZ(buf,"ala ma asa",11); buf1=buf;
fprintf(stderr," Po przepisz");
      }
    }while(buf !=NULL);
   fprintf(stderr,"\nZaalok. %d bajtow lmall=%d",sum,lml);
fprintf(stderr," %s",buf1);   
   my_free(buf1+2);
   l=Lista;
   if(l==NULL) exit(0);
   while(Lista!=NULL)
    {next=*(void **)l; free_list(&Lista,l); l=next;}
fprintf(stderr,". Lista=%d lmall=%d", Lista,lml);
fprintf(stderr,"\nPrzed alloc");
  lml=0; sum=0;
  do{l=alloc_list(&Lista,PORC); i++; sum+=PORC;}while(l !=NULL);
  fprintf(stderr,"\nZaalok. %d bajtow lmall=%d",sum,lml);
  l=Lista;
  while(Lista!=NULL)
   {next=*(void **)l; free_list(&Lista,l); l=next;}
fprintf(stderr,". Lista=%d lmall=%d", Lista,lml);
  exit(1);
 } 
