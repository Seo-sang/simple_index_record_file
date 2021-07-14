#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "person.h"
//�ʿ��� ��� ��� ���ϰ� �Լ��� �߰��� �� ����

// ���� ������� �����ϴ� ����� ���� �ٸ� �� ������ �ణ�� ������ �Ӵϴ�.
// ���ڵ� ������ ������ ������ ���� �����Ǳ� ������ ����� ���α׷����� ���ڵ� ���Ϸκ��� �����͸� �а� �� ����
// ������ ������ ����մϴ�. ���� �Ʒ��� �� �Լ��� �ʿ��մϴ�.
// 1. readPage(): �־��� ������ ��ȣ�� ������ �����͸� ���α׷� ������ �о�ͼ� pagebuf�� �����Ѵ�
// 2. writePage(): ���α׷� ���� pagebuf�� �����͸� �־��� ������ ��ȣ�� �����Ѵ�
// ���ڵ� ���Ͽ��� ������ ���ڵ带 �аų� ���ο� ���ڵ带 ���ų� ���� ���ڵ带 ������ ����
// ���� readPage() �Լ��� ȣ���Ͽ� pagebuf�� ������ ��, ���⿡ �ʿ信 ���� ���ο� ���ڵ带 �����ϰų�
// ���� ���ڵ� ������ ���� ��Ÿ�����͸� �����մϴ�. �׸��� �� �� writePage() �Լ��� ȣ���Ͽ� ������ pagebuf��
// ���ڵ� ���Ͽ� �����մϴ�. �ݵ�� ������ ������ �аų� ��� �մϴ�.
//
// ����: ������ �������κ��� ���ڵ�(���� ���ڵ� ����)�� �аų� �� �� ������ ������ I/O�� ó���ؾ� ������,
// ��� ���ڵ��� ��Ÿ�����͸� �����ϰų� �����ϴ� ��� ������ ������ ó������ �ʰ� ���� ���ڵ� ������ �����ؼ� ó���Ѵ�.

//
// ������ ��ȣ�� �ش��ϴ� �������� �־��� ������ ���ۿ� �о �����Ѵ�. ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
//
void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, (PAGE_SIZE * pagenum) + 16, SEEK_SET);
	if(fread(pagebuf, PAGE_SIZE, 1, fp) != 1) {
		fprintf(stderr, "fread error\n");
		exit(1);
	}
}

//
// ������ ������ �����͸� �־��� ������ ��ȣ�� �ش��ϴ� ���ڵ� ������ ��ġ�� �����Ѵ�. 
// ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, (PAGE_SIZE * pagenum) + 16, SEEK_SET);
	if(fwrite(pagebuf, PAGE_SIZE, 1, fp) != 1) {
		fprintf(stderr, "fwrite error\n");
		exit(1);
	}
}

//
// ���ο� ���ڵ带 ������ �� �͹̳ηκ��� �Է¹��� ������ Person ����ü�� ���� �����ϰ�, pack() �Լ��� ����Ͽ�
// ���ڵ� ���Ͽ� ������ ���ڵ� ���¸� recordbuf�� �����. 
// 
void pack(char *recordbuf, const Person *p)
{
	strcpy(recordbuf, p->id);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->name);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->age);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->addr);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->phone);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->email);
	strcat(recordbuf, "#");
}

// 
// �Ʒ��� unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� ���ڵ带 ����ü�� ��ȯ�� �� ����Ѵ�.
//
void unpack(const char *recordbuf, Person *p)
{
	char* buf = (char*)malloc(sizeof(recordbuf));
	strcpy(buf, recordbuf);
	char* ptr = strtok(buf, "#");
	strcpy(p->id, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->name, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->age, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->addr, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->phone, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->email, ptr);
	free(buf);
}

