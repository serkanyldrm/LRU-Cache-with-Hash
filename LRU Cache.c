#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//belli bir say�ya kadar asal say�lar� hesaplay�p bir dizide tutan fonksiyonda kullan�lan struct yap�s�
typedef struct{
	int sayi;
	int cikar;
}sayilar;
//cache yap�s�n�n olu�turulurken kullan�lan struct yap�s�
struct cache{
    char kimlikNo[10];
    char isim[20];
	char soyisim[20];
	int dogumTarihi;
	char adres[20];  
    struct cache *next;
};
//cache i�in olu�turulan linkli listenin head de�eri global olarak tan�mlan�r b�ylelikle cache'in ba��na ekleme yap�l�rken ula��labilir.
struct cache *head = NULL;
//hash tablosunu olu�turan struct yap�s�
typedef struct {
    char kimlikNo[10];
    int cacheSirasi;
    int silindiMi;//o g�zdeki verinin silinip silinmedi�ini tutan flag de�eri
}node;

int tabloUzunlugu(float cacheUzunlugu);//kullan�c�dan al�nan cache uzunlu�u de�erinden tablo uzunlu�unu hesaplayan fonksiyon
int h1(int key,int m);//h1 de�erini a��klayan fonksiyon
int h2(int key,int m);//h2 de�erini hesaplayan fonksiyon
int h(int key, int i, int m);//h1 ve h2 de�erleri ile hash de�erini hesaplayan fonksyion
void insertCache(char kimlikNo[],char isim[],char soyisim[],int dogumTarihi, char adres[]);//cachein ba��na ekleme yapan fonksiyon.
int hornerMethod(char kimlikNo[]);//horner metodu ile key de�erini hesaplayan fonksiyon
int isCacheFull(int cacheLimit);//cache'in full olup olmad���n� d�nd�ren fonksiyon
void insertHash(char kimlikNo[],int hash, int uzunluk, node hashTable[uzunluk]);//hash tablosuna ekleme yapan fonksiyon
void deleteCache(int uzunluk, node hashTable[uzunluk]);//cache tablosu doldu�u zaman son eleman� silen fonksiyon
void updateHash(int uzunluk, node hashTable[uzunluk]);//cache'e yeni eleman eklendi�inde s�ralamalar� de�i�ti�i zaman hash tablosunu g�ncelleyen fonksiyon
void switchHash(int uzunluk, node hashTable[uzunluk]);//cache te bulunan eleman ba�a al�nd��� zaman s�ralamar� hash tablosunda g�ncelleyen fonksiyon


