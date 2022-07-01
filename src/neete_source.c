/*
Next Evolution Enhanced Text Editor (NEETE), A Visual Diagrammatic Text Editor
Copyright (C) 2016 - 2022 Tinashe Gwena

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include <unistd.h>

#include "cJSON.h"

#include "dynarray.h"

#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#define json_add_number(id, num, dest, label)		id = cJSON_CreateNumber(num); \
        if (id == NULL) 	\
            {           	\
            goto label;     \
            }               \
        cJSON_AddItemToObject(dest, #id, id);

#define json_add_number_arr(id, num, dest, label)		id = cJSON_CreateNumber(num);; \
        	if (id == NULL)	\
            	{			\
            	goto label;	\
            	}			\
        	cJSON_AddItemToArray(dest, id);

#define json_add_string(id, str, dest, label)        id = cJSON_CreateString(str); \
        if (id == NULL)		\
            {				\
            goto label;		\
            }				\
        cJSON_AddItemToObject(dest, #id, id);

#define json_add_array(id, dest, label)			id = cJSON_CreateArray(); \
    	if (id == NULL)		\
    	{					\
        	goto label;		\
    	}					\
    	cJSON_AddItemToObject(dest, #id, id);

#define current_nodegroup codeBox_list[current_codeBox]

#define the_node_group codeBox_list[this_codeBox_id].node_group

#define the_contained_group  codeBox_list[this_codeBox_id].node_group
typedef struct {
    int node_id;
    int codeBox_id;

    GtkWidget 	*b_head_strip;
    GtkEntry 	*e_txt_codebox_head;

    GtkSourceView	*s_source_view;
    GtkTextBuffer   *e_txt_codebox_code;

    GtkWidget 	*b_tail_strip;
    GtkEntry 	*e_txt_codebox_tail;
} a_ribbon_element;
typedef struct {
    int nodes[100];
    int length;
} a_node_array;
typedef struct {
    int x;
    int y;
    int width;
    int height;
} a_nodezone;
typedef struct {
    GtkWidget *b_rbtn_c_cpp;
    GtkWidget *b_rbtn_html;
    GtkWidget *b_rbtn_java;
    GtkWidget *b_rbtn_javascript;
    GtkWidget *b_rbtn_latex;
    GtkWidget *b_rbtn_php;
    GtkWidget *b_rbtn_python;
    GtkWidget *b_rbtn_verilog;
    GtkWidget *b_rbtn_plain_text;
} language_widgets;
typedef struct {
    GtkWidget *r_movebtn;
    GtkWidget *r_containbtn;
    GtkWidget *r_linkbtn;
} tools_widgets;
typedef struct {
    float width;
    float height;
    int rowchars;
    bool valid;
    unsigned char positions[10];
    unsigned char num_positions;
} a_header_dimension;
typedef struct {
    int node;
    int codeBox_id;
    int containing_node_id;

    bool is_single;

    a_ribbon_element *editing_ribbon;
    int ribbon_count;
} a_node_note;
typedef enum {
    CLOSE,
    QUIT
} closing_modes;
typedef struct {
    a_nodezone array[20];
    int length;
} a_nodezone_array;
typedef struct {
    int x;
    int y;
    int width;
    int height;
    int next;
    int previous;
    int cont_head;
    int cont_by;
    int priority;
    int* contained;

    char* name;
    char* description;

    char* head;
    char* tail;

    char* real_code;

    int node_id;

    char is_file;
    int file_tab_id;

    int* obscuring;
    int* obscured_by;
    bool obscured;

    char status;

    a_header_dimension header_dim;
} a_node;
typedef struct {
    int node;
    int zone;
    int codeBox_id;
    float scale_factor;
    float x;
    float y;
    float node_x;
    float node_y;
    float node_width;
    float node_height;
    float frame_x;
    float frame_y;
    bool node_found;
} a_nodezoneinfo;
typedef struct {
    float x;
    float y;
    float scaleFactor;
} a_container_pos;
typedef struct {
    int node;
    int codeBox_id;
    double scroll_x;
    double scroll_y;
} focus_item;
typedef struct {
    float r;
    float g;
    float b;
} RGB;
typedef struct {
    int original;
    int newone;
} a_translate_vector;
struct {
    int node;
    int source_codeBox;
} node_to_copy;
typedef struct {
//GtkWidget *menu_popup;
//GtkWidget *w_txtvw_main;            // Pointer to text view object
    GtkWidget *w_dlg_file_choose;       // Pointer to file chooser dialog box
    GtkWidget *w_dlg_file_save;       // Pointer to file save dialog box
//GtkTextBuffer *textbuffer_main;     // Pointer to text buffer
    GtkWidget *drawingArea;

//----------------------------------
    GtkWidget 	*w_dlg_codebox;   	// the codebox editor window
    GtkEntry 	*e_txt_codebox_name;
    GtkEntry 	*e_txt_codebox_description;
    GtkWidget       *b_chkbtn_codebox_ignore;
    GtkWidget       *b_chkbtn_codebox_isfile;
    GtkEntry 	*e_txt_codebox_head;
    GtkEntry 	*e_txt_codebox_priority;
    GtkSourceView	*s_source_view;
    GtkTextBuffer   *e_txt_codebox_code;
    GtkEntry 	*e_txt_codebox_tail;
    GtkNotebook	*t_ntb_nete_tabs;
    GtkWidget	*w_darea_scroll;
    GtkLabel 	*l_lbl_codebox_focus;
    GtkWidget       *c_cont_scrollwin;
    GtkWidget       *b_box_ribbon;
//GtkWidget *

//------------------------------------
//  options window
    GtkWidget 	*w_dlg_options;
    GtkEntry 	*e_txt_options_destinations;

// ----------------------------------------
// search bar
    GtkEntry	*e_txt_search_line;
    GtkLabel 	*l_lbl_main_feedback;

// -----------------------------------
// quit warning
    GtkDialog *w_msg_file_save_warn;

    GtkWidget 	*b_mnu_wrap;

    GtkWidget *w_dlg_about;

    language_widgets language_buttons;
    tools_widgets tool_radio_buttons;

//----------------------------------------------
    GtkFileFilter *netefilter;
    GtkFileFilter *sessionfilter;
} app_widgets;
typedef enum {
    GRID,
    FINE,
    NONE
} snapping_modes;
typedef struct {
    bool last_touched;
    bool last_read;
    bool libre;
    bool obscured;
    bool being_dragged;
    bool being_resized;
    bool to_ignore;

} node_mode_flags;
typedef struct {
    int codeBox_id;
} a_tab_to_codeBox;
typedef enum {
    CES,
    ABR,
    ABL,
    ATR,
    ATL,
    EL,
    ER,
    ET,
    EB
} shapepostype;
typedef enum {
    CELASTIC,
    ANCHOR,
    ELASTIC
} shapebasetype;
typedef struct {
    float x;
    float y;
    float auxx;
    float auxy;
} fixed_coords;
typedef enum {
    RECTANGLE,
    PATH,
    ARC
} shape_prims;
typedef struct {
    shape_prims basic_shape;
    shapepostype shape_pos_type;
    shapebasetype shape_base_type;
    float *parameters;
} shape_element;
typedef struct {
    shape_element* shapes_sequence;
    int anchor_params[8];
    int elastic_params[4];
} a_body_shape;
typedef enum {
    MOVE,
    EXITENTER,
    JOIN
} toolingmodes;
typedef struct {
    int node_ID;
    int x_coord;
    int y_coord;
    int xy_combined;
} a_qsort_element;
typedef struct {
    a_node *node_group;
    int current_codeBox;
    float zoomScaleFactor;
    a_node_note topNode;
    a_node_note cont_by;
    a_node_note last_node_read;
    bool is_embedded;
    GtkWidget *codebox_container;
    char *destination;
    char *name;
    char *fileName;
    char *fileNameSolo;
    char *fileDirectory;
    bool node_group_altered;
    bool fileSaveAsMode;
    focus_item *focusList;
    double scroll_x;
    double scroll_y;
    GtkWidget *current_highlight_button;
    bool line_wrapping;
    int group_id;

    GtkSourceLanguageManager *manager;
    GtkSourceLanguage *language;
} a_node_group_context;
RGB colorConverter(int hexValue);
void redraw(cairo_t *cr) ;
void draw_arc(cairo_t *cr, int node1, int node2, int codeBox_id, int frame_x, int frame_y, float scale_factor);
void drawGrid(cairo_t *cr);
void drawNode(cairo_t *cr, int node, int codeBox_id, int frame_x, int frame_y, int frame_width, int frame_height, float scale_factor, node_mode_flags node_modes, bool container_state);
int node_group_push(int codeBox_id, int x, int y, int width, int height, int next, int previous, int cont_head, int cont_by, int priority, int* contained, char *name, char *description, char *head, char *tail, char *real_code, int node_id, char status, char is_file);
int node_group_push_id(int codeBox_id, int x, int y, int width, int height, int next, int previous, int cont_head, int cont_by, int priority, int* contained, char *name, char *description, char *head, char *tail, char *real_code, int node_id, char status, char is_file);
int node_zones_push(int x, int y, int width, int height);
void init_arrays();
void containerRemoveNode(int containing_node, int  inner_node, int codeBox_id);
float calculateScale(int node, int codeBox_id, int *contained_nodes, int contained_codeBox_id,int y_displacement, float scale_factor);
void init_nodes();
void get_absolutePosition(a_container_pos *container_pos, int node, int codeBox_id);
void doMouseDown2(int x, int y, gpointer data);
void doMouseDrag2(int x, int y, gpointer data);
void doDoubleClick(int x, int y, gpointer data, app_widgets *app_wdgts);
void doMouseUp2(int x, int y, gpointer data);
void getNodeAndZone(a_nodezoneinfo *return_nodezone, int *nodes, a_node_note top_node_struct, int x, int y, int frame_x, int frame_y, float scale_factor, int omit);
void fixZones(int node, int codeBox_id);
float snapGrid(float invalue);
void deactivate_quit(void);
int * getPrimaryNodes(a_node_note top_node_struct);
void load_codeBox(char *nete_string, app_widgets *app_wdgts, int codeBox_id);
void launchBox(int node, int codeBox_id, int line_start, app_widgets *app_wdgts);
void create_text_ribbon(int node, int codeBox_id, int line_start, app_widgets *app_wdgts);
void clear_ribbon(app_widgets *app_wdgts);
void ribbon_submit(void);
void node_group_update_id(int node_id, int group_id, char *name, char *description, char *head, int priority,  char *real_code, char *tail, char status, char is_file, int file_tab_id);
void node_group_update_id_min(int node_id, int group_id,  char *head, char *real_code, char *tail);
char* stringify_nodes(int codeBox_id);
char* stringify_session(void);
void draw_text_in_box(cairo_t *cr, char *real_code, float x, float y, float width, float height);
void drawX(cairo_t *cr, int x, int y, int width, int height);
void draw_nodes(cairo_t *cr, int *nodes, int codeBox_id, int x, int y, int width, int height, float scale_factor, bool container_state);
void string_init(char** string_ptr);
void string_free(char** string_ptr);
void string_set(char** destination, char* value);
void string_add(char** destination, char* value);
void clearRenderedData(void);
void renderDataNodes(int* nodes, bool allow_previous);
void renderRibbonDataNodes(int* nodes, bool allow_previous, app_widgets *app_wdgts);
void renderLineDataNodes(int* nodes, bool allow_previous);
void init_node_group_context(a_node_group_context *temp_group_list);
int pasteNode(int parent, int node, int source_codeBox);
void fixPasted(int source_codeBox);
int get_key(int key);
void clearTranslations(void);
int count_string_lines(char *string);
void line_search(app_widgets *app_wdgts);
void write_to_file(app_widgets *app_wdgts);
void actual_write_to_file(char* filename, int CodeBox_id);
int create_new_tab(app_widgets *app_wdgts);
void write_feedback(app_widgets *app_wdgts, char *text);
bool recontain_node(a_nodezoneinfo* firstNodeZone, a_nodezoneinfo* secondNodeZone, int codeBox_id, int x, int y);
bool join_nodes(a_nodezoneinfo* firstNodeZone, a_nodezoneinfo* secondNodeZone, int codeBox_id);
bool check_not_descendent(int main_node, int desc_node, int codeBox_id);
bool check_not_circular(int main_node, int susp_node, int codeBox_id);
void check_obscured(int node, int codeBox_id);
void check_obscuring(int node, int codeBox_id);
void clear_obscured_by(int node, int codeBox_id);
void clear_obscuring(int node, int codeBox_id);
void splice_out_node(int *array, int inner_node);
void draw_libre_arc(cairo_t *cr, int node, int frame_x, int frame_y, float scale_factor);
void mark_unsaved(app_widgets *app_wdgts);
bool check_unsaved(void);
void remove_nodegroup(app_widgets *app_wdgts);
void set_node_group(int new_group_index, app_widgets *app_wdgts);
void delete_group_context(a_node_group_context *temp_group_list);
void emptyout_node(a_node *node);
a_header_dimension get_header_dimensions(cairo_t *cr, char *header_text, int header_font_size, int min_width, int max_width);
int load_codeBox_file(char *file_name, app_widgets *app_wdgts, bool is_embedded);
void load_session(char *nete_string, app_widgets *app_wdgts);
void draw_body(cairo_t *cr, int x,int  y, int width, int height, int priority, float children_scale_factor, bool last_read, int body_id, bool container_state);
void init_body_shape(a_body_shape * bshape);
void init_shape_element(shape_element * elmnt);
void create_sample_shapes();
void fix_poscoords(fixed_coords* return_coords, shapepostype shape_poscoord, float x, float y, float auxx, float auxy, int minx, int miny, int maxx, int maxy, int shape_id );
void load_body_shapes();
void quick_sort(a_qsort_element array[], int low, int high);
int qpartition(a_qsort_element array[], int low, int high);
void qswap(a_qsort_element *a, a_qsort_element *b);
int* rearrange_nodes(int *nodes);
GtkWidget *pmenu;
cairo_pattern_t *pattern;

int screenWidth = 4095;
int screenHeight = 2047;

int grid_size = 20; // number of pixels
int major_grid = 5; //number of minor grids

int current_tab_id = 0;
int current_codeBox = 0;
a_node_group_context *codeBox_list;
a_node_note node_being_edited;
a_node_note node_being_dragged;
a_node_note node_being_resized;
int node_displacement_count = 1;

int *rendered_data;
int *rendered_counts;
int line_count = 0;
a_nodezone_array node_zones;

a_nodezoneinfo tempNodeZone;
bool no_node_selected = true;

int current_x;
int current_y;

a_body_shape *body_shapes;

a_nodezone rightClickCoord;
a_nodezone codeboxWindowPos;

bool nodeHeadFound = false;
int group_nodeHeadFound = -1;
bool nodeBodyFlankFound = false;
int group_nodeBodyFlankFound = -1;
bool nodeWidthAdjust = false;
int group_nodeWidthFound = -1;
char *output_text;
char *last_file_directory;

bool mousePressed = false;
bool mouse_down = false;

bool adminOp = false;
int focusNode_x;
int focusNode_y;
bool sessionMode = false;
bool fileSaveAsMode = false;
bool first_time_codebox = true;
char default_destination[] = "neete_out.txt";

bool exporting_file = false;

GTimer *last_refresh_time;
toolingmodes toolmode = MOVE;
closing_modes close_warn_mode = CLOSE;

a_tab_to_codeBox *tab_codeBoxes;

GtkSourceLanguageManager *manager;
GtkSourceLanguage *language;
int node_head_width = 70;
node_head_extrawidth = 35;
int node_head_height = 20;
int node_descript_height = 0;
int node_descript_width = 0;
int node_spacer_height = 2;
int node_text_height = 15;
int node_text_size = 12;
int node_flank_width = 15;
int node_resize_width = 15;
bool copyNode = false;
a_translate_vector *node_translation;

int priority_colour[] = {0xff0000, 0xff8000, 0xb3b300, 0x663300, 0x80ff00, 0x888888, 0x00664b, 0x005e80, 0x0000ff, 0x8000ff, 0x33334d};

int head_zone;
int body_zone;
int flank1_zone;
int flank2_zone;
int resize_zone;
gboolean on_drawingarea_draw( __attribute__((unused)) GtkWidget *widget, cairo_t *cr, __attribute__((unused)) gpointer data)  {
    redraw(cr);

    return FALSE;
}
gint on_btn_line_submit_clicked(__attribute__((unused)) GtkButton *widget,
                                app_widgets *app_wdgts)
{
    line_search(app_wdgts);

    return TRUE;
}
void on_txt_search_line_activate (GtkEntry *entry, app_widgets *app_wdgts, gpointer  user_data) {
    line_search(app_wdgts);

    return TRUE;
}
gint on_drawingarea_button_press_event(GtkWidget *widget, GdkEventButton *event, app_widgets *app_wdgts, gpointer data) {
    const guint RIGHT_CLICK = 3;

    if (event->type == GDK_BUTTON_PRESS) {

        GdkEventButton *bevent = (GdkEventButton *) event;

        if (bevent->button == RIGHT_CLICK) {
            rightClickCoord.x = (guint)event->x;
            rightClickCoord.y = (guint)event->y;
            gtk_menu_popup_at_pointer(GTK_MENU(pmenu), event);
        } else {

            mouse_down = true;

            doMouseDown2((guint)event->x, (guint)event->y, data);
        }

        return TRUE;
    }

    if(event->type == GDK_DOUBLE_BUTTON_PRESS) {
        doDoubleClick((guint)event->x, (guint)event->y, data, app_wdgts);
    }


    gtk_widget_queue_draw( widget);
    mark_unsaved(app_wdgts);

    return TRUE;
}
gint on_drawingarea_motion_notify_event(GtkWidget *widget, GdkEventButton *event,
                                        gpointer data)
{
    gulong refresh_msec = 0;

    if(mouse_down) {

        doMouseDrag2((guint)event->x, (guint)event->y, data);

        g_timer_elapsed (last_refresh_time, &refresh_msec);
        // we are going to limit refreshes while dragging to 10 times a second
        //printf("refresh time %dms   \n", refresh_msec);

        if(refresh_msec > 20000) {
            gtk_widget_queue_draw( widget);
            g_timer_start (last_refresh_time);
        }
    }
    return TRUE;
}
gint on_drawingarea_button_release_event(GtkWidget *widget, GdkEventButton *event, app_widgets *app_wdgts, gpointer data) {
    doMouseUp2((guint)event->x, (guint)event->y, data);

    mouse_down = false;

    gtk_widget_queue_draw( widget);
    mark_unsaved(app_wdgts);

    return TRUE;
}
gint on_drawingarea_drag_motion(__attribute__((unused)) GtkButton *widget,
                                __attribute__((unused)) gint x,
                                __attribute__((unused)) gint  y,
                                __attribute__((unused)) guint time,
                                __attribute__((unused)) gpointer data)
{
    return TRUE;
}
int nete_new_node(__attribute__((unused)) GtkWidget *widget, GtkWidget *ddarea) {
    int* temp_contained_array = dynarray_create(int);

    int* primary_nodes;
    int destination_group;
    int destination_container;
    int destination_topNode;

    float destination_x = (rightClickCoord.x / current_nodegroup.zoomScaleFactor);
    float destination_y = (rightClickCoord.y / current_nodegroup.zoomScaleFactor);

    a_container_pos container_pos;
    a_node_note contained_top_node_struct;
    float better_scale;

    a_nodezoneinfo newNodeZone;
    a_nodezoneinfo newnodeNodeZone;

    primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

    getNodeAndZone(&newNodeZone, primary_nodes, current_nodegroup.topNode, rightClickCoord.x, rightClickCoord.y, 0, 0, current_nodegroup.zoomScaleFactor, -1);
    dynarray_destroy(primary_nodes);

    if(codeBox_list[newNodeZone.codeBox_id].node_group[newNodeZone.node].is_file == 1 && codeBox_list[newNodeZone.codeBox_id].node_group[newNodeZone.node].file_tab_id != -1) {
        destination_group = codeBox_list[newNodeZone.codeBox_id].node_group[newNodeZone.node].file_tab_id;
        destination_container = -1;
        destination_topNode = -1;

        get_absolutePosition(&container_pos, newNodeZone.node, newNodeZone.codeBox_id);

        contained_top_node_struct.node = -1;
        contained_top_node_struct.codeBox_id = destination_group;
        primary_nodes = getPrimaryNodes(contained_top_node_struct);
        better_scale = container_pos.scaleFactor * calculateScale(newNodeZone.node, newNodeZone.codeBox_id, primary_nodes, destination_group, 0, container_pos.scaleFactor);
        dynarray_destroy(primary_nodes);

        destination_x = (rightClickCoord.x - container_pos.x) / better_scale;
        destination_y = (rightClickCoord.y - container_pos.y) / better_scale;
    } else {
        destination_group = newNodeZone.codeBox_id;
        destination_container =  newNodeZone.node;
        destination_topNode = current_nodegroup.topNode.node;
    }



    printf("inserting into %d %d with topnode %d\n", newNodeZone.node, newNodeZone.codeBox_id, codeBox_list[newNodeZone.codeBox_id].topNode.node);


    newnodeNodeZone.node = node_group_push(
                               destination_group,
                               destination_x,
                               destination_y,
                               100,
                               20,
                               -1,
                               -1,
                               -1,
                               destination_topNode,
                               5,
                               temp_contained_array,
                               "New Node",
                               "",
                               "",
                               "",
                               "",
                               -1,
                               0,
                               0
                           );

    newnodeNodeZone.codeBox_id = destination_group;

    if(destination_topNode != -1) {
        dynarray_push(codeBox_list[destination_group].node_group[destination_topNode].contained, newnodeNodeZone.node);
    }

    if((destination_container != codeBox_list[destination_group].topNode.node ) && (newNodeZone.zone == 1 || newNodeZone.zone == 2 || newNodeZone.zone == 3)) {
        recontain_node(&newnodeNodeZone, &newNodeZone, destination_group, rightClickCoord.x, rightClickCoord.y);
    }

    check_obscuring(newnodeNodeZone.node, destination_group);
    check_obscured(newnodeNodeZone.node, destination_group);

    dynarray_destroy(temp_contained_array);

    gtk_widget_queue_draw(ddarea);
    deactivate_quit();
    return TRUE;
}
int nete_copy_node(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) GtkWidget *ddarea) {
    mousePressed = false;

    int* primary_nodes;

    primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

    getNodeAndZone(&tempNodeZone, primary_nodes, current_nodegroup.topNode, rightClickCoord.x, rightClickCoord.y, 0, 0, current_nodegroup.zoomScaleFactor, -1);

    dynarray_destroy(primary_nodes);

    if (tempNodeZone.node != -1) {
    }

    if (tempNodeZone.zone == head_zone) {
        copyNode = true;

        node_to_copy.node = tempNodeZone.node;
        node_to_copy.source_codeBox = current_codeBox;
    }

    return 0;
}
int nete_paste_node(__attribute__((unused)) GtkWidget *widget, GtkWidget *ddarea) {
    int* primary_nodes;
    a_nodezoneinfo newNodeZone;

    a_translate_vector temp_translate;
    temp_translate.original = -1;
    temp_translate.newone = -1;

    a_nodezoneinfo pastedNodeZone;

    dynarray_push(node_translation, temp_translate);

    pastedNodeZone.node = pasteNode(current_nodegroup.topNode.node, node_to_copy.node, node_to_copy.source_codeBox);
    fixPasted(node_to_copy.source_codeBox);
    clearTranslations();

    primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

    getNodeAndZone(&newNodeZone, primary_nodes, current_nodegroup.topNode, rightClickCoord.x, rightClickCoord.y, 0, 0, current_nodegroup.zoomScaleFactor, -1);

    if(newNodeZone.node != current_nodegroup.topNode.node && (newNodeZone.zone == 1 || newNodeZone.zone == 2 || newNodeZone.zone == 3)) {
        recontain_node(&pastedNodeZone, &newNodeZone, newNodeZone.codeBox_id, rightClickCoord.x, rightClickCoord.y);
    }

    check_obscuring(pastedNodeZone.node, current_codeBox);
    check_obscured(pastedNodeZone.node, current_codeBox);
    gtk_widget_queue_draw(ddarea);
    deactivate_quit();

    dynarray_destroy(primary_nodes);

    return 0;
}
int nete_focus_node(__attribute__((unused)) GtkWidget *widget, app_widgets *app_wdgts) {
    int this_codeBox_id;

    mousePressed = false;
    focus_item temp_focus_item;
    int* primary_nodes;
    GtkAdjustment *scrollx;
    GtkAdjustment *scrolly;

    primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

    getNodeAndZone(&tempNodeZone, primary_nodes, current_nodegroup.topNode, rightClickCoord.x, rightClickCoord.y, 0, 0, current_nodegroup.zoomScaleFactor, -1);

    dynarray_destroy(primary_nodes);

    this_codeBox_id = tempNodeZone.codeBox_id;

    if (tempNodeZone.node != -1) {
        if (tempNodeZone.zone == head_zone) {
            scrollx = gtk_scrolled_window_get_hadjustment (app_wdgts->w_darea_scroll);
            scrolly = gtk_scrolled_window_get_vadjustment (app_wdgts->w_darea_scroll);
            temp_focus_item.scroll_x = gtk_adjustment_get_value (scrollx);
            temp_focus_item.scroll_y = gtk_adjustment_get_value (scrolly);
            gtk_adjustment_set_value (scrollx, 0.0);
            gtk_adjustment_set_value (scrolly, 0.0);
            temp_focus_item.node = current_nodegroup.topNode.node;
            temp_focus_item.codeBox_id = current_nodegroup.topNode.codeBox_id;
            dynarray_push(current_nodegroup.focusList, temp_focus_item);
            if(the_node_group[tempNodeZone.node].is_file == 1 && the_node_group[tempNodeZone.node].file_tab_id != -1) {
                current_nodegroup.topNode.node = -1;
                current_nodegroup.topNode.codeBox_id = the_node_group[tempNodeZone.node].file_tab_id;
            } else {
                current_nodegroup.topNode.node = tempNodeZone.node;
                current_nodegroup.topNode.codeBox_id = tempNodeZone.codeBox_id;
            }
            gtk_label_set_text (app_wdgts->l_lbl_codebox_focus, current_nodegroup.node_group[tempNodeZone.node].name);
            gtk_widget_show(app_wdgts->l_lbl_codebox_focus);
            gtk_widget_queue_draw(app_wdgts->drawingArea);
        }
    }

    return 0;
}
int nete_restore_all(__attribute__((unused)) GtkWidget *widget, app_widgets *app_wdgts) {
    int temp_topNode;
    focus_item temp_focus_item;
    GtkAdjustment *scrollx;
    GtkAdjustment *scrolly;
    if(dynarray_length(current_nodegroup.focusList) > 0) {
        scrollx = gtk_scrolled_window_get_hadjustment (app_wdgts->w_darea_scroll);
        scrolly = gtk_scrolled_window_get_vadjustment (app_wdgts->w_darea_scroll);
        dynarray_pop(current_nodegroup.focusList, &temp_focus_item);
        current_nodegroup.topNode.node = temp_focus_item.node;
        current_nodegroup.topNode.codeBox_id = temp_focus_item.codeBox_id;
        gtk_adjustment_set_value (scrollx, temp_focus_item.scroll_x);
        gtk_adjustment_set_value (scrolly, temp_focus_item.scroll_y);
    }

    if(current_nodegroup.topNode.node == -1) {
        gtk_widget_hide(app_wdgts->l_lbl_codebox_focus);
    } else {
        gtk_label_set_text (app_wdgts->l_lbl_codebox_focus, current_nodegroup.node_group[current_nodegroup.topNode.node].name);
    }
    gtk_widget_queue_draw(app_wdgts->drawingArea);

    return 0;
}
gboolean on_msg_file_save_warn_delete_event (GtkWidget *widget, GdkEvent  *event, gpointer   user_data) {
    gtk_widget_hide(widget);

    return TRUE;
}
void on_window_delete_event(GtkWidget *object, app_widgets *app_wdgts, gpointer user_data) {

    if(check_unsaved()) {
        close_warn_mode = QUIT;
        gtk_widget_show(user_data);
    } else {
        gtk_main_quit();
    }
}
void on_msg_file_save_warn_close (GtkDialog *dialog, gpointer   user_data) {
}
void on_msg_file_save_warn_response (GtkDialog *dialog, gint       response_id, app_widgets *app_wdgts) {
    if(response_id == -9) {
        gtk_widget_hide(dialog);
    } else if(response_id == -8) {
        switch(close_warn_mode) {
        case CLOSE:
            remove_nodegroup(app_wdgts);
            gtk_widget_hide(dialog);
            break;
        case QUIT:
            gtk_main_quit();
            break;
        }
    }
}
void on_btn_codebox_submit_clicked(__attribute__((unused)) GtkButton *widget, app_widgets *app_wdgts, __attribute__((unused)) gpointer data) {
    const gchar *name;
    const gchar *description;
    const gchar *head;
    const gchar *priority;
    const gchar *real_code;
    const gchar *tail;

    char status = 0;
    char isfile = 0;
    int file_tab_id = -1;

    int priority_num = 0;

    if(node_being_edited.node != -1 && node_being_edited.codeBox_id != -1) {

        name = gtk_entry_get_text (app_wdgts->e_txt_codebox_name);
        description = gtk_entry_get_text (app_wdgts->e_txt_codebox_description);

        if (gtk_toggle_button_get_active (app_wdgts->b_chkbtn_codebox_ignore)) {
            status = 1;
        } else {
            status = 0;
        }

        head = gtk_entry_get_text (app_wdgts->e_txt_codebox_head);
        priority = gtk_entry_get_text (app_wdgts->e_txt_codebox_priority);
        priority_num = atoi(priority);
        if(priority_num < 0) {
            priority_num = 0;
        }
        if(priority_num > 9) {
            priority_num = 9;
        }

        tail = gtk_entry_get_text (app_wdgts->e_txt_codebox_tail);

        if (gtk_toggle_button_get_active (app_wdgts->b_chkbtn_codebox_isfile)) {
            file_tab_id = load_codeBox_file(real_code, app_wdgts, true);
            codeBox_list[file_tab_id].cont_by.node = node_being_edited.node;
            codeBox_list[file_tab_id].cont_by.codeBox_id = node_being_edited.codeBox_id;

            printf("file_loaded\n");
            isfile = 1;
        } else {
            isfile = 0;
        }

        node_group_update_id(
            node_being_edited.node,
            node_being_edited.codeBox_id,
            (char *) name,
            (char *) description,
            (char *) head,
            priority_num,
            (char *) real_code,
            (char *) tail,
            status,
            isfile,
            file_tab_id);

        ribbon_submit();

        node_being_edited.node = -1;
        node_being_edited.codeBox_id = -1;
    }

    gtk_window_get_position(GTK_WINDOW(app_wdgts->w_dlg_codebox), &codeboxWindowPos.x, &codeboxWindowPos.y);
    first_time_codebox = false;

    gtk_widget_hide(app_wdgts->w_dlg_codebox);

    gtk_widget_queue_draw(app_wdgts->drawingArea);
    deactivate_quit();
    mark_unsaved(app_wdgts);
}
gboolean on_window_codebox_delete_event( GtkWidget *widget, __attribute__((unused)) GdkEvent *event, __attribute__((unused)) gpointer data) {
    gtk_window_get_position(GTK_WINDOW(widget), &codeboxWindowPos.x, &codeboxWindowPos.y);
    first_time_codebox = false;

    gtk_widget_hide(widget);
    return TRUE;
}
void on_btn_codebox_tocodebox_clicked(__attribute__((unused)) GtkButton *widget, app_widgets *app_wdgts, __attribute__((unused)) gpointer data) {
    gchar *selected_text;
    int* temp_contained_array = dynarray_create(int);
    int top_node;
    int newnodeID;

    GtkTextIter start;
    GtkTextIter end;


    if(node_being_edited.is_single) {
        if(gtk_text_buffer_get_selection_bounds (node_being_edited.editing_ribbon[0].e_txt_codebox_code, &start, &end)) {

            selected_text = gtk_text_buffer_get_text(node_being_edited.editing_ribbon[0].e_txt_codebox_code, &start, &end, FALSE);

            gtk_text_buffer_delete_selection (node_being_edited.editing_ribbon[0].e_txt_codebox_code, FALSE, FALSE);

            top_node = current_nodegroup.node_group[node_being_edited.node].cont_by;

            newnodeID = node_group_push(
                            current_codeBox,
                            current_nodegroup.node_group[node_being_edited.node].x + node_displacement_count * 5,
                            current_nodegroup.node_group[node_being_edited.node].y + node_displacement_count * 5,
                            100,
                            20,
                            -1,
                            -1,
                            -1,
                            top_node,
                            5,
                            temp_contained_array,
                            "New Node",
                            "",
                            "",
                            "",
                            selected_text,
                            -1,
                            0,
                            0
                        );

            if(top_node != -1 ) {
                dynarray_push(current_nodegroup.node_group[top_node].contained, newnodeID);
            }

            check_obscuring(newnodeID, current_codeBox);
            check_obscured(newnodeID, current_codeBox);

            dynarray_destroy(temp_contained_array);
            g_free(selected_text);

            gtk_widget_queue_draw(app_wdgts->drawingArea);
            deactivate_quit();

            node_displacement_count ++;

        }
    }
}
void on_btn_codebox_tohead_clicked(__attribute__((unused)) GtkButton *widget, app_widgets *app_wdgts, __attribute__((unused)) gpointer data) {
    gchar *selected_text;
    const gchar *head_text;

    char* final_text;

    int* temp_contained_array = dynarray_create(int);
    int top_node;
    int newnodeID;

    GtkTextIter start;
    GtkTextIter end;

    if(node_being_edited.is_single) {
        if(gtk_text_buffer_get_selection_bounds (node_being_edited.editing_ribbon[0].e_txt_codebox_code, &start, &end)) {
            string_init(&final_text);

            selected_text = gtk_text_buffer_get_text(node_being_edited.editing_ribbon[0].e_txt_codebox_code, &start, &end, FALSE);

            gtk_text_buffer_delete_selection (node_being_edited.editing_ribbon[0].e_txt_codebox_code, FALSE, FALSE);

            head_text = gtk_entry_get_text (app_wdgts->e_txt_codebox_head);
            string_set(&final_text, head_text);
            string_add(&final_text, " ");
            string_add(&final_text, selected_text);

            gtk_entry_set_text (app_wdgts->e_txt_codebox_head, final_text);

            string_free(&final_text);
            g_free(selected_text);
        }
    }
}
void on_btn_codebox_totail_clicked(__attribute__((unused)) GtkButton *widget, app_widgets *app_wdgts, __attribute__((unused)) gpointer data) {
    gchar *selected_text;
    const gchar *tail_text;

    char* final_text;

    int* temp_contained_array = dynarray_create(int);
    int top_node;
    int newnodeID;

    GtkTextIter start;
    GtkTextIter end;


    if(node_being_edited.is_single) {
        if(gtk_text_buffer_get_selection_bounds (node_being_edited.editing_ribbon[0].e_txt_codebox_code, &start, &end)) {
            string_init(&final_text);

            selected_text = gtk_text_buffer_get_text(node_being_edited.editing_ribbon[0].e_txt_codebox_code, &start, &end, FALSE);

            gtk_text_buffer_delete_selection (node_being_edited.editing_ribbon[0].e_txt_codebox_code, FALSE, FALSE);

            tail_text = gtk_entry_get_text (app_wdgts->e_txt_codebox_tail);

            string_set(&final_text, selected_text);
            string_add(&final_text, " ");
            string_add(&final_text, tail_text);

            gtk_entry_set_text (app_wdgts->e_txt_codebox_tail, final_text);

            string_free(&final_text);
            g_free(selected_text);
        }

    }
}
void on_mnu_item_new_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    a_tab_to_codeBox temp_tab_codeBox;

    temp_tab_codeBox.codeBox_id = create_new_tab(app_wdgts);
    dynarray_push(tab_codeBoxes, temp_tab_codeBox);
    current_tab_id =  dynarray_length(tab_codeBoxes) - 1;


    gtk_notebook_set_current_page (app_wdgts->t_ntb_nete_tabs,
                                   current_tab_id);
}
void on_mnu_item_open_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gchar *file_name = NULL;        // Name of file to open from dialog box
    gchar *file_contents = NULL;    // For reading contents of file
    gboolean file_success = FALSE;  // File read status

    char *last_slash;

    char *tab_name;

    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose), app_wdgts->netefilter);
    if (strlen(last_file_directory) > 5) {
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose), last_file_directory);
    }
    gtk_widget_show(app_wdgts->w_dlg_file_choose);

    if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_choose)) == GTK_RESPONSE_OK) {
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose));
        if (file_name != NULL) {
            load_codeBox_file(file_name, app_wdgts, false);
        }
        g_free(file_name);
    }
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);
}
void on_mnu_item_save_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    write_to_file(app_wdgts);
}
void on_mnu_item_saveas_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    current_nodegroup.fileSaveAsMode = true;
    write_to_file(app_wdgts);
}
void on_mnu_item_close_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    if(current_nodegroup.node_group_altered) {
        close_warn_mode = CLOSE;
        gtk_widget_show(app_wdgts->w_msg_file_save_warn);
    } else {
        remove_nodegroup(app_wdgts);
    }
}
void on_mnu_item_render_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    FILE *fp;

    char feedback_date[30];
    char *feedback_text;

    int* primary_nodes;

    primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

    string_init(&feedback_text);
    string_set(&output_text, "");
    clearRenderedData();
    renderDataNodes(primary_nodes, false);

    fp = fopen(current_nodegroup.destination, "w");

    fprintf(fp, "%s\n", output_text);

    fclose(fp);

    string_set(&feedback_text, "wrote ");
    string_add(&feedback_text, current_nodegroup.destination);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(feedback_date, " : %d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    string_add(&feedback_text, feedback_date);
    write_feedback(app_wdgts, feedback_text);

    dynarray_destroy(primary_nodes);
}
void on_mnu_item_export_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    cairo_surface_t *surface2;
    cairo_t *cr2;

    exporting_file = true;

    surface2 = cairo_svg_surface_create("svgfile.svg", 4096, 2048);
    cr2 = cairo_create(surface2);

    redraw(cr2);
    cairo_surface_destroy(surface2);
    cairo_destroy(cr2);


    exporting_file = false;
}
void on_mnu_open_session_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gchar *file_name = NULL;        // Name of file to open from dialog box
    gchar *file_contents = NULL;    // For reading contents of file
    gboolean file_success = FALSE;  // File read status

    char *last_slash;

    char *tab_name;
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose), app_wdgts->sessionfilter);
    gtk_widget_show(app_wdgts->w_dlg_file_choose);

    if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_choose)) == GTK_RESPONSE_OK) {
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose));
        if (file_name != NULL) {

            printf("session filename -> %s\n", file_name);

            file_success = g_file_get_contents(file_name, &file_contents, NULL, NULL);
            if (file_success) {

                printf("session data -> \n\n%s\n", file_contents);
                load_session(file_contents, app_wdgts);
            }
            g_free(file_contents);

        }
        g_free(file_name);
    }
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);
}
void on_mnu_save_session_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gchar *file_name = NULL;
    char *output_filename;
    char *last_slash;
    char *tab_name;

    char feedback_text[50];

    char *string_session;

    FILE *fp;
    bool write_to_file = false;

    string_init(&output_filename);

    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save), app_wdgts->sessionfilter);

    gtk_widget_show(app_wdgts->w_dlg_file_save);

    if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_save)) == GTK_RESPONSE_OK) {
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save));

        string_set(&output_filename, file_name);

        if(strncmp(".prj", (file_name + strlen(file_name) - 4), 4) == 0) {
            printf("session file\n");
        } else {
            string_add(&output_filename, ".prj");
        }

        // ====== set up the tab's name =========================
        last_slash = strrchr(file_name, '/');
        string_init(&tab_name);
        string_set(&tab_name, last_slash + 1);

        write_to_file = true;

        string_free(&tab_name) ;

        g_free(file_name);
    }
    gtk_widget_hide(app_wdgts->w_dlg_file_save);


// =========== then write the actual file ===========

    if(write_to_file) {
        fp = fopen(output_filename, "w");

        string_session = stringify_session();
        printf("output session -> \n\n%s\n", string_session);
        fputs(string_session, fp);
        fclose(fp);

        free(string_session);
        string_free(&output_filename);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(feedback_text, "session saved: %d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        write_feedback(app_wdgts, feedback_text);
    }

}
void on_mnu_nete_quit_activate(__attribute__((unused)) GtkMenuItem *menuitem,  gpointer user_data) {
    if(check_unsaved()) {
        close_warn_mode = QUIT;
        gtk_widget_show(user_data);
    } else {
        gtk_main_quit();
    }
}
void on_mnu_move_item_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gtk_toggle_button_set_active (app_wdgts->tool_radio_buttons.r_movebtn, TRUE);
}
void on_mnu_contain_item_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gtk_toggle_button_set_active (app_wdgts->tool_radio_buttons.r_containbtn, TRUE);
}
void on_mnu_link_item_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gtk_toggle_button_set_active (app_wdgts->tool_radio_buttons.r_linkbtn, TRUE);
}
void on_mnu_findline_item_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gtk_entry_grab_focus_without_selecting(app_wdgts->e_txt_search_line);
}
void on_mnu_options_item_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gtk_entry_set_text (app_wdgts->e_txt_options_destinations, current_nodegroup.destination);

    gtk_widget_show(app_wdgts->w_dlg_options);
}
void on_btn_options_submit_clicked(__attribute__((unused)) GtkButton *widget, app_widgets *app_wdgts, __attribute__((unused)) gpointer data) {
    current_nodegroup.destination = (char *)gtk_entry_get_text (app_wdgts->e_txt_options_destinations);
    gtk_widget_hide(app_wdgts->w_dlg_options);
}
gboolean on_window_options_delete_event(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) GdkEvent *event, __attribute__((unused)) gpointer data) {
    gtk_widget_hide(widget);
    return TRUE;
}
void on_cmnu_wrap_toggled (GtkCheckMenuItem *checkmenuitem, app_widgets *app_wdgts, gpointer user_data) {
    int new_wrapping_mode;
    int i;

    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->b_mnu_wrap))) {
        new_wrapping_mode = GTK_WRAP_WORD_CHAR;
        current_nodegroup.line_wrapping = true;
    } else {
        new_wrapping_mode = GTK_WRAP_NONE;
        current_nodegroup.line_wrapping = false;
    }
    for(i = 0; i < dynarray_length(node_being_edited.editing_ribbon); i++) {
        gtk_text_view_set_wrap_mode (node_being_edited.editing_ribbon[i].s_source_view, new_wrapping_mode);
    }

    gtk_widget_queue_draw(app_wdgts->b_box_ribbon);
    gtk_widget_queue_draw(app_wdgts->c_cont_scrollwin);
    gtk_widget_queue_draw(app_wdgts->w_dlg_codebox);
}
void on_rbtn_language_toggled (GtkCheckMenuItem *checkmenuitem, app_widgets *app_wdgts, gpointer user_data) {
    int i;

    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_c_cpp))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "c");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_c_cpp;
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_html))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "html");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_html;
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_java))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "java");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_java;
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_javascript))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "js");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_javascript;
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_latex))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "latex");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_latex;
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_php))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "php");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_php;
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_python))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "python");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_python;
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_verilog))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "verilog");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_verilog;
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_plain_text))) {
        current_nodegroup.language = gtk_source_language_manager_get_language(manager, "text");
        current_nodegroup.current_highlight_button = app_wdgts->language_buttons.b_rbtn_plain_text;
    }
    for(i = 0; i < dynarray_length(node_being_edited.editing_ribbon); i++) {
        gtk_source_buffer_set_language(node_being_edited.editing_ribbon[i].e_txt_codebox_code, current_nodegroup.language);
    }
}
void on_mnu_zoom_in_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    current_nodegroup.zoomScaleFactor += 0.2;
    gtk_widget_queue_draw( app_wdgts->drawingArea);
}
void on_mnu_zoom_out_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    current_nodegroup.zoomScaleFactor -= 0.2;
    gtk_widget_queue_draw( app_wdgts->drawingArea);
}
void on_mnu_zoom_normal_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    current_nodegroup.zoomScaleFactor = 1;
    gtk_widget_queue_draw( app_wdgts->drawingArea);
}
void on_mnu_about_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gtk_widget_show(app_wdgts->w_dlg_about);
}
void on_dlg_about_response(GtkDialog *dialog, gint response_id, app_widgets *app_wdgts) {
    gtk_widget_hide(app_wdgts->w_dlg_about);
}
gboolean on_dlg_about_delete_event (GtkWidget *widget, GdkEvent  *event, gpointer   user_data) {
    gtk_widget_hide(widget);

    return TRUE;
}
void on_btn_rdo_move_toggled(GtkToggleButton *togglebutton, __attribute__((unused)) gpointer user_data) {
    if (gtk_toggle_button_get_active(togglebutton)) {
        toolmode = MOVE;
    }
}
void on_btn_rdo_exenter_toggled (GtkToggleButton *togglebutton, __attribute__((unused)) gpointer user_data) {
    if (gtk_toggle_button_get_active(togglebutton)) {
        toolmode = EXITENTER;
    }

}
void on_btn_rdo_link_toggled (GtkToggleButton *togglebutton, __attribute__((unused)) gpointer user_data) {
    if (gtk_toggle_button_get_active(togglebutton)) {
        toolmode = JOIN;
    }
}
gint on_ntb_nete_tabs_switch_page(__attribute__((unused)) GtkButton *widget, __attribute__((unused)) GtkWidget *page, guint page_num, app_widgets *app_wdgts, __attribute__((unused)) gpointer data)
{
    set_node_group(tab_codeBoxes[page_num].codeBox_id, app_wdgts);
    current_tab_id = page_num;

    return TRUE;
}
int node_group_push(int codeBox_id, int x, int y, int width, int height, int next, int previous, int cont_head, int cont_by, int priority, int* contained, char *name, char *description, char *head, char *tail, char *real_code, int node_id, char status, char is_file) {
    unsigned long int i;

    long unsigned int node_pos;

    a_node temp_node;

    temp_node.obscuring = dynarray_create(int);
    temp_node.obscured_by = dynarray_create(int);
    temp_node.obscured = false;

    temp_node.x = x;
    temp_node.y = y;
    temp_node.width = width;
    temp_node.height = height;
    temp_node.next = next;
    temp_node.previous = previous;
    temp_node.cont_head = cont_head;
    temp_node.cont_by = cont_by;
    temp_node.priority = priority;
    temp_node.status = status;
    temp_node.is_file = is_file;
    temp_node.file_tab_id = -1;

    temp_node.contained = dynarray_create(int);

    for(i = 0; i < dynarray_length(contained); i++) {
        dynarray_push(temp_node.contained, contained[i]);
    }
    string_init(&temp_node.name);
    string_set(&temp_node.name, name);

    string_init(&temp_node.description);
    string_set(&temp_node.description, description);

    string_init(&temp_node.head);
    string_set(&temp_node.head, head);

    string_init(&temp_node.tail);
    string_set(&temp_node.tail, tail);

    string_init(&temp_node.real_code);
    string_set(&temp_node.real_code, real_code);


    temp_node.header_dim.width = 1;
    temp_node.header_dim.height = 1;
    temp_node.header_dim.rowchars = 1;
    temp_node.header_dim.valid = false;

    dynarray_push(codeBox_list[codeBox_id].node_group, temp_node);

    node_pos = dynarray_length(codeBox_list[codeBox_id].node_group) - 1;

    codeBox_list[codeBox_id].node_group[node_pos].node_id = node_pos;
    return node_pos;
}
int node_zones_push(int x, int y, int width, int height) {
    node_zones.array[node_zones.length].x = x;
    node_zones.array[node_zones.length].y = y;
    node_zones.array[node_zones.length].width = width;
    node_zones.array[node_zones.length].height = height;

    node_zones.length ++;

    return node_zones.length;
}
int node_group_push_id(int codeBox_id, int x, int y, int width, int height, int next, int previous, int cont_head, int cont_by, int priority, int* contained, char *name, char *description, char *head, char *tail, char *real_code, int node_id, char status, char is_file) {
    char *new_code;

    unsigned long int i;

    long unsigned int node_pos;

    a_node temp_node;

    temp_node.obscuring = dynarray_create(int);
    temp_node.obscured_by = dynarray_create(int);
    temp_node.obscured = false;

    temp_node.x = x;
    temp_node.y = y;
    temp_node.width = width;
    temp_node.height = height;
    temp_node.next = next;
    temp_node.previous = previous;
    temp_node.cont_head = cont_head;
    temp_node.cont_by = cont_by;
    temp_node.priority = priority;
    temp_node.status = status;
    temp_node.is_file = is_file;
    temp_node.file_tab_id = -1;

    temp_node.contained = dynarray_create(int);

    for(i = 0; i < dynarray_length(contained); i++) {
        dynarray_push(temp_node.contained, contained[i]);
    }

    string_init(&temp_node.name);
    string_set(&temp_node.name, name);

    string_init(&temp_node.description);
    string_set(&temp_node.description, description);

    string_init(&temp_node.head);
    string_set(&temp_node.head, head);

    string_init(&temp_node.tail);
    string_set(&temp_node.tail, tail);

    string_init(&temp_node.real_code);
    string_set(&temp_node.real_code, real_code);

    temp_node.header_dim.width = 1;
    temp_node.header_dim.height = 1;
    temp_node.header_dim.rowchars = 1;
    temp_node.header_dim.valid = false;

    dynarray_push(codeBox_list[codeBox_id].node_group, temp_node);
    node_pos = dynarray_length(codeBox_list[codeBox_id].node_group) - 1;

    codeBox_list[codeBox_id].node_group[node_pos].node_id = node_pos;
    return node_pos;
}
void node_group_update_id(int node_id, int group_id, char *name, char *description, char *head, int priority,  char *real_code, char *tail, char status, char is_file, int file_tab_id) {
    codeBox_list[group_id].node_group[node_id].priority = priority;

    string_set(&codeBox_list[group_id].node_group[node_id].name, name);
    string_set(&codeBox_list[group_id].node_group[node_id].description, description);

    string_set(&codeBox_list[group_id].node_group[node_id].head, head);
    string_set(&codeBox_list[group_id].node_group[node_id].tail, tail);
    codeBox_list[group_id].node_group[node_id].status = status;

    codeBox_list[group_id].node_group[node_id].is_file = is_file;
    codeBox_list[group_id].node_group[node_id].file_tab_id = file_tab_id;

    codeBox_list[group_id].node_group[node_id].header_dim.valid = false;
}
void node_group_update_id_min(int node_id, int group_id,  char *head, char *real_code, char *tail) {
    string_set(&codeBox_list[group_id].node_group[node_id].head, head);
    string_set(&codeBox_list[group_id].node_group[node_id].tail, tail);
    string_set(&codeBox_list[group_id].node_group[node_id].real_code, real_code);

    codeBox_list[group_id].node_group[node_id].header_dim.valid = false;
}
void deactivate_quit() {
    current_nodegroup.node_group_altered = true;
}
int count_string_lines(char *string) {
    char *p;
    int num_newlines = 0;

    if(strlen(string) > 0) {
        num_newlines ++;
        p = string;
        p = strchr(p,'\n');
        while(p != NULL) {
            p = strchr(p + 1,'\n');
            num_newlines ++;
        }
    }

    return num_newlines;
}
void line_search(app_widgets *app_wdgts) {
    unsigned int i;
    int searching_line;
    char *line_number_string = gtk_entry_get_text (app_wdgts->e_txt_search_line);
    int line_number = 0;
    int node_with_line = 0;
    int first_line;
    char feedback_text[30];

    int* primary_nodes;

    line_number = atoi(line_number_string);

    if(line_number == 0) {
        write_feedback(app_wdgts, "please input a number");
    } else {
        clearRenderedData();
        line_count = 1;
        primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

        renderLineDataNodes(primary_nodes, false);
        dynarray_destroy(primary_nodes);

        i = 0;
        searching_line = rendered_counts[0];
        while(searching_line < line_number && i < dynarray_length(rendered_counts)) {
            i++;
            searching_line = rendered_counts[i];
        }

        if(i >= dynarray_length(rendered_counts)) {
            write_feedback(app_wdgts, "line not found");
        } else {
            if(line_number == rendered_counts[i]) {
                node_with_line = rendered_data[i];
                first_line = rendered_counts[i] + 1;
                write_feedback(app_wdgts, "check head or tail");
            } else {
                node_with_line = rendered_data[i - 1];
                first_line = rendered_counts[i - 1];

                sprintf(feedback_text, "check line %d", (line_number - rendered_counts[i - 1] + 1));
                write_feedback(app_wdgts, feedback_text);
            }
            current_nodegroup.last_node_read.node = node_with_line;
            current_nodegroup.last_node_read.codeBox_id = current_codeBox;

            launchBox(node_with_line, current_codeBox, first_line, app_wdgts);
        }
    }
}
int create_new_tab(app_widgets *app_wdgts) {
    GtkWidget	*tab_label;
    GtkWidget	*a_box1;
    GtkAdjustment *scrollx;
    GtkAdjustment *scrolly;

    int old_nodegroup_id;

    tab_label = gtk_label_new ("new_file");
    a_box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show (tab_label);
    gtk_widget_show (a_box1);
    gtk_notebook_append_page(app_wdgts->t_ntb_nete_tabs, a_box1, tab_label);

    old_nodegroup_id = current_codeBox;

    a_node_group_context temp_group_list;

    init_node_group_context(&temp_group_list);

    dynarray_push(codeBox_list, temp_group_list);
    current_codeBox = dynarray_length(codeBox_list) - 1;

    codeBox_list[current_codeBox].group_id = current_codeBox;
    codeBox_list[current_codeBox].topNode.codeBox_id = current_codeBox;

    scrollx = gtk_scrolled_window_get_hadjustment (app_wdgts->w_darea_scroll);
    scrolly = gtk_scrolled_window_get_vadjustment (app_wdgts->w_darea_scroll);
    codeBox_list[old_nodegroup_id].scroll_x = gtk_adjustment_get_value (scrollx);
    codeBox_list[old_nodegroup_id].scroll_y = gtk_adjustment_get_value (scrolly);
    gtk_adjustment_set_value (scrollx, 0.0);
    gtk_adjustment_set_value (scrolly, 0.0);

    current_nodegroup.codebox_container = a_box1;

    g_object_ref(app_wdgts->l_lbl_codebox_focus);
    g_object_ref(app_wdgts->w_darea_scroll);
    gtk_container_remove (codeBox_list[old_nodegroup_id].codebox_container, app_wdgts->w_darea_scroll);
    gtk_container_remove (codeBox_list[old_nodegroup_id].codebox_container, app_wdgts->l_lbl_codebox_focus);
    gtk_container_add (current_nodegroup.codebox_container, app_wdgts->l_lbl_codebox_focus);
    gtk_container_add (current_nodegroup.codebox_container, app_wdgts->w_darea_scroll);
    g_object_unref(app_wdgts->w_darea_scroll);
    g_object_unref(app_wdgts->l_lbl_codebox_focus);

    return current_codeBox;
}
void write_feedback(app_widgets *app_wdgts, char *text) {
    gtk_label_set_text (app_wdgts->l_lbl_main_feedback, text);
}
RGB colorConverter(int hexValue) {
    RGB rgbColor;
    rgbColor.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
    rgbColor.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
    rgbColor.b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte

    return rgbColor;
}
void init_nodes() {
    int* temp_contained_array;

    temp_contained_array = dynarray_create(int);

    head_zone = node_zones_push(
                    0,
                    0,
                    node_head_width,
                    node_head_height
                ) - 1; // head zone
    body_zone = node_zones_push(
                    0,
                    node_head_height + node_spacer_height + node_descript_height + node_spacer_height,
                    50,
                    50
                ) - 1; // body zone
    flank1_zone = node_zones_push(
                      0,
                      node_head_height + node_spacer_height + node_descript_height + node_spacer_height,
                      50,
                      50
                  ) - 1;
    flank2_zone = node_zones_push(
                      0,
                      node_head_height + node_spacer_height + node_descript_height + node_spacer_height,
                      50,
                      50
                  ) - 1;
    resize_zone = node_zones_push(
                      0,
                      node_head_height + node_spacer_height + node_descript_height + node_spacer_height,
                      50,
                      50
                  ) - 1;

    node_group_push(
        current_codeBox,
        200,
        100,
        150,
        150,
        -1,
        -1,
        -1,
        -1,
        5,
        temp_contained_array,
        "first node",
        "this is a first node",
        "",
        "",
        "",
        0,
        0,
        0
    );
    dynarray_destroy(temp_contained_array);
}
void init_arrays() {
    node_zones.length = 0;
    a_tab_to_codeBox temp_tab_codeBox;
    a_node_group_context temp_group_list;

    node_being_dragged.node = -1;
    node_being_resized.node = -1;
    node_being_edited.node = -1;
    node_being_edited.codeBox_id = -1;
    node_being_edited.editing_ribbon = dynarray_create(a_ribbon_element);

    string_init(&output_text);
    string_init(&last_file_directory);
    string_set(&last_file_directory, "");
    rendered_data = dynarray_create(int);
    rendered_counts = dynarray_create(int);
    node_translation = dynarray_create(a_translate_vector);

    codeBox_list = dynarray_create(a_node_group_context);

    tab_codeBoxes = dynarray_create(a_tab_to_codeBox);
    temp_tab_codeBox.codeBox_id = 0;
    dynarray_push(tab_codeBoxes, temp_tab_codeBox);

    init_node_group_context(&temp_group_list);
    dynarray_push(codeBox_list, temp_group_list);

    body_shapes = dynarray_create(a_body_shape);
    create_sample_shapes();
}
void init_node_group_context(a_node_group_context *temp_group_list) {
    string_init(&(temp_group_list->name));
    string_init(&(temp_group_list->destination));
    string_init(&(temp_group_list->fileName));
    string_init(&(temp_group_list->fileNameSolo));
    string_init(&(temp_group_list->fileDirectory));
    temp_group_list->zoomScaleFactor = 1.0;
    temp_group_list->topNode.node = -1;
    temp_group_list->topNode.codeBox_id = 0;
    temp_group_list->last_node_read.node = -1;
    temp_group_list->last_node_read.codeBox_id = 0;
    temp_group_list->node_group_altered = false;
    temp_group_list->fileSaveAsMode = true;

    string_set(&(temp_group_list->destination), default_destination);
    string_set(&(temp_group_list->name), "new_file");
    string_set(&(temp_group_list->fileName), "");
    string_set(&(temp_group_list->fileNameSolo), "");
    string_set(&(temp_group_list->fileDirectory), "");

    temp_group_list->node_group = dynarray_create(a_node);

    temp_group_list->focusList = dynarray_create(focus_item);

    temp_group_list->current_highlight_button = NULL;
    temp_group_list->line_wrapping = true;

    temp_group_list->is_embedded = false;

    temp_group_list->manager = gtk_source_language_manager_get_default();
    temp_group_list->language = gtk_source_language_manager_get_language(manager, "c");
}
void delete_group_context(a_node_group_context *temp_group_list) {
    int i;
    string_free(&(temp_group_list->name));
    string_free(&(temp_group_list->destination));
    string_free(&(temp_group_list->fileName));
    for(i = 0; i < dynarray_length(temp_group_list->node_group); i++) {
        emptyout_node(&(temp_group_list->node_group[i]));
    }

    dynarray_destroy(temp_group_list->node_group);
    dynarray_destroy(temp_group_list->focusList);
}
void emptyout_node(a_node *node) {
    dynarray_destroy(node->contained);

    string_free(&(node->name));
    string_free(&(node->description));

    string_free(&(node->head));
    string_free(&(node->tail));

    string_free(&node->real_code);

    dynarray_destroy(node->obscuring);
    dynarray_destroy(node->obscured_by);
}
void init_shape_element(shape_element * elmnt) {
    elmnt->parameters = dynarray_create(float);
}
void init_body_shape(a_body_shape * bshape) {
    bshape->shapes_sequence = dynarray_create(shape_element);
}
void create_sample_shapes() {
    float rectparams[] = {0,0,1,1};
    float pathparams[] = {0,0.15, 0,0.85, 0.15,1, 0.85,1, 1,0.85, 1,0.15, 0.85,0, 0.15,0, 0,0.15};

    float shapes_params[8][4] = {
        {0, 0, 10, 10},
        {0, 0, 10, 10},
        {0, 0, 10, 10},
        {0, 0, 10, 10},
        {10, 0, 0, 1},
        {0, 0, 10, 1},
        {0, 0, 1, 0},
        {0, 10, 1, 10}
    };

    shapepostype shape_pos_types[] = {ATR, ATL, ABR, ABL, EL, ER, ET, EB};
    int anchor_params[] = {10,10,10,10,10,10,10,10};
    int elastic_params[] = {10,10,10,10};

    int i, j;

    shape_element temp_shape_element;
    a_body_shape temp_body_shape;
    shape_element temp_shape_element2;
    a_body_shape temp_body_shape2;
    shape_element temp_shape_element3[8];
    a_body_shape temp_body_shape3;
    shape_element temp_shape_element4;
    a_body_shape temp_body_shape4;

//--------------------------------------------------------------------------
    for(i = 0; i < 4; i++) {
        init_shape_element(&temp_shape_element3[i]);
        temp_shape_element3[i].basic_shape = RECTANGLE;
        temp_shape_element3[i].shape_base_type = ANCHOR;
        temp_shape_element3[i].shape_pos_type = shape_pos_types[i];
        for(j = 0; j < 4; j++) {
            dynarray_push(temp_shape_element3[i].parameters, shapes_params[i][j]);

        }

        init_shape_element(&temp_shape_element3[i+4]);
        temp_shape_element3[i+4].basic_shape = PATH;
        temp_shape_element3[i+4].shape_base_type = ELASTIC;
        temp_shape_element3[i+4].shape_pos_type = shape_pos_types[i+4];
        for(j = 0; j < 4; j++) {
            dynarray_push(temp_shape_element3[i+4].parameters, shapes_params[i+4][j]);

        }

    }

    init_body_shape(&temp_body_shape3);
    for(i = 0; i < 8; i++) {
        dynarray_push(temp_body_shape3.shapes_sequence, temp_shape_element3[i]);
    }

    for(i = 0; i < 8; i++) {
        temp_body_shape3.anchor_params[i] = anchor_params[i];
    }

    for(i = 0; i < 4; i++) {
        temp_body_shape3.elastic_params[i] = elastic_params[i];
    }

//============================================================================================

    load_body_shapes();

}
void load_body_shapes() {

    char pathbuf[2048];
    ssize_t pathlen = readlink("/proc/self/exe", pathbuf, 2047);

    pathbuf[pathlen]='\0';

    char * ptr;
    int    ch = '/';
    char * default_shape_name = "default.shp";
    int shape_name_length = strlen(default_shape_name);

    ptr = strrchr( pathbuf, ch );

    *(ptr + 1) = '\0';

    strcat(ptr+1, default_shape_name);
    *(ptr + 1 + shape_name_length) = '\0';

    gchar *shape_string = NULL;    // For reading contents of file
    gboolean file_success = FALSE;  // File read status

    cJSON *contained_member;
    cJSON *elastic_params_array;
    cJSON *anchor_params_array;
    cJSON *parameters_array;
    cJSON *single_parameter;
    int* temp_contained;
    float* temp_parameters;
    int* temp_anchor_params;
    int* temp_elastic_params;
    double temp_param;
    float temp_param_float;

    cJSON *basic_shape;
    cJSON *shapes_sequence_array;
    cJSON *shape_base_type;
    cJSON *shape_pos_type;

    cJSON *shapes_json;
    cJSON *specific_shape;

    int i, j;

    shape_element* temp_shape_element;
    a_body_shape* temp_body_shape;

    file_success = g_file_get_contents(pathbuf, &shape_string, NULL, NULL);
    if (file_success) {

        shapes_json = cJSON_Parse(shape_string);

        cJSON_ArrayForEach(specific_shape, shapes_json) {
            temp_body_shape = malloc (sizeof (a_body_shape));
            init_body_shape(temp_body_shape);

            anchor_params_array = cJSON_GetObjectItem(specific_shape, "anchor_params");
            i = 0;
            cJSON_ArrayForEach(contained_member, anchor_params_array) {
                if(i<8) {
                    temp_body_shape->anchor_params[i] = contained_member->valueint;
                }
                i++;
            }

            elastic_params_array = cJSON_GetObjectItem(specific_shape, "elastic_params");
            i = 0;
            cJSON_ArrayForEach(contained_member, elastic_params_array) {
                if(i<4) {
                    temp_body_shape->elastic_params[i] = contained_member->valueint;
                }
                i++;
            }
            shapes_sequence_array = cJSON_GetObjectItem(specific_shape, "shapes_sequence");
            cJSON_ArrayForEach(contained_member, shapes_sequence_array) {

                temp_shape_element = malloc (sizeof (shape_element));
                init_shape_element(temp_shape_element);

                basic_shape = cJSON_GetObjectItem(contained_member, "basic_shape");
                temp_shape_element->basic_shape = basic_shape->valueint;
                shape_base_type = cJSON_GetObjectItem(contained_member, "shape_base_type");
                temp_shape_element->shape_base_type = shape_base_type->valueint;
                shape_pos_type = cJSON_GetObjectItem(contained_member, "shape_pos_type");
                temp_shape_element->shape_pos_type = shape_pos_type->valueint;

                parameters_array = cJSON_GetObjectItem(contained_member, "parameters");
                cJSON_ArrayForEach(single_parameter, parameters_array) {
                    temp_param = single_parameter->valuedouble;
                    temp_param_float = (float)temp_param;
                    dynarray_push(temp_shape_element->parameters, temp_param_float);
                }

                dynarray_push(temp_body_shape->shapes_sequence, *temp_shape_element);
            }
            dynarray_push(body_shapes, *temp_body_shape);
        }
    }
}
void write_to_file(app_widgets *app_wdgts) {
    gchar *file_name = NULL;
    char *output_filename;
    char *last_slash;
    char *tab_name;

    char feedback_text[30];
    char temp_dir_name[1024];
    char *temp_untitled_name;
    int dir_name_length;

    char *string_nodegroup;

    FILE *fp;
    bool write_to_file = false;

    string_init(&output_filename);
    string_init(&temp_untitled_name);


    if (strlen(current_nodegroup.fileName) > 5 && !(current_nodegroup.fileSaveAsMode)) {
        string_set(&output_filename, current_nodegroup.fileName);
        write_to_file = true;
    } else {
        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save), app_wdgts->netefilter);

        if (strlen(current_nodegroup.fileName) > 5) {
            gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save), current_nodegroup.fileName);
        } else {
            if (strlen(last_file_directory) > 5) {
                string_set(&temp_untitled_name, last_file_directory);
                string_add(&temp_untitled_name, "/Untitled.nete");
                gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save), temp_untitled_name);
            } else {
                gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save), "Untitled.nete");
            }
        }
        gtk_widget_show(app_wdgts->w_dlg_file_save);

        if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_save)) == GTK_RESPONSE_OK) {
            file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save));

            string_set(&output_filename, file_name);

            if(strncmp(".nete", (file_name + strlen(file_name) - 5), 5) == 0 || strncmp(".json", (file_name + strlen(file_name) - 5), 5) == 0) {
                printf("nete file\n");
            } else {
                string_add(&output_filename, ".nete");
            }

            // ====== set up the tab's name =========================
            last_slash = strrchr(file_name, '/');
            string_init(&tab_name);
            string_set(&tab_name, last_slash + 1);

            dir_name_length = last_slash  - file_name;

            string_set(&(current_nodegroup.fileNameSolo), last_slash + 1);
            strncpy(temp_dir_name, file_name, dir_name_length);
            temp_dir_name[dir_name_length] = 0;
            string_set(&(current_nodegroup.fileDirectory), temp_dir_name);
            string_set(&last_file_directory, temp_dir_name);
            gtk_notebook_set_tab_label_text (app_wdgts->t_ntb_nete_tabs,
                                             gtk_notebook_get_nth_page (app_wdgts->t_ntb_nete_tabs, current_codeBox), tab_name);

            // ======== then the name and filename
            string_set(&(current_nodegroup.name), tab_name);
            string_set(&(current_nodegroup.fileName), output_filename);
            write_to_file = true;

            string_free(&tab_name) ;
            g_free(file_name);
        }
        gtk_widget_hide(app_wdgts->w_dlg_file_save);
    }

// =========== then write the actual file ===========

    if(write_to_file) {
        actual_write_to_file(output_filename, current_codeBox);

        current_nodegroup.node_group_altered = false;
        current_nodegroup.fileSaveAsMode = false;

        string_free(&output_filename);
        string_free(&temp_untitled_name);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(feedback_text, "file saved: %d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        write_feedback(app_wdgts, feedback_text);

        current_nodegroup.node_group_altered = false;
        mark_unsaved(app_wdgts);
    }
}
void actual_write_to_file(char* filename, int codeBox_id) {
    int this_codeBox_id = codeBox_id;
    char *output_filename;
    FILE *fp;

    char *string_nodegroup;
    int i;

    string_init(&output_filename);

    fp = fopen(filename, "w");

    string_nodegroup = stringify_nodes(codeBox_id);
    fputs(string_nodegroup, fp);
    fclose(fp);

    for(i = 0; i < dynarray_length(the_node_group); i++) {
        if(the_node_group[i].is_file == 1 && the_node_group[i].file_tab_id != -1) {
            string_set(&output_filename, the_node_group[i].real_code);
            actual_write_to_file(output_filename, the_node_group[i].file_tab_id);

        }
    }

    string_free(&output_filename);
}
int load_codeBox_file(char *file_name, app_widgets *app_wdgts, bool is_embedded) {
    gchar *file_contents = NULL;    // For reading contents of file
    gboolean file_success = FALSE;  // File read status

    char *last_slash;

    char *tab_name;

    int dir_name_length;
    char temp_dir_name[1024];

    int temp_current_codeBox = -1;
    a_tab_to_codeBox temp_tab_codeBox;
    int temp_tab_id;

    file_success = g_file_get_contents(file_name, &file_contents, NULL, NULL);
    if (file_success) {
        if(is_embedded) {
            a_node_group_context temp_group_list;

            init_node_group_context(&temp_group_list);

            dynarray_push(codeBox_list, temp_group_list);
            temp_current_codeBox = dynarray_length(codeBox_list) - 1;

            codeBox_list[temp_current_codeBox].group_id = temp_current_codeBox;
            codeBox_list[temp_current_codeBox].topNode.codeBox_id = temp_current_codeBox;
            load_codeBox(file_contents, app_wdgts, temp_current_codeBox);

        } else {

            temp_tab_codeBox.codeBox_id = create_new_tab(app_wdgts);
            dynarray_push(tab_codeBoxes, temp_tab_codeBox);

            temp_tab_id =  dynarray_length(tab_codeBoxes) - 1;

            load_codeBox(file_contents, app_wdgts, current_codeBox);

            last_slash = strrchr(file_name, '/');
            string_init(&tab_name);
            string_set(&tab_name, last_slash + 1);

            dir_name_length = last_slash  - file_name;

            string_set(&(current_nodegroup.fileNameSolo), last_slash + 1);
            strncpy(temp_dir_name, file_name, dir_name_length);
            temp_dir_name[dir_name_length] = 0;
            string_set(&(current_nodegroup.fileDirectory), temp_dir_name);
            string_set(&last_file_directory, temp_dir_name);

            gtk_notebook_set_tab_label_text (app_wdgts->t_ntb_nete_tabs,
                                             gtk_notebook_get_nth_page (app_wdgts->t_ntb_nete_tabs, temp_tab_id),
                                             tab_name);

            string_set(&(current_nodegroup.name), tab_name);
            string_set(&(current_nodegroup.fileName), file_name);

            string_free(&tab_name) ;

            gtk_notebook_set_current_page (app_wdgts->t_ntb_nete_tabs,
                                           temp_tab_id);

            current_tab_id = temp_tab_id;

            current_nodegroup.fileSaveAsMode = false;

        }

        gtk_widget_queue_draw(app_wdgts->drawingArea);
    }
    g_free(file_contents);

    return temp_current_codeBox;
}
void load_codeBox(char *nete_string, app_widgets *app_wdgts, int codeBox_id) {
    cJSON *incoming_node_group = NULL;
    cJSON *code_node = NULL;
    cJSON *contained_member;
    int* temp_contained;

    cJSON *nete_json;

    int real_priority;
    unsigned int i;
    char real_status = 0;
    char real_is_file = 0;
    int returned_file_tab_id;

    cJSON *x;
    cJSON *y;
    cJSON *width;
    cJSON *height;
    cJSON *next;
    cJSON *previous;
    cJSON *cont_head;
    cJSON *cont_by;
    cJSON *priority;
    cJSON *contained_array;
    cJSON *name;
    cJSON *description;
    cJSON *head;
    cJSON *tail;
    cJSON *real_code;
    cJSON *node_id;
    cJSON *status;
    cJSON *is_file;

    cJSON *destination;
    cJSON *zoomScaleFactor;
    cJSON *topNode;

    nete_json = cJSON_Parse(nete_string);

    zoomScaleFactor = cJSON_GetObjectItem(nete_json, "zoomScaleFactor");
    topNode = cJSON_GetObjectItem(nete_json, "topNode");
    destination = cJSON_GetObjectItem(nete_json, "destination");

    current_nodegroup.zoomScaleFactor = zoomScaleFactor->valuedouble;
    current_nodegroup.topNode.node = topNode->valueint;
    string_set(&(current_nodegroup.destination), destination->valuestring);

    incoming_node_group = cJSON_GetObjectItem(nete_json, "node_group");
    cJSON_ArrayForEach(code_node, incoming_node_group)
    {
        x = cJSON_GetObjectItem(code_node, "x");
        y = cJSON_GetObjectItem(code_node, "y");
        width = cJSON_GetObjectItem(code_node, "width");
        height = cJSON_GetObjectItem(code_node, "height");
        next = cJSON_GetObjectItem(code_node, "next");
        previous = cJSON_GetObjectItem(code_node, "previous");
        cont_head = cJSON_GetObjectItem(code_node, "cont_head");
        cont_by = cJSON_GetObjectItem(code_node, "cont_by");
        priority = cJSON_GetObjectItem(code_node, "priority");
        contained_array = cJSON_GetObjectItem(code_node, "contained");

        temp_contained = dynarray_create(int);
        cJSON_ArrayForEach(contained_member, contained_array) {
            dynarray_push(temp_contained, contained_member->valueint);
        }

        name = cJSON_GetObjectItem(code_node, "name");
        description = cJSON_GetObjectItem(code_node, "description");
        head = cJSON_GetObjectItem(code_node, "head");
        tail = cJSON_GetObjectItem(code_node, "tail");
        real_code = cJSON_GetObjectItem(code_node, "real_code");

        node_id = cJSON_GetObjectItem(code_node, "node_id");
        status = cJSON_GetObjectItem(code_node, "status");
        is_file = cJSON_GetObjectItem(code_node, "is_file");

        if(status) {
            real_status = (char) status->valueint;
        }

        if(is_file) {
            real_is_file = (char) is_file->valueint;
        }

        if (!cJSON_IsNumber(priority)) {
            real_priority = atoi(priority->valuestring);
        } else {
            real_priority = priority->valueint;
        }
        node_group_push_id(
            codeBox_id,
            x->valueint,
            y->valueint,
            width->valueint,
            height->valueint,
            next->valueint,
            previous->valueint,
            cont_head->valueint,
            cont_by->valueint,
            real_priority,
            temp_contained,
            name->valuestring,
            description->valuestring,
            head->valuestring,
            tail->valuestring,
            real_code->valuestring,
            0,
            real_status,
            real_is_file
        );

        dynarray_destroy(temp_contained);
    }

    for(i = 0; i < dynarray_length(codeBox_list[codeBox_id].node_group); i++) {
        check_obscured(i, codeBox_id);

        if(codeBox_list[codeBox_id].node_group[i].is_file == 1) {
            returned_file_tab_id = load_codeBox_file(codeBox_list[codeBox_id].node_group[i].real_code, app_wdgts, true);
            codeBox_list[codeBox_id].node_group[i].file_tab_id = returned_file_tab_id;
            codeBox_list[returned_file_tab_id].cont_by.node = i;
            codeBox_list[returned_file_tab_id].cont_by.codeBox_id = codeBox_id;
        }
    }

    cJSON_Delete(nete_json);
}
void load_session(char *nete_string, app_widgets *app_wdgts) {
    cJSON *incoming_node_group = NULL;
    cJSON *code_node = NULL;
    cJSON *contained_member;
    int* temp_contained;

    cJSON *nete_json;

    int real_priority;
    unsigned int i;
    char real_status = 0;

    cJSON *name;
    cJSON *fileName;
    cJSON *topNode;
    cJSON *zoomScaleFactor;
    cJSON *destination;

    char * file_name;

    nete_json = cJSON_Parse(nete_string);

    cJSON_ArrayForEach(code_node, nete_json)
    {
        fileName = cJSON_GetObjectItem(code_node, "fileName");
        if(fileName) {
            string_init(&file_name);

            string_set(&file_name, fileName->valuestring);

            printf("file name -> %s\n", file_name);
            load_codeBox_file(file_name, app_wdgts, false);
            string_free(&file_name) ;
        }
    }

    cJSON_Delete(nete_json);
}
char* stringify_session(void) {
    char *string = NULL;

    cJSON *output_node_group = NULL;
    cJSON *codebox = NULL;

    cJSON *name;
    cJSON *fileName;
    cJSON *zoomScaleFactor;
    cJSON *topNode;
    cJSON *destination;

    size_t index = 0;
    unsigned long int i = 0;

    output_node_group = cJSON_CreateArray();
    if (output_node_group == NULL)
    {
        goto end;
    }

    for (index = 0; index < (dynarray_length(codeBox_list)); ++index)
    {

        if (strlen(codeBox_list[index].fileName) > 5) {
            cJSON *output_code = cJSON_CreateObject();
            if (output_code == NULL)
            {
                goto end;
            }
            cJSON_AddItemToArray(output_node_group, output_code);

            json_add_string(name, codeBox_list[index].name, output_code, end);
            json_add_string(fileName, codeBox_list[index].fileName, output_code, end);
            json_add_string(destination, codeBox_list[index].destination, output_code, end);
            json_add_number(topNode, codeBox_list[index].topNode.node, output_code, end);
            json_add_number(zoomScaleFactor, codeBox_list[index].zoomScaleFactor, output_code, end);

        }
    }
    string = cJSON_Print(output_node_group);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print output_code.\n");
    }

end:
    cJSON_Delete(output_node_group);
    return string;

}
char* stringify_nodes(int codeBox_id) {
    int this_codeBox_id = codeBox_id;
    char *string = NULL;

    cJSON *output_node_group = NULL;
    cJSON *codebox = NULL;

    cJSON *x = NULL;
    cJSON *y = NULL;
    cJSON *width = NULL;
    cJSON *height = NULL;
    cJSON *next = NULL;
    cJSON *previous = NULL;
    cJSON *cont_head = NULL;
    cJSON *cont_by = NULL;
    cJSON *priority = NULL;
    cJSON *name = NULL;
    cJSON *description = NULL;
    cJSON *head = NULL;
    cJSON *tail = NULL;
    cJSON *real_code = NULL;
    cJSON *node_id = NULL;

    cJSON *contained = NULL;
    cJSON *contained_item = NULL;

    cJSON *status = NULL;
    cJSON *is_file = NULL;

    cJSON *fileName;
    cJSON *zoomScaleFactor;
    cJSON *topNode;
    cJSON *destination;

    size_t index = 0;
    unsigned long int i = 0;

    cJSON *output_code = cJSON_CreateObject();
    if (output_code == NULL)
    {
        goto end;
    }

    json_add_string(name, codeBox_list[this_codeBox_id].name, output_code, end);
    json_add_string(fileName, "", output_code, end);
    json_add_string(destination, codeBox_list[this_codeBox_id].destination, output_code, end);
    json_add_number(topNode, codeBox_list[this_codeBox_id].topNode.node, output_code, end);
    json_add_number(zoomScaleFactor, codeBox_list[this_codeBox_id].zoomScaleFactor, output_code, end);

    output_node_group = cJSON_CreateArray();
    if (output_node_group == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(output_code, "node_group", output_node_group);

    for (index = 0; index < (dynarray_length(the_node_group)); ++index)
    {
        codebox = cJSON_CreateObject();
        if (codebox == NULL)
        {
            goto end;
        }
        cJSON_AddItemToArray(output_node_group, codebox);

        json_add_number(x, the_node_group[index].x, codebox, end);
        json_add_number(y, the_node_group[index].y, codebox, end);
        json_add_number(width, the_node_group[index].width, codebox, end);
        json_add_number(height, the_node_group[index].height, codebox, end);
        json_add_number(next, the_node_group[index].next, codebox, end);
        json_add_number(previous, the_node_group[index].previous, codebox, end);
        json_add_number(cont_head, the_node_group[index].cont_head, codebox, end);
        json_add_number(cont_by, the_node_group[index].cont_by, codebox, end);

        json_add_array(contained, codebox, end);

        for(i = 0; i < dynarray_length(the_node_group[index].contained); i++) {
            json_add_number_arr(contained_item, the_node_group[index].contained[i], contained, end);
        }
        json_add_number(priority, the_node_group[index].priority, codebox, end);
        json_add_string(name, the_node_group[index].name, codebox, end);
        json_add_string(description, the_node_group[index].description, codebox, end);
        json_add_string(head, the_node_group[index].head, codebox, end);
        json_add_string(tail, the_node_group[index].tail, codebox, end);
        json_add_string(real_code, the_node_group[index].real_code, codebox, end);
        json_add_number(node_id, the_node_group[index].node_id, codebox, end);
        json_add_number(status, the_node_group[index].status, codebox, end);
        json_add_number(is_file, the_node_group[index].is_file, codebox, end);
    }

    string = cJSON_Print(output_code);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print output_code.\n");
    }

end:
    cJSON_Delete(output_code);
    return string;

}
void string_init(char** string_ptr) {
    char *ptr = malloc(16);
    *string_ptr = ptr;
}
void string_free(char** string_ptr) {
    free(*string_ptr);
}
void string_set(char** destination, char* value) {
    int new_size = strlen(value);

    *destination = realloc(*destination, sizeof(char)*new_size + 1);

    if(*destination == NULL) {
        printf("Unable to realloc() ptr!");
    } else {
        strcpy(*destination, value);
    }
}
void string_add(char** destination, char* value) {
    int new_size = strlen(*destination) + strlen(value);

    *destination = realloc(*destination, sizeof(char)*new_size + 1);

    if(*destination == NULL) {
        printf("Unable to realloc() ptr!");
    } else {
        strcat(*destination, value);
    }
}
void splice_out_node(int *array, int inner_node) {
    unsigned int i = 0;
    unsigned int j = 0;

    int temp_node;

    unsigned int array_length = dynarray_length(array);

    while(i < array_length) {
        if(array[i] == inner_node) {
            j = i;
            while(j < array_length) {
                array[j] = array[j + 1];
                j++;
            }
            if(array_length > 0) {
                dynarray_pop(array, &temp_node);
            }
        }
        i++;
    }
}
bool check_unsaved(void) {
    unsigned int i;
    bool return_value = false;

    for(i = 0; i < dynarray_length(codeBox_list); i++) {
        if(codeBox_list[i].node_group_altered) {
            return_value = true;
        }
    }

    return return_value;
}
void mark_unsaved(app_widgets *app_wdgts) {
    char *tab_name;
    string_init(&tab_name);
    string_set(&tab_name, current_nodegroup.name);

    if(current_nodegroup.node_group_altered) {
        string_add(&tab_name, "*");
    }

    gtk_notebook_set_tab_label_text (app_wdgts->t_ntb_nete_tabs,
                                     gtk_notebook_get_nth_page (app_wdgts->t_ntb_nete_tabs, current_tab_id), tab_name);
    string_free(&tab_name);
}
void qswap(a_qsort_element *a, a_qsort_element *b) {
    a_qsort_element t;

    t.node_ID = a->node_ID;
    t.xy_combined = a->xy_combined;

    a->node_ID = b->node_ID;
    a->xy_combined = b->xy_combined;

    b->node_ID = t.node_ID;
    b->xy_combined = t.xy_combined;

}
int qpartition(a_qsort_element array[], int low, int high) {

    int pivot = array[high].xy_combined;
    int i = (low - 1);

    for (int j = low; j < high; j++) {
        if (array[j].xy_combined <= pivot) {
            i++;
            qswap(&array[i], &array[j]);
        }
    }

    qswap(&array[i + 1], &array[high]);

    return (i + 1);
}
void quick_sort(a_qsort_element  array[], int low, int high) {

    if (low < high) {
        int piv = qpartition(array, low, high);
        quick_sort(array, low, piv - 1);

        quick_sort(array, piv + 1, high);
    }
}
int* rearrange_nodes(int *nodes) {
    int i;
    int max_x = 0;
    int y_mult;
    int num_nodes = dynarray_length(nodes);
    a_qsort_element qsort_array[num_nodes];

    int *return_nodes = dynarray_create(int);

    for (i = 0; i < num_nodes; i++) {
        qsort_array[i].node_ID = nodes[i];
        qsort_array[i].x_coord = abs(current_nodegroup.node_group[nodes[i]].x / 120);
        qsort_array[i].y_coord = abs(current_nodegroup.node_group[nodes[i]].y / 60);

        if(qsort_array[i].x_coord > max_x) {
            max_x = qsort_array[i].x_coord;

        }
    }

    y_mult = max_x + 1;

    for (i = 0; i < num_nodes; i++) {
        qsort_array[i].xy_combined = qsort_array[i].y_coord * y_mult + qsort_array[i].x_coord;
    }

    quick_sort(qsort_array, 0, num_nodes - 1);

    for (i = 0; i < num_nodes; i++) {
        dynarray_push(return_nodes, qsort_array[i].node_ID);
    }

    return return_nodes;

}
void containerRemoveNode(int containing_node, int  inner_node, int codeBox_id) {
    int this_codeBox_id = codeBox_id;

    splice_out_node(the_node_group[containing_node].contained, inner_node);

    the_node_group[inner_node].cont_by = -1;

    deactivate_quit();
}
void remove_nodegroup(app_widgets *app_wdgts) {
    unsigned int i = 0;
    a_node_group_context temp_item;
    unsigned int array_length = dynarray_length(codeBox_list);
    unsigned int tab_array_length = dynarray_length(tab_codeBoxes);

    a_tab_to_codeBox temp_tab_item;
    int final_codebox_index = current_codeBox;
    int present_final_index = current_codeBox + 1;
    int index_to_splice =  current_codeBox;

    int final_tabcodebox_index = current_tab_id;
    int present_tabfinal_index = current_tab_id + 1;

    if(dynarray_length(codeBox_list) > 1) {
        if(current_codeBox == (dynarray_length(codeBox_list) - 1)) {
            final_codebox_index = current_codeBox - 1;
            present_final_index = current_codeBox - 1;
        }

        if(current_tab_id == (dynarray_length(tab_codeBoxes) - 1)) {
            final_tabcodebox_index = current_tab_id - 1;
            present_tabfinal_index = current_tab_id - 1;
        }

        set_node_group(present_final_index, app_wdgts);
        delete_group_context(&(codeBox_list[index_to_splice]));

        i = current_tab_id;

        while(i < tab_array_length) {
            tab_codeBoxes[i] = tab_codeBoxes[i + 1];
            i++;
        }
        if(tab_array_length > 0) {
            dynarray_pop(tab_codeBoxes, &temp_tab_item);
        }

        gtk_notebook_remove_page (app_wdgts->t_ntb_nete_tabs, current_tab_id);

        set_node_group(tab_codeBoxes[final_tabcodebox_index].codeBox_id, app_wdgts);
    }
}
void set_node_group(int new_group_index, app_widgets *app_wdgts) {
    int old_nodegroup_id;
    unsigned int new_page_num = new_group_index;
    GtkAdjustment *scrollx;
    GtkAdjustment *scrolly;

    old_nodegroup_id = current_codeBox;
    current_codeBox = new_page_num;

    if(current_nodegroup.topNode.node != -1) {
        gtk_label_set_text (app_wdgts->l_lbl_codebox_focus, current_nodegroup.node_group[current_nodegroup.topNode.node].name);
        gtk_widget_show(app_wdgts->l_lbl_codebox_focus);
    } else {
        gtk_widget_hide(app_wdgts->l_lbl_codebox_focus);
    }

    scrollx = gtk_scrolled_window_get_hadjustment (app_wdgts->w_darea_scroll);
    scrolly = gtk_scrolled_window_get_vadjustment (app_wdgts->w_darea_scroll);
    codeBox_list[old_nodegroup_id].scroll_x = gtk_adjustment_get_value (scrollx);
    codeBox_list[old_nodegroup_id].scroll_y = gtk_adjustment_get_value (scrolly);
    gtk_adjustment_set_value (scrollx, current_nodegroup.scroll_x);
    gtk_adjustment_set_value (scrolly, current_nodegroup.scroll_y);

    no_node_selected = true;
    g_object_ref(app_wdgts->l_lbl_codebox_focus);
    g_object_ref(app_wdgts->w_darea_scroll);
    gtk_container_remove (codeBox_list[old_nodegroup_id].codebox_container, app_wdgts->w_darea_scroll);
    gtk_container_remove (codeBox_list[old_nodegroup_id].codebox_container, app_wdgts->l_lbl_codebox_focus);
    gtk_container_add (current_nodegroup.codebox_container, app_wdgts->l_lbl_codebox_focus);
    gtk_container_add (current_nodegroup.codebox_container, app_wdgts->w_darea_scroll);

    if(current_nodegroup.current_highlight_button != NULL) {
        gtk_check_menu_item_set_active (current_nodegroup.current_highlight_button, TRUE);
    }

    if(current_nodegroup.line_wrapping) {
        gtk_check_menu_item_set_active (app_wdgts->b_mnu_wrap, TRUE);
    } else {
        gtk_check_menu_item_set_active (app_wdgts->b_mnu_wrap, FALSE);

    }
}
void ribbon_submit(void) {
    const gchar *head;
    const gchar *real_code;
    const gchar *tail;

    int i;

    GtkTextIter start;
    GtkTextIter end;

    if(node_being_edited.is_single) {
        gtk_text_buffer_get_start_iter (node_being_edited.editing_ribbon[0].e_txt_codebox_code, &start);
        gtk_text_buffer_get_end_iter (node_being_edited.editing_ribbon[0].e_txt_codebox_code, &end);

        real_code = gtk_text_buffer_get_text(node_being_edited.editing_ribbon[0].e_txt_codebox_code, &start, &end, FALSE);

        string_set(&codeBox_list[node_being_edited.codeBox_id].node_group[node_being_edited.node].real_code, real_code);

    } else {

        for(i = 0; i < dynarray_length(node_being_edited.editing_ribbon); i++) {
            gtk_text_buffer_get_start_iter (node_being_edited.editing_ribbon[i].e_txt_codebox_code, &start);
            gtk_text_buffer_get_end_iter (node_being_edited.editing_ribbon[i].e_txt_codebox_code, &end);


            head = gtk_entry_get_text (node_being_edited.editing_ribbon[i].e_txt_codebox_head);
            real_code = gtk_text_buffer_get_text(node_being_edited.editing_ribbon[i].e_txt_codebox_code, &start, &end, FALSE);
            tail = gtk_entry_get_text (node_being_edited.editing_ribbon[i].e_txt_codebox_tail);

            node_group_update_id_min(node_being_edited.editing_ribbon[i].node_id,
                                     node_being_edited.editing_ribbon[i].codeBox_id,
                                     (char *) head,
                                     (char *) real_code,
                                     (char *) tail);
        }
    }
}
void get_absolutePosition(a_container_pos *container_pos, int node, int codeBox_id) {
    int this_codeBox_id = codeBox_id;

    float node_x = the_node_group[node].x;
    float node_y = the_node_group[node].y;
    float current_scale;

    int* primary_nodes;
    a_node_note top_node_struct;

    a_container_pos upper_position;

    if(the_node_group[node].cont_by == current_nodegroup.topNode.node && codeBox_id == current_nodegroup.topNode.codeBox_id) {
        container_pos->x = (node_x*current_nodegroup.zoomScaleFactor);
        container_pos->y = (node_y + node_head_height + node_spacer_height)*current_nodegroup.zoomScaleFactor;
        container_pos->scaleFactor = current_nodegroup.zoomScaleFactor;
    } else {
        if(the_node_group[node].cont_by == -1 && codeBox_list[codeBox_id].cont_by.node != -1) {
            get_absolutePosition(&upper_position, codeBox_list[codeBox_id].cont_by.node, codeBox_list[codeBox_id].cont_by.codeBox_id);

            top_node_struct.node = -1;
            top_node_struct.codeBox_id = codeBox_id;
            primary_nodes = getPrimaryNodes(top_node_struct);

            current_scale = calculateScale(codeBox_list[codeBox_id].cont_by.node, codeBox_list[codeBox_id].cont_by.codeBox_id, primary_nodes, codeBox_id, 0, upper_position.scaleFactor) * upper_position.scaleFactor;
            dynarray_destroy(primary_nodes);

        } else if(the_node_group[node].cont_by != -1) {
            get_absolutePosition(&upper_position, the_node_group[node].cont_by, codeBox_id);

            current_scale = calculateScale(the_node_group[node].cont_by, codeBox_id, the_node_group[the_node_group[node].cont_by].contained, codeBox_id, 0, upper_position.scaleFactor) * upper_position.scaleFactor;
        }
        container_pos->x = ((node_x*current_scale) + upper_position.x);
        container_pos->y = (((node_y )*current_scale) + (node_head_height + node_spacer_height)*upper_position.scaleFactor + upper_position.y );
        container_pos->scaleFactor = current_scale;

    }
}
void launchBox(int node, int codeBox_id, int line_start, app_widgets *app_wdgts) {
    int this_codeBox_id = codeBox_id;

    char num_buf[3];
    int codebox_width, codebox_height;

    gtk_entry_set_text (app_wdgts->e_txt_codebox_name, the_node_group[node].name);
    gtk_entry_set_text (app_wdgts->e_txt_codebox_description, the_node_group[node].description);

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app_wdgts->b_chkbtn_codebox_ignore), FALSE);
    if(the_node_group[node].status == 1) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app_wdgts->b_chkbtn_codebox_ignore), TRUE);
    }

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app_wdgts->b_chkbtn_codebox_isfile), FALSE);
    if(the_node_group[node].is_file == 1) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app_wdgts->b_chkbtn_codebox_isfile), TRUE);
    }

    gtk_entry_set_text (app_wdgts->e_txt_codebox_head, the_node_group[node].head);
    sprintf(num_buf, "%d", the_node_group[node].priority);
    gtk_entry_set_text (app_wdgts->e_txt_codebox_priority, num_buf);
    gtk_entry_set_text (app_wdgts->e_txt_codebox_tail, the_node_group[node].tail);

    node_being_edited.node = node;
    node_being_edited.codeBox_id = codeBox_id;
    clear_ribbon(app_wdgts);
    create_text_ribbon(node, codeBox_id, 0, app_wdgts);

    gtk_window_present (app_wdgts->w_dlg_codebox);
    gtk_window_get_size (app_wdgts->w_dlg_codebox, &codebox_width, &codebox_height);

    if(first_time_codebox) {
        gtk_window_move (app_wdgts->w_dlg_codebox, gdk_screen_width() - codebox_width, 10);
    } else {
        gtk_window_move (app_wdgts->w_dlg_codebox, codeboxWindowPos.x, codeboxWindowPos.y);
    }

    node_displacement_count = 1;
}
void create_text_ribbon(int node, int codeBox_id, int line_start, app_widgets *app_wdgts) {
    int this_codeBox_id = codeBox_id;
    int i, body_id;
    int new_wrapping_mode;

    node_being_edited.ribbon_count = 0;

    a_ribbon_element *temp_ribbon_element;

    if(current_nodegroup.line_wrapping) {
        new_wrapping_mode = GTK_WRAP_WORD_CHAR;
    } else {
        new_wrapping_mode = GTK_WRAP_NONE;
    }

    if(the_node_group[node].is_file == 1 && the_node_group[node].file_tab_id != -1) {
        body_id = 2;
    } else if(dynarray_length(the_node_group[node].contained) != 0) {
        body_id = 1;
    } else {
        body_id = 0;
    }

    if(body_id == 0) {
        node_being_edited.is_single = true;

        temp_ribbon_element = malloc (sizeof (a_ribbon_element));

        temp_ribbon_element->e_txt_codebox_code = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
        temp_ribbon_element->s_source_view = gtk_source_view_new_with_buffer (temp_ribbon_element->e_txt_codebox_code);
        gtk_source_buffer_set_language(temp_ribbon_element->e_txt_codebox_code, current_nodegroup.language);

        gtk_text_buffer_set_text(temp_ribbon_element->e_txt_codebox_code, current_nodegroup.node_group[node].real_code, -1);
        gtk_source_view_set_show_line_numbers(temp_ribbon_element->s_source_view, TRUE);
        gtk_text_view_set_monospace (temp_ribbon_element->s_source_view, TRUE);
        gtk_text_view_set_wrap_mode (temp_ribbon_element->s_source_view, new_wrapping_mode);
        gtk_text_view_set_left_margin (temp_ribbon_element->s_source_view, 2);
        gtk_source_view_set_show_right_margin (temp_ribbon_element->s_source_view, TRUE);
        gtk_source_view_set_right_margin_position(temp_ribbon_element->s_source_view, 80);
        gtk_source_view_set_show_line_marks (temp_ribbon_element->s_source_view, TRUE);
        gtk_container_add (GTK_CONTAINER (app_wdgts->b_box_ribbon), temp_ribbon_element->s_source_view);
        gtk_widget_show (temp_ribbon_element->s_source_view);

        dynarray_push(node_being_edited.editing_ribbon, *temp_ribbon_element);

    } else if(body_id == 1) {
        node_being_edited.is_single = false;
        if(dynarray_length(node_being_edited.editing_ribbon) == 0) {
            renderRibbonDataNodes(the_node_group[node].contained, false, app_wdgts);
        }
    }
}
void clear_ribbon(app_widgets *app_wdgts) {
    a_ribbon_element temp_ribbon_element;

    if(node_being_edited.is_single) {
        dynarray_pop(node_being_edited.editing_ribbon, &temp_ribbon_element);
        gtk_container_remove(GTK_CONTAINER (app_wdgts->b_box_ribbon), temp_ribbon_element.s_source_view);
        gtk_widget_destroy(temp_ribbon_element.s_source_view);
    } else {
        while(dynarray_length(node_being_edited.editing_ribbon)) {

            dynarray_pop(node_being_edited.editing_ribbon, &temp_ribbon_element);
            gtk_container_remove(GTK_CONTAINER (app_wdgts->b_box_ribbon), temp_ribbon_element.b_head_strip);
            gtk_widget_destroy(temp_ribbon_element.b_head_strip);
            gtk_container_remove(GTK_CONTAINER (app_wdgts->b_box_ribbon), temp_ribbon_element.s_source_view);
            gtk_widget_destroy(temp_ribbon_element.s_source_view);
            gtk_container_remove(GTK_CONTAINER (app_wdgts->b_box_ribbon), temp_ribbon_element.b_tail_strip);
            gtk_widget_destroy(temp_ribbon_element.b_tail_strip);

        }
    }
}
bool recontain_node(a_nodezoneinfo* firstNodeZone, a_nodezoneinfo* secondNodeZone, int codeBox_id, int x, int y) {
    int this_codeBox_id = codeBox_id;

    int tempNode;
    int old_tempNode;

    int x_difference;
    int y_difference;

    float better_scale;

    a_node_note contained_top_node_struct;
    int* primary_nodes;

    a_container_pos container_pos;
    container_pos.x = 0;
    container_pos.y = 0;
    container_pos.scaleFactor = current_nodegroup.zoomScaleFactor;

    if (the_node_group[firstNodeZone->node].cont_by != secondNodeZone->node  && check_not_descendent(firstNodeZone->node, secondNodeZone->node, codeBox_id) && check_not_circular(firstNodeZone->node, secondNodeZone->node, codeBox_id)) {
        if(the_node_group[firstNodeZone->node].cont_by != -1) {
            containerRemoveNode(the_node_group[firstNodeZone->node].cont_by, firstNodeZone->node, firstNodeZone->codeBox_id);
        }
        the_node_group[firstNodeZone->node].cont_by = secondNodeZone->node;

        better_scale = current_nodegroup.zoomScaleFactor;

        container_pos.x = 0;
        container_pos.y = 0;
        container_pos.scaleFactor = current_nodegroup.zoomScaleFactor;

        if(secondNodeZone->node != codeBox_list[codeBox_id].topNode.node) {
            get_absolutePosition(&container_pos, secondNodeZone->node, codeBox_id);

            if(the_node_group[secondNodeZone->node].is_file == 1 && the_node_group[secondNodeZone->node].file_tab_id != -1) {
                contained_top_node_struct.node = -1;
                contained_top_node_struct.codeBox_id = the_node_group[secondNodeZone->node].file_tab_id;
                primary_nodes = getPrimaryNodes(contained_top_node_struct);
                better_scale = container_pos.scaleFactor * calculateScale(secondNodeZone->node, codeBox_id, primary_nodes, the_node_group[secondNodeZone->node].file_tab_id, 0, container_pos.scaleFactor);
                dynarray_destroy(primary_nodes);
            } else {
                better_scale = container_pos.scaleFactor * calculateScale(secondNodeZone->node, codeBox_id, the_node_group[secondNodeZone->node].contained, codeBox_id, 0, container_pos.scaleFactor);
            }
        }

        if(secondNodeZone->node != -1) {
            dynarray_push(the_node_group[secondNodeZone->node].contained, firstNodeZone->node);
        }
        if(secondNodeZone->node != codeBox_list[codeBox_id].topNode.node) {

        }
        the_node_group[firstNodeZone->node].x = (x - container_pos.x) / better_scale;
        the_node_group[firstNodeZone->node].y = (y - container_pos.y) / better_scale;
        tempNode = the_node_group[firstNodeZone->node].previous;

        while (tempNode != -1) {
            if(the_node_group[tempNode].cont_by != -1) {
                containerRemoveNode(the_node_group[tempNode].cont_by, tempNode, codeBox_id);
            }

            if(secondNodeZone->node != -1) {
                dynarray_push(the_node_group[secondNodeZone->node].contained, tempNode);
            }
            the_node_group[tempNode].cont_by = secondNodeZone->node;

            x_difference = the_node_group[tempNode].x - firstNodeZone->node_x;
            y_difference = the_node_group[tempNode].y - firstNodeZone->node_y;

            the_node_group[tempNode].x = (x - container_pos.x +x_difference) / better_scale;
            the_node_group[tempNode].y = (y - container_pos.y +y_difference) / better_scale;

            old_tempNode = tempNode;
            tempNode = the_node_group[tempNode].previous;
        }
        if(secondNodeZone->node != codeBox_list[codeBox_id].topNode.node) {
            if (old_tempNode != codeBox_list[codeBox_id].topNode.node && the_node_group[secondNodeZone->node].cont_head != -1) {
                the_node_group[secondNodeZone->node].cont_head = old_tempNode;
            }
        }

        tempNode = the_node_group[firstNodeZone->node].next;

        while (tempNode != -1) {
            if(the_node_group[tempNode].cont_by != -1) {
                containerRemoveNode(the_node_group[tempNode].cont_by, tempNode, codeBox_id);
            }

            if(secondNodeZone->node != -1) {
                dynarray_push(the_node_group[secondNodeZone->node].contained, tempNode);
            }
            the_node_group[tempNode].cont_by = secondNodeZone->node;

            x_difference = the_node_group[tempNode].x - firstNodeZone->node_x;
            y_difference = the_node_group[tempNode].y - firstNodeZone->node_y;

            the_node_group[tempNode].x = (x - container_pos.x +x_difference) / better_scale;
            the_node_group[tempNode].y = (y - container_pos.y +y_difference) / better_scale;

            tempNode = the_node_group[tempNode].next;
        }
        deactivate_quit();

        return true;
    }
    return true;
}
int * getPrimaryNodes(a_node_note top_node_struct) {
    int* return_nodes = dynarray_create(int);
    unsigned int i;
    for(i = 0; i < dynarray_length(codeBox_list[top_node_struct.codeBox_id].node_group); i++) {
        if(codeBox_list[top_node_struct.codeBox_id].node_group[i].cont_by == top_node_struct.node) {
            dynarray_push(return_nodes, i);
        }
    }
    return return_nodes;
}
void fixZones(int node, int codeBox_id) {
    int this_codeBox_id = codeBox_id;

    int dynamic_head_width  = (int)the_node_group[node].header_dim.width;
    int dynamic_head_height = (int)the_node_group[node].header_dim.height;

    node_zones.array[head_zone].width = dynamic_head_width;
    node_zones.array[head_zone].height = dynamic_head_height;

    node_zones.array[body_zone].x = node_flank_width;
    node_zones.array[body_zone].y = dynamic_head_height + node_spacer_height + node_descript_height + node_spacer_height;
    node_zones.array[body_zone].width = the_node_group[node].width - node_flank_width * 2;
    node_zones.array[body_zone].height = the_node_group[node].height;

    node_zones.array[flank1_zone].x = 0;
    node_zones.array[flank1_zone].y =  dynamic_head_height + node_spacer_height + node_descript_height + node_spacer_height;
    node_zones.array[flank1_zone].width = node_flank_width;
    node_zones.array[flank1_zone].height = the_node_group[node].height;

    node_zones.array[flank2_zone].x = the_node_group[node].width - node_flank_width;
    node_zones.array[flank2_zone].y = dynamic_head_height + node_spacer_height + node_descript_height + node_spacer_height;
    node_zones.array[flank2_zone].width = node_flank_width;
    node_zones.array[flank2_zone].height = the_node_group[node].height - node_resize_width;

    node_zones.array[resize_zone].x = the_node_group[node].width - node_resize_width;
    node_zones.array[resize_zone].y = dynamic_head_height + node_spacer_height + node_descript_height + node_spacer_height + the_node_group[node].height - node_resize_width;
    node_zones.array[resize_zone].width = node_resize_width;
    node_zones.array[resize_zone].height = node_resize_width;
}
bool check_not_descendent(int main_node, int desc_node, int codeBox_id) {
    int this_codeBox_id = codeBox_id;
    unsigned int i = 0;
    for (i = 0; i < dynarray_length(the_node_group[main_node].contained); i++) {
        if(the_node_group[main_node].contained[i] == desc_node)
            return false;

        if(!check_not_descendent(the_node_group[main_node].contained[i], desc_node, codeBox_id))
            return false;

    }

    return true;
}
float calculateScale(int node, int codeBox_id, int *contained_nodes, int contained_codeBox_id, __attribute__((unused)) int y_displacement, float scale_factor) {
    int this_codeBox_id = contained_codeBox_id;
    unsigned long int i;
    int max_x = 0;
    int max_y = 0;
    int current_x;
    int current_y;
    int current_node;
    float node_max_x = codeBox_list[codeBox_id].node_group[node].width * scale_factor - 17 * scale_factor;
    float node_max_y = codeBox_list[codeBox_id].node_group[node].height * scale_factor  - 5*scale_factor;
    float scale_x = 1;
    float scale_y = 1;

    for(i = 0; i < dynarray_length(contained_nodes); i++) {
        current_node = contained_nodes[i];
        current_x = (the_contained_group[current_node].x + the_contained_group[current_node].width) * scale_factor;
        current_y = (the_contained_group[current_node].y + the_contained_group[current_node].height + (the_contained_group[current_node].header_dim.height +node_spacer_height + node_descript_height + node_spacer_height)) * scale_factor;

        if(current_x > max_x)
            max_x = current_x;
        if(current_y > max_y)
            max_y = current_y;
    }

    if(max_x > node_max_x)
        scale_x = node_max_x / max_x;
    if(max_y > node_max_y)
        scale_y = node_max_y / max_y;

    scale_factor = scale_y;

    if(scale_x < scale_y)
        scale_factor = scale_x;

    return scale_factor;
}
bool check_not_circular(int main_node, int susp_node, int codeBox_id) {
    int this_codeBox_id = codeBox_id;
    int tempNode = the_node_group[main_node].previous;

    while (tempNode != -1) {
        if(tempNode == susp_node)
            return false;

        if(!check_not_descendent(tempNode, susp_node, codeBox_id))
            return false;

        tempNode = the_node_group[tempNode].previous;
    }

    tempNode = the_node_group[main_node].next;

    while (tempNode != -1) {
        if(tempNode == susp_node)
            return false;

        if(!check_not_descendent(tempNode, susp_node, codeBox_id))
            return false;

        tempNode = the_node_group[tempNode].next;

    }

    return true;
}
void  getNodeAndZone(a_nodezoneinfo *return_nodezone, int *nodes, a_node_note top_node_struct, int x, int y, int frame_x, int frame_y, float scale_factor, int omit) {
    int current_node;

    int x_min;
    int x_max;
    int y_min;
    int y_max;

    unsigned int i, j, obsc_ind;

    float head_height;
    float descript_height;
    float spacer;
    float children_scale_factor;

    int this_codeBox_id = top_node_struct.codeBox_id;
    int* contained_file_nodes;
    a_node_note contained_top_node_struct;

    return_nodezone->node_found = false;

    for(obsc_ind = 0; obsc_ind < 2; obsc_ind++) {
        for(i = 0; i < dynarray_length(nodes); i++) {
            current_node = nodes[i];
            if((obsc_ind == 0 && the_node_group[current_node].obscured) || (obsc_ind == 1 && !the_node_group[current_node].obscured)) {
                head_height  = the_node_group[current_node].header_dim.height * scale_factor;
                descript_height = node_descript_height * scale_factor;
                spacer  = node_spacer_height *scale_factor;

                if(the_node_group[current_node].is_file == 1 && the_node_group[current_node].file_tab_id != -1) {
                    contained_top_node_struct.node = -1;
                    contained_top_node_struct.codeBox_id = the_node_group[current_node].file_tab_id;

                    contained_file_nodes = getPrimaryNodes(contained_top_node_struct);
                    children_scale_factor = scale_factor * calculateScale(current_node, top_node_struct.codeBox_id, contained_file_nodes, contained_top_node_struct.codeBox_id, scale_factor, head_height +spacer + descript_height + spacer);

                    getNodeAndZone(return_nodezone, contained_file_nodes, contained_top_node_struct, x, y, the_node_group[current_node].x * scale_factor + frame_x, the_node_group[current_node].y * scale_factor + frame_y +  ( head_height +spacer + descript_height + spacer), children_scale_factor, omit);

                    dynarray_destroy(contained_file_nodes);
                } else if(dynarray_length(the_node_group[current_node].contained) != 0) {
                    children_scale_factor = scale_factor * calculateScale(current_node, top_node_struct.codeBox_id, the_node_group[current_node].contained, top_node_struct.codeBox_id, scale_factor, head_height +spacer + descript_height + spacer);
                    getNodeAndZone(return_nodezone, (the_node_group[current_node].contained), top_node_struct, x, y, the_node_group[current_node].x * scale_factor + frame_x, the_node_group[current_node].y * scale_factor + frame_y +  ( head_height +spacer + descript_height + spacer), children_scale_factor, omit);
                }
                if(return_nodezone->node_found) {
                    return;
                }

                fixZones(current_node, top_node_struct.codeBox_id);
                for(j = 0; j < node_zones.length; j++) {
                    x_min = frame_x + (the_node_group[current_node].x+node_zones.array[j].x) * scale_factor;
                    x_max = frame_x + (the_node_group[current_node].x+node_zones.array[j].x + node_zones.array[j].width) * scale_factor;
                    y_min = frame_y + (the_node_group[current_node].y+node_zones.array[j].y) * scale_factor;
                    y_max = frame_y + (the_node_group[current_node].y+node_zones.array[j].y + node_zones.array[j].height) * scale_factor;
                    if(current_node != omit && x > x_min && x < x_max && y > y_min && y < y_max) {
                        return_nodezone->node = current_node;
                        return_nodezone->codeBox_id = top_node_struct.codeBox_id;
                        return_nodezone->zone = j;
                        return_nodezone->scale_factor = scale_factor;
                        return_nodezone->x = x;
                        return_nodezone->y = y;
                        return_nodezone->node_x = the_node_group[current_node].x;
                        return_nodezone->node_y = the_node_group[current_node].y;
                        return_nodezone->node_width = the_node_group[current_node].width;
                        return_nodezone->node_height = the_node_group[current_node].height;
                        return_nodezone->frame_x = frame_x;
                        return_nodezone->frame_y = frame_y;
                        return_nodezone->node_found = true;

                        return;
                    }
                }
            }
        }
    }
    return_nodezone->node = codeBox_list[this_codeBox_id].topNode.node;
    return_nodezone->codeBox_id = codeBox_list[this_codeBox_id].topNode.codeBox_id;
    return_nodezone->zone = body_zone;
    return_nodezone->scale_factor = codeBox_list[this_codeBox_id].zoomScaleFactor;
    return_nodezone->x = 0;
    return_nodezone->y = 0;
    return_nodezone->node_x = 0;
    return_nodezone->node_y = 0;
    return_nodezone->node_width = 0;
    return_nodezone->node_height = 0;
    return_nodezone->frame_x = 0;
    return_nodezone->frame_y = 0;
    return_nodezone->node_found = false;
}
int pasteNode(int parent, int node, int source_codeBox) {
    int newID;
    unsigned int i;
    int *temp_contained;
    a_translate_vector temp_translate;

    float x = codeBox_list[source_codeBox].node_group[node].x;
    float y = codeBox_list[source_codeBox].node_group[node].y;

    if(parent == current_nodegroup.topNode.node) {
        x = rightClickCoord.x / current_nodegroup.zoomScaleFactor;
        y = rightClickCoord.y / current_nodegroup.zoomScaleFactor;
    }

    int width = codeBox_list[source_codeBox].node_group[node].width;
    int height = codeBox_list[source_codeBox].node_group[node].height;
    int next = -1;
    int previous = -1;
    int cont_head = -1;
    int cont_by = parent;
    int priority = codeBox_list[source_codeBox].node_group[node].priority;

    int dest_codeBox = current_codeBox; // to be generalized

    temp_contained = dynarray_create(int);

    newID = node_group_push(
                dest_codeBox,
                x,
                y,
                width,
                height,
                next,
                previous,
                cont_head,
                cont_by,
                priority,
                temp_contained,
                codeBox_list[source_codeBox].node_group[node].name,
                codeBox_list[source_codeBox].node_group[node].description,
                codeBox_list[source_codeBox].node_group[node].head,
                codeBox_list[source_codeBox].node_group[node].tail,
                codeBox_list[source_codeBox].node_group[node].real_code,
                0,
                0,
                0
            );

    temp_translate.original = node;
    temp_translate.newone = newID;
    dynarray_push(node_translation, temp_translate);

    if(current_nodegroup.topNode.node != -1 && parent == current_nodegroup.topNode.node) {
        dynarray_push(current_nodegroup.node_group[parent].contained, newID);
    }
    for (i = 0; i < dynarray_length(codeBox_list[source_codeBox].node_group[node].contained); i++) {
        pasteNode(newID, codeBox_list[source_codeBox].node_group[node].contained[i], source_codeBox);
    }

    return newID;
}
int get_key(int key) {
    unsigned int i = 0;

    for(i = 0; i < dynarray_length(node_translation); i++) {
        if(node_translation[i].original == key)
            return node_translation[i].newone;
    }

    return -1;
}
void fixPasted(int source_codeBox) {
    unsigned int i = 0;
    unsigned int j = 0;

    int translated_contained;

    for(i = 0; i < dynarray_length(node_translation); i++) {
        if(node_translation[i].original != -1) {
            if(current_nodegroup.node_group[node_translation[i].newone].cont_by != -1) {
                current_nodegroup.node_group[node_translation[i].newone].next = get_key(codeBox_list[source_codeBox].node_group[node_translation[i].original].next);
                current_nodegroup.node_group[node_translation[i].newone].previous = get_key(codeBox_list[source_codeBox].node_group[node_translation[i].original].previous);
            }
            current_nodegroup.node_group[node_translation[i].newone].cont_head = get_key(codeBox_list[source_codeBox].node_group[node_translation[i].original].cont_head);

            for(j = 0; j < dynarray_length(codeBox_list[source_codeBox].node_group[node_translation[i].original].contained); j++) {
                translated_contained = get_key(codeBox_list[source_codeBox].node_group[node_translation[i].original].contained[j]);
                dynarray_push(current_nodegroup.node_group[node_translation[i].newone].contained, translated_contained);
            }
        }
    }
}
void clearTranslations() {
    a_translate_vector temp_translate;

    while(dynarray_length(node_translation) > 0)
        dynarray_pop(node_translation, &temp_translate);
}
void doMouseDown2(int x, int y, __attribute__((unused)) gpointer data) {
    mousePressed = true;

    int* primary_nodes;
    a_nodezoneinfo newNodeZone;

    primary_nodes = getPrimaryNodes(current_nodegroup.topNode);
    if(no_node_selected) {
        getNodeAndZone(&tempNodeZone, primary_nodes, current_nodegroup.topNode, x, y, 0, 0, current_nodegroup.zoomScaleFactor, -1);
        if(tempNodeZone.node != -1) {
            if(tempNodeZone.zone == head_zone) {
                node_being_dragged.node = tempNodeZone.node;
                node_being_dragged.codeBox_id = tempNodeZone.codeBox_id;
            }

            if(tempNodeZone.zone == resize_zone) {
                node_being_resized.node = tempNodeZone.node;
                node_being_resized.codeBox_id = tempNodeZone.codeBox_id;
            }
            no_node_selected = false;
        }
    } else {
        switch(toolmode) {
        case EXITENTER:
            getNodeAndZone(&newNodeZone, primary_nodes, current_nodegroup.topNode, x, y, 0, 0, current_nodegroup.zoomScaleFactor, tempNodeZone.node);

            if(codeBox_list[newNodeZone.codeBox_id].node_group[newNodeZone.node].is_file == 1 && codeBox_list[newNodeZone.codeBox_id].node_group[newNodeZone.node].file_tab_id != -1) {

                newNodeZone.codeBox_id = codeBox_list[newNodeZone.codeBox_id].node_group[newNodeZone.node].file_tab_id;
                newNodeZone.node = -1;
            }

            if(tempNodeZone.codeBox_id == newNodeZone.codeBox_id) {
                recontain_node(&tempNodeZone, &newNodeZone, newNodeZone.codeBox_id, x, y);
                check_obscuring(tempNodeZone.node, tempNodeZone.codeBox_id);
                check_obscured(tempNodeZone.node, tempNodeZone.codeBox_id);
            }
            no_node_selected = true;

            break;
        case JOIN:
            getNodeAndZone(&newNodeZone, primary_nodes, current_nodegroup.topNode, x, y, 0, 0, current_nodegroup.zoomScaleFactor, tempNodeZone.node);
            if(newNodeZone.node != -1) {
                join_nodes(&tempNodeZone, &newNodeZone, newNodeZone.codeBox_id);
                no_node_selected = true;
            }

            break;
        }
    }
    dynarray_destroy(primary_nodes);
}
void doMouseDrag2(int x, int y, __attribute__((unused)) gpointer data) {
    int xDifference = 0;
    int yDifference = 0;
    int this_codeBox_id;

    if(toolmode == MOVE) {
        if (mousePressed && tempNodeZone.zone == 0) {
            this_codeBox_id = tempNodeZone.codeBox_id;
            xDifference = snapGrid((x / tempNodeZone.scale_factor))*tempNodeZone.scale_factor - tempNodeZone.x;
            yDifference = snapGrid((y / tempNodeZone.scale_factor))*tempNodeZone.scale_factor - tempNodeZone.y;

            the_node_group[tempNodeZone.node].x = snapGrid(tempNodeZone.node_x + xDifference/tempNodeZone.scale_factor);
            the_node_group[tempNodeZone.node].y = snapGrid(tempNodeZone.node_y + yDifference/tempNodeZone.scale_factor);
            no_node_selected = true;
        }
        if (mousePressed && tempNodeZone.zone == 4) {
            this_codeBox_id = tempNodeZone.codeBox_id;
            xDifference = snapGrid((x / tempNodeZone.scale_factor))*tempNodeZone.scale_factor - tempNodeZone.x;
            yDifference = snapGrid((y / tempNodeZone.scale_factor))*tempNodeZone.scale_factor - tempNodeZone.y;

            the_node_group[tempNodeZone.node].width = snapGrid(tempNodeZone.node_width + xDifference/tempNodeZone.scale_factor);
            the_node_group[tempNodeZone.node].height = snapGrid(tempNodeZone.node_height + yDifference/tempNodeZone.scale_factor);
            no_node_selected = true;
        }
    }
}
void doDoubleClick(int x, int y, __attribute__((unused)) gpointer data, app_widgets *app_wdgts) {
    int* primary_nodes;

    primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

    mousePressed = false;
    getNodeAndZone(&tempNodeZone, primary_nodes, current_nodegroup.topNode, x, y, 0, 0, current_nodegroup.zoomScaleFactor, -1);

    if (tempNodeZone.node != -1 && tempNodeZone.zone == 0) {
        current_nodegroup.last_node_read.node = tempNodeZone.node;
        current_nodegroup.last_node_read.codeBox_id = tempNodeZone.codeBox_id;

        launchBox(tempNodeZone.node, tempNodeZone.codeBox_id, 0,  app_wdgts);
    }
}
void doMouseUp2(int x, int y, __attribute__((unused)) gpointer data) {

    int* primary_nodes;
    a_nodezoneinfo newNodeZone;

    if (tempNodeZone.zone == 0) {

        primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

        getNodeAndZone(&newNodeZone, primary_nodes, current_nodegroup.topNode, x, y, 0, 0, current_nodegroup.zoomScaleFactor, tempNodeZone.node);

        dynarray_destroy(primary_nodes);

        if(toolmode == MOVE) {
            check_obscured(tempNodeZone.node, tempNodeZone.codeBox_id);
            check_obscuring(tempNodeZone.node, tempNodeZone.codeBox_id);
        }

        if (((newNodeZone.node != current_nodegroup.topNode.node && newNodeZone.codeBox_id == current_nodegroup.topNode.codeBox_id) && (newNodeZone.zone == 1 || newNodeZone.zone == 2 || newNodeZone.zone == 3 ) && newNodeZone.node != tempNodeZone.node) || (newNodeZone.node == current_nodegroup.topNode.node && newNodeZone.codeBox_id == current_nodegroup.topNode.codeBox_id) ) {
        }
    }
    if (tempNodeZone.zone == 4) {
        if(toolmode == MOVE) {
            check_obscuring(tempNodeZone.node, tempNodeZone.codeBox_id);
        }
    }

    if(toolmode == MOVE) {
        tempNodeZone.node = -1;
        tempNodeZone.zone = -1;
        tempNodeZone.scale_factor = 1;
        tempNodeZone.x = 0;
        tempNodeZone.y = 0;
        tempNodeZone.node_x = 0;
        tempNodeZone.node_y = 0;
        tempNodeZone.node_width = 0;
        tempNodeZone.node_height = 0;
        tempNodeZone.frame_x = 0;
        tempNodeZone.frame_y = 0;

        no_node_selected = true;
    }

    node_being_dragged.node = -1;
    node_being_resized.node = -1;

    mousePressed = false;
    nodeHeadFound = false;
    nodeBodyFlankFound = false;
    nodeWidthAdjust = false;
}
bool join_nodes(a_nodezoneinfo* firstNodeZone, a_nodezoneinfo* secondNodeZone, int codeBox_id) {
    int this_codeBox_id = codeBox_id;
    if (firstNodeZone->zone == 0 || firstNodeZone->zone == 1 || firstNodeZone->zone == 2 || firstNodeZone->zone == 3) {
        if (firstNodeZone->node != codeBox_list[codeBox_id].topNode.node && (secondNodeZone->zone == 0 || secondNodeZone->zone == 1 || secondNodeZone->zone == 2 || secondNodeZone->zone == 3) && secondNodeZone->node != firstNodeZone->node) {
            if(the_node_group[firstNodeZone->node].cont_by == the_node_group[secondNodeZone->node].cont_by ) {
                if(the_node_group[firstNodeZone->node].next == -1 && the_node_group[secondNodeZone->node].previous == -1 && check_not_circular(firstNodeZone->node, secondNodeZone->node, codeBox_id)) {
                    the_node_group[firstNodeZone->node].next = secondNodeZone->node;
                    the_node_group[secondNodeZone->node].previous = firstNodeZone->node;
                } else if (the_node_group[firstNodeZone->node].next == secondNodeZone->node) {
                    the_node_group[firstNodeZone->node].next = -1;
                    the_node_group[secondNodeZone->node].previous = -1;
                }

                deactivate_quit();
            }
        }
    }
    return true;
}
float snapGrid(float invalue) {
    float return_value;

    float gridvalue;

    gridvalue = (grid_size / 2);

    return_value = round(invalue/gridvalue) * gridvalue;

    return return_value;
}
void check_obscured(int node, int codeBox_id) {
    int i;
    int contained_node;
    int this_codeBox_id = codeBox_id;

    the_node_group[node].obscured = false;

    clear_obscured_by(node, codeBox_id);
    if(the_node_group[node].cont_by == -1) {
        for(i = 0; i < dynarray_length(the_node_group); i++) {
            if(the_node_group[i].cont_by == -1) {
                if( (the_node_group[node].x + node_head_width/2) > the_node_group[i].x &&
                        the_node_group[node].x < (the_node_group[i].x + the_node_group[i].width) &&
                        the_node_group[node].y > the_node_group[i].y &&
                        the_node_group[node].y < (the_node_group[i].y + the_node_group[i].height + the_node_group[i].header_dim.height)
                  ) {
                    the_node_group[node].obscured = true;
                    dynarray_push(the_node_group[node].obscured_by, i);
                    dynarray_push(the_node_group[i].obscuring, node);
                }
            }
        }
    } else {
        for(i = 0; i < dynarray_length(the_node_group[the_node_group[node].cont_by].contained); i++) {
            contained_node = the_node_group[the_node_group[node].cont_by].contained[i];
            if( (the_node_group[node].x + node_head_width/2) > the_node_group[contained_node].x &&
                    the_node_group[node].x < (the_node_group[contained_node].x + the_node_group[contained_node].width) &&
                    the_node_group[node].y > the_node_group[contained_node].y &&
                    the_node_group[node].y < (the_node_group[contained_node].y + the_node_group[contained_node].height + the_node_group[contained_node].header_dim.height)
              ) {
                the_node_group[node].obscured = true;
                dynarray_push(the_node_group[node].obscured_by, contained_node);
                dynarray_push(the_node_group[contained_node].obscuring, node);
            }
        }
    }
}
void clear_obscured_by(int node, int codeBox_id) {
    int i;
    int j;
    int obscuring_node;
    int temp_node;

    int this_codeBox_id = codeBox_id;

    for(i = 0; i < dynarray_length(the_node_group[node].obscured_by); i++) {
        obscuring_node = the_node_group[node].obscured_by[i];

        splice_out_node(the_node_group[obscuring_node].obscuring, node);
    }

    while(dynarray_length(the_node_group[node].obscured_by) > 0) {
        dynarray_pop(the_node_group[node].obscured_by, &temp_node);
    }
}
void check_obscuring(int node, int codeBox_id) {
    int i;
    int contained_node;

    int this_codeBox_id = codeBox_id;

    clear_obscuring(node, codeBox_id);

    if(the_node_group[node].cont_by == -1) {
        for(i = 0; i < dynarray_length(the_node_group); i++) {
            if(the_node_group[i].cont_by == -1) {
                if( (the_node_group[i].x + node_head_width/2) > the_node_group[node].x &&
                        the_node_group[i].x < (the_node_group[node].x + the_node_group[node].width) &&
                        the_node_group[i].y > the_node_group[node].y &&
                        the_node_group[i].y < (the_node_group[node].y + the_node_group[node].height + the_node_group[node].header_dim.height)
                  ) {
                    the_node_group[i].obscured = true;
                    dynarray_push(the_node_group[i].obscured_by, node);
                    dynarray_push(the_node_group[node].obscuring, i);
                }
            }
        }
    } else {
        for(i = 0; i < dynarray_length(the_node_group[the_node_group[node].cont_by].contained); i++) {
            contained_node = the_node_group[the_node_group[node].cont_by].contained[i];
            if( (the_node_group[contained_node].x + node_head_width/2) > the_node_group[node].x &&
                    the_node_group[contained_node].x < (the_node_group[node].x + the_node_group[node].width) &&
                    the_node_group[contained_node].y > the_node_group[node].y &&
                    the_node_group[contained_node].y < (the_node_group[node].y + the_node_group[node].height + the_node_group[node].header_dim.height)
              ) {
                the_node_group[contained_node].obscured = true;
                dynarray_push(the_node_group[contained_node].obscured_by, node);
                dynarray_push(the_node_group[node].obscuring, contained_node);
            }
        }
    }
}
void clear_obscuring(int node, int codeBox_id) {
    int i;
    int j;
    int obscured_node;
    int temp_node;

    int this_codeBox_id = codeBox_id;

    for(i = 0; i < dynarray_length(the_node_group[node].obscuring); i++) {
        obscured_node = the_node_group[node].obscuring[i];

        splice_out_node(the_node_group[obscured_node].obscured_by, node);
        if(dynarray_length(the_node_group[obscured_node].obscured_by) == 0) {
            the_node_group[obscured_node].obscured = false;
        }
    }
    while(dynarray_length(the_node_group[node].obscuring) > 0) {
        dynarray_pop(the_node_group[node].obscuring, &temp_node);
    }
}
void redraw(cairo_t *cr) {
    RGB tempcolour;
    unsigned int i;
    tempcolour = colorConverter(0x222222);

    int* primary_nodes;

    if(!exporting_file) {
        cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
        cairo_rectangle(cr, 0, 0, screenWidth, screenHeight);
        cairo_fill(cr);

        drawGrid(cr);
    }
    primary_nodes = getPrimaryNodes(current_nodegroup.topNode);

    draw_nodes(cr, primary_nodes, current_nodegroup.topNode.codeBox_id, 0, 0, 4095, 2047, current_nodegroup.zoomScaleFactor, true);
    dynarray_destroy(primary_nodes);

}
void draw_arc(cairo_t *cr, int node1, int node2, int codeBox_id, int frame_x, int frame_y, float scale_factor) {
    int this_codeBox_id = codeBox_id;

    RGB tempcolour;
    int arc_overhang = 10;
    int current_head_height1 = node_head_height;
    int current_head_height2 = node_head_height;
    if((the_node_group[node1].header_dim.valid)) {
        current_head_height1 = the_node_group[node1].header_dim.height;
    }
    if((the_node_group[node2].header_dim.valid)) {
        current_head_height2 = the_node_group[node2].header_dim.height;
    }

    tempcolour = colorConverter(0xcccccc);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_move_to (cr, frame_x + (the_node_group[node1].x + the_node_group[node1].width)*scale_factor + 2, frame_y + (the_node_group[node1].y + (current_head_height1 + node_spacer_height + node_descript_height + node_spacer_height) + the_node_group[node1].height / 2)*scale_factor);
    cairo_line_to (cr, frame_x + (the_node_group[node1].x + the_node_group[node1].width + arc_overhang)*scale_factor + 2, frame_y + (the_node_group[node1].y + (current_head_height1 + node_spacer_height + node_descript_height + node_spacer_height) + the_node_group[node1].height / 2)*scale_factor);
    cairo_line_to (cr, frame_x + (the_node_group[node2].x - arc_overhang)*scale_factor - 2, frame_y + (the_node_group[node2].y + (current_head_height2 + node_spacer_height + node_descript_height + node_spacer_height) + the_node_group[node2].height / 2)*scale_factor);
    cairo_line_to (cr, frame_x + the_node_group[node2].x*scale_factor - 2, frame_y + (the_node_group[node2].y + (current_head_height2 + node_spacer_height + node_descript_height + node_spacer_height) + the_node_group[node2].height / 2)*scale_factor);
    cairo_set_line_width (cr, 4);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke (cr);
    cairo_set_line_width (cr, 1);
}
void draw_text_in_box(cairo_t *cr, char *real_code, float x, float y, float width, float height) {
// ----------------- text vars
    cairo_font_extents_t my_font_extents;
    float real_code_y = 45.0;

    char *p;
    char *last_str;
    char oneline[128];
    int one_line_length;
    int one_line_max_length = 126;
    cairo_save(cr);
    cairo_scaled_font_extents (cairo_get_scaled_font(cr), &my_font_extents);

    cairo_rectangle(cr, x, y, width - my_font_extents.max_x_advance, height);
    cairo_clip(cr);

    real_code_y = y + my_font_extents.ascent;

    last_str = real_code;
    p = strchr(last_str,'\n');
    while(p != NULL) {
        one_line_length = p-last_str;

        if(one_line_length > one_line_max_length)
            one_line_length = one_line_max_length;

        strncpy(oneline, last_str, one_line_length);
        oneline[one_line_length] = '\0'; // place the null terminator
        cairo_move_to(cr, x, real_code_y);
        cairo_show_text(cr, oneline);
        last_str = p + 1;
        p = strchr(last_str,'\n');
        real_code_y += my_font_extents.ascent;
    }
    one_line_length = strlen(last_str);

    if(one_line_length > one_line_max_length)
        one_line_length = one_line_max_length;

    strncpy(oneline, last_str, one_line_length);
    oneline[one_line_length] = '\0';
    cairo_move_to(cr, x, real_code_y);
    cairo_show_text(cr, oneline);

    cairo_reset_clip (cr);
    cairo_restore(cr);
}
void drawX(cairo_t *cr, int x, int y, int width, int height) {
    RGB tempcolour;
    tempcolour = colorConverter(0x888888);
    cairo_set_line_width (cr, 4);
    cairo_set_source_rgba (cr, tempcolour.r, tempcolour.g, tempcolour.b, 0.65);
    cairo_move_to (cr, x, y);
    cairo_line_to (cr, x + width, y + height);

    cairo_move_to (cr,x, y + height);
    cairo_line_to (cr, x + width, y);
    cairo_stroke (cr);
    cairo_set_line_width (cr, 1);
}
a_header_dimension get_header_dimensions(cairo_t *cr, char *header_text, int header_font_size, int min_width, int max_width) {
    cairo_text_extents_t my_font_extents;

    cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, header_font_size);

    cairo_scaled_font_text_extents (cairo_get_scaled_font(cr), header_text, &my_font_extents);

    int total_width = (int)my_font_extents.width;

    cairo_scaled_font_text_extents (cairo_get_scaled_font(cr), "MM", &my_font_extents);
    int m_width = (int)my_font_extents.width;
    int temp_width;

    int line_height = (int)my_font_extents.height;

    int len = strlen(header_text);
    int row_len = 0;
    int num_rows = 0;
    int num_num_rows = 0;
    int word_count;
    int i;
    int position;
    int current_position = 0;
    int last_position = 0;

    int max_pixel_width = min_width;
    char *position_pointer;
    char single_row[100];
    int row_width;
    bool header_valid;
    bool end_pending = false;

    a_header_dimension return_dim;

    return_dim.width = min_width + m_width;
    return_dim.height = line_height + line_height;
    return_dim.rowchars = len;
    return_dim.positions[0] = len;
    return_dim.num_positions = 1;

    if(total_width > min_width && total_width <= max_width) {
        return_dim.width = total_width + m_width;
        return_dim.height = line_height + line_height;
    } else if(total_width > max_width) {
        row_len = ceil(((float)max_width/(float)total_width) * len);
        num_rows = ceil((float)total_width/(float)max_width);
        position = 0;
        header_valid = true;
        while(header_valid) {
            current_position = position;
            word_count = 0;
            row_width = 0;

            while(row_width < max_width) {
                last_position = current_position;
                if(end_pending) {
                    header_valid = false;
                    break;
                }
                position_pointer = strchr(header_text + current_position + 1,' ');
                if(position_pointer == NULL) {
                    end_pending = true;
                    current_position = len;
                } else {
                    current_position = (position_pointer - header_text) ;
                }
                memcpy( &single_row[0], header_text + position, current_position - position);
                single_row[current_position - position] = '\0';

                cairo_scaled_font_text_extents (cairo_get_scaled_font(cr), single_row, &my_font_extents);
                row_width = my_font_extents.width;
                word_count ++;
            }

            if(word_count > 1) {
                memcpy( &single_row[0], header_text + position, last_position - position );
                single_row[last_position - position] = '\0';
                position = last_position;
                end_pending = false;
            } else if(word_count <= 1) {
                if(!end_pending) {
                    memcpy( &single_row[0], header_text + position, row_len );
                    single_row[row_len] = '\0';
                    position += row_len;
                } else if(end_pending && (len - position) > row_len) {
                    memcpy( &single_row[0], header_text + position, row_len );
                    single_row[row_len] = '\0';
                    position += row_len;
                    end_pending = false;
                } else {
                    memcpy( &single_row[0], header_text + position, len - position );
                    single_row[len - position] = '\0';
                    position = len;
                }
            } else if(!header_valid) {
                memcpy( &single_row[0], header_text + position, len - position );
                single_row[len - position] = '\0';
                position = len;
            }

            cairo_scaled_font_text_extents (cairo_get_scaled_font(cr), single_row, &my_font_extents);
            temp_width = my_font_extents.width;
            if(temp_width > max_pixel_width) {
                max_pixel_width = temp_width;
            }
            return_dim.positions[num_num_rows] = (unsigned char)position;
            num_num_rows ++;
        }

        return_dim.width = max_pixel_width + m_width;
        return_dim.height = (line_height * num_num_rows + (line_height + line_height));
        return_dim.rowchars = row_len;
        return_dim.num_positions = num_num_rows;
    }
    return return_dim;
}
void draw_body(cairo_t *cr, int x,int  y, int width, int height, int priority, float children_scale_factor, bool last_read, int body_id, bool container_state) {
    RGB tempcolour;
    fixed_coords corr_coord;
    float rectx;
    float recty;
    float rectwidth;
    float rectheight;

    int maxx = x + width;
    int maxy = y + height;

    float arcradius;
    float arc_start_angle;
    float arc_end_angle;

    int i, j;

    static const double dashed1[] = {5.0, 3.0};
    static int len1  = sizeof(dashed1) / sizeof(dashed1[0]);
    static const double dashnone[] = {};

    cairo_set_line_width (cr, 2);

//========== body outline params ========================
    if(last_read) {
        cairo_set_dash(cr, dashed1, len1, 0);
        cairo_set_line_width (cr, 3);
    }

    if(!exporting_file) {
        if(body_id == 0) {
            tempcolour = colorConverter(0x4b5e4e);
        } else if(body_id == 1) {
            if(container_state) {
                tempcolour = colorConverter(0x444444);
            } else {
                tempcolour = colorConverter(0x324057);
            }
        } else {
            tempcolour = colorConverter(0x444444);
        }
        cairo_set_source_rgba (cr, tempcolour.r, tempcolour.g, tempcolour.b, 0.75);
        cairo_rectangle(cr, x, y, width, height);
        cairo_fill(cr);
    }

    tempcolour = colorConverter(priority_colour[priority]);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    for(i = 0; i < dynarray_length(body_shapes[body_id].shapes_sequence); i++) {
        if(body_shapes[body_id].shapes_sequence[i].basic_shape == RECTANGLE) {
            rectx = body_shapes[body_id].shapes_sequence[i].parameters[0];
            recty = body_shapes[body_id].shapes_sequence[i].parameters[1];
            rectwidth = body_shapes[body_id].shapes_sequence[i].parameters[2];
            rectheight = body_shapes[body_id].shapes_sequence[i].parameters[3];

            if(body_shapes[body_id].shapes_sequence[i].shape_base_type == ANCHOR) {
                fix_poscoords(&corr_coord, body_shapes[body_id].shapes_sequence[i].shape_pos_type, rectx, recty, 0, 0, x, y, maxx, maxy, body_id);
                cairo_rectangle(cr, corr_coord.x, corr_coord.y, rectwidth, rectheight);
            } else if(body_shapes[body_id].shapes_sequence[i].shape_base_type == ELASTIC) {
                fix_poscoords(&corr_coord, body_shapes[body_id].shapes_sequence[i].shape_pos_type, rectx, recty, rectwidth, rectheight, x, y, maxx, maxy, body_id);
                cairo_rectangle(cr, corr_coord.x, corr_coord.y, corr_coord.auxx, corr_coord.auxy);
            } else {
                cairo_rectangle(cr, x + (rectx*width), y + (recty*height), width*rectwidth, height*rectheight);
            }
            cairo_stroke(cr);
        }

        if(body_shapes[body_id].shapes_sequence[i].basic_shape == PATH) {
            rectx = body_shapes[body_id].shapes_sequence[i].parameters[0];
            recty = body_shapes[body_id].shapes_sequence[i].parameters[1];
            cairo_close_path(cr);
            if(body_shapes[body_id].shapes_sequence[i].shape_base_type == ANCHOR) {
                fix_poscoords(&corr_coord, body_shapes[body_id].shapes_sequence[i].shape_pos_type, rectx, recty, 0, 0, x, y, maxx, maxy, body_id);
                cairo_move_to(cr, corr_coord.x, corr_coord.y);
            } else if(body_shapes[body_id].shapes_sequence[i].shape_base_type == ELASTIC) {
                fix_poscoords(&corr_coord, body_shapes[body_id].shapes_sequence[i].shape_pos_type, rectx, recty, 0, 0, x, y, maxx, maxy, body_id);
                cairo_move_to(cr, corr_coord.x, corr_coord.y);

            } else {
                cairo_move_to(cr, x + (rectx*width), y + (recty*height));
            }

            for(j = 2; j < dynarray_length(body_shapes[body_id].shapes_sequence[i].parameters); j+=2) {
                rectx = body_shapes[body_id].shapes_sequence[i].parameters[j];
                recty = body_shapes[body_id].shapes_sequence[i].parameters[j+1];

                if(body_shapes[body_id].shapes_sequence[i].shape_base_type == ANCHOR) {
                    fix_poscoords(&corr_coord, body_shapes[body_id].shapes_sequence[i].shape_pos_type, rectx, recty, 0, 0, x, y, maxx, maxy, body_id);
                    cairo_line_to(cr, corr_coord.x, corr_coord.y);
                } else if(body_shapes[body_id].shapes_sequence[i].shape_base_type == ELASTIC) {
                    fix_poscoords(&corr_coord, body_shapes[body_id].shapes_sequence[i].shape_pos_type, rectx, recty, 0, 0, x, y, maxx, maxy, body_id);
                    cairo_line_to(cr, corr_coord.x, corr_coord.y);

                } else {
                    cairo_line_to(cr, x + (rectx*width), y + (recty*height));
                }
            }
            cairo_stroke(cr);
        }

        if(body_shapes[body_id].shapes_sequence[i].basic_shape == ARC) {
            rectx = body_shapes[body_id].shapes_sequence[i].parameters[0];
            recty = body_shapes[body_id].shapes_sequence[i].parameters[1];
            arcradius = body_shapes[body_id].shapes_sequence[i].parameters[2];
            arc_start_angle = body_shapes[body_id].shapes_sequence[i].parameters[3];
            arc_end_angle = body_shapes[body_id].shapes_sequence[i].parameters[4];
            cairo_close_path(cr);

            if(body_shapes[body_id].shapes_sequence[i].shape_base_type == ANCHOR) {
                fix_poscoords(&corr_coord, body_shapes[body_id].shapes_sequence[i].shape_pos_type, rectx, recty, 0, 0, x, y, maxx, maxy, body_id);
                cairo_arc (cr, corr_coord.x, corr_coord.y, arcradius, arc_start_angle, arc_end_angle);
            } else if(body_shapes[body_id].shapes_sequence[i].shape_base_type == ELASTIC) {
                fix_poscoords(&corr_coord, body_shapes[body_id].shapes_sequence[i].shape_pos_type, rectx, recty, arcradius, arcradius, x, y, maxx, maxy, body_id);
                cairo_arc (cr, corr_coord.x, corr_coord.y, arcradius, arc_start_angle, arc_end_angle);
            } else {
                cairo_arc (cr, x + (rectx*width), y + (recty*height), arcradius*width, arc_start_angle, arc_end_angle);
            }

            cairo_stroke_preserve(cr);
        }
    }

    cairo_set_line_width (cr, 1);
    tempcolour = colorConverter(0x444444);
    cairo_set_dash(cr, dashnone, 0, 0);

    if(children_scale_factor > 0.35) {
    } else {
        cairo_set_source(cr, pattern);
        tempcolour = colorConverter(0x444444);
        cairo_rectangle(cr, x + 5, y + 5, width - 10, height - 10);
        cairo_fill(cr);
    }
}
void draw_libre_arc(cairo_t *cr, int node, int frame_x, int frame_y, float scale_factor) {
    RGB tempcolour;

    tempcolour = colorConverter(0x339933);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);

    cairo_move_to(cr, frame_x + (current_nodegroup.node_group[node].x + current_nodegroup.node_group[node].width)*scale_factor + 2, frame_y + (current_nodegroup.node_group[node].y + (node_head_height + node_spacer_height + node_descript_height + node_spacer_height) + current_nodegroup.node_group[node].height / 2)*scale_factor);
    cairo_line_to(cr, frame_x, frame_y );

    cairo_set_line_width (cr, 1);
    cairo_stroke (cr);
}
void drawGrid(cairo_t *cr) {
    int i = 0;
    int j = 0;
    RGB tempcolour;

    tempcolour = colorConverter(0x323232);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_set_line_width (cr, 2);
    for(i = 0; i <= screenWidth; i+= grid_size) {
        if(j == major_grid) {
            cairo_set_line_width (cr, 2);
            tempcolour = colorConverter(0x323232);
            cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
            j = 0;
        } else {
            cairo_set_line_width (cr, 2);
            tempcolour = colorConverter(0x292929);
            cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
        }
        cairo_move_to (cr, i * current_nodegroup.zoomScaleFactor,0);
        cairo_line_to (cr, i * current_nodegroup.zoomScaleFactor,screenHeight);
        cairo_stroke (cr);
        j++;
    }

    for(i = 0; i <= screenHeight; i+= grid_size) {
        if(j == major_grid) {
            cairo_set_line_width (cr, 2);
            tempcolour = colorConverter(0x323232);
            cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
            j = 0;
        } else {
            cairo_set_line_width (cr, 1);
            tempcolour = colorConverter(0x292929);
            cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
        }
        cairo_move_to (cr, 0,i * current_nodegroup.zoomScaleFactor);
        cairo_line_to (cr, screenWidth,i * current_nodegroup.zoomScaleFactor);
        cairo_stroke (cr);
        j++;
    }
}
void draw_nodes(cairo_t *cr, int *nodes, int codeBox_id, int x, int y, int width, int height, float scale_factor, bool container_state) {
    int i;
    int j;
    node_mode_flags node_modes;

    int list_id;
    int this_codeBox_id;

    if(codeBox_id == -1) {
        list_id = current_codeBox;
    } else {
        list_id = codeBox_id;
    }

    this_codeBox_id = list_id;

    for(j = 0; j < 3; j++) {
        for (i = 0; i < (dynarray_length(nodes) ); i++) {
            if (j == 0 && the_node_group[nodes[i]].next != -1) {
                draw_arc(cr, nodes[i], the_node_group[nodes[i]].next, list_id, x, y, scale_factor);
            }

            if((j == 1 && !the_node_group[nodes[i]].obscured) || (j == 2 && the_node_group[nodes[i]].obscured)) {
                node_modes.last_touched = false;
                node_modes.last_read = false;
                node_modes.libre = false;
                node_modes.obscured = false;
                node_modes.being_dragged = false;
                node_modes.being_resized = false;
                node_modes.to_ignore = false;

                if(j == 2) {
                    node_modes.obscured = true;
                }

                if(codeBox_id == current_nodegroup.last_node_read.codeBox_id && nodes[i] == current_nodegroup.last_node_read.node) {
                    node_modes.last_read = true;
                }
                if(codeBox_id == node_being_dragged.codeBox_id && nodes[i] == node_being_dragged.node) {
                    node_modes.being_dragged = true;
                }
                if(codeBox_id == node_being_resized.codeBox_id && nodes[i] == node_being_resized.node) {
                    node_modes.being_resized = true;
                }

                if(the_node_group[nodes[i]].x < 0 || the_node_group[nodes[i]].y < 0) {
                    node_modes.libre = true;
                    draw_libre_arc(cr, nodes[i],x, y, scale_factor);
                }

                if(!no_node_selected && nodes[i] == tempNodeZone.node) {
                    node_modes.last_touched = true;
                }
                if(the_node_group[nodes[i]].status == 1) {
                    node_modes.to_ignore = true;
                }

                drawNode(cr, nodes[i], list_id, x, y, width, height, scale_factor, node_modes, container_state);
            }
        }
    }
}
void fix_poscoords(fixed_coords* return_coords, shapepostype shape_poscoord, float x, float y, float auxx, float auxy, int minx, int miny, int maxx, int maxy, int shape_id ) {

    float returnx;
    float returny;
//order is ABLwh ABRwh ATLwh ATRwh
    int ABLw = body_shapes[shape_id].anchor_params[0];
    int ABLh = body_shapes[shape_id].anchor_params[1];
    int ABRw = body_shapes[shape_id].anchor_params[2];
    int ABRh = body_shapes[shape_id].anchor_params[3];
    int ATLw = body_shapes[shape_id].anchor_params[4];
    int ATLh = body_shapes[shape_id].anchor_params[5];
    int ATRw = body_shapes[shape_id].anchor_params[6];
    int ATRh = body_shapes[shape_id].anchor_params[7];

//EL ER ET EB
    int ELw = body_shapes[shape_id].elastic_params[0];
    int ERw = body_shapes[shape_id].elastic_params[1];
    int ETw = body_shapes[shape_id].elastic_params[2];
    int EBw = body_shapes[shape_id].elastic_params[3];

    int tempwh;
    float return_auxx;
    float return_auxy;


    switch(shape_poscoord) {
    case ABR:
        returnx = maxx - ABRw - x;
        returny = maxy - ABRh - y;
        break;
    case ABL:
        returnx = minx + x;
        returny = maxy - ABLh - y;
        break;
    case ATR:

        returnx = maxx - ATRw - x;
        returny = miny + y;
        break;
    case ATL:
        returnx = minx + x;
        returny = miny + y;
        break;

    case EL:
        tempwh = ((maxy - miny) - (ATLh + ABLh));
        returnx = minx + x;
        returny = tempwh*y + ATLh + miny;
        return_auxx = auxx;
        return_auxy = tempwh*auxy;
        break;
    case ER:
        tempwh = ((maxy - miny) - (ATRh + ABRh));
        returnx = maxx - ERw + x;
        returny = tempwh*y + ATRh + miny;
        return_auxx = auxx;
        return_auxy = tempwh*auxy;
        break;
    case ET:
        tempwh = ((maxx - minx) - (ATLw + ATRw));
        returnx = tempwh*x + ATLw + minx;
        returny = miny + y;
        return_auxx = tempwh*auxx;
        return_auxy = auxy;
        break;
    case EB:
        tempwh = ((maxx - minx) - (ABLw + ABRw));
        returnx = tempwh*x + ABLw + minx;
        returny = maxy - EBw + y;
        return_auxx = tempwh*auxx;
        return_auxy = auxy;
        break;
    }

    return_coords->x = returnx;
    return_coords->y = returny;
    return_coords->auxx = return_auxx;
    return_coords->auxy = return_auxy;
}
void drawNode(cairo_t *cr, int node, int codeBox_id, int frame_x, int frame_y, __attribute__((unused)) int frame_width, __attribute__((unused)) int frame_height, float scale_factor, node_mode_flags node_modes, bool container_state) {
    RGB tempcolour;

    int this_codeBox_id = codeBox_id;

    int contained_node;

    float x = the_node_group[node].x * scale_factor + frame_x;
    float y = the_node_group[node].y * scale_factor + frame_y;
    int height = the_node_group[node].height;
    int width = the_node_group[node].width;
    char name[128];
    strcpy(name, the_node_group[node].name);

    static const double dashed1[] = {5.0, 3.0};
    static int len1  = sizeof(dashed1) / sizeof(dashed1[0]);
    static const double dashnone[] = {};

    node_mode_flags inner_node_modes;

    char single_row[100];
    unsigned long int i = 0;
    int header_len;

    unsigned char header_text_positions[10];
    unsigned char header_text_num_positions;

    int body_id = 0;
    int text_size = node_text_size;
    a_header_dimension calculated_dimensions;
    if(!(the_node_group[node].header_dim.valid)) {
        calculated_dimensions = get_header_dimensions(cr, the_node_group[node].name, node_text_size, node_head_width, (node_head_width + node_head_extrawidth) );

        the_node_group[node].header_dim.width = calculated_dimensions.width;
        the_node_group[node].header_dim.height = calculated_dimensions.height;
        the_node_group[node].header_dim.rowchars = calculated_dimensions.rowchars;

        the_node_group[node].header_dim.num_positions = calculated_dimensions.num_positions;
        memcpy( &the_node_group[node].header_dim.positions[0], &calculated_dimensions.positions[0], calculated_dimensions.num_positions );

        the_node_group[node].header_dim.valid = true;

        check_obscured(node, codeBox_id);
        check_obscuring(node, codeBox_id);
    }

    float head_width  = the_node_group[node].header_dim.width * scale_factor;
    float head_height = the_node_group[node].header_dim.height * scale_factor;
    int head_rowchars = the_node_group[node].header_dim.rowchars;
    header_text_num_positions = the_node_group[node].header_dim.num_positions;
    int head_num_rows = header_text_num_positions;
    int head_row_height = ceil(head_height / head_num_rows);
    int head_text_position = 0;
    memcpy( &header_text_positions[0], &the_node_group[node].header_dim.positions[0], header_text_num_positions );

    float descript_height = node_descript_height * scale_factor;
    float descript_width = node_descript_width * scale_factor;
    float body_height = height * scale_factor;
    float body_width = width * scale_factor;
    float spacer  = node_spacer_height *scale_factor;
    float children_scale_factor;

    a_node_note contained_top_node_struct;
    int* primary_nodes;

    if(the_node_group[node].is_file == 1 && the_node_group[node].file_tab_id != -1) {
        contained_top_node_struct.node = -1;
        contained_top_node_struct.codeBox_id = the_node_group[node].file_tab_id;
        primary_nodes = getPrimaryNodes(contained_top_node_struct);
        children_scale_factor = scale_factor * calculateScale(node, codeBox_id, primary_nodes, the_node_group[node].file_tab_id, scale_factor, head_height +spacer + descript_height + spacer);
        dynarray_destroy(primary_nodes);
    } else {
        children_scale_factor = scale_factor * calculateScale(node, codeBox_id, the_node_group[node].contained, codeBox_id, scale_factor, head_height +spacer + descript_height + spacer);
    }

    if(scale_factor < 0.75)
        text_size = floor(node_text_height * scale_factor);

// ============  we start the head =============
    tempcolour = colorConverter(0x888888);
    if(node_modes.last_touched) {
        tempcolour = colorConverter(0x4169E1);
    } else if(node_modes.obscured) {
        tempcolour = colorConverter(0xF4A460);
    } else if(node_modes.libre) {
        tempcolour = colorConverter(0x339933);
    } else {
        tempcolour = colorConverter(0x333333);
    }
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_rectangle(cr, x, y, head_width, head_height);
    cairo_fill(cr);
// ===============================================

//=========== body outline shadow ================================
    if(!exporting_file) {
        cairo_set_line_width (cr, 3);
        tempcolour = colorConverter(0x222222);
        cairo_set_source_rgba (cr, tempcolour.r, tempcolour.g, tempcolour.b, 0.5);
        cairo_rectangle(cr, x, y + head_height +spacer + descript_height + spacer, body_width + 2, body_height + 2);
        cairo_stroke(cr);
    }
//====================================================

    if(the_node_group[node].is_file == 1 && the_node_group[node].file_tab_id != -1) {
        body_id = 2;
    } else if(dynarray_length(the_node_group[node].contained) != 0) {
        body_id = 1;
    } else {
        body_id = 0;
    }

    draw_body(cr, x, y + head_height +spacer + descript_height + spacer, body_width, body_height, the_node_group[node].priority, children_scale_factor, node_modes.last_read, body_id, container_state);

// ============ resize tab ======================================
    tempcolour = colorConverter(0x777777);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_rectangle(cr, x + body_width - 15*scale_factor, y + body_height + head_height +spacer + descript_height + spacer - 15*scale_factor, 15*scale_factor - 2, 15*scale_factor - 2);
    cairo_fill(cr);
// ================================================================

//==============drag outline ==========================
    if(node_modes.being_dragged) {
        tempcolour = colorConverter(0x4169E1);
        cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
        cairo_set_line_width (cr, 3);
        cairo_rectangle(cr, x - 2, y - 2, head_width + 4, head_height + 4);
        cairo_stroke(cr);
    }
//=====================================================

//==============resize outline ==========================
    if(node_modes.being_resized) {
        tempcolour = colorConverter(0x4169E1);
        cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
        cairo_set_line_width (cr, 3);
        cairo_rectangle(cr, x + body_width - 15*scale_factor - 2, y + body_height + head_height +spacer + descript_height + spacer - 15*scale_factor - 2, 15*scale_factor - 2 + 4, 15*scale_factor - 2 + 4);

        cairo_stroke(cr);
    }
//=====================================================

    if(the_node_group[node].width > 75 && the_node_group[node].height > 35) {
        tempcolour = colorConverter(0xcccccc);
        cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);

        cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size (cr, text_size);

        draw_text_in_box(cr, the_node_group[node].real_code, x + spacer, y + head_height+spacer, body_width, body_height);
    }

    tempcolour = colorConverter(0xffffff);

    cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, text_size);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);

    header_len = strlen(name);

    for(i = 0; i < header_text_num_positions; i++) {
        memcpy( &single_row[0], &name[head_text_position], header_text_positions[i] - head_text_position);
        single_row[header_text_positions[i] - head_text_position] = '\0';

        cairo_move_to (cr, x + spacer, y + (head_row_height * i ) + (head_row_height/2) + (head_row_height/8) + spacer);
        cairo_show_text (cr, single_row);

        head_text_position = header_text_positions[i];
    }

    i = 0;

    if(children_scale_factor > 0.35) {
        if(the_node_group[node].is_file == 1 && the_node_group[node].file_tab_id != -1) {
            contained_top_node_struct.node = -1;
            contained_top_node_struct.codeBox_id = the_node_group[node].file_tab_id;
            primary_nodes = getPrimaryNodes(contained_top_node_struct);
            draw_nodes(cr, primary_nodes, the_node_group[node].file_tab_id, x, y+ head_height +spacer + descript_height + spacer, the_node_group[node].width, the_node_group[node].height, children_scale_factor, !container_state);
            dynarray_destroy(primary_nodes);
        } else {
            draw_nodes(cr, the_node_group[node].contained, codeBox_id, x, y+ head_height +spacer + descript_height + spacer, current_nodegroup.node_group[node].width, current_nodegroup.node_group[node].height, children_scale_factor, !container_state);
        }
    }

    if(node_modes.to_ignore) {
        drawX(cr, x, y + head_height +spacer + descript_height + spacer, body_width, body_height);
    }
}
void clearRenderedData() {
    int temp_id = 0;

    while (dynarray_length(rendered_data) > 0) {
        dynarray_pop(rendered_data, &temp_id);
    }
    while (dynarray_length(rendered_counts) > 0) {
        dynarray_pop(rendered_counts, &temp_id);
    }
}
void renderDataNodes(int* nodes, bool allow_previous) {
    int node_priority;
    int p;
    unsigned long int j;
    int* next_node;
    bool not_to_ignore;

    int* ordered_nodes;

    ordered_nodes = rearrange_nodes(nodes);

    for(p = 0; p <= 9; p++) {
        for (j = 0; j < dynarray_length(ordered_nodes); j++) {
            node_priority = current_nodegroup.node_group[ordered_nodes[j]].priority;
            if(node_priority == -1)
                node_priority = 5;

            not_to_ignore = (current_nodegroup.node_group[ordered_nodes[j]].status == 0);

            if ((allow_previous || current_nodegroup.node_group[ordered_nodes[j]].previous == -1) && node_priority == p) {
                if(not_to_ignore) {
                    if(strlen(current_nodegroup.node_group[ordered_nodes[j]].head) > 0) {
                        string_add(&output_text, current_nodegroup.node_group[ordered_nodes[j]].head);
                        string_add(&output_text, "\r\n");
                    }
                    if (dynarray_length(current_nodegroup.node_group[ordered_nodes[j]].contained) == 0) {
                        dynarray_push(rendered_data, ordered_nodes[j]);
                        if(strlen(current_nodegroup.node_group[ordered_nodes[j]].real_code) > 0) {
                            string_add(&output_text, current_nodegroup.node_group[ordered_nodes[j]].real_code);
                            string_add(&output_text, "\r\n");
                        }
                    } else {
                        renderDataNodes(current_nodegroup.node_group[ordered_nodes[j]].contained, false);
                    }
                    if(strlen(current_nodegroup.node_group[ordered_nodes[j]].tail) > 0) {
                        string_add(&output_text, current_nodegroup.node_group[ordered_nodes[j]].tail);
                        string_add(&output_text, "\r\n");
                    }
                }
                if (current_nodegroup.node_group[ordered_nodes[j]].next != -1) {
                    next_node = dynarray_create(int);
                    dynarray_push(next_node, current_nodegroup.node_group[ordered_nodes[j]].next);
                    renderDataNodes(next_node, true);
                    dynarray_destroy(next_node);
                }
            }
        }
    }

    dynarray_destroy(ordered_nodes);
}
void renderLineDataNodes(int* nodes, bool allow_previous) {
    int node_priority;

    unsigned int j;
    int p;
    int *next_node;

    bool not_to_ignore;

    int *ordered_nodes;

    ordered_nodes = rearrange_nodes(nodes);

    for(p = 0; p <= 9; p++) {
        for (j = 0; j < dynarray_length(ordered_nodes); j++) {
            node_priority = current_nodegroup.node_group[ordered_nodes[j]].priority;

            if(node_priority == -1)
                node_priority = 5;

            not_to_ignore = (current_nodegroup.node_group[ordered_nodes[j]].status == 0);

            if ((allow_previous || current_nodegroup.node_group[ordered_nodes[j]].previous == -1) && node_priority == p) {
                if(not_to_ignore) {
                    if(strlen(current_nodegroup.node_group[ordered_nodes[j]].head) > 0) {
                        dynarray_push(rendered_data, ordered_nodes[j]);
                        dynarray_push(rendered_counts, line_count);
                        line_count++;
                    }
                    if (dynarray_length(current_nodegroup.node_group[ordered_nodes[j]].contained) == 0) {
                        if(strlen(current_nodegroup.node_group[ordered_nodes[j]].real_code) > 0) {
                            dynarray_push(rendered_data, ordered_nodes[j]);
                            dynarray_push(rendered_counts, line_count);
                            line_count += count_string_lines(current_nodegroup.node_group[ordered_nodes[j]].real_code);
                        }
                    } else {
                        renderLineDataNodes(current_nodegroup.node_group[ordered_nodes[j]].contained, false);
                    }
                    if(strlen(current_nodegroup.node_group[ordered_nodes[j]].tail) > 0) {
                        dynarray_push(rendered_data, ordered_nodes[j]);
                        dynarray_push(rendered_counts, line_count);
                        line_count++;
                    }
                }
                if (current_nodegroup.node_group[ordered_nodes[j]].next != -1) {
                    next_node = dynarray_create(int);
                    dynarray_push(next_node, current_nodegroup.node_group[ordered_nodes[j]].next);
                    renderLineDataNodes(next_node, true);
                    dynarray_destroy(next_node);
                }
            }
        }
    }
    dynarray_destroy(ordered_nodes);
}
void renderRibbonDataNodes(int* nodes, bool allow_previous, app_widgets *app_wdgts) {
    int node_priority;
    int p;
    unsigned long int j;
    int* next_node;
    bool not_to_ignore;

    int ribbon_id;
    char ribbon_id_text[10];

    int new_wrapping_mode;

    int *ordered_nodes;

    a_ribbon_element *temp_ribbon_element;
    GtkLabel 	*temp_lbl_head_id;
    GtkLabel 	*temp_lbl_tail_id;

    GdkColor color = {0, 155<<8, 120<<8, 120<<8};

    if(current_nodegroup.line_wrapping) {
        new_wrapping_mode = GTK_WRAP_WORD_CHAR;
    } else {
        new_wrapping_mode = GTK_WRAP_NONE;
    }

    ordered_nodes = rearrange_nodes(nodes);

    for(p = 0; p <= 9; p++) {
        for (j = 0; j < dynarray_length(ordered_nodes); j++) {
            node_priority = current_nodegroup.node_group[ordered_nodes[j]].priority;
            if(node_priority == -1)
                node_priority = 5;

            not_to_ignore = (current_nodegroup.node_group[ordered_nodes[j]].status == 0);

            if ((allow_previous || current_nodegroup.node_group[ordered_nodes[j]].previous == -1) && node_priority == p) {
                if(not_to_ignore) {
                    temp_ribbon_element = malloc (sizeof (a_ribbon_element));

                    ribbon_id = node_being_edited.ribbon_count + 1;
                    node_being_edited.ribbon_count++;

                    //------------ head --------------------------------------------------
                    temp_ribbon_element->b_head_strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
                    memset(&ribbon_id_text, 0, 10);
                    sprintf(&ribbon_id_text, "[%d]>", ribbon_id);
                    temp_lbl_head_id = gtk_label_new(ribbon_id_text);
                    gtk_container_add(GTK_CONTAINER(temp_ribbon_element->b_head_strip), temp_lbl_head_id);
                    gtk_widget_show (temp_lbl_head_id);
                    gtk_box_set_child_packing (GTK_CONTAINER(temp_ribbon_element->b_head_strip),
                                               temp_lbl_head_id,
                                               FALSE, FALSE, 0, GTK_PACK_START);
                    temp_ribbon_element->e_txt_codebox_head = gtk_entry_new();
                    gtk_entry_set_has_frame(temp_ribbon_element->e_txt_codebox_head, FALSE);
                    gtk_widget_modify_base(GTK_WIDGET(temp_ribbon_element->e_txt_codebox_head), GTK_STATE_NORMAL, &color);
                    gtk_container_add(GTK_CONTAINER(temp_ribbon_element->b_head_strip), temp_ribbon_element->e_txt_codebox_head);
                    gtk_box_set_child_packing (GTK_CONTAINER(temp_ribbon_element->b_head_strip),
                                               temp_ribbon_element->e_txt_codebox_head,
                                               TRUE, TRUE, 0, GTK_PACK_START);
                    //======================================================


                    //--------------- code ----------------------------------
                    temp_ribbon_element->e_txt_codebox_code = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
                    temp_ribbon_element->s_source_view = gtk_source_view_new_with_buffer (temp_ribbon_element->e_txt_codebox_code);
                    gtk_source_buffer_set_language(temp_ribbon_element->e_txt_codebox_code, current_nodegroup.language);
                    gtk_text_view_set_monospace (temp_ribbon_element->s_source_view, TRUE);
                    gtk_text_view_set_wrap_mode (temp_ribbon_element->s_source_view, new_wrapping_mode);
                    gtk_text_view_set_left_margin (temp_ribbon_element->s_source_view, 2);
                    gtk_source_view_set_show_right_margin (temp_ribbon_element->s_source_view, TRUE);
                    gtk_source_view_set_right_margin_position(temp_ribbon_element->s_source_view, 80);
                    gtk_source_view_set_show_line_marks (temp_ribbon_element->s_source_view, TRUE);
                    gtk_source_view_set_highlight_current_line(temp_ribbon_element->s_source_view, TRUE);
                    // ======================================================

                    //----------------tail ---------------------------------------
                    temp_ribbon_element->b_tail_strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
                    memset(&ribbon_id_text, 0, 10);
                    sprintf(&ribbon_id_text, "<[%d]", ribbon_id);
                    temp_lbl_tail_id = gtk_label_new(ribbon_id_text);
                    gtk_container_add(GTK_CONTAINER(temp_ribbon_element->b_tail_strip), temp_lbl_tail_id);
                    gtk_widget_show (temp_lbl_tail_id);
                    gtk_box_set_child_packing (GTK_CONTAINER(temp_ribbon_element->b_tail_strip),
                                               temp_lbl_tail_id,
                                               FALSE, FALSE, 0, GTK_PACK_START);

                    temp_ribbon_element->e_txt_codebox_tail = gtk_entry_new();
                    gtk_entry_set_has_frame(temp_ribbon_element->e_txt_codebox_tail, FALSE);
                    gtk_container_add(GTK_CONTAINER(temp_ribbon_element->b_tail_strip), temp_ribbon_element->e_txt_codebox_tail);
                    gtk_box_set_child_packing (GTK_CONTAINER(temp_ribbon_element->b_tail_strip),
                                               temp_ribbon_element->e_txt_codebox_tail,
                                               TRUE, TRUE, 0, GTK_PACK_START);

                    //================================================================================

                    temp_ribbon_element->node_id = ordered_nodes[j];
                    temp_ribbon_element->codeBox_id = current_codeBox;

                    gtk_entry_set_text (temp_ribbon_element->e_txt_codebox_head, current_nodegroup.node_group[ordered_nodes[j]].head);
                    gtk_container_add (GTK_CONTAINER (app_wdgts->b_box_ribbon), temp_ribbon_element->b_head_strip);
                    gtk_widget_show (temp_ribbon_element->e_txt_codebox_head);
                    gtk_widget_show (temp_ribbon_element->b_head_strip);
                    if (dynarray_length(current_nodegroup.node_group[ordered_nodes[j]].contained) == 0) {
                        dynarray_push(rendered_data, ordered_nodes[j]);
                        gtk_text_buffer_set_text(temp_ribbon_element->e_txt_codebox_code, current_nodegroup.node_group[ordered_nodes[j]].real_code, -1);
                        gtk_container_add (GTK_CONTAINER (app_wdgts->b_box_ribbon), temp_ribbon_element->s_source_view);
                        gtk_widget_show (temp_ribbon_element->s_source_view);
                    } else {
                        renderRibbonDataNodes(current_nodegroup.node_group[ordered_nodes[j]].contained, false, app_wdgts);
                    }
                    gtk_entry_set_text (temp_ribbon_element->e_txt_codebox_tail, current_nodegroup.node_group[ordered_nodes[j]].tail);
                    gtk_container_add (GTK_CONTAINER (app_wdgts->b_box_ribbon), temp_ribbon_element->b_tail_strip);
                    gtk_widget_show (temp_ribbon_element->e_txt_codebox_tail);
                    gtk_widget_show (temp_ribbon_element->b_tail_strip);
                    dynarray_push(node_being_edited.editing_ribbon, *temp_ribbon_element);
                }
                if (current_nodegroup.node_group[ordered_nodes[j]].next != -1) {
                    next_node = dynarray_create(int);
                    dynarray_push(next_node, current_nodegroup.node_group[ordered_nodes[j]].next);
                    renderRibbonDataNodes(next_node, true, app_wdgts);
                    dynarray_destroy(next_node);
                }
            }
        }
    }
    dynarray_destroy(ordered_nodes);
}
int main(int argc, char **argv)  {
    GtkWidget *window;
    GtkBuilder *builder;
    GError *error = NULL;

    GtkWidget *newnodeMi;
    GtkWidget *popup_separator1;
    GtkWidget *copynodeMi;
    GtkWidget *pastenodeMi;
    GtkWidget *popup_separator2;
    GtkWidget *focusMi;
    GtkWidget *restoreMi;

    PangoAttrList *attrlist = pango_attr_list_new();
    PangoAttribute *attr = pango_attr_size_new_absolute(20 * PANGO_SCALE);

    RGB tempcolour;
    cairo_surface_t *pattern_surface;
    cairo_t *pattcr;

    app_widgets     *widgets = g_slice_new(app_widgets);
    gtk_init(&argc,&argv);

    pango_attr_list_insert(attrlist, attr);
    builder = gtk_builder_new ();
    if( gtk_builder_add_from_resource (builder,"/org/neete/ui/neete.gui",&error) == 0)
    {
        printf("gtk_builder_add_from_file FAILED %s\n",error->message);
        return EXIT_FAILURE;
    }

    init_arrays();
    init_nodes();
    window  = GTK_WIDGET (gtk_builder_get_object (builder,"window"));
    widgets->l_lbl_codebox_focus = GTK_LABEL(gtk_builder_get_object(builder, "lbl_codebox_focus"));
    widgets->w_darea_scroll = GTK_WIDGET(gtk_builder_get_object(builder, "darea_scroll"));
    widgets->drawingArea = GTK_WIDGET(gtk_builder_get_object(builder, "drawingarea"));
    widgets->w_dlg_file_choose = GTK_WIDGET(gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->w_dlg_file_save = GTK_WIDGET(gtk_builder_get_object(builder, "dlg_file_save"));
    widgets->w_dlg_codebox = GTK_WIDGET(gtk_builder_get_object(builder, "window_codebox"));
    widgets->e_txt_codebox_name = GTK_ENTRY(gtk_builder_get_object(builder, "txt_codebox_name"));
    widgets->e_txt_codebox_description = GTK_ENTRY(gtk_builder_get_object(builder, "txt_codebox_description"));
    widgets->b_chkbtn_codebox_ignore = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "chkbtn_codebox_ignore"));
    widgets->b_chkbtn_codebox_isfile = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "chkbtn_codebox_isfile"));
    widgets->e_txt_codebox_head = GTK_ENTRY(gtk_builder_get_object(builder, "txt_codebox_head"));
    widgets->e_txt_codebox_priority = GTK_ENTRY(gtk_builder_get_object(builder, "txt_codebox_priority"));
    widgets->c_cont_scrollwin = GTK_WIDGET(gtk_builder_get_object(builder, "cont_codebox_sw"));
    widgets->b_box_ribbon = GTK_WIDGET(gtk_builder_get_object(builder, "bx_ribbon_test"));
    widgets->s_source_view = GTK_SOURCE_VIEW(gtk_builder_get_object(builder, "txt_codebox_code"));
    widgets->e_txt_codebox_code = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->s_source_view)));
    widgets->e_txt_codebox_tail = GTK_ENTRY(gtk_builder_get_object(builder, "txt_codebox_tail"));
    widgets->t_ntb_nete_tabs = GTK_NOTEBOOK (gtk_builder_get_object (builder, "ntb_nete_tabs"));
    current_nodegroup.codebox_container = GTK_WIDGET(gtk_builder_get_object(builder, "init_darea_box"));

    widgets->e_txt_search_line = GTK_ENTRY(gtk_builder_get_object(builder, "txt_search_line"));

    widgets->w_dlg_options = GTK_WIDGET(gtk_builder_get_object(builder, "window_options"));
    widgets->w_dlg_about = GTK_WIDGET(gtk_builder_get_object(builder, "dlg_about"));
    widgets->e_txt_options_destinations = GTK_ENTRY(gtk_builder_get_object(builder, "txt_options_destinations"));

    widgets->l_lbl_main_feedback = GTK_LABEL(gtk_builder_get_object(builder, "lbl_main_feedback"));

    widgets->w_msg_file_save_warn = GTK_DIALOG(gtk_builder_get_object(builder, "msg_file_save_warn"));

    widgets->b_mnu_wrap = GTK_WIDGET(gtk_builder_get_object(builder, "cmnu_wrap"));

    widgets->language_buttons.b_rbtn_c_cpp = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_c_cpp"));
    widgets->language_buttons.b_rbtn_html = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_html"));
    widgets->language_buttons.b_rbtn_java = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_java"));
    widgets->language_buttons.b_rbtn_javascript = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_javascript"));
    widgets->language_buttons.b_rbtn_latex = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_latex"));
    widgets->language_buttons.b_rbtn_php = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_php"));
    widgets->language_buttons.b_rbtn_python = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_python"));
    widgets->language_buttons.b_rbtn_verilog = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_verilog"));
    widgets->language_buttons.b_rbtn_plain_text = GTK_WIDGET(gtk_builder_get_object(builder, "rbtn_plain_text"));

    widgets->tool_radio_buttons.r_movebtn = GTK_WIDGET(gtk_builder_get_object(builder, "btn_rdo_move"));
    widgets->tool_radio_buttons.r_containbtn = GTK_WIDGET(gtk_builder_get_object(builder, "btn_rdo_exenter"));
    widgets->tool_radio_buttons.r_linkbtn = GTK_WIDGET(gtk_builder_get_object(builder, "btn_rdo_link"));


    widgets->netefilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(widgets->netefilter, "*.nete");
    gtk_file_filter_add_pattern(widgets->netefilter, "*.json");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(widgets->w_dlg_file_choose), widgets->netefilter);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(widgets->w_dlg_file_save), widgets->netefilter);

    widgets->sessionfilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(widgets->sessionfilter, "*.prj");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(widgets->w_dlg_file_choose), widgets->sessionfilter);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(widgets->w_dlg_file_save), widgets->sessionfilter);

    gtk_entry_set_attributes(GTK_ENTRY(widgets->e_txt_codebox_name), attrlist);
    pango_attr_list_unref(attrlist);

    manager = gtk_source_language_manager_get_default();
    language = gtk_source_language_manager_get_language(manager, "c");
    gtk_source_buffer_set_language(widgets->e_txt_codebox_code, language);
    pmenu = gtk_menu_new();
    newnodeMi = gtk_menu_item_new_with_label("New Node");
    gtk_widget_show(newnodeMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), newnodeMi);

    g_signal_connect(G_OBJECT(newnodeMi), "activate",
                     G_CALLBACK(nete_new_node), widgets->drawingArea);

    popup_separator1 = gtk_separator_menu_item_new ();
    gtk_widget_show(popup_separator1);
    gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), popup_separator1);
    copynodeMi = gtk_menu_item_new_with_label("Copy");
    gtk_widget_show(copynodeMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), copynodeMi);

    g_signal_connect(G_OBJECT(copynodeMi), "activate",
                     G_CALLBACK(nete_copy_node), widgets->drawingArea);
    pastenodeMi = gtk_menu_item_new_with_label("Paste");
    gtk_widget_show(pastenodeMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), pastenodeMi);

    g_signal_connect(G_OBJECT(pastenodeMi), "activate",
                     G_CALLBACK(nete_paste_node), widgets->drawingArea);

    popup_separator2 = gtk_separator_menu_item_new ();
    gtk_widget_show(popup_separator2);
    gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), popup_separator2);
    focusMi = gtk_menu_item_new_with_label("Focus");
    gtk_widget_show(focusMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), focusMi);

    g_signal_connect(G_OBJECT(focusMi), "activate",
                     G_CALLBACK(nete_focus_node), widgets);
    restoreMi = gtk_menu_item_new_with_label("Restore");
    gtk_widget_show(restoreMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), restoreMi);

    g_signal_connect(G_OBJECT(restoreMi), "activate",
                     G_CALLBACK(nete_restore_all), widgets);
    gtk_builder_connect_signals(builder, widgets);
    g_object_unref(builder);

    last_refresh_time = g_timer_new ();;
    gtk_widget_show_all (window);

    gtk_widget_hide(widgets->l_lbl_codebox_focus);  // hide it here after the show_all command, otherwise it shows again

// ============== pattern for oversmall details ====================
    pattern_surface = cairo_image_surface_create(
                          CAIRO_FORMAT_ARGB32, 10, 10);

    pattcr = cairo_create(pattern_surface);
    tempcolour = colorConverter(0x888888);
    cairo_set_source_rgba (pattcr, tempcolour.r, tempcolour.g, tempcolour.b, 0.75);
    cairo_move_to(pattcr, 0, 0);
    cairo_line_to(pattcr, 10, 10);
    cairo_move_to(pattcr, 10, 0);
    cairo_line_to(pattcr, 0, 10);
    cairo_stroke(pattcr);
    cairo_destroy(pattcr);

    pattern = cairo_pattern_create_for_surface(pattern_surface);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
// ===========================================================

    gtk_main();
    cairo_surface_destroy(pattern_surface);
    cairo_pattern_destroy(pattern);
    g_slice_free(app_widgets, widgets);
    return EXIT_SUCCESS;
}