//
// ���ο� ���ڵ带 �����ϴ� ����� �����ϸ�, �͹̳ηκ��� �Է¹��� �ʵ尪���� ����ü�� ������ �� �Ʒ� �Լ��� ȣ���Ѵ�.
//
void add(FILE *fp, const Person *p)
{
	char header_area[16];
	char recordbuf[MAX_RECORD_SIZE];
	char pagebuf[PAGE_SIZE];
	memset(recordbuf, 0, sizeof(recordbuf));
	pack(recordbuf, p);
	int pages, records, pnum, rnum;
	fseek(fp, 0, SEEK_SET);//���� ���ڵ� �ּ�
	if(fread(header_area, sizeof(header_area), 1, fp) != 1) {
		fprintf(stderr, "fread error\n");
		exit(1);
	}
	memcpy(&pages, header_area, sizeof(int)); //��� ���ڵ� ���� �ҷ�����
	memcpy(&records, header_area+4, sizeof(int));
	memcpy(&pnum, header_area+8, sizeof(int));
	memcpy(&rnum, header_area+12, sizeof(int));

	if((pages == 0) && (records == 0)) { //������ �� ���
		pages++; records++;
		char inputpage[PAGE_SIZE];
		int newoffset = 0;
		int newlength = strlen(recordbuf);
		memset(inputpage, 0, sizeof(inputpage));
		int one = 1;
		memcpy(inputpage, &one, sizeof(one));
		memcpy(inputpage+4, &newoffset, sizeof(int));
		memcpy(inputpage+8, &newlength, sizeof(int));
		memcpy(inputpage + HEADER_AREA_SIZE, recordbuf, strlen(recordbuf));
		writePage(fp, inputpage, pages - 1);
		memcpy(header_area, &pages, sizeof(int));
		memcpy(header_area + 4, &records, sizeof(int));
		fseek(fp, 0, SEEK_SET);
		fwrite(header_area, sizeof(header_area), 1, fp);
	}
	else if((pnum == -1) &&(rnum == -1)) { //append �ϴ� ���
		readPage(fp, pagebuf, pages-1);
		int pagerecords;
		memcpy(&pagerecords, pagebuf, sizeof(int));
		int offset, length;
		memcpy(&offset, pagebuf+8 * (pagerecords-1) + 4, sizeof(int));
		memcpy(&length, pagebuf+8 * (pagerecords-1) + 8, sizeof(int));
		if(((HEADER_AREA_SIZE - 4) / 8) == pagerecords || (DATA_AREA_SIZE - (offset+length)) < strlen(recordbuf)) { //page ���� �Ҵ��ϴ� ���
			char inputpage[PAGE_SIZE];
			int newoffset = 0, newlength = strlen(recordbuf);
			memset(inputpage, 0, sizeof(inputpage));
			int one = 1;
			memcpy(inputpage, &one, sizeof(one));
			memcpy(inputpage+4, &newoffset, sizeof(int));
			memcpy(inputpage+8, &newlength, sizeof(int));
			memcpy(inputpage + HEADER_AREA_SIZE, recordbuf, strlen(recordbuf));
			pages++;
			writePage(fp, inputpage, pages-1);
			records++;
			memcpy(header_area, &pages, sizeof(int));
			memcpy(header_area +4, &records, sizeof(int));
			fseek(fp, 0, SEEK_SET);
			fwrite(header_area, sizeof(header_area), 1, fp);
		}
		else { //page�� �߰�
			int newoffset, newlength;
			newoffset = offset+length;
			newlength = strlen(recordbuf);
			memcpy(pagebuf + 4 + (pagerecords * 8), &newoffset, sizeof(newoffset));
			memcpy(pagebuf + 8 + (pagerecords * 8), &newlength, sizeof(newlength));
			pagerecords++;
			memcpy(pagebuf, &pagerecords, sizeof(records));
			memcpy(pagebuf + HEADER_AREA_SIZE + (offset+length), recordbuf, newlength);
			writePage(fp, pagebuf, pages-1);
			records++;
			memcpy(header_area + 4, &records, sizeof(int));
			fseek(fp, 0, SEEK_SET);
			fwrite(header_area, sizeof(header_area), 1, fp);
		}
	}
	else { //�������ڵ� Ȯ���ϴ� ���
		int prevpage = -2, prevrecord = -2;
		char inputpage[PAGE_SIZE];
		char pagebuf[PAGE_SIZE];
		char prevbuf[PAGE_SIZE];
		char header_area[16];
		memset(header_area, 0, sizeof(header_area));
		memset(inputpage, 0, sizeof(inputpage));
		memset(pagebuf, 0, sizeof(inputpage));
		memset(prevbuf, 0, sizeof(prevpage));
		int pagerecord, del_length, del_offset;
		while((pnum != -1) && (rnum != -1)) { //������ ���ڵ�� ��ȯ�ϸ� Ȯ��
			readPage(fp, pagebuf, pnum); //�ش� ������ ����
			memcpy(&del_offset, pagebuf +4 + (8 * rnum), sizeof(int)); //offset�� length Ȯ��
			memcpy(&del_length, pagebuf + 8 + (8 * rnum), sizeof(int));
			if(del_length < strlen(recordbuf)) { //���̰� ���ڶ��
				prevpage = pnum;
				prevrecord = rnum;
				memcpy(&pnum, pagebuf + HEADER_AREA_SIZE + del_offset + 1, sizeof(int));//���� ��ġ�� �̵�
				memcpy(&rnum, pagebuf + HEADER_AREA_SIZE + del_offset + 5, sizeof(int));
				continue;
			}
			else {//���̰� ������
				if((prevpage == -2) && (prevrecord == -2)) { //���� ��尡 header record�� ���
					fseek(fp, 0, SEEK_SET);
					fread(header_area, sizeof(header_area), 1, fp);
					memcpy(header_area + 8, pagebuf + HEADER_AREA_SIZE +del_offset + 1, sizeof(int));//������ ��ġ ����� ����
					memcpy(header_area + 12, pagebuf + HEADER_AREA_SIZE + del_offset + 5, sizeof(int));
					fseek(fp, 0, SEEK_SET);
					fwrite(header_area, sizeof(header_area), 1, fp);
				}
				else { //���� ��尡 header record�� �ƴ� ���
					int off, len;
					if(prevpage != pnum) { //���� �������� ���� �������� �ٸ� ���
						readPage(fp, prevbuf, prevpage);
						int pp = 0, pr = 0;
						memcpy(&off, prevbuf +4 + (8*prevrecord), sizeof(int));
						memcpy(&len, prevbuf +8 + (8*prevrecord), sizeof(int));
						memcpy(&pp, pagebuf + HEADER_AREA_SIZE + del_offset + 1, sizeof(int));
						memcpy(&pr, pagebuf + HEADER_AREA_SIZE + del_offset + 5, sizeof(int));
						memcpy(prevbuf + HEADER_AREA_SIZE + off + 1, &pp, sizeof(int));
						memcpy(prevbuf + HEADER_AREA_SIZE + off + 5, &pr, sizeof(int));
						writePage(fp, prevbuf, prevpage);
					}
					else {
						memcpy(&off, pagebuf + 4 + (8 * prevrecord), sizeof(int));
						memcpy(pagebuf + HEADER_AREA_SIZE + off + 1, pagebuf+HEADER_AREA_SIZE + del_offset + 1, sizeof(int));
						memcpy(pagebuf + HEADER_AREA_SIZE + off + 5, pagebuf + HEADER_AREA_SIZE + del_offset + 5, sizeof(int));
					}
				}
				memcpy(pagebuf + HEADER_AREA_SIZE + del_offset, recordbuf, strlen(recordbuf));
				writePage(fp, pagebuf, pnum);
				break;
			}
		}
		if((pnum == -1) && (rnum == -1)) {
			memset(pagebuf, 0, sizeof(pagebuf));
			readPage(fp, pagebuf, pages-1);
			int pagerecords;
			memcpy(&pagerecords, pagebuf, sizeof(int));
			int offset, length;
			memcpy(&offset, pagebuf + 8 * (pagerecords-1) + 4, sizeof(int));
			memcpy(&length, pagebuf + 8 * (pagerecords-1) + 8, sizeof(int));
			fseek(fp, 0, SEEK_SET);
			fread(header_area, sizeof(header_area), 1, fp);
			if(((HEADER_AREA_SIZE - 4) / 8) == pagerecords ||(DATA_AREA_SIZE - (offset+length)) < strlen(recordbuf)) { //page ���� �Ҵ��ϴ� ���
				char inputpage[PAGE_SIZE];
				int newoffset = 0, newlength = strlen(recordbuf);
				memset(inputpage, 0, sizeof(inputpage));
				int one = 1;
				memcpy(inputpage, &one, sizeof(one)); //������ page�� �߰�
				memcpy(inputpage+4, &newoffset, sizeof(int));
				memcpy(inputpage+8, &newlength, sizeof(int));
				memcpy(inputpage + HEADER_AREA_SIZE, recordbuf, strlen(recordbuf));
				writePage(fp , inputpage, pages);
				pages++;
				records++;
				memcpy(header_area, &pages, sizeof(int));
				memcpy(header_area +4, &records, sizeof(int));
				fseek(fp, 0, SEEK_SET);
				fwrite(header_area, sizeof(header_area), 1, fp);
			}
			else { //page�� �߰�
				int newoffset, newlength;
				newoffset = offset+length;
				newlength = strlen(recordbuf);
				memcpy(pagebuf + 4 + (pagerecords * 8), &newoffset, sizeof(newoffset));
				memcpy(pagebuf + 8 + (pagerecords * 8), &newlength, sizeof(newlength));
				pagerecords++;
				memcpy(pagebuf, &pagerecords, sizeof(pagerecords));
				memcpy(pagebuf + HEADER_AREA_SIZE + (offset+length), recordbuf, newlength);
				writePage(fp, pagebuf, pages-1);
				records++;
				memcpy(header_area+4, &records, sizeof(int));
				fseek(fp, 0, SEEK_SET);
				fwrite(header_area, sizeof(header_area), 1, fp);
			}
		}
	}
}

