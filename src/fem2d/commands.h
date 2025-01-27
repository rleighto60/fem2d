/*
$Id: command.h 1.1 1995/01/23 17:40:26 leighton Exp leighton $
    Copyright (C) 1991-2013 Russell Leighton <rleighto@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

program     -   command.h

description -   Header file for command functions

$Log: command.h $
 * Revision 1.1  1994/01/23  18:20:39  leighton
 * Initial revision
 *
*/

#define MAXARG 24

#ifdef COMMANDS
#define WIDTH 1200
#define HEIGHT 1024

struct view {
    int id;
    float xmin,xmax,ymin,ymax,xtol,ytol;
};

#define VERSION "3.0"

extern char hostname[];

extern int close_down;

extern char *_args[];
extern char field[];

struct view view = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
int erase=TRUE;
int lastnode=-1,lastelem=-1;
float Xorg=0.0,Yorg=0.0;
float vals[MAXARG];
static char filename[200] = {0};
FILE *hfp = NULL;

struct color defcolors[12] = {
     { 4, 0, 0,15 },
     { 5, 0, 9,15 },
     { 6, 0,13,15 },
     { 7, 0,15,15 },
     { 8, 0,15, 8 },
     { 9, 0,15, 0 },
     {10,12,15, 0 },
     {11,15,15, 0 },
     {12,15,13, 0 },
     {13,15, 9, 0 },
     {14,15, 5, 0 },
     {15,15, 0, 0 }
};

extern int check();
extern int eval();
extern int eval_arg();
extern int outside(float,float);
extern int parse();
extern int readdata();
extern int readgroup(int,char *);
extern int savedata();
extern int savegroup(int,char *);

extern void clearwindow(FILE *);
extern void error();
extern void filldeform(float,FILE *);
extern void fillelem(int,float,int,FILE *);
extern void fillmodel(FILE *);
extern void fillresult(int,int,int,float,float,FILE *);
extern void findwindow(int);
extern void freemem();
extern void plot(float,float,int,FILE *);
extern void plotbound(void);
extern void plotcross(long,long,int,FILE *);
extern void plotdbc(int,int,FILE *);
extern void plotdeform(float,FILE *);
extern void plotdraw(FILE *);
extern void plotelem(int,float,int,int,FILE *);
extern void plotlabel(int,int,FILE *);
extern void plotline(int,int,FILE *);
extern void plotmodel(FILE *);
extern void plotnode(int,int,int,FILE *);
extern void plotpbc(int,int,FILE *);
extern void plotpoint(int,int,FILE *);
extern void plotresult(int,int,int,float,float,FILE *);
extern void plotsym(long,long,long,long,long,long,long,long,long,float,FILE *);
extern void showpage(FILE *);
extern void writeclose(FILE *);
extern void writeprolog(FILE *);

int arg2id(char *,int,int,int *);
int eval_args(int,int,char **);
int getalias(char *);
int getfun(char *);
int getlabel(int);
int get_line(int);
int getname(char *);
int newelem(int);
int newlabel(int);
int newline(int);
int newmat(int,int);
int newnode(int);
int newpoint(int);
int *newobject(int,int,int *);
void toglobal(float,float,float *,float *);
void tolocal(float,float,float *,float *);
void stolower(char *);

void _com_err(),_com_plane(),_com_axisym(),_com_problem(),_com_super(),
     _com_mat(),_com_lam(),_com_node(),_com_elem(),_com_pbc(),_com_dbc(),
     _com_check(),_com_eval(),_com_select(),_com_save(),_com_read(),
     _com_reset(),_com_stop(),_com_version(),_com_info(),_com_open(),
     _com_close(),_com_plot(),_com_show(),_com_fill(),_com_hard(),
     _com_session(),_com_window(),_com_coord(),_com_grid(),_com_find(),
     _com_zoom(),_com_move(),_com_clear(),_com_group(),_com_set(),_com_get(),
     _com_alias(),_com_cd(),_com_why(),_com_color(),_com_view(),_com_auto(),
     _com_point(),_com_line(),_com_label();

#else

char *_args[MAXARG];
char field[320];

extern void error();
extern int getalias(char *);
extern int parse();

extern void _com_err(),_com_plane(),_com_axisym(),_com_problem(),_com_super(),
     _com_mat(),_com_lam(),_com_node(),_com_elem(),_com_pbc(),_com_dbc(),
     _com_check(),_com_eval(),_com_select(),_com_save(),_com_read(),
     _com_reset(),_com_stop(),_com_version(),_com_info(),_com_open(),
     _com_close(),_com_plot(),_com_show(),_com_fill(),_com_hard(),
     _com_session(),_com_window(),_com_coord(),_com_grid(),_com_find(),
     _com_zoom(),_com_move(),_com_clear(),_com_group(),_com_set(),_com_get(),
     _com_alias(),_com_cd(),_com_why(),_com_color(),_com_view(),_com_auto(),
     _com_point(),_com_line(),_com_label();

static void (*commands[])() = {
_com_err,
_com_plane,_com_axisym,_com_problem,_com_super,_com_mat,_com_lam,_com_node,
_com_elem,_com_pbc,_com_dbc,_com_check,_com_eval,_com_select,_com_save,
_com_read,_com_reset,_com_stop,_com_version,_com_info,_com_open,_com_close,
_com_plot,_com_show,_com_fill,_com_hard,_com_session,_com_window,_com_coord,
_com_grid,_com_find,_com_zoom,_com_move,_com_clear,_com_group,_com_set,
_com_get,_com_alias,_com_cd,_com_why,_com_color,_com_view,_com_auto,_com_point,
_com_line,_com_label
};

#endif
