#include <stdlib.h>
#include <stdio.h>
#include "environment.h"
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ui.h"
#include "easy_plot_c.h"

void** cmd_map;
char** cmd_help;

struct ui
{
	environment env;
	int bee_honey_count;
	int bee_sniffer_count;
	int distance;
	double percent;
	int iteration;
	char* type_distribution;
	char line[512 + 64];
} view;

static void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

static int skip_to_value(const char *line) {
    int it = 1;
    for (; line[it] != '\0' && line[it] != '=' && it < 512; it++);
    if (line[it] != '=')
        return -1;
    it++;
    for (; line[it] == ' ' && it < 512; it++);
    return it;
}

static void skip_token(const char *line, int *it) {
    while (line[*it] && isspace((unsigned char)line[*it]))
        (*it)++;
    while (line[*it] && !isspace((unsigned char)line[*it]))
        (*it)++;
}

void print_help() {
	printf("Commands:\n");

	for (int i = 0; i < 128; i++) {
		if (!cmd_map[i]) continue;

		printf("  >  %c: ", i);
		if (cmd_help[i]) printf("%s", cmd_help[i]);
		printf("\n");
	}
	printf("\n");
}

void print_run() {
	point result = env_iteration(view.env);
	print_point(result);
}

int read_int(const char* c){
	if (!c)
		return 0;
	while (isspace((unsigned char)*c))
		c++;
	int sign = 1;
	if (*c == '-') {
		sign = -1;
		c++;
	} else if (*c == '+') {
		c++;
	}
	int res = 0;
	while (*c >= '0' && *c <= '9') {
		res = res * 10 + (*c - '0');
		c++;
	}
	return sign * res;
}

double read_double(const char* str) {
    if (str == NULL) return 0.0;
    
    while (isspace((unsigned char)*str)) {
        str++;
    }
  
    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    double result = 0.0;
    while (isdigit((unsigned char)*str)) {
        result = result * 10.0 + (*str - '0');
        str++;
    }
    
    if (*str == '.') {
        str++;
        double fraction = 0.1;
        while (isdigit((unsigned char)*str)) {
            result += (*str - '0') * fraction;
            fraction *= 0.1;
            str++;
        }
    }
    
    if (*str == 'e' || *str == 'E') {
        str++;
        int exp_sign = 1;
        if (*str == '-') {
            exp_sign = -1;
            str++;
        } else if (*str == '+') {
            str++;
        }
        
        int exponent = 0;
        while (isdigit((unsigned char)*str)) {
            exponent = exponent * 10 + (*str - '0');
            str++;
        }
        
        if (exp_sign == -1) {
            result *= pow(10.0, -exponent);
        } else {
            result *= pow(10.0, exponent);
        }
    }
    
    return sign * result;
}