//
// �ֹι�ȣ�� ��ġ�ϴ� ���ڵ带 ã�Ƽ� �����ϴ� ����� �����Ѵ�.
//
void delete(FILE *fp, const char *id)
{
	char header_area[16];
	char pagebuf[PAGE_SIZE];
	char recordbuf[MAX_RECORD_SIZE];
	int pages, pagerecords, pnum, rnum;
	int now = 0;
	char delbuf[9];
	char pid[14];
	char* ptr;
	memset(pid, 0, sizeof(pid));
	strcpy(delbuf, "*");
	memset(header_area, 0, sizeof(header_area));
	fseek(fp, 0, SEEK_SET);
	fread(header_area, sizeof(header_area), 1, fp);
	memcpy(&pages, header_area, sizeof(int));
	memcpy(&pnum, header_area + 8, sizeof(int));
	memcpy(&rnum, header_area + 12, sizeof(int));
	for(int i = 0; i < pages; i++) {
		readPage(fp, pagebuf, i);
		memcpy(&pagerecords, pagebuf, sizeof(int));
		int offset, length;
		for(int j = 0; j < pagerecords; j++) {
			memcpy(&offset, pagebuf + (j*8) + 4, sizeof(int));
			memcpy(&length, pagebuf + (j*8) + 8, sizeof(int));
			memset(recordbuf, 0, sizeof(recordbuf));
			memcpy(recordbuf, pagebuf + offset + HEADER_AREA_SIZE, length);
			if(recordbuf[0] == '*') continue;
			ptr = strtok(recordbuf, "#");
			strcpy(pid, ptr);
			if(strcmp(pid, id) == 0) {
				memcpy(delbuf + 1, &pnum, sizeof(int));
				memcpy(delbuf + 5, &rnum, sizeof(int));
				memcpy(pagebuf + offset + HEADER_AREA_SIZE, delbuf, sizeof(delbuf));
				writePage(fp, pagebuf, i);
				memcpy(header_area+8, &i, sizeof(int));
				memcpy(header_area+12, &j, sizeof(int));
				fseek(fp, 0, SEEK_SET);
				fwrite(header_area, sizeof(header_area), 1, fp);
				return;
			}
		}
	}
	return;
}


