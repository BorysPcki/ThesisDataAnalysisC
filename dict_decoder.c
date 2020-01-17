#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "zstd_decompress.h"

typedef unsigned char u8;

static size_t read_file(const char *path, u8 **ptr)
{
    FILE* const f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "failed to open file %s \n", path);
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    size_t const size = (size_t)ftell(f);
    rewind(f);

    *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "failed to allocate memory to hold %s \n", path);
        exit(1);
    }

    size_t const read = fread(*ptr, 1, size, f);
    if (read != size) {  /* must read everything in one pass */
        fprintf(stderr, "error while reading file %s \n", path);
        exit(1);
    }

    fclose(f);

    return read;
}

int main()
{
    printf("Start\n");
    const char *dict_path = "V:\\thesis\\10.01.2020_19.24_dict_64_16";
    const char* extension = ".csv";
    char* dict_path_with_extension;
    dict_path_with_extension = malloc(strlen(dict_path)+1+4);
    strcpy(dict_path_with_extension, dict_path);
    strcat(dict_path_with_extension, extension);

    u8* dict = NULL;
    size_t dict_size = 0;
    dict_size = read_file(dict_path, &dict);
    int table_length = 512;

    dictionary_t* const parsed_dict = create_dictionary();
    if (dict) {
        parse_dictionary(parsed_dict, dict, dict_size);
    }

    uint32_t id = dictionary_get_id(parsed_dict);
    printf("Dict size: %d \n", dict_size);
    printf("Dict id: %lu \n", id);

    
    uint8_t *sym = dictionary_get_symbols(parsed_dict);
    uint8_t *num = dictionary_get_num_bits(parsed_dict);
    int max_bits = dictionary_get_max_bits(parsed_dict);

    // uint16_t *base = dictionary_get_base(parsed_dict);
    // 
    // printf("Dict base: \n");
    // for (int i =0; i <8; i++) {
    //     printf("%d ", base[i]);
    // }
    // printf("\n");

    printf("Dict num: \n");
    for (int i =0; i <table_length; i++) {
        printf("%d ", num[i]);
    }
    printf("\n\n");
    

    printf("Dict sym: \n");
    for (int i =0; i <512; i++) {
        printf("%d ", sym[i]);
    }
    printf("\n\n");

    printf("Max bits: %d\n\n", max_bits);

    int histo[12] = {0};
    for (int i = 0; i < table_length; i++) {
        histo[num[i]] += 1;
    }
    
    for (int i =0; i <12; i++) {
        printf("%d ", histo[i]);
    }
    printf("\n\n");

    FILE * fp;
    int i;
    fp = fopen (dict_path_with_extension,"w");
    for(i = 0; i < table_length;i++){
        fprintf (fp, "%d %d\n", num[i], sym[i]);
    } 
    fclose (fp);
    return 0;

    printf("Completed");
    free_dictionary(parsed_dict);
    free(dict);
    return 0;
}
