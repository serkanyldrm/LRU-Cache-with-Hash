#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//belli bir sayýya kadar asal sayýlarý hesaplayýp bir dizide tutan fonksiyonda kullanýlan struct yapýsý
typedef struct{
	int sayi;
	int cikar;
}sayilar;
//cache yapýsýnýn oluþturulurken kullanýlan struct yapýsý
struct cache{
    char kimlikNo[10];
    char isim[20];
	char soyisim[20];
	int dogumTarihi;
	char adres[20];  
    struct cache *next;
};
//cache için oluþturulan linkli listenin head deðeri global olarak tanýmlanýr böylelikle cache'in baþýna ekleme yapýlýrken ulaþýlabilir.
struct cache *head = NULL;
//hash tablosunu oluþturan struct yapýsý
typedef struct {
    char kimlikNo[10];
    int cacheSirasi;
    int silindiMi;//o gözdeki verinin silinip silinmediðini tutan flag deðeri
}node;

int tabloUzunlugu(float cacheUzunlugu);//kullanýcýdan alýnan cache uzunluðu deðerinden tablo uzunluðunu hesaplayan fonksiyon
int h1(int key,int m);//h1 deðerini açýklayan fonksiyon
int h2(int key,int m);//h2 deðerini hesaplayan fonksiyon
int h(int key, int i, int m);//h1 ve h2 deðerleri ile hash deðerini hesaplayan fonksyion
void insertCache(char kimlikNo[],char isim[],char soyisim[],int dogumTarihi, char adres[]);//cachein baþýna ekleme yapan fonksiyon.
int hornerMethod(char kimlikNo[]);//horner metodu ile key deðerini hesaplayan fonksiyon
int isCacheFull(int cacheLimit);//cache'in full olup olmadýðýný döndüren fonksiyon
void insertHash(char kimlikNo[],int hash, int uzunluk, node hashTable[uzunluk]);//hash tablosuna ekleme yapan fonksiyon
void deleteCache(int uzunluk, node hashTable[uzunluk]);//cache tablosu dolduðu zaman son elemaný silen fonksiyon
void updateHash(int uzunluk, node hashTable[uzunluk]);//cache'e yeni eleman eklendiðinde sýralamalarý deðiþtiði zaman hash tablosunu güncelleyen fonksiyon
void switchHash(int uzunluk, node hashTable[uzunluk]);//cache te bulunan eleman baþa alýndýðý zaman sýralamarý hash tablosunda güncelleyen fonksiyon