void swap(char* a, char* b) {
	char tmp[21];
	memcpy(tmp, a, 21);
	memcpy(a, b, 21);
	memcpy(b, tmp, 21);
}

//
// �־��� ���ڵ� ����(recordfp)�� �̿��Ͽ� ���� �ε��� ����(idxfp)�� �����Ѵ�.
//
void createIndex(FILE *idxfp, FILE *recordfp)
{
	int count = 0;
	int pages, records, pagerecords, midx;
	long long mnum, now;
	char key[14];
	char* ptr;
	char header_area[16];
	char pagebuf[PAGE_SIZE];
	char recordbuf[MAX_RECORD_SIZE];
	char **sortrecord;
	memset(key, 0, sizeof(key));
	memset(pagebuf, 0, sizeof(pagebuf));
	memset(header_area, 0, sizeof(header_area));
	fseek(recordfp, 0, SEEK_SET);
	fread(header_area, sizeof(header_area), 1, recordfp);
	memcpy(&pages, header_area, sizeof(int));
	memcpy(&records, header_area +sizeof(int), sizeof(int));
	sortrecord = (char**)malloc(sizeof(char*)*pages*records);
	for(int i = 0; i < pages*records; i++) {
		sortrecord[i] = (char*)malloc(sizeof(char)*21);
		memset(sortrecord[i], 0, sizeof(sortrecord[i]));
	}
	for(int i = 0; i < pages; i++) {
		readPage(recordfp, pagebuf, i);
		memcpy(&pagerecords, pagebuf, sizeof(int));
		int offset, length;
		for(int j = 0; j < pagerecords; j++) {
			memcpy(&offset, pagebuf + (j*8) + 4, sizeof(int));
			memcpy(&length, pagebuf + (j*8) + 8, sizeof(int));
			memset(recordbuf, 0, sizeof(recordbuf));
			memcpy(recordbuf, pagebuf + offset + HEADER_AREA_SIZE, length);
			if(recordbuf[0] == '*') continue;
			ptr = strtok(recordbuf, "#");
			strcpy(sortrecord[count], ptr);
			memcpy(sortrecord[count] + 13, &i, sizeof(int));
			memcpy(sortrecord[count] + 13 + sizeof(int), &j, sizeof(int));
			count++;
		}
	}

	for(int i = 0; i < count-1; i++) {
		midx = i;
		memcpy(key, sortrecord[i], 13);
		key[sizeof(key)] = '\0';
		mnum = strtoll(key,NULL, 10);
		for(int j = i + 1; j < count; j++) {
			memcpy(key, sortrecord[j], 13);
			key[sizeof(key)] = '\0';
			now = strtoll(key, NULL, 10);
			if(mnum > now) {
				midx = j;
				mnum = now;
			}
		}
		if(midx != i)
			swap(sortrecord[midx], sortrecord[i]);
	}
	fseek(idxfp, 0, SEEK_SET);
	fwrite(&count, sizeof(int), 1, idxfp);
	for(int i = 0; i < count; i++) {
		fwrite(sortrecord[i], 21, 1, idxfp);
	}
}

