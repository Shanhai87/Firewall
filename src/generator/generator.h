/**
 * @file generator.h
 * @author lpuddy (tenochtitlan2010@yandex.ru)
 * @brief Заголовочный файл с описанием функций и структур для программы
 * generator
 * @version 1.0
 * @date май-июнь 2024 года
 * @warning Данная порграмма создана в учебных целях
 * @copyright Copyright (c) 2024
 */
#ifndef SRC_GENERATOR_GENERATOR_H_
#define SRC_GENERATOR_GENERATOR_H_

/**
 * @brief Позволяет использовать функции, которые не являются частью
 * стандарной библиотеки C, но являются частью стандарта POSIX.1-2017
 */
#define _POSIX_C_SOURCE 200809L

#include <time.h>

#include "../utils.h"

#define GEN_SETTINGS "src/generator/gen_settings"

enum data_mode {
  NO_MODE = 0,
  SRC_IP_MODE = 1,
  DST_IP_MODE = 2,
  PROTO_MODE = 3
};

enum info_mode { DEFAULT_MODE = 0, FILE_MODE = 1 };

typedef struct s_info {
  t_list *src_ip;
  t_list *dst_ip;
  t_list *proto;
  size_t src_ip_count;
  size_t dst_ip_count;
  size_t proto_count;
  int mode;
} t_info;

int main(int argc, char *argv[]);
void check_args(int argc, char **argv, t_info *info);
int init_info(t_info *info);

/**
 * @warning Новые протоколы добавить здесь
 */
int default_init(t_info *info);

int save_new_ip(t_list **ip, int part1, int part2, int part3, int part4,
                int mask);
t_list *default_init_lst_ip(int part1, int part2, int part3, int part4,
                            int mask);
int save_new_protocol(t_list **proto, int protocol);
t_list *default_init_lst_proto(int protocol);
int *default_init_proto(int protocol);
int get_info_from_file(FILE *file, t_info *info);
int parse_info_from_file(t_info *info, char *line, int *mode);
int check_mode(char *str, int *mode);
int check_info_from_file(t_info *info, char *str, int *mode);
int save_ip(t_list **ip, char *str);
int check_range_ip_format(t_list **ip, char *str);
int get_ip_from_str(char *str, int *num, int *mask, int can_has_mask);

/**
 * @warning Новые протоколы добавить здесь
 */
int save_protocol(t_list **proto, char *str);

int free_all(t_info *info);
void count_number_of_options(t_info *info);
size_t count_ip_options(t_list *list_ip);
size_t get_rand_number();
void get_rand_ip(t_list *list_ip, size_t num);
void convert_and_print_ip(size_t num);
void get_rand_proto(t_list *list_proto, size_t num);

#endif  // SRC_GENERATOR_GENERATOR_H_