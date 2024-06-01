/**
 * @file firewall.h
 * @author lpuddy (tenochtitlan2010@yandex.ru)
 * @brief Заголовочный файл с описанием функций и структур для программы
 * firewall
 * @version 1.0
 * @date май-июнь 2024 года
 * @warning Данная порграмма создана в учебных целях
 * @copyright Copyright (c) 2024
 */
#ifndef SRC_FIREWALL_H_
#define SRC_FIREWALL_H_

/**
 * @brief Позволяет использовать функции, которые не являются частью
 * стандарной библиотеки C, но являются частью стандарта POSIX.1-2017
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>  ///<Для печати в канал вывода и работы с файлами.
#include <stdlib.h>  ///<Для динамического выделения памяти.
#include <string.h>  ///<Для использования стандартных функций, работающих со строками.

#include "utils.h"

#define DB_1 "database/db1"
#define DB_2 "database/db2"
#define DB_TEST "test_db"

enum status { ACCEPT = 1, DROP = 2 };

/**
 * @brief          Структура для хранения информации о соединении.
 * @param status   1 - accept (разрешено); 2 - drop (запрещено).
 * @param src_ip   IP адрес источника.
 * @param dest_ip  IP адрес назначения.
 * @param src_port Порт источника.
 * @param dest_port Порт назначения.
 * @param protocol Протокол соединения (TCP, UDP, IGMP).
 */
typedef struct s_data {
  int status;  ///<-- 1 - accept (разрешено); 2 - drop (запрещено).
  t_ip *src_ip;   ///<-- IP адрес источника.
  t_ip *dest_ip;  ///<-- IP адрес назначения.
  int src_port;   ///<-- Порт источника.
  int dest_port;  ///<-- Порт назначения.
  int protocol;   ///<-- Протокол соединения.
} t_data;

int main(int argc, char *argv[]);
int check_args(int argc, char *argv[], t_list **rules);
int get_rules_from_file(char *filename, t_list **rules);
int check_data_from_file(char **str, t_list **rules);
t_data *get_data_from_file(char **str);
void set_src_ip_from_file(t_data **data, char *str);
void set_dst_ip_from_file(t_data **data, char *str);

/**
 * @warning Новые протоколы добавить здесь
 */
void set_protocol_from_file(t_data **data, char *str);

void set_status_from_file(t_data **data, char *str);
void set_ip(char *str, t_ip **ip);
void free_res(t_ip **ip, char **tmp, char *err);
t_data *new_data();
void free_data_with_error(t_data **data, char *err);
void free_data(void *data);
void parse_user_input(char *line, int *err, t_list **rules);
void print_rules(t_list *rules);
void print_one_rule(t_data *data, int num);
void print_ip(t_ip *ip, char *str);
void print_protocol(int protocol);
void print_status(int status);
void check_data_from_user(char **str, t_list *rules, int *err);
void save_new_rule_by_user(char **str, t_list *rules, int *err);
void check_status(t_data *data, t_list *rules);
int data_compare(t_data *rule, t_data *dst);
t_data *get_data_from_user(char **str, int *err);
int set_ip_by_user(char *str, t_data **data, t_ip **ip);
int set_port_by_user(char *str, t_data **data, int *port);

/**
 * @warning Новые протоколы добавить здесь
 */
int set_protocol_by_user(char *str, t_data **data, int *protocol);

#endif  // SRC_FIREWALL_H_