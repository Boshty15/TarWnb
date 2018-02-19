#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "file_structure.h"

void extract(char *filename)
{
	FILE *fOutput = NULL;
	fOutput = fopen(filename, "rb");

	if(fOutput == NULL){
        printf("Error extract branje datoteke!");
        return -1;
	}

	fseek(fOutput,0L,SEEK_END);
	unsigned long fSize = ftell(fOutput);
	fseek(fOutput, 0L, SEEK_SET);

	char *fileBuffer = (char*) malloc(fSize + 1);
	//memset(fileBuffer, 0, fSize);
	fread(fileBuffer, fSize, 1, fOutput);

	unsigned long currentPos = 0;
	char *pointer = fileBuffer;

	while (currentPos < fSize)
	{
		pointer = fileBuffer + currentPos;
		struct file_header* header = (struct file_header *) pointer;
		currentPos += sizeof(struct file_header);
		//printf("Folder name: %s\n",header->folderName);
		//printf("%s\n",&header->name);

		if(strcmp(header->isFolder,"true") == 0){
                 //fi = strtok(header_file_name,"/");

            char *newDirectory = malloc(header->folderName + 1);
            newDirectory= strtok(header->folderName,"/");
            sprintf(newDirectory, "%s%s", header->folderName,"extracted");
           // printf("New directory name: %s\n", newDirectory);

            mkdir(newDirectory,0770);

            DIR *dir;

             if ((dir = opendir (header->folderName)) != NULL){
               // printf("Directory odprt!\n");
                char *header_file_name = malloc(sizeof(header->name)+1);
                strcpy(header_file_name,header->name);
                //printf("2: %s\n",header_file_name);

                char *fi;
                fi = strtok(header_file_name,"/");
                fi = strtok(NULL,"/");
               // printf("FI: %s\n",fi);

                sprintf(newDirectory,"%s%s",newDirectory,"/");
                sprintf(newDirectory,"%s%s",newDirectory,fi);
               // printf("%s\n",newDirectory);


                FILE *newFile = NULL;
                char buffName[1000];
               // printf("1\n");
               // sprintf(buffName, "%s%s", header->name);

                //printf("Header->name: %s\n", header->name);
                //printf("File name: %s\n",header->name);
                newFile = fopen(newDirectory, "wb+");

                if(newFile == NULL){
                    printf("Error extract pisanje datoteke2!\n");
                    return -1;
                    }

                char *dataPointer = fileBuffer + currentPos;
                fwrite(dataPointer, header->data_length, 1, newFile);
                currentPos += header->data_length;

                fclose(newFile);

             }

		}else{
           // printf("is folder false!\n");


            FILE *newFile = NULL;
            char buffName[1000];
            sprintf(buffName, "%s%s", header->name, ".extracted");
            printf("File name: %s\n",header->name);
            //printf("%s\n", header->name);
            newFile = fopen(buffName, "wb");

            if(newFile == NULL){
                printf("Error extract pisanje datoteke!\n");
                return -1;
                }

            char *dataPointer = fileBuffer + currentPos;
            fwrite(dataPointer, header->data_length, 1, newFile);
            currentPos += header->data_length;

            fclose(newFile);


		}
	}
	fclose(fOutput);
}

