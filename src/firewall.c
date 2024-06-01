#include "firewall.h"

int main(int argc, char *argv[]) {
  t_list *rules = NULL;
  int err = 0;
  err = check_args(argc, argv, &rules);
  char *line = NULL;
  size_t len = 0;
  while (!err && getline(&line, &len, stdin) != -1) {
    parse_user_input(line, &err, &rules);
    free(line);
    line = NULL;
  }
  if (line) free(line);
  ft_lstclear(&rules, free_data);
  return err == 1 ? 1 : 0;
}

int check_args(int argc, char *argv[], t_list **rules) {
  int err = 0;
  if (argc == 2) {
    if (!strcmp(argv[1], "--db1")) {
      err = get_rules_from_file(DB_1, rules);
    } else if (!strcmp(argv[1], "--db2")) {
      err = get_rules_from_file(DB_2, rules);
    } else if (!strcmp(argv[1], "--db_test")) {
      err = get_rules_from_file(DB_TEST, rules);
    } else {
      err = 1;
      printf("unknown command: %s\n", argv[1]);
    }
  } else if (argc > 1) {
    err = print_error("invalid number of arguments");
  }
  return err;
}

int get_rules_from_file(char *filename, t_list **rules) {
  int err = 0;
  char **str;
  size_t len = 0;
  char *line = NULL;
  FILE *file = open_file(filename);
  if (!file) err = 1;
  while (!err && getline(&line, &len, file) != -1) {
    if (!str_isspace(line)) {
      str = str_split(line, NULL);
      if (!str) {
        err = print_error("malloc error");
      } else {
        err = check_data_from_file(str, rules);
      }
      str_free_after_split(str);
    }
    free(line);
    line = NULL;
  }
  if (line) free(line);
  if (file) fclose(file);
  return err;
}

int check_data_from_file(char **str, t_list **rules) {
  t_data *data = NULL;
  t_list *new = NULL;
  int err = 1;
  data = get_data_from_file(str);
  if (data) {
    new = ft_lstnew(data);
    if (!new) {
      free_data_with_error(&data, "malloc error");
    } else {
      ft_lstadd_back(rules, new);
      err = 0;
    }
  }
  return err;
}

t_data *get_data_from_file(char **str) {
  t_data *data = new_data();
  if (!data) print_error("malloc error");
  int i = -1;
  while (data && str[++i]) {
    if (!strcmp(str[i], "src:")) {
      set_src_ip_from_file(&data, str[++i]);
    } else if (!strcmp(str[i], "dst:")) {
      set_dst_ip_from_file(&data, str[++i]);
    } else if (!strcmp(str[i], "proto:")) {
      set_protocol_from_file(&data, str[++i]);
    } else if (!strcmp(str[i], "=>")) {
      set_status_from_file(&data, str[++i]);
    } else {
      free_data_with_error(&data, "unknown data in file");
    }
  }
  if (data && !data->status) free_data_with_error(&data, "status is required");
  return data;
}

void set_src_ip_from_file(t_data **data, char *str) {
  if (!str) return free_data_with_error(data, "not enough data in file");
  set_ip(str, &(*data)->src_ip);
  if (!(*data)->src_ip) free_data_with_error(data, NULL);
}

void set_dst_ip_from_file(t_data **data, char *str) {
  if (!str) return free_data_with_error(data, "not enough data in file");
  set_ip(str, &(*data)->dest_ip);
  if (!(*data)->dest_ip) free_data_with_error(data, NULL);
}

//новые протоколы добавить здесь
void set_protocol_from_file(t_data **data, char *str) {
  if (!str) return free_data_with_error(data, "not enough data in file");
  if (strcmp(str, "any")) {
    if (!strcmp(str, "tcp"))
      (*data)->protocol = TCP;
    else if (!strcmp(str, "udp"))
      (*data)->protocol = UDP;
    else if (!strcmp(str, "igmp"))
      (*data)->protocol = IGMP;
    else
      free_data_with_error(data, "invalid protocol value");
  }
}

void set_status_from_file(t_data **data, char *str) {
  if (!str) return free_data_with_error(data, "not enough data in file");
  if (!strcmp(str, "ACCEPT"))
    (*data)->status = ACCEPT;
  else if (!strcmp(str, "DROP"))
    (*data)->status = DROP;
  else
    free_data_with_error(data, "invalid status");
}