int main(){
	FILE *fp;
	fp = fopen("test.txt","r");
	int kacAdet;// .txt dosyas�nda ka� adet veri oldu�unun tutuldu�u de�i�ken.
	int i,j,k;
	int m;//tablo uzunlu�u
	float cacheUz;//cache uzunlu�u
	int sira;//cache'teki s�ras�
	int key;//horner metodu ile hesaplanan key de�eri
	int hash;//hash de�eri
	
	printf("Kac adet ornek var: ");
	scanf("%d",&kacAdet);
	printf("Cache uzunlugunu giriniz: ");
	scanf("%f",&cacheUz);
	if(cacheUz == 1){
		printf("1'den buyuk bir sayi giriniz!!\n");
		printf("Cache uzunlugunu giriniz: ");
		scanf("%f",&cacheUz);
	}
	
	m = tabloUzunlugu(cacheUz);//kullan�c�dan cache uzunlu�u al�n�r ve tablo uzunlu�u hesaplan�r.	
	node *hashTable =(node*) calloc(m,(sizeof(node)));//hash tablosu i�in memmory allokasyonu yap�l�r.
	//silindi de�erini tutan flag de�eri en ba�ta -1'e e�itlenir. -1 o g�ze daha �nce hi� u�ranmam�� olmas� anlam�na gelmektedir.
	for(i=0;i<m;i++){
		hashTable[i].silindiMi = -1;
	}
	//cache'e eklenicek eleman i�in memory a��l�r.
	struct cache *new_node = (struct cache*)malloc((sizeof(struct cache)));
	//.txt dosyas�ndan s�ras�yla okuma yap�l�r
	for(k=0;k<kacAdet;k++){
		fscanf(fp,"%s %s %s %d %s\n", new_node->kimlikNo,new_node->isim, new_node->soyisim, &new_node->dogumTarihi, new_node->adres);
		printf("\n---------------------------------------------------------------------------------\n");
		printf("\nDosyadan okunan bilgiler: %s %s %s %d %s\n", new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);
		//okunan kimlikNo �nce horner metodundan sonra h fonksiyonuna yollan�r.
		key = hornerMethod(new_node->kimlikNo);
		i=0;
		hash = h(key,i,m);
		//kimlikNo hash tablosunda var m� aramas� yap�l�r.
		printf("\nKimlik No Bilgileri Hash Tablosunda Araniyor...\n");
		if(hashTable[hash].silindiMi == -1){//e�er hash de�erine daha �nce hi� u�ranmam��sa daha fazla bakmadan yok diyebiliriz.
			printf("\n%s Hash tablosunda bulunmuyor\n",new_node->kimlikNo);
			if(!isCacheFull(cacheUz)){//cache tablosu dolu de�il ise ba�a eklenir.
				insertCache(new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);//cache'in ba��na ekleme yap�l�r.
				updateHash(m,hashTable);//cache'teki t�m elemanlar�n s�ras� de�i�ti�i i�in hash tablosunda da g�ncelleme yapar.
				insertHash(new_node->kimlikNo,hash,m,hashTable);//cache'in ba��na yeni eklenen eleman hash tablosuna da eklenir.
			}else{//e�er cache full ise cache'teki en eski eleman silindikten sonra ba�a eklenir.
				deleteCache(m,hashTable);//�nce cache'in sonundaki eleman silinir.
				insertCache(new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);
				updateHash(m,hashTable);
				insertHash(new_node->kimlikNo,hash,m,hashTable);	
			}
		}else{//ilk bak�lan hash de�erina daha �nce u�rand�ysa kimlikNo de�eri ayn� ve silindiMi flag'i 0 veya hi� u�ranmam�� bir hash de�eri bulana kadar double hashing yap�l�r.
			while((strcmp(hashTable[hash].kimlikNo,new_node->kimlikNo) || hashTable[hash].silindiMi != 0) && hashTable[hash].silindiMi != -1 && i<=m){
				i++;
				hash = h(key,i,m);
			}
			if(hashTable[hash].silindiMi == -1 || i>m){//e�er hi� u�ranmam�� bir hash g�z�ne eri�ilirse hash tablosunda yok diyebiliriz.
				printf("\nHash tablosunda bulunmuyor\n");
				i=0;
				hash = h(key,i,m);
				while(hashTable[hash].silindiMi != -1 && hashTable[hash].silindiMi != 1){//hash tablosunda olmad��� bulunduktan sonra hash tablosunda yaz�laca�� yer bulunmal�d�r. i=0 dan ba�lanarak silindiFlag'i 1 veyas -1 olan ilk yer bulunana kadar double hashing yap�l�r.
					i++;
					hash = h(key,i,m);
				}
				if(!isCacheFull(cacheUz)){//cache full de�il ise
				insertCache(new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);
				updateHash(m,hashTable);
				insertHash(new_node->kimlikNo,hash,m,hashTable);
				}else{//cache full ise
				deleteCache(m,hashTable);
				insertCache(new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);
				updateHash(m,hashTable);
				insertHash(new_node->kimlikNo,hash,m,hashTable);
				}	
			}else{//e�er eleman bulunursa cache'te ba�a al�n�r ve cache'teki t�m elemanlar�n s�ras� hash tablosunda g�ncellenir.
				printf("\n%s Hash tablosunda bulundu!!\n",new_node->kimlikNo);
				struct cache *iter;
				struct cache *new_node;
				iter = head;
				new_node = head;
				sira = hashTable[hash].cacheSirasi;
				//eleman�n cache'teki s�ras� hash tablosundan �ekildikten sonra cache tablosunda gerekli yere ula��l�p bilgiler ekrana yazd�r�l�r.
				if(hashTable[hash].cacheSirasi == 1){
					printf("\nBilgiler: \n%s %s %s %d %s\n",iter->kimlikNo,iter->isim,iter->soyisim,iter->dogumTarihi,iter->adres);
					printf("\nCache durumu:");
					while(iter!=NULL){
						printf("\n%s %s %s %d %s",iter->kimlikNo,iter->isim,iter->soyisim,iter->dogumTarihi,iter->adres);
						iter = iter->next;
					}
					printf("\n\nHash durumu:\n");
					for(i=0;i<m;i++){
						printf("indis: %d kimlikNo: %s cacheSira: %d silindiMi:%d\n",i,hashTable[i].kimlikNo, hashTable[i].cacheSirasi, hashTable[i].silindiMi);
					}
				}else{
					for(j=1;j<sira;j++){
						iter = iter->next;	
					}
				printf("\nBilgiler: \n%s %s %s %d %s\n",iter->kimlikNo,iter->isim,iter->soyisim,iter->dogumTarihi,iter->adres);
				//eleman�n cache'teki s�ras� hash tablosundan �ekildikten sonra ondan bir �nceki eleman�n nextini bir sonraki elemana e�itleyerek o eleman� linkli listeden kopar�r�z.
				for(j=1;j<sira-1;j++){
					new_node = new_node->next;
				}
				new_node->next = iter->next;
				insertCache(iter->kimlikNo, iter->isim,iter->soyisim,iter->dogumTarihi,iter->adres);//bulunan eleman cache'te ba�a eklenir.
				switchHash(m,hashTable);//cache'teki t�m elemanlar�n yeri de�i�ti�i i�in yerleri hash tablosunda da g�ncellenir.
				iter = head;	
				}
			}
		}	
	}
	free(head);
	free(hashTable);
	fclose(fp);
	return 0;
}

