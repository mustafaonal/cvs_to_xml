#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <ctype.h>
#define COMMA ','
#define TAB '\t'
#define SEMICOLON ';'
#define WINDOWS 1
#define LINUX 2
#define MACOS 3


void csv2xml_converter(char* file_name, char separator, int opsys)//tüm işlemlerin yapıldığı fonksiyon
{
    FILE *file; //csv dosyası için
    FILE *file2; //xml dosyası için

    char words; //dosyadan alınan verileri okumak için
    int line = 0; //row id ve column header işlemleri için satır sayısını tutuyor
    char column_header[20][30]; //ilk satırda yazılanları elementlerin içinde tekrardan çağırmak için
    char xml_file_name[strlen(file_name)]; //oluşturulan xml dosyasının adı
    char root_element[strlen(file_name) - 4];

    strncpy(root_element, file_name, strlen(file_name)-4); //uzantı kısmını atıp dosya ismiyle aynı isimde root oluşturuyor
    root_element[strlen(file_name) - 4] = '\0';

    strncpy(xml_file_name, file_name, strlen(file_name)-4); //csv dosyasının adını alıp xml dosyasına kopyalıyor ve .xml kısmını ekliyor
    xml_file_name[strlen(file_name)-4] = '.';
    xml_file_name[strlen(file_name)-3] = 'x';
    xml_file_name[strlen(file_name)-2] = 'm';
    xml_file_name[strlen(file_name)-1] = 'l';
    xml_file_name[strlen(file_name)] = '\0';
   
    if ((file = fopen(file_name, "r")) == NULL) //okumak için csv dosyası açılıyor
    {
        printf("Cannot open file!\n"); // Program exits if file pointer returns NULL
        exit(1);
    }

    if((file2 = fopen(xml_file_name, "w")) == NULL) //yazmak için xml dosyası oluşturuluyor
    {
        printf("Cannot open file!\n"); // Program exits if file pointer returns NULL
        exit(1);
    }


    fprintf(file2, "<%s>\n", root_element); //en baştan root element i yazıyor
    words = fgetc(file); //ilk harfi alıyor
    while(words != EOF) //read file word by word and finish loop when see end of line character
    {
        int separator_num = 0; //ilk line da char array e elementleri atamak ve diğer linelarda elementleri doğru şekilde yerleştirmek için kullanılıyor
        int count = 0; //ilk line daki element isimlerini array e doğru şekilde yerleştirebilmek için kullanılıyor
        
        if(line != 0)//row id leri yazıyor
        {
            fprintf(file2, "    <row id=\"%d\">\n", line);
        }
        if(opsys == WINDOWS || opsys == MACOS) //windows ve macos da satır sonlarında 'r' karakteri olduğu için aynı while içinde çalışıyor
        {
            while(words != '\r') //her satırda bir dönen loop
            {
                bool flag = true; //içeride seperator a göre loop olduğu için 'r' görüp görmediğini kontrol ediyor
                int empty_element = 0; //boş element olduğunda tagler farklı yazıldığı için, onu kontrol ediyor
                if (line != 0)
                {
                    if (words == separator)
                    {
                        empty_element++;
                    }else
                    {
                        fprintf(file2, "        <%s>", column_header[separator_num]);//tag açıyor
                    }
                }
                if(line == 0)//elementleri column header a eklemek için
                {
                    column_header[separator_num-1][count] = '\0';//bir önceki element name in sonuna \0 ekliyor
                    count = 0;                                   //ve count u sıfırlıyor, çünkü seperator görülmüş ve yeni element okunmaya başlanacak
                }
                while (words != separator)//seperator görene kadar dönüyor
                {
                    if(words == '\r')//burada line sonu görürse break veriyor
                    {                //satırın sonundaki elementin sonunda seperator yerine, 'r' gördüğü için
                        flag = false;
                        break;
                    }else if(line != 0)//harfleri tag in içine fgetc ile tek tek alarak yazıyor
                    {
                        fprintf(file2, "%c", words);
                        words = fgetc(file);
                    }else if(line == 0) //elementleri column header a eklemek için
                    {
                        if(words == ' ') //boşluk karakteri yerine alt çizgi atıyor. Örneğin first name --> first_name
                            column_header[separator_num][count] = '_'; //count sıfırdan başlıyor ve her harfde artıyor
                        else
                            column_header[separator_num][count] = tolower(words);//for example First_Name --> first_name
                        
                        count++;
                        words = fgetc(file);
                    }         
                }
                if(words != '\r')//dosyadan yeni harf alıyor
                {
                    words = fgetc(file);
                }
                if(line != 0)//kapanış tag lerini eklemek için
                {
                    if(empty_element != 0)//normal elementlerin kapanış tagini yazıyor
                    {
                        fprintf(file2, "        <%s/>", column_header[separator_num]);
                    }else//empty element in tag i yazılıyor
                    {
                        fprintf(file2, "</%s>", column_header[separator_num]);
                    }
                    fprintf(file2, "\n");//yeni satıra geçileceği için
                }
                separator_num++;
                if(flag == true && words == '\r')//satırın en sonundaki elementleri doğru yazmak için
                {
                    fprintf(file2, "        <%s/>\n", column_header[separator_num]);
                }
            }   
            if(line != 0)//row kapanış tag i
            {
                fprintf(file2, "    </row>\n");
            }
        }else if(opsys == LINUX)//burada linux ve nacos un içinde yazandan farklı olarak kontrolleri '\n' ile yapıyor
        {                       //bunun dışında yazan diğer her şey yukarıdaki if ile aynı
           while(words != '\n')
            {
                bool flag = true;
                int empty_element = 0;
                if (line != 0)
                {
                    if (words == separator)
                    {
                        empty_element++;
                    }else
                    {
                        fprintf(file2, "        <%s>", column_header[separator_num]);
                    }
                }
                if(line == 0)
                {
                    column_header[separator_num-1][count] = '\0';
                    count = 0;
                }
                while (words != separator)
                {
                    if(words == '\n')
                    {
                        flag = false;
                        break;
                    }else if(line != 0)
                    {
                        fprintf(file2, "%c", words);
                        words = fgetc(file);
                    }else if(line == 0) 
                    {
                        if(words == ' ')
                            column_header[separator_num][count] = '_';
                        else
                            column_header[separator_num][count] = tolower(words);
                        
                        count++;
                        words = fgetc(file);
                    }         
                }
                if(words != '\n')
                {
                    words = fgetc(file);
                }
                if(line != 0)
                {
                    if(empty_element != 0)
                    {
                        fprintf(file2, "        <%s/>", column_header[separator_num]);
                    }else
                    {
                        fprintf(file2, "</%s>", column_header[separator_num]);
                    }
                    fprintf(file2, "\n");
                }
                separator_num++;
                if(flag == true && words == '\n')
                {
                    fprintf(file2, "        <%s/>\n", column_header[separator_num]);
                }
            }   
            if(line != 0)
            {
                fprintf(file2, "    </row>\n");
            }
        }
        if(opsys == WINDOWS) //windows da /r/n yani iki karakter olduğu için bir tane fazla karakter alıyor
        {
            words = fgetc(file);
        }
        words = fgetc(file);//satır sonunda bulunan karakteri geçip diğer satırın ilk karakterini görmek için
        line++;
    }
    fprintf(file2, "</%s>\n", root_element); //root uun kapanış tag i
    fclose(file);
    fclose(file2);
}


