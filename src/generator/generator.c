#include "generator.h"

int main(int argc, char *argv[]) {
  t_info info;
  info.src_ip = NULL;
  info.dst_ip = NULL;
  info.proto = NULL;
  info.mode = DEFAULT_MODE;
  check_args(argc, argv, &info);
  if (init_info(&info)) return 1;
  count_number_of_options(&info);
  srand(time(NULL));
  get_rand_ip(info.src_ip, get_rand_number() % info.src_ip_count);
  get_rand_ip(info.dst_ip, get_rand_number() % info.dst_ip_count);
  printf("any any ");
  get_rand_proto(info.proto, get_rand_number() % info.proto_count);
  free_all(&info);
}

void check_args(int argc, char **argv, t_info *info) {
  if (argc == 1) {
    return;
  } else if (argc == 2 && !strcmp(argv[1], "--file")) {
    info->mode = FILE_MODE;
  } else {
    printf("unknown command: %s\n", argv[1]);
  }
}

int init_info(t_info *info) {
  int result = 0;
  if (info->mode == DEFAULT_MODE) {
    if (default_init(info)) result = print_error("malloc error");
  } else if (info->mode == FILE_MODE) {
    FILE *file = open_file(GEN_SETTINGS);
    if (!file) {
      result = 1;
    } else {
      if (get_info_from_file(file, info)) result = 1;
      fclose(file);
    }
  }
  return result;
}

//новые протоколы добавить здесь
int default_init(t_info *info) {
  int result = 0;
  result = save_new_ip(&info->src_ip, 0, 0, 0, 0, 0);
  if (result) return free_all(info);
  result = save_new_ip(&info->dst_ip, 0, 0, 0, 0, 0);
  if (result) return free_all(info);
  result = save_new_protocol(&info->proto, TCP);
  if (result) return free_all(info);
  result = save_new_protocol(&info->proto, UDP);
  if (result) return free_all(info);
  return result;
}

int save_new_ip(t_list **ip, int part1, int part2, int part3, int part4,
                int mask) {
  int result = 0;
  t_list *list_ip = NULL;
  list_ip = default_init_lst_ip(part1, part2, part3, part4, mask);
  if (list_ip)
    ft_lstadd_back(ip, list_ip);
  else
    result = 1;
  return result;
}

t_list *default_init_lst_ip(int part1, int part2, int part3, int part4,
                            int mask) {
  t_ip *ip = create_ip();
  t_list *new = NULL;
  if (ip) {
    ip->part[0] = part1;
    ip->part[1] = part2;
    ip->part[2] = part3;
    ip->part[3] = part4;
    ip->mask = mask;
    calculate_min_max_ip(ip);
    new = ft_lstnew(ip);
    if (!new) free(ip);
  }
  return new;
}

int save_new_protocol(t_list **proto, int protocol) {
  int result = 0;
  t_list *pro = NULL;
  pro = default_init_lst_proto(protocol);
  if (pro)
    ft_lstadd_back(proto, pro);
  else
    result = 1;
  return result;
}

t_list *default_init_lst_proto(int protocol) {
  t_list *new = NULL;
  int *proto = default_init_proto(protocol);
  if (proto) {
    new = ft_lstnew(proto);
    if (!new) free(proto);
  }
  return new;
}

int *default_init_proto(int protocol) {
  int *proto = NULL;
  proto = malloc(sizeof(int));
  if (proto) *proto = protocol;
  return proto;
}

int get_info_from_file(FILE *file, t_info *info) {
  char *line = NULL;
  size_t len = 0;
  int mode = NO_MODE;  // 1 -src_ip, 2 - dst_ip, 3 - proto
  int result = 0;
  while (!result && getline(&line, &len, file) != -1) {
    if (!str_isspace(line)) {
      if (parse_info_from_file(info, line, &mode)) result = 1;
    }
    free(line);
    line = NULL;
  }
  if (line) free(line);
  if (!result && (!info->src_ip || !info->dst_ip || !info->proto))
    result = print_error("not enough parameters in file");
  if (result) free_all(info);
  return result;
}

int parse_info_from_file(t_info *info, char *line, int *mode) {
  int result = 0;
  char **str = NULL;
  str = str_split(line, NULL);
  if (!str) {
    result = print_error("malloc error");
  } else if (str_count_args(str) != 1) {
    result = print_error("invalid data format in file");
  } else {
    result = check_mode(str[0], mode);
    if (result) {
      result = check_info_from_file(info, str[0], mode);
    }
    str_free_after_split(str);
  }
  return result;
}

int check_mode(char *str, int *mode) {
  int result = 0;
  if (!strcmp(str, "src:"))
    *mode = SRC_IP_MODE;
  else if (!strcmp(str, "dst:"))
    *mode = DST_IP_MODE;
  else if (!strcmp(str, "proto:"))
    *mode = PROTO_MODE;
  else
    result = 1;
  return result;
}