int tabloUzunlugu(float cacheUzunlugu){
	int i,j,k,iter,sayi;
	float loadFactor = 0.6;
	float table;
	table = cacheUzunlugu/loadFactor;
	int asalSayilar[100];
	sayilar Sayilar[100];
	for(i=0;i<100;i++){//a��lan dizinin t�m elemanlar� doldurulur.
		Sayilar[i].cikar = 0;
		Sayilar[i].sayi = i;
	}
	for(i=2;i<50;i++){//dizinin yar�s�na kadar gidilir ve s�ras�yla her eleman�n katlar� i�aretlenir.
		if(Sayilar[i].cikar != 1){
			k=i;
			iter=2;
			sayi = k * iter;
			while(sayi<=100){
				Sayilar[sayi].cikar = 1; 
				sayi = k * iter;
				iter++;
			}	
		}
	}
	k=0;
	for(i=0;i<100;i++){
		if(Sayilar[i].cikar != 1){
			asalSayilar[k] = Sayilar[i].sayi;
			k++;	
		}
	}
	i=0;
	sayi = asalSayilar[i];
	while(sayi<=table){
		i++;
		sayi = asalSayilar[i];
	}
	return sayi;//tablo uzunlu�una denk gelen asal say� d�nd�r�l�r.	
}

int h1(int key,int m){
	return key%m;
}
int h2(int key,int m){
	int m2;
	m2=m-1;
	return 1+(key%m2);
}
int h(int key, int i, int m){
	int h_1,h_2,hash;
	h_1 = h1(key,m);
	h_2 = h2(key,m);
	hash = (h_1+(i*(h_2)))%m;
	return hash;
}

void insertCache(char kimlikNo[],char isim[],char soyisim[],int dogumTarihi, char adres[]){
	//cache'in ba��na eleman eklenir 
	struct cache *node =(struct cache*) malloc((sizeof(struct cache)));
	strcpy(node->adres,adres);
	node->dogumTarihi = dogumTarihi;
	strcpy(node->isim,isim);
	strcpy(node->kimlikNo,kimlikNo);
	strcpy(node->soyisim,soyisim);
	node->next = head;
	head = node;
	struct cache *iter;
	iter = head;
	
	printf("\nCache durumu:");
	while(iter!=NULL){
		printf("\n%s %s %s %d %s",iter->kimlikNo,iter->isim,iter->soyisim,iter->dogumTarihi,iter->adres);
		iter = iter->next;
	}
}

