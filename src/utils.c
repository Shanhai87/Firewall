#include "utils.h"

void ft_lstadd_back(t_list **begin, t_list *new_node) {
  t_list *temp;
  temp = *begin;
  if (!*begin)
    *begin = new_node;
  else {
    while (temp->next) temp = temp->next;
    temp->next = new_node;
  }
}

void ft_lstadd_front(t_list **begin, t_list *new_node) {
  if (!*begin)
    *begin = new_node;
  else {
    new_node->next = *begin;
    *begin = new_node;
  }
}

void ft_lstdel_front(t_list **begin, void (*del)(void *)) {
  t_list *current;
  if (begin && *begin) {
    current = *begin;
    *begin = current->next;
    ft_lstdelone(current, (del));
  }
}

void ft_lstdel(t_list **begin, void (*del)(void *), size_t index) {
  if (index >= ft_lstsize(*begin)) return;
  if (index == 1) {
    ft_lstdel_front(begin, del);
  } else {
    t_list *current;
    t_list *elem;
    size_t i = 0;
    if (begin && *begin) {
      current = *begin;
      while (++i != index - 1) current = current->next;
      elem = current->next;
      current->next = elem->next;
      ft_lstdelone(elem, (del));
    }
  }
}

t_list *ft_lstnew(void *content) {
  t_list *new_node = NULL;
  new_node = malloc(sizeof(t_list));
  if (new_node) {
    new_node->content = content;
    new_node->next = NULL;
  }
  return (new_node);
}

void ft_lstdelone(t_list *lst, void (*del)(void *)) {
  if (lst != NULL) {
    if (del != NULL && lst->content != NULL) del(lst->content);
    free(lst);
    lst = NULL;
  }
}

void ft_lstclear(t_list **begin, void (*del)(void *)) {
  t_list *current;
  t_list *next;
  if (begin && *begin) {
    current = *begin;
    while (current) {
      next = current->next;
      ft_lstdelone(current, (del));
      current = next;
    }
    *begin = NULL;
  }
}

size_t ft_lstsize(t_list *begin) {
  size_t count_elem = 0;
  while (begin != NULL) {
    count_elem++;
    begin = begin->next;
  }
  return (count_elem);
}

t_list *ft_lstget(t_list *begin, size_t index) {
  t_list *result = NULL;
  if (index >= ft_lstsize(begin)) return result;
  if (begin) {
    result = begin;
    size_t i = -1;
    while (++i != index) result = result->next;
  }
  return result;
}

int print_error(char *str) {
  printf("err: %s\n", str);
  return 1;
}

int is_file(const char *path) {
  struct stat s;
  int result = 0;
  if (!stat(path, &s)) {
    if ((s.st_mode & 0040000))
      print_error("Is a directory");
    else if (s.st_mode & 0100000)
      result = 1;
    else
      print_error("Is not a file");
  } else
    print_error("No such file or directory");
  return result;
}

FILE *open_file(const char *filename) {
  FILE *file = NULL;
  if (is_file(filename)) {
    if (!(file = fopen(filename, "r"))) print_error("Permission denied");
  }
  return file;
}

void str_free_after_split(char **str) {
  if (str) {
    for (int i = 0; str[i]; ++i) free(str[i]);
    free(str);
  }
}

static int get_count_substrings(const char *str, char *delim) {
  int count = 0;
  size_t len = delim ? strlen(delim) : 0;
  while (*str) {
    if (!delim)
      while (isspace(*str)) ++str;
    else if (!strncmp(str, delim, len))
      str += len;
    if (*str) {
      ++count;
      while (*str && (delim ? strncmp(str, delim, len) : !isspace(*str))) ++str;
    }
  }
  return count;
}

char **str_split(const char *str, char *delim) {
  int count = get_count_substrings(str, delim);
  size_t len = delim ? strlen(delim) : 0;
  char **result = malloc(sizeof(char *) * (count + 1));
  if (!result) return NULL;
  int start = count = 0;
  size_t length = strlen(str);
  for (size_t i = 0; i < length;) {
    if (!delim)
      while (isspace(str[i])) ++i;
    else if (!strncmp(&str[i], delim, len))
      i += len;
    if (str[i]) {
      start = i;
      while (str[i] &&
             (delim ? strncmp(&str[i], delim, len) : !isspace(str[i])))
        ++i;
      result[count] = malloc(sizeof(char) * (i - start + 1));
      if (!result[count]) {
        str_free_after_split(result);
        return NULL;
      }
      memcpy(result[count], str + start, i - start);
      result[count++][i - start] = '\0';
    }
  }
  result[count] = NULL;
  return result;
}

int str_isspace(char *str) {
  while (*str && isspace(*str)) ++str;
  return *str ? 0 : 1;
}

int str_count_args(char **str) {
  int count = 0;
  if (str) {
    while (str[count]) ++count;
  }
  return count;
}

int str_get_int(char *str) {
  int result = 0;
  if (str) {
    while (*str && isdigit(*str)) result = result * 10 + (*str++ - 48);
    if (*str && !isdigit(*str)) result = -1;
  } else {
    result = -1;
  }
  return result;
}

size_t convert_ip_to_number(int *part) {
  size_t num_ip = 0;
  for (int i = 0, ofs = 24; i < 4; ++i, ofs -= 8)
    num_ip += (size_t)part[i] << ofs;
  return num_ip;
}

void calculate_min_max_ip(t_ip *ip) {
  if (ip) {
    size_t num_ip, mask1, mask2;
    num_ip = convert_ip_to_number(ip->part);
    mask1 = ((size_t)1 << (32 - ip->mask)) - 1;
    mask2 = ((size_t)1 << 32) - mask1 - 1;
    ip->max = num_ip | mask1;
    ip->min = num_ip & mask2;
  }
}

t_ip *create_ip() {
  t_ip *ip = malloc(sizeof(t_ip));
  if (!ip) return NULL;
  ip->part[0] = ip->part[1] = ip->part[2] = ip->part[3] = -1;
  ip->min = ip->max = -1;
  ip->mask = 32;
  return ip;
}

int get_ip_mask(char *str) {
  int mask = 32;
  for (int i = 0; str[i]; ++i) {
    if (str[i] == '/') {
      mask = str_get_int(&str[i + 1]);
      str[i] = '\0';
      if (mask > 32) mask = -1;
      return mask;
    }
  }
  return mask;
}