void set_ip(char *str, t_ip **ip) {
  if (strcmp(str, "any")) {
    char **tmp = str_split(str, ".");
    if (str_count_args(tmp) != 4) {
      return free_res(ip, tmp, "invalid ip format");
    }
    (*ip)->mask = get_ip_mask(tmp[3]);
    if ((*ip)->mask == -1) {
      return free_res(ip, tmp, "invalid mask format");
    }
    for (int i = 0; i < 4; ++i) {
      (*ip)->part[i] = str_get_int(tmp[i]);
      if ((*ip)->part[i] < 0 || (*ip)->part[i] > 255)
        return free_res(ip, tmp, "invalid ip value");
    }
    calculate_min_max_ip(*ip);
    str_free_after_split(tmp);
  }
}

void free_res(t_ip **ip, char **tmp, char *err) {
  free(*ip);
  *ip = NULL;
  str_free_after_split(tmp);
  print_error(err);
}

t_data *new_data() {
  t_data *data = malloc(sizeof(t_data));
  if (!data) return NULL;
  data->src_ip = data->dest_ip = NULL;
  data->src_ip = create_ip();
  if (!data->src_ip) {
    free_data(data);
    return NULL;
  }
  data->dest_ip = create_ip();
  if (!data->dest_ip) {
    free_data(data);
    return NULL;
  }
  data->dest_port = data->src_port = data->protocol = -1;
  data->status = 0;
  return data;
}

void free_data_with_error(t_data **data, char *err) {
  if (data && *data) {
    free_data(*data);
    *data = NULL;
  }
  if (err) print_error(err);
}

void free_data(void *data) {
  t_data *tmp = data;
  if (tmp) {
    if (tmp->src_ip) {
      free(tmp->src_ip);
      tmp->src_ip = NULL;
    }
    if (tmp->dest_ip) {
      free(tmp->dest_ip);
      tmp->dest_ip = NULL;
    }
    free(tmp);
  }
}

void parse_user_input(char *line, int *err, t_list **rules) {
  if (!str_isspace(line)) {
    char **str;
    str = str_split(line, NULL);
    if (!str) {
      *err = print_error("malloc error");
    } else if (str_count_args(str) == 1 && !strcmp(str[0], "exit")) {
      *err = 2;
    } else if (str_count_args(str) == 1 && !strcmp(str[0], "show")) {
      print_rules(*rules);
    } else if (str_count_args(str) == 2 && !strcmp(str[0], "delete")) {
      size_t num = str_get_int(str[1]);
      if (num <= 0 || num > ft_lstsize(*rules)) {
        print_error("invalid number of rule");
      } else {
        ft_lstdel(rules, free_data, num);
        printf("rule %ld has been successfully deleted\n", num);
      }
    } else {
      check_data_from_user(str, *rules, err);
    }
    str_free_after_split(str);
  }
}

void print_rules(t_list *rules) {
  if (!rules) {
    printf("no rules\n");
  } else {
    int count = 0;
    t_list *tmp = rules;
    while (tmp) {
      print_one_rule(tmp->content, ++count);
      tmp = tmp->next;
    }
  }
}

void print_one_rule(t_data *data, int num) {
  printf("\t%d", num);
  print_ip(data->src_ip, "src");
  print_ip(data->dest_ip, "dst");
  print_protocol(data->protocol);
  print_status(data->status);
  printf("\n");
}

void print_ip(t_ip *ip, char *str) {
  if (ip->min == -1) {
    printf("\t%s: %15s ", str, "any");
  } else {
    printf("\t%s: %03d.%03d.%03d.%03d ", str, ip->part[0], ip->part[1],
           ip->part[2], ip->part[3]);
  }
}

void print_protocol(int protocol) {
  if (protocol == -1) {
    printf("\tproto: %5s", "any");
  } else {
    if (protocol == TCP) {
      printf("\tproto: tsp(%d)", TCP);
    } else if (protocol == UDP) {
      printf("\tproto: udp(%d)", UDP);
    } else if (protocol == IGMP) {
      printf("\tproto: igmp(%d)", IGMP);
    }
  }
}