void deleteCache(int uzunluk, node hashTable[uzunluk]){
	//cache'in sonundaki eleman silinir.
	char kimlik[10];
	int key;
	int i;
	int hash;
	struct cache *iter;
	iter = head;
	while(iter->next->next!=NULL){
		iter = iter->next;
	}
	strcpy(kimlik,iter->next->kimlikNo);
	key = hornerMethod(kimlik);
	i=0;
	hash = h(key,i,uzunluk);
	while(strcmp(kimlik,hashTable[hash].kimlikNo)){
		i++;
		hash = h(key,i,uzunluk);
	}
	hashTable[hash].silindiMi = 1;//silindiMi flag'i 1 yap�l�r
	hashTable[hash].cacheSirasi = -1;//silinen eleman�n cache s�rasi -1 olarak i�aretlenir.
	iter->next = NULL;
}

int isCacheFull(int cacheLimit){
	//cache'in full olup olmad��� d�nd�r�l�r.
	int i;
	struct cache *iter;
	iter = head;
	i=1;
	while(iter!=NULL && i<cacheLimit){
		iter=iter->next;
		i++;
	}
	if(iter == NULL){
		printf("\nCache Dolu Degil\n");
		return 0;
	}
	else{
		printf("\nCache Dolu\n");
		return 1;
	}
}

int hornerMethod(char kimlikNo[]){
	//horner metodu ile kimlikNo stringi key'e �evrilir.
	int i,k;
	int sayi,carpan;
	sayi = 0;
	carpan = 31;
	for(i=0;i<5;i++){
		sayi = sayi + pow(carpan,i)*(kimlikNo[i]-48);
	}
	return sayi;
}

void insertHash(char kimlikNo[],int hash,int uzunluk, node hashTable[uzunluk]){
	//hash tablosuna ekleme yap�l�r.
	struct cache *iter;
	char kimlik[10];
	int i;
	int found;
	int key;
	int hashYeni;
	iter = head;
	iter = iter->next;
	strcpy(hashTable[hash].kimlikNo,kimlikNo);
	hashTable[hash].silindiMi = 0;
	hashTable[hash].cacheSirasi = 1;
	printf("\n\nHash durumu:\n");
	for(i=0;i<uzunluk;i++){
		printf("indis: %d kimlikNo: %s cacheSira: %d silindiMi:%d\n",i,hashTable[i].kimlikNo, hashTable[i].cacheSirasi, hashTable[i].silindiMi);
	}
}

void updateHash(int uzunluk, node hashTable[uzunluk]){
	//hash tablosunda cache s�ralar�n� g�nceller
	struct cache *iter;
	int i;
	int key;
	int hash;
	iter = head;
	iter = iter->next;
	while(iter!=NULL){
		key = hornerMethod(iter->kimlikNo);
		i=0;
		hash = h(key,i,uzunluk);
		while(strcmp(hashTable[hash].kimlikNo,iter->kimlikNo)||hashTable[hash].silindiMi != 0){
			i++;
			hash = h(key,i,uzunluk);
		}
	
	hashTable[hash].cacheSirasi++;
	iter = iter->next;
	}
}

void switchHash(int uzunluk, node hashTable[uzunluk]){
	//cache'te eleman bulununca �a��r�lan bu fonksiyon hash'teki s�ralar� g�nceller.
	struct cache *iter;
	int i;
	int key;
	int hash;
	int sira;
	sira=1;
	iter = head;

	while(iter!=NULL){
		key = hornerMethod(iter->kimlikNo);
		i=0;
		hash = h(key,i,uzunluk);
	
		while(strcmp(hashTable[hash].kimlikNo,iter->kimlikNo)||hashTable[hash].silindiMi != 0){
			i++;
			hash = h(key,i,uzunluk);
		}
	
		hashTable[hash].cacheSirasi = sira;
		iter = iter->next;
		sira++;
	}
	printf("\n\nHash durumu:\n");
	for(i=0;i<uzunluk;i++){
		printf("indis: %d kimlikNo: %s cacheSira: %d silindiMi:%d\n",i,hashTable[i].kimlikNo, hashTable[i].cacheSirasi, hashTable[i].silindiMi);
	}
}