//
// �־��� ���� �ε��� ����(idxfp)�� �̿��Ͽ� �ֹι�ȣ Ű���� ��ġ�ϴ� ���ڵ��� �ּ�, �� ������ ��ȣ�� ���ڵ� ��ȣ�� ã�´�.
// �̶�, �ݵ�� ���� �˻� �˰����� ����Ͽ��� �Ѵ�.
//
void binarysearch(FILE *idxfp, const char *id, int *pageNum, int *recordNum)
{
	int left = 0, right;
	long long searchid = strtoll(id, NULL, 10);
	long long keyid;
	int mid, count, N = 0;
	char record[21];
	char key[14];
	fseek(idxfp, 0, SEEK_SET);
	fread(&count, sizeof(int), 1, idxfp);
	right = count-1;
	while(left <= right) {
		N++;
		mid = (left + right) / 2;
		fseek(idxfp, sizeof(int) + (mid*21), SEEK_SET);
		fread(record, 21, 1, idxfp);
		memcpy(key, record, 14);
		key[13] = '\0';
		keyid = strtoll(key, NULL, 10);
		if(keyid == searchid) {
			memcpy(pageNum, record + 13, sizeof(int));
			memcpy(recordNum, record + 13 + sizeof(int), sizeof(int));
			printf("#reads:%d\n",N);
			break;
		}
		else if(keyid < searchid) {
			left = mid+1;
		}
		else if(keyid > searchid) {
			right = mid-1;
		}
	}
	if((*pageNum == -1) && (*recordNum == -1)) {
		printf("#reads:%d\n", N);
	}
	return;
}