void print_set_parametr() {
	printf("Setting parameter values\n\
Select the parameter number and write a command like this: \"1 = 100\"\n\
1. bee_honey_count:      amount of honey bees\n\
2. bee_sniffer_count:    amount of sniffer (scout) bees\n\
3. distance:             distance between neighbour points\n\
4. percent:              percent of ranging points\n\
5. iteration:            amount of iterations\n\
6. type_distribution:    function to distribure honey bees on ranged points\n\
 												 to choose function, select number:\n\
													1. rnd_uniform  \n\
													2. rnd_linear  \n\
													3. rnd_square \n\
													4. rnd_exp\n\
write \"exit\" for exit\n");
	while(1){
		printf("  >> ");

		if (!fgets(view.line, sizeof(view.line), stdin)) {
			return;
		}
		trim_newline(view.line);
		if (view.line[0] == '\0') {
			continue;
		}

		char cmd = (char)tolower((unsigned char)view.line[0]);
		if(cmd == 'e') break;
		int it = skip_to_value(view.line);
		if (it < 0) {
			printf("bad command\n");
			continue;
		}
		switch (cmd)
		{
			case 'h':
			printf("Setting parameter values\n\
Select the parameter number and write a command like this: \"1 = 100\"\n\
1. bee_honey_count:      amount of honey bees\n\
2. bee_sniffer_count:    amount of sniffer (scout) bees\n\
3. distance:             distance between neighbour points\n\
4. percent:              percent of ranging points\n\
5. iteration:            amount of iterations\n\
6. type_distribution:    function to distribure honey bees on ranged points\n\
 												 to choose function, select number:\n\
													1. rnd_linear  \n\
													2. rnd_exp  \n\
													3. rnd_uniform  \n\
													4. rnd_square\n\
write \"exit\" for exit\n");
			break;
		case '1':
			view.bee_honey_count = read_int(view.line + it);
			env_set_honey_amount(view.env, view.bee_honey_count);
			printf("bee_honey_count = %d\n", view.bee_honey_count);
			break;
		case '2':
			view.bee_sniffer_count = read_int(view.line + it);
			env_set_sniffer_amount(view.env, view.bee_sniffer_count);
			printf("bee_sniffer_count = %d\n", view.bee_sniffer_count);
			break;
		case '3':
			view.distance = read_int(view.line + it);
			env_set_distance(view.env, view.distance);
			printf("distance = %d\n", view.distance);
			break;
		case '4':
			view.percent = read_double(view.line + it);
			env_set_percent_range(view.env, view.percent);
			printf("percent = %lf\n", view.percent);
			break;
		case '5':
			view.iteration = read_int(view.line + it);
			env_set_iterations(view.env, view.iteration);
			printf("iteration = %d\n", view.iteration);
			break;
		case '6':{
			switch (view.line[it])
			{
			case '1':
				view.type_distribution = "uniform";
				env_set_func(view.env, 1);
				break;
			case '2':
				view.type_distribution = "linear";
				env_set_func(view.env, 2);
				break;
			case '3':
				view.type_distribution = "square";
				env_set_func(view.env, 3);
				break;
			case '4':
				view.type_distribution = "exp";
				env_set_func(view.env, 4);
				break;
			
			default:
				printf("bad number\n");
				break;
			}
			printf("type_distribution = %s\n", view.type_distribution);
			
		}
			break;
		
		default:
			break;
		}
	}
}
void print_get_parametr() {
	printf("Enviroment's parametrs:\n\
bee_honey_count     %d\n\
bee_sniffer_count   %d\n\
distance            %d\n\
percent             %lf\n\
iteration           %d\n\
type_distribution   %s\n", view.bee_honey_count,
	view.bee_sniffer_count,
	view.distance,
	view.percent,
	view.iteration,
	view.type_distribution);
}
void print_test() {
	printf("Setting parameter test\n\
Select the parameter number and write a command like this: \"1 = 1 10 1\", pattern: \"number = start end step\"\n\
1. bee_honey_count:      amount of honey bees\n\
2. bee_sniffer_count:    amount of sniffer (scout) bees\n\
3. distance:             distance between neighbour points\n\
4. iteration:            amount of iterations\n\
write \"exit\" for exit\n");
	while(1){
		printf("  >> ");

		if (!fgets(view.line, sizeof(view.line), stdin)) {
			return;
		}
		trim_newline(view.line);
		if (view.line[0] == '\0') {
			continue;
		}

		char cmd = (char)tolower((unsigned char)view.line[0]);
		if(cmd == 'e') break;

		int it = skip_to_value(view.line);
		if (it < 0) {
			printf("bad command\n");
			continue;
		}

		int start = read_int(view.line + it);
		skip_token(view.line, &it);
		if (!view.line[it])
			continue;
		int end = read_int(view.line + it);
		skip_token(view.line, &it);
		if (!view.line[it])
			continue;
		int step = read_int(view.line + it);
		if(step == 0) return;

		int testn = 0;
		for(int i = start; i <= end; i += step) testn++;
		double *x = calloc(testn, sizeof(double));
		double *y = calloc(testn, sizeof(double));
		int test_it = 0;
		char* title = "";


		switch (cmd)
		{
			case 'e':
				return;
			case 'h':
			printf("Setting parameter test\n\
Select the parameter number and write a command like this: \"1 = 1 10 1\", pattern: \"number = start end step\"\n\
1. bee_honey_count:      amount of honey bees\n\
2. bee_sniffer_count:    amount of sniffer (scout) bees\n\
3. distance:             distance between neighbour points\n\
4. iteration:            amount of iterations\n\
write \"exit\" for exit\n");
			break;
		case '1':
			for(int i = start; i <= end; i += step){
				env_set_honey_amount(view.env, i);
				point result = env_iteration(view.env);
				x[test_it] = i;
				y[test_it++] = pnt_quality(result);
				pnt_quality_print(result);
			}
			env_set_honey_amount(view.env, view.bee_honey_count);
			title = "honey_amount";
			break;
		case '2':
			for(int i = start; i <= end; i += step){
				env_set_sniffer_amount(view.env, i);
				point result = env_iteration(view.env);
				x[test_it] = i;
				y[test_it++] = pnt_quality(result);
				pnt_quality_print(result);
			}
			env_set_sniffer_amount(view.env, view.bee_sniffer_count);
			title = "sniffer_amount";
			break;
		case '3':
			for(int i = start; i <= end; i += step){
				env_set_distance(view.env, i);
				point result = env_iteration(view.env);;
				x[test_it] = i;
				y[test_it++] = pnt_quality(result);
				pnt_quality_print(result);
			}
			env_set_distance(view.env, view.distance);
			title = "distance";
			break;
		case '4':
			for(int i = start; i <= end; i += step){
				env_set_iterations(view.env, i);
				point result = env_iteration(view.env);;
				x[test_it] = i;
				y[test_it++] = pnt_quality(result);
				pnt_quality_print(result);
			}
			env_set_iterations(view.env, view.iteration);
			title = "iterations";
			break;
		
		default:
			break;
		}
		easy_plot_xy("test", x, y, testn, title, "quality");
	}
}


