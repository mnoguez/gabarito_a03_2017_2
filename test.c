#include "simplefs.h"
#include "simplegrade.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>



// tenta ler um número de caracteres iguais de um arquivo
int read_chars(indice_fs_t vfs, char * filename, char c, int num){
    char * buffer;
    indice_arquivo_t f;
    int i;
    int ok = 0; 

    f = f_open(vfs, filename, LEITURA);
    if (f){
        buffer = (char *) calloc(num, sizeof(char));
        assert(buffer);
        if (f_read(f, num, buffer)){
            for(i=0; i<num; i++){
                if (buffer[i] != c)
                    break;
            }        
            if (i == num)
                ok = 1;
        }
        free(buffer);
        f_close(f);
    }
    return ok;
}


// tenta escrever um número de caracteres iguais de um arquivo
int write_chars(indice_fs_t vfs, char * filename, char c, int num){
    char * buffer;
    indice_arquivo_t f;
    int i;
    int ok = 0; 

    f = f_open(vfs, filename, ESCRITA);
    if (f){
        buffer = (char *) calloc(num, sizeof(char));
        for(i=0; i<num; i++){
                buffer[i] = c;
        }        
        assert(buffer);
        if (f_write(f, num, buffer))
            ok = 1;
        free(buffer);
        f_close(f);
    }
    return ok;
}

void test_too_few_blocks(){
    WHEN("Criando novo VFS");
    IF("Menos blocos que necessário");
    THEN("Deve retornar FALHA");

    remove("grademe.bin");
    isEqual(initfs("grademe.bin",5,512),FALHA,1);
    
}

void test_enough_blocks(){
    WHEN("Criando novo VFS");
    IF("Número de blocos razoável");
    THEN("Deve retornar SUCESSO");

    remove("grademe.bin");
    isEqual(initfs("grademe.bin",500,512),SUCESSO,1);

    remove("grademe.bin");
    IF("Número é o mínimo (1 boot, 1 super, 1 bitmap, blocos com diretorio, 1 bloco de dados");
    THEN("Deve retornar SUCESSO");

    isEqual(initfs("grademe.bin",144,512),SUCESSO,1);    
}


void test_block_sizes(){
    WHEN("Variando tamanho de blocos");
    IF("Número de blocos é múltiplo de 512");
    THEN("Deve retornar SUCESSO");

    remove("grademe.bin");
    isEqual(initfs("grademe.bin",500,2048),SUCESSO,1);

    IF("Número de blocos não é múltiplo de 512");
    THEN("Deve retornar FALHA");

    remove("grademe.bin");
    isEqual(initfs("grademe.bin",500,4095),FALHA,1);

}



void test_vfs_exists(){
    remove("grademe.bin");
    WHEN("Criando sistema de arquivos");
    IF("Crio duas vezes com mesmo nome");
    THEN("Deve dar erro");
    isNotEqual(initfs("grademe.bin",500,512), initfs("grademe.bin",500,512),1);
}


void test_delete(){
    indice_arquivo_t f;
    indice_fs_t vfs;
    remove("grademe.bin");
    WHEN("Criando e removendo arquivo");
    IF("Crio duas vezes com mesmo nome");
    THEN("Deve funcionar erro");
    if ((vfs=initfs("grademe.bin",300,512))){
        if ((f=f_open(vfs, "teste", ESCRITA)) == SUCESSO){
            f_close(f);
            f_delete(f);
            isEqual(f_open(vfs, "teste", LEITURA), FALHA, 1);
        }
        else isEqual(0,1,1);
    }
    else isEqual(0,1,1);
}