int main(int argc, char *argv[])
{
	FILE *fp;  // ���ڵ� ������ ���� ������
	FILE *fp2;
	int pageNum = -1, recordNum = -1, offset, length;
	Person p;
	char header_area[16];
	char pagebuf[PAGE_SIZE];
	char* recordbuf;
	memset(header_area, 0, sizeof(header_area));
	if((argc != 9) && (argc != 4) && (argc != 5)) {
		fprintf(stderr, "%d, usage : %s a <record file name> <field values list> | %s d <record file name> <field value>\n", argc, argv[0], argv[0]);
		exit(1);
	}
	if(argv[1][0] == 'a') {
		if(access(argv[2], F_OK) < 0) {
			if((fp = fopen(argv[2], "w+")) < 0) {
				fprintf(stderr, "%s file open error\n", argv[2]);
				exit(1);
			}
			int zeronum = 0;
			int onenum = -1;
			memcpy(header_area, &zeronum, sizeof(int));
			memcpy(header_area+sizeof(int), &zeronum, sizeof(int));
			memcpy(header_area+(2*sizeof(int)), &onenum, sizeof(int));
			memcpy(header_area+(3*sizeof(int)), &onenum, sizeof(int));
			fwrite(header_area, sizeof(header_area), 1, fp);
		}
		else {
			if((fp = fopen(argv[2], "r+")) < 0) {
				fprintf(stderr, "%s file open error\n", argv[2]);
				exit(1);
			}
		}
		strcpy(p.id, argv[3]);
		strcpy(p.name, argv[4]);
		strcpy(p.age, argv[5]);
		strcpy(p.addr, argv[6]);
		strcpy(p.phone, argv[7]);
		strcpy(p.email, argv[8]);
		add(fp, &p);
		fclose(fp);
	}
	else if(argv[1][0] == 'd') {
		if(access(argv[2] , F_OK) < 0) {
			fprintf(stderr, "%s is not exist. You can't find record.\n", argv[2]);
			exit(1);
		}
		else {
			if((fp = fopen(argv[2], "r+")) < 0) {
				fprintf(stderr, "%s file open error\n", argv[2]);
				exit(1);
			}
			delete(fp, argv[3]);
			fclose(fp);
		}
	}
	else if(argv[1][0] == 'i') {
		if(access(argv[2], F_OK) < 0) {
			fprintf(stderr, "%s is not exist.\n", argv[2]);
			exit(1);
		}
		else {
			if((fp = fopen(argv[2], "r+")) < 0) {
				fprintf(stderr, "%s file open error\n", argv[2]);
				exit(1);
			}
			if((fp2 = fopen(argv[3], "w+")) <0) {
				fprintf(stderr, "%s file open error\n", argv[3]);
				exit(1);
			}
			createIndex(fp2, fp);
			fclose(fp);
			fclose(fp2);
		}
	}
	else if(argv[1][0] == 'b') {
		if(access(argv[2], F_OK) < 0) {
			fprintf(stderr, "%s is now exist.\n", argv[2]);
			exit(1);
		}
		else {
			if((fp = fopen(argv[3], "r+")) < 0) {
				fprintf(stderr, "%s file open error\n", argv[3]);
				exit(1);
			}
			if((fp2 = fopen(argv[2], "r+")) < 0) {
				fprintf(stderr, "%s file open error\n", argv[2]);
				exit(1);
			}
			binarysearch(fp, argv[4], &pageNum, &recordNum);
			if(pageNum == -1 && recordNum == -1) {
				printf("no persons\n");
			}
			else {
				readPage(fp2, pagebuf, pageNum);
				memcpy(&offset, pagebuf + (recordNum*8) + sizeof(int), sizeof(int));
				memcpy(&length, pagebuf + (recordNum*8) + (2*sizeof(int)), sizeof(int));
				recordbuf = (char*)malloc(sizeof(char)*length);
				memcpy(recordbuf, pagebuf + HEADER_AREA_SIZE + offset, length);
				unpack(recordbuf, &p);
				printf("id=%s\n",p.id);
				printf("name=%s\n",p.name);
				printf("age=%s\n",p.age);
				printf("addr=%s\n",p.addr);
				printf("phone=%s\n",p.phone);
				printf("email=%s\n",p.email);
				free(recordbuf);
			}
			fclose(fp);
		}
	}

	return 1;
}