void access_cmd() {
	while(1){
		printf("  >  ");

		if (!fgets(view.line, sizeof(view.line), stdin)) {
			return;
		}
		trim_newline(view.line);
		if (view.line[0] == '\0') {
			continue;
		}

		char cmd = (char)tolower((unsigned char)view.line[0]);
		if(cmd == 'q') break;
		if (cmd_map[cmd]) {
			((void(*)())cmd_map[cmd])();
		} else {
			printf("Unknown command\n");
		}
	}
}

void run_ui() {
	view.bee_honey_count = 200;
	view.bee_sniffer_count = 50;
	view.distance = 3;
	view.percent = 0.2;
	view.iteration = 100;
	view.type_distribution = "uniform";
	view.env = create_environment(50, 200, 3, 1000, "input.txt");

	cmd_map = calloc(128, sizeof(void*));
	cmd_help = calloc(128, sizeof(char*));

	cmd_map['h'] = print_help;
	cmd_help['h'] = "print help";
	cmd_map['s'] = print_set_parametr;
	cmd_help['s'] = "set environment's parametrs";
	cmd_map['i'] = print_get_parametr;
	cmd_help['i'] = "print environment's parametrs";
	cmd_map['r'] = print_run;
	cmd_help['r'] = "run environment";
	cmd_map['g'] = print_test;
	cmd_help['g'] = "run test for grafics";

	print_general();
}

void print_general() {
	printf("Commands:\n");

	for (int i = 0; i < 128; i++) {
		if (!cmd_map[i]) continue;

		printf("  >  %c: ", i);
		if (cmd_help[i]) printf("%s", cmd_help[i]);
		printf("\n");
	}
	printf("\n");

	access_cmd();
}