void test_names(){
    WHEN("Criando arquivos");
    remove("grademe.bin");
    initfs("grademe.bin",500,512);
    indice_fs_t vfs = openfs("grademe.bin");
    if (vfs){
        IF("Nome do arquivo pequeno e simples");
        THEN("Deve criar");
        isGreaterThan(f_open(vfs, "teste", ESCRITA),0, 1);

        IF("Nome do arquivo tem .");
        THEN("Deve criar");
        isGreaterThan(f_open(vfs, "teste.txt", ESCRITA),0, 1);


        IF("Nome do arquivo contém muitos caracteres");
        THEN("Deve criar");
        isGreaterThan(f_open(vfs, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", ESCRITA),0, 1);

        IF("Nome do arquivo contém caracteres demais");
        THEN("Não deve criar");
        isNotEqual(f_open(vfs, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", ESCRITA),1, 1);
    }
    else isEqual(0,1,4); //you lost
    closefs(vfs);
}


// void test_multiple_fs(){
//     WHEN("Criando vários VFS");
//     THEN("Devo conseguir cria-los e acessá-los");
//     remove("grademe.bin");
//     remove("grademe2.bin");
//     initfs("grademe.bin",500);
//     initfs("grademe2.bin",500);
//     indice_fs_t vfs = openfs("grademe.bin");
//     indice_fs_t vfs2 = openfs("grademe2.bin");
//     isNotEqual(vfs2, FALHA, 1);
//     f_closefs(vfs);
//     f_closefs(vfs2);
// }


void test_one_file_one_block(){
    int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",300,512);
    indice_fs_t vfs = openfs("grademe.bin");
    WHEN("Crio um arquivo com um bloco");
    THEN("Conteúdo lido deve ser igual ao escrito");
    
    if (write_chars(vfs, "a.txt", 'a', 4096)){
       if (read_chars(vfs, "a.txt", 'a', 4096)){
                ok = 1;
        }
    }
    isEqual(ok, 1, 5);
    closefs(vfs);
}


void test_stress_bitmap(){
    int ok=1;
    int i;
    remove("grademe.bin");
    initfs("grademe.bin",300,512);
    indice_fs_t vfs = openfs("grademe.bin");
    WHEN("Crio e removo um arquivo com um bloco várias vezes");
    THEN("Não pode falhar");
    
    for(i=0; (i<300) && ok; i++){
        if (write_chars(vfs, "a.txt", 'a', 4096)){
            indice_arquivo_t f = f_open(vfs, "a.txt", LEITURA);
            f_delete(f);
        }
        else {
            ok = 0; 
        }
    }
    isEqual(ok, 1, 5);
    closefs(vfs);
}


void test_multiple_blocks(){
    int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500,512);
    indice_fs_t vfs = openfs("grademe.bin");

    WHEN("Crio um arquivo com vários blocos");
    THEN("Conteúdo lido deve ser igual ao escrito");
    
    if (write_chars(vfs, "a.txt", 'a', 512000)){
        if (read_chars(vfs, "a.txt", 'a', 512000))
            ok = 1;
    }
    isEqual(ok, 1, 5);
    closefs(vfs);    
}

void test_seek(){
     int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500,512);
    indice_fs_t vfs = openfs("grademe.bin");

    WHEN("Faço um seek em um arquivo");
    THEN("Deve corrigir o ponteiro de lido/escrito");
    
    char buffer[10];
    char s[] =  "123456789";
    indice_arquivo_t f;
    int i;

    f = f_open(vfs, "teste.txt", LEITURAESCRITA);
    if (f){
        f_write(f, 10, s);
        f_seek(f,0);
        if (f_read(f, 10, buffer)){
            for(i=0; i<10; i++){
                if (buffer[i] != s[i])
                    break;
            }        
            if (i == 10)
                ok = 1;
        }
        f_close(f);
    }
    isEqual(ok, 1, 2);



    WHEN("Faço um seek após o fim do arquivo");
    THEN("Devo parar no fim dele (append like)");


    f = f_open(vfs, "teste2.txt", LEITURAESCRITA);
    if (f){
        f_write(f, 9, s);
        f_seek(f,0);
        f_seek(f,15);
        f_write(f+9, 1, s);
        if (f_read(f, 10, buffer)){
            for(i=0; i<10; i++){
                if (buffer[i] != s[i])
                    break;
            }        
            if (i == 10)
                ok = 1;
        }
        f_close(f);
    }
    isEqual(ok, 1, 2);



    closefs(vfs);    
}

void test_date(){
    int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500,512);
    indice_fs_t vfs = openfs("grademe.bin");
    WHEN("Verifico datas em um arquivo");
    THEN("Ordenamento deve ser preservado");
    
    if (write_chars(vfs, "a.txt", 'a', 4096)){
        time_t created, last_mod; 
        sleep(5);
        indice_arquivo_t f = f_open(vfs, "a.txt", LEITURAESCRITA);
        created = f_creation(f);
        
        sleep(5);

        f_write(f, 1, "a");
        last_mod = f_last_modified(f);
        
        if (created < last_mod)
            ok++;
        f_close(f);
    
    }
    isEqual(ok, 1, 5);
    closefs(vfs);
}


void test_many_files(){
    int i,ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",1000,512);
    indice_fs_t vfs = openfs("grademe.bin");

    WHEN("Crio múltiplos arquivos");
    THEN("Deve funcionar");

    for(i=0; (i<256) && ok; i++){
        char nome[10];
        sprintf(nome, "%d.txt", i);
        if (f_open(vfs, nome, ESCRITA)==FALHA){
            ok = 0;
        }
    }
    isEqual(ok, 1, 5);
    closefs(vfs);
}



int main(){
    DESCRIBE("test_too_few_blocks");
    test_too_few_blocks();
    DESCRIBE("test_enough_blocks");
    test_enough_blocks();
    DESCRIBE("test_vfs_exists");
    test_vfs_exists();
    DESCRIBE("test_names");
    test_names();
    DESCRIBE("test_one_file_one_block");
    test_one_file_one_block();
    DESCRIBE("test_stress_bitmap");
    test_stress_bitmap();
    DESCRIBE("test_multiple_blocks");
    test_multiple_blocks();
    DESCRIBE("test_seek");
    test_seek();
    DESCRIBE("test_date");
    test_date();
    DESCRIBE("test_many_files");
    test_many_files();
	printf("Grading \n");
	GRADEME();

	if (grade==maxgrade)
		return 0;
	else return grade;

    remove("grademe.bin");
}