int main(int argc, char *argv[])
{
    if(strcmp(argv[1], "-h") == 0) //print information to screen about how other arguments can be used
    {
        printf("\nCSV2XML <inputfile> <outputfile> [-separator <P1>][-opsys <P2>]\n");
        printf("first argument must be INPUTFILE NAME(csv file)\n");
        printf("second argument must be OUTPUTFILE NAME(xml file)\n");
        printf("Warning!!! inpufile and outputfile name must be same(not extensions)\n");
        printf("third argumnet must be -separator\n");
        printf("fourth argument can be 1,2 or 3\n");
        printf("1 stan for comma, 2 stan for tab and 3 stan for semicolon\n");
        printf("fifth argument must be -opsys\n");
        printf("sixth argument can be 1,2 or 3\n");
        printf("1 stan for windows, 2 stan for linux and 3 stan for macos\n");
        exit(1);
    }

    char separator;//seperator ve opsys kontrolü için
    int opsys;     //ikisini fonksiyona gönderiyor

    if(strlen(argv[1]) != strlen(argv[2])) //input ve output dosya isimleri aynı uzunlıkta olmalı
    {
        printf("\nInput file name and Output file name must be same(not extensions). You may use -h commnet to help.\n");
        exit(1);
    }

    char csv_file_name[strlen(argv[1])-4];//input file ın uzantısını atıp kuraya yazıyor
    char xml_file_name[strlen(argv[2])-4];//output file ın uzantısını atıp buraya yazıyor
    strncpy(csv_file_name, argv[1], strlen(argv[1])-4);
    strncpy(xml_file_name, argv[2], strlen(argv[2])-4);
    csv_file_name[strlen(argv[1])-4] = '\0';
    xml_file_name[strlen(argv[2])-4] = '\0';

    if(strcmp(csv_file_name, xml_file_name) != 0) //input ve output dosyaları aynı isimde olması gerekiyor
    {
        printf("\nInput file name and Output file name must be same(not extensions). You may use -h commnet to help.\n");
        exit(1);
    }



    if(atoi(argv[4]) == 1)//seperator ı kontrol ediyor ve atamasını yapıyor
    {
        separator = COMMA;
    }else if(atoi(argv[4]) == 2)
    {
        separator = TAB;
    }else if(atoi(argv[4]) == 3)
    {
        separator = SEMICOLON;
    }else{
        printf("\nError! Wrong Command Line Argument Passed. You may use -h commnet to help.\n");
        exit(1);
    }

    //argumanların doğru girilip girilmediğini kontrol ediyor
    if(strcmp(argv[3], "-separator") != 0)
    {
        printf("\nError! Wrong Command Line Argument Passed. You may use -h commnet to help.\n");
        exit(1);
    }

    if(strcmp(argv[5], "-opsys") == 0 && atoi(argv[6]) == 1 || strcmp(argv[5], "-opsys") == 0 && atoi(argv[6]) == 2 || strcmp(argv[5], "-opsys") == 0 && atoi(argv[6]) == 3)
    {
        opsys = atoi(argv[6]);
    }else
    {
        printf("\nError! Wrong Command Line Argument Passed. You may use -h commnet to help.\n");
        exit(1);
    }

    csv2xml_converter(argv[1], separator, opsys); //fonksiyona parametreleri veriyor

    return 0;
}