void add_to_archive(char *file_name, struct file_header header,FILE *fOutput){
    //printf("ISFOLDER: %s\n",header.isFolder);
    if(strcmp(header.isFolder,"true") == 0){
        char *newFile_name = malloc(strlen(header.folderName) + strlen(file_name)+1);
        strcpy(newFile_name,header.folderName);
        strcat(newFile_name,file_name);
        strcpy(file_name,newFile_name);
        //header.isFolder = true;
        memset(header.isFolder,0,50);
        strcpy(header.isFolder,"true");
        //printf("New file name: %s\n", file_name);

    }
        FILE *f = NULL;
        f = fopen(file_name,"rb");
        //printf("File name: %s\n",file_name);

        if(f == NULL){
            printf("ERROR\n");
            return -1;
        }

        if(f != NULL){
            char buff[2048];

            memset(header.name,0,100);
            strcpy(header.name,file_name);

            //header.isFolder = false;
            //memset(header.folderName,0,100);
            fseek(f,0L,SEEK_END);
            header.data_length = ftell(f);
            fseek(f,0L, SEEK_SET);

            fwrite(&header,sizeof(struct file_header),1,fOutput);

            while(fgets(buff,2048,f) != NULL){
                fputs(buff,fOutput);
                //printf("%s\n",buff);
            }
            /*while(fread(buff,2048,0,f) != NULL){
                fwrite(buff,2048,0,f);
            }*/
           //printf("%d\n", header.data_length);
        }

        fclose(f);
}

void archive(char *path, int argumenti, int **argv){
        FILE *fOutput = NULL;
        fOutput = fopen(path,"wb");

        if(fOutput == NULL){
            printf("Error archive pisanje!\n");
            return -1;
        }

        struct file_header header;

        int i;
        for(i = 3; i < argumenti;i++){
            struct stat sb;

            DIR *dir;
            struct dirent *ent;

            char *pathMapa = argv[i];

            if(stat(pathMapa, &sb) == 0 && S_ISDIR(sb.st_mode)){
                //MAPA
               // printf("mapa\n");

                if ((dir = opendir (pathMapa)) != NULL) {
                  while ((ent = readdir (dir)) != NULL) {
                        if(strcmp(ent->d_name,".") != 0){
                            if(strcmp(ent->d_name,"..") != 0){
                                //branje mape
                                 //memset(header.isFolder,0,50);
                                 //header.isFolder = true;
                                 memset(header.folderName,0,100);
                                 strcpy(header.folderName,pathMapa);
                                 memset(header.isFolder,0,50);
                                strcpy(header.isFolder,"true");
                                 //printf("Folder name: %s\n",header.folderName);

                                 add_to_archive(ent->d_name,header,fOutput);
                                 //header.isFolder = true;
                                 /*if(header.isFolder){
                                    printf("true\n");
                                 }else{
                                    printf("false\n");
                                 }*/
                            }

                        }
                       // printf("2: %s\n", ent->d_name);
                  }

                  closedir (dir);
                }else{
                    printf("Error archive odpiranje mape!\n");
                    return -1;
                }
            }else{
                //FILE
                //printf("file\n");

                char *file_name = argv[i];
                //header.isFolder = false;
                 memset(header.isFolder,0,50);
                strcpy(header.isFolder,"false");
                add_to_archive(file_name,header,fOutput);
            }
        }
        fclose(fOutput);
        printf("Uspesno...\n");
}

void help(){
    printf("Program help\n");
    printf("archive: -z zipDatoteka (nastete datoteke za dodajanje v arhiv.)\n");
    printf("extract: -z zipDatoteka\n");
    printf("Primer archive: ./tarWnb -z test main.c document/\n");
    printf("Primer extract: ./tarWnb -u test\n");
}
int main(int argc, int **argv)
{
    bool zipPomoc = false;
    bool unZipPomoc = false;

    char* output_file;

    int argumenti = argc;

    int c;
    extern char *optarg;
    extern int optind;
    extern int optopt;

    while((c = getopt(argumenti, argv, ":zuh")) != -1){
            switch(c){
            case 'u':
                unZipPomoc = true;

                break;
            case 'z':
                zipPomoc = true;
                break;
            case 'h':
                help();
                break;
            }
        }

    if(zipPomoc == true){
        printf("ZIP\n");

    output_file = argv[2];
    //strcat(output_file,".tarwnb");
  //  printf("%s\n",output_file);

    archive(output_file,argumenti,argv);



    }
    else if(unZipPomoc == true){
        printf("UNZIP\n");
        //printf("%s\n",argv[2]);
        extract(argv[2]);
        printf("Unzip uspel...\n");

    }
    return 0;
}
