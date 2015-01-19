#ifndef GPIO_H
#define GPIO_H

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int pgio);
int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
int gpio_set_value(unsigned int gpio, unsigned int value);
int gpio_get_value(unsigned int gpio, unsigned int *value);
int gpio_set_edge(unsigned int gpio, char *egde);
int gpio_fd_open_read(unsigned int gpio);
int gpio_fd_open_write(unsigned int gpio);
int gpio_fd_close(int fd);

#endif