int main(){
	FILE *fp;
	fp = fopen("test.txt","r");
	int kacAdet;// .txt dosyasýnda kaç adet veri olduðunun tutulduðu deðiþken.
	int i,j,k;
	int m;//tablo uzunluðu
	float cacheUz;//cache uzunluðu
	int sira;//cache'teki sýrasý
	int key;//horner metodu ile hesaplanan key deðeri
	int hash;//hash deðeri
	
	printf("Kac adet ornek var: ");
	scanf("%d",&kacAdet);
	printf("Cache uzunlugunu giriniz: ");
	scanf("%f",&cacheUz);
	if(cacheUz == 1){
		printf("1'den buyuk bir sayi giriniz!!\n");
		printf("Cache uzunlugunu giriniz: ");
		scanf("%f",&cacheUz);
	}
	
	m = tabloUzunlugu(cacheUz);//kullanýcýdan cache uzunluðu alýnýr ve tablo uzunluðu hesaplanýr.	
	node *hashTable =(node*) calloc(m,(sizeof(node)));//hash tablosu için memmory allokasyonu yapýlýr.
	//silindi deðerini tutan flag deðeri en baþta -1'e eþitlenir. -1 o göze daha önce hiç uðranmamýþ olmasý anlamýna gelmektedir.
	for(i=0;i<m;i++){
		hashTable[i].silindiMi = -1;
	}
	//cache'e eklenicek eleman için memory açýlýr.
	struct cache *new_node = (struct cache*)malloc((sizeof(struct cache)));
	//.txt dosyasýndan sýrasýyla okuma yapýlýr
	for(k=0;k<kacAdet;k++){
		fscanf(fp,"%s %s %s %d %s\n", new_node->kimlikNo,new_node->isim, new_node->soyisim, &new_node->dogumTarihi, new_node->adres);
		printf("\n---------------------------------------------------------------------------------\n");
		printf("\nDosyadan okunan bilgiler: %s %s %s %d %s\n", new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);
		//okunan kimlikNo önce horner metodundan sonra h fonksiyonuna yollanýr.
		key = hornerMethod(new_node->kimlikNo);
		i=0;
		hash = h(key,i,m);
		//kimlikNo hash tablosunda var mý aramasý yapýlýr.
		printf("\nKimlik No Bilgileri Hash Tablosunda Araniyor...\n");
		if(hashTable[hash].silindiMi == -1){//eðer hash deðerine daha önce hiç uðranmamýþsa daha fazla bakmadan yok diyebiliriz.
			printf("\n%s Hash tablosunda bulunmuyor\n",new_node->kimlikNo);
			if(!isCacheFull(cacheUz)){//cache tablosu dolu deðil ise baþa eklenir.
				insertCache(new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);//cache'in baþýna ekleme yapýlýr.
				updateHash(m,hashTable);//cache'teki tüm elemanlarýn sýrasý deðiþtiði için hash tablosunda da güncelleme yapar.
				insertHash(new_node->kimlikNo,hash,m,hashTable);//cache'in baþýna yeni eklenen eleman hash tablosuna da eklenir.
			}else{//eðer cache full ise cache'teki en eski eleman silindikten sonra baþa eklenir.
				deleteCache(m,hashTable);//önce cache'in sonundaki eleman silinir.
				insertCache(new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);
				updateHash(m,hashTable);
				insertHash(new_node->kimlikNo,hash,m,hashTable);	
			}
		}else{//ilk bakýlan hash deðerina daha önce uðrandýysa kimlikNo deðeri ayný ve silindiMi flag'i 0 veya hiç uðranmamýþ bir hash deðeri bulana kadar double hashing yapýlýr.
			while((strcmp(hashTable[hash].kimlikNo,new_node->kimlikNo) || hashTable[hash].silindiMi != 0) && hashTable[hash].silindiMi != -1 && i<=m){
				i++;
				hash = h(key,i,m);
			}
			if(hashTable[hash].silindiMi == -1 || i>m){//eðer hiç uðranmamýþ bir hash gözüne eriþilirse hash tablosunda yok diyebiliriz.
				printf("\nHash tablosunda bulunmuyor\n");
				i=0;
				hash = h(key,i,m);
				while(hashTable[hash].silindiMi != -1 && hashTable[hash].silindiMi != 1){//hash tablosunda olmadýðý bulunduktan sonra hash tablosunda yazýlacaðý yer bulunmalýdýr. i=0 dan baþlanarak silindiFlag'i 1 veyas -1 olan ilk yer bulunana kadar double hashing yapýlýr.
					i++;
					hash = h(key,i,m);
				}
				if(!isCacheFull(cacheUz)){//cache full deðil ise
				insertCache(new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);
				updateHash(m,hashTable);
				insertHash(new_node->kimlikNo,hash,m,hashTable);
				}else{//cache full ise
				deleteCache(m,hashTable);
				insertCache(new_node->kimlikNo,new_node->isim, new_node->soyisim, new_node->dogumTarihi, new_node->adres);
				updateHash(m,hashTable);
				insertHash(new_node->kimlikNo,hash,m,hashTable);
				}	
			}else{//eðer eleman bulunursa cache'te baþa alýnýr ve cache'teki tüm elemanlarýn sýrasý hash tablosunda güncellenir.
				printf("\n%s Hash tablosunda bulundu!!\n",new_node->kimlikNo);
				struct cache *iter;
				struct cache *new_node;
				iter = head;
				new_node = head;
				sira = hashTable[hash].cacheSirasi;
				//elemanýn cache'teki sýrasý hash tablosundan çekildikten sonra cache tablosunda gerekli yere ulaþýlýp bilgiler ekrana yazdýrýlýr.
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
				//elemanýn cache'teki sýrasý hash tablosundan çekildikten sonra ondan bir önceki elemanýn nextini bir sonraki elemana eþitleyerek o elemaný linkli listeden koparýrýz.
				for(j=1;j<sira-1;j++){
					new_node = new_node->next;
				}
				new_node->next = iter->next;
				insertCache(iter->kimlikNo, iter->isim,iter->soyisim,iter->dogumTarihi,iter->adres);//bulunan eleman cache'te baþa eklenir.
				switchHash(m,hashTable);//cache'teki tüm elemanlarýn yeri deðiþtiði için yerleri hash tablosunda da güncellenir.
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
	for(i=0;i<100;i++){//açýlan dizinin tüm elemanlarý doldurulur.
		Sayilar[i].cikar = 0;
		Sayilar[i].sayi = i;
	}
	for(i=2;i<50;i++){//dizinin yarýsýna kadar gidilir ve sýrasýyla her elemanýn katlarý iþaretlenir.
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
	return sayi;//tablo uzunluðuna denk gelen asal sayý döndürülür.	
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
	//cache'in baþýna eleman eklenir 
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
	hashTable[hash].silindiMi = 1;//silindiMi flag'i 1 yapýlýr
	hashTable[hash].cacheSirasi = -1;//silinen elemanýn cache sýrasi -1 olarak iþaretlenir.
	iter->next = NULL;
}

int isCacheFull(int cacheLimit){
	//cache'in full olup olmadýðý döndürülür.
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
	//horner metodu ile kimlikNo stringi key'e çevrilir.
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
	//hash tablosuna ekleme yapýlýr.
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
	//hash tablosunda cache sýralarýný günceller
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
	//cache'te eleman bulununca çaðýrýlan bu fonksiyon hash'teki sýralarý günceller.
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