void print_status(int status) {
  if (status == ACCEPT) {
    printf("\t=> ACCEPT");
  } else if (status == DROP) {
    printf("\t=> DROP");
  }
}

void check_data_from_user(char **str, t_list *rules, int *err) {
  int num_of_args = str_count_args(str);
  if (num_of_args == 5) {
    t_data *data = get_data_from_user(str, err);
    if (data) {
      check_status(data, rules);
      free_data(data);
    }
  } else if (num_of_args == 7) {
    save_new_rule_by_user(str, rules, err);
  } else {
    printf("unknown command\n");
  }
}

void save_new_rule_by_user(char **str, t_list *rules, int *err) {
  if (!strcmp(str[0], "save") &&
      (!strcmp(str[1], "accept") || !strcmp(str[1], "drop"))) {
    t_data *data = get_data_from_user(&str[2], err);
    if (data) {
      if (!strcmp(str[1], "accept")) {
        data->status = ACCEPT;
      } else {
        data->status = DROP;
      }
      t_list *new = ft_lstnew(data);
      if (!new) {
        free_data_with_error(&data, "malloc error");
        *err = 1;
      } else {
        ft_lstadd_back(&rules, new);
        printf("the rule %ld has been saved successfully\n", ft_lstsize(rules));
      }
    }
  } else {
    printf("unknown command\n");
  }
}

void check_status(t_data *data, t_list *rules) {
  if (!rules) {
    printf("drop\n");
    return;
  }
  int found = 0;
  t_list *tmp = rules;
  while (!found && tmp) {
    found = data_compare(tmp->content, data);
    if (found) {
      if (found == ACCEPT) {
        printf("accept\n");
      } else if (found == DROP) {
        printf("drop\n");
      }
    }
    tmp = tmp->next;
  }
  if (!found) printf("drop\n");
}

int data_compare(t_data *rule, t_data *dst) {
  if (dst->src_ip->mask != 32 || dst->dest_ip->mask != 32) return 0;
  if (rule->src_ip->min != -1 && (rule->src_ip->min > dst->src_ip->min ||
                                  rule->src_ip->max < dst->src_ip->min))
    return 0;
  if (rule->dest_ip->min != -1 && (rule->dest_ip->min > dst->dest_ip->min ||
                                   rule->dest_ip->max < dst->dest_ip->min))
    return 0;
  if (rule->protocol != -1 && rule->protocol != dst->protocol) return 0;
  return rule->status;
}

t_data *get_data_from_user(char **str, int *err) {
  t_data *data = new_data();
  if (!data) {
    *err = print_error("malloc error");
    return NULL;
  }
  if (set_ip_by_user(str[0], &data, &data->src_ip)) {
    return NULL;
  }
  if (set_ip_by_user(str[1], &data, &data->dest_ip)) {
    return NULL;
  }
  if (set_port_by_user(str[2], &data, &data->src_port)) {
    return NULL;
  }
  if (set_port_by_user(str[3], &data, &data->dest_port)) {
    return NULL;
  }
  if (set_protocol_by_user(str[4], &data, &data->protocol)) {
    return NULL;
  }
  return data;
}

int set_ip_by_user(char *str, t_data **data, t_ip **ip) {
  int result = 0;
  set_ip(str, ip);
  if (!*ip) {
    free_data_with_error(data, NULL);
    result = 1;
  }
  return result;
}

int set_port_by_user(char *str, t_data **data, int *port) {
  int result = 0;
  if (!strcmp(str, "any")) {
    *port = -1;
  } else {
    *port = str_get_int(str);
    if (*port < 0 || *port > 65535) {
      free_data_with_error(data, "invalid src port value");
      result = 1;
    }
  }
  return result;
}

//новые протоколы добавить здесь
int set_protocol_by_user(char *str, t_data **data, int *protocol) {
  int result = 0;
  if (!strcmp(str, "any")) {
    *protocol = -1;
  } else {
    *protocol = str_get_int(str);
    if (*protocol != TCP && *protocol != UDP && *protocol != IGMP) {
      free_data_with_error(data, "invalid protocol value");
      result = 1;
    }
  }
  return result;
}