int check_info_from_file(t_info *info, char *str, int *mode) {
  int result = 0;
  if (*mode == NO_MODE) {
    result = 1;
  } else if (*mode == SRC_IP_MODE) {
    result = save_ip(&info->src_ip, str);
  } else if (*mode == DST_IP_MODE) {
    result = save_ip(&info->dst_ip, str);
  } else if (*mode == PROTO_MODE) {
    result = save_protocol(&info->proto, str);
  }
  return result;
}

int save_ip(t_list **ip, char *str) {
  int result = 0, num[4], mask = 32;
  result = check_range_ip_format(ip, str);
  if (result == 2) {
    result = get_ip_from_str(str, num, &mask, 1);
    if (!result) result = save_new_ip(ip, num[0], num[1], num[2], num[3], mask);
  }
  return result;
}

//проверка случая 1.1.1.1-10.10.10.10
int check_range_ip_format(t_list **ip, char *str) {
  int result = 0;
  char **substr = NULL;
  substr = str_split(str, "-");
  if (str_count_args(substr) != 2)
    result = 2;
  else {
    int num[4], mask = 32;
    result = get_ip_from_str(substr[0], num, &mask, 0);
    if (!result) {
      result = save_new_ip(ip, num[0], num[1], num[2], num[3], mask);
      if (result) print_error("malloc error");
    }
    if (!result) {
      result = get_ip_from_str(substr[1], num, &mask, 0);
      if (!result) {
        t_ip *tmp = ft_lstget(*ip, ft_lstsize(*ip) - 1)->content;
        ssize_t number = convert_ip_to_number(num);
        if (tmp->min < number)
          tmp->max = number;
        else
          tmp->min = number;
      }
    }
  }
  str_free_after_split(substr);
  return result;
}

int get_ip_from_str(char *str, int *num, int *mask, int can_has_mask) {
  int result = 0;
  char **substr = NULL;
  substr = str_split(str, ".");
  if (str_count_args(substr) != 4) {
    result = print_error("invalid ip value");
  } else {
    if (can_has_mask) {
      *mask = get_ip_mask(substr[3]);
      if (*mask == -1) result = print_error("invalid mask value");
    }
    for (int i = 0; i < 4 && !result; ++i) {
      num[i] = str_get_int(substr[i]);
      if (num[i] == -1) result = print_error("invalid ip value");
    }
  }
  str_free_after_split(substr);
  return result;
}

//новые протоколы добавить здесь
int save_protocol(t_list **proto, char *str) {
  int result = 0;
  if (!strcmp(str, "tcp")) {
    result = save_new_protocol(proto, TCP);
    if (result) print_error("malloc error");
  } else if (!strcmp(str, "udp")) {
    result = save_new_protocol(proto, UDP);
    if (result) print_error("malloc error");
  } else if (!strcmp(str, "igmp")) {
    result = save_new_protocol(proto, IGMP);
    if (result) print_error("malloc error");
  } else {
    result = print_error("invalid protocol value");
  }
  return result;
}

int free_all(t_info *info) {
  ft_lstclear(&info->src_ip, free);
  ft_lstclear(&info->dst_ip, free);
  ft_lstclear(&info->proto, free);
  return 1;
}

void count_number_of_options(t_info *info) {
  info->src_ip_count = count_ip_options(info->src_ip);
  info->dst_ip_count = count_ip_options(info->dst_ip);
  info->proto_count = ft_lstsize(info->proto);
}

size_t count_ip_options(t_list *list_ip) {
  t_list *tmp = list_ip;
  t_ip *ip;
  size_t result = 0;
  while (tmp) {
    ip = tmp->content;
    result += ip->max - ip->min + 1;
    tmp = tmp->next;
  }
  return result;
}

size_t get_rand_number() { return ((size_t)rand() << 24) | (size_t)rand(); }

void get_rand_ip(t_list *list_ip, size_t num) {
  size_t i = 0;
  ssize_t result = -1;
  t_list *tmp = list_ip;
  t_ip *ip = NULL;
  while (result == -1) {
    ip = tmp->content;
    if (i + ip->max - ip->min + 1 > num) {
      result = ip->min + num - i;
    } else {
      i += ip->max - ip->min + 1;
      tmp = tmp->next;
    }
  }
  convert_and_print_ip(result);
}

void convert_and_print_ip(size_t num) {
  size_t tmp = 0;
  for (int i = 24; i >= 0; i -= 8) {
    tmp = num >> i;
    printf("%ld", tmp);
    if (i) printf(".");
    num -= tmp << i;
  }
  printf(" ");
}

void get_rand_proto(t_list *list_proto, size_t num) {
  t_list *tmp = list_proto;
  size_t i = 0;
  while (i++ < num) {
    tmp = tmp->next;
  }
  printf("%d\n", *((int *)tmp->content));
}
