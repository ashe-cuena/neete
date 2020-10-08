#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <glib.h>

#include "cJSON.h"

#include "dynarray.h"
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

    int* obscuring;
    int* obscured_by;
    bool obscured;

} a_node;
typedef struct {
    int node;
    int zone;
    float scale_factor;
    float x;
    float y;
    float node_x;
    float node_y;
    float node_width;
    float node_height;
    float frame_x;
    float frame_y;
} a_nodezoneinfo;
typedef enum {
    GRID,
    FINE,
    NONE
} snapping_modes;
typedef struct {
    float r;
    float g;
    float b;
} RGB;
typedef struct {
    a_nodezone array[20];
    int length;
} a_nodezone_array;
typedef struct {
    float x;
    float y;
    float scaleFactor;
} a_container_pos;
typedef struct {
    GtkWidget *w_dlg_file_choose;       // Pointer to file chooser dialog box
    GtkWidget *w_dlg_file_save;       // Pointer to file save dialog box
    GtkWidget *drawingArea;
//----------------------------------
    GtkWidget 	*w_dlg_codebox;   	// the codebox editor window
    GtkEntry 	*e_txt_codebox_name;
    GtkEntry 	*e_txt_codebox_description;
    GtkEntry 	*e_txt_codebox_head;
    GtkEntry 	*e_txt_codebox_priority;
    GtkSourceView	*s_source_view;
    GtkTextBuffer *e_txt_codebox_code;
    GtkEntry 	*e_txt_codebox_tail;
    GtkNotebook	*t_ntb_nete_tabs;
    GtkWidget	*w_darea_scroll;
    GtkLabel 	*l_lbl_codebox_focus;
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
} app_widgets;
typedef struct {
    int original;
    int newone;
} a_translate_vector;
struct {
    int node;
    int source_codeBox;

} node_to_copy;
typedef enum {
    MOVE,
    EXITENTER,
    JOIN
} toolingmodes;
typedef struct {
    bool last_touched;
    bool last_read;
    bool libre;
    bool obscured;
    bool being_dragged;
    bool being_resized;

} node_mode_flags;
typedef struct {
    a_node *node_group;
    int current_codeBox;
    float zoomScaleFactor;
    int topNode;
    int last_node_read;
    GtkWidget *codebox_container;
    char *destination;
    char *name;
    char *fileName;
    bool node_group_altered;
    bool fileSaveAsMode;
} a_node_group_context;
RGB colorConverter(int hexValue);
void redraw(cairo_t *cr) ;
void draw_arc(cairo_t *cr, int node1, int node2, int frame_x, int frame_y, float scale_factor);
void drawGrid(cairo_t *cr);
void drawNode(cairo_t *cr, int node, int frame_x, int frame_y, int frame_width, int frame_height, float scale_factor, node_mode_flags node_modes);
int node_group_push(int x, int y, int width, int height, int next, int previous, int cont_head, int cont_by, int priority, int* contained, char *name, char *description, char *head, char *tail, char *real_code, int node_id);
int node_zones_push(int x, int y, int width, int height);

void init_arrays();
void containerRemoveNode(int containing_node, int  inner_node);
float calculateScale(int node, int y_displacement, float scale_factor);
void init_nodes();
void get_absolutePosition(a_container_pos *container_pos, int node);
void doMouseDown2(int x, int y, gpointer data);
void doMouseDrag2(int x, int y, gpointer data);
void doDoubleClick(int x, int y, gpointer data, app_widgets *app_wdgts);
void doMouseUp2(int x, int y, gpointer data);
void getNodeAndZone(a_nodezoneinfo *return_nodezone, int *nodes, int x, int y, int frame_x, int frame_y, float scale_factor, int omit);
void fixZones(int node);
float snapGrid(float invalue);
void deactivate_quit(void);
int * getPrimaryNodes();
void load_codeBox(char *nete_string);
void launchBox(int node, int line_start, app_widgets *app_wdgts);
void node_group_update_id(int node_id, int group_id, char *name, char *description, char *head, int priority,  char *real_code, char *tail);
char* stringify_nodes(void);
void draw_text_in_box(cairo_t *cr, char *real_code, float x, float y, float width, float height);
void string_init(char** string_ptr);
void string_free(char** string_ptr);
void string_set(char** destination, char* value);
void string_add(char** destination, char* value);
void renderData(void);
void clearRenderedData(void);
void renderDataNodes(int* nodes);
void renderANode(int node);
void renderLineData(void);
void clearRenderedLineData(void);
void renderLineDataNodes(int* nodes);
void renderLineANode(int node);
void init_node_group_context(a_node_group_context *temp_group_list);
int pasteNode(int parent, int node, int source_codeBox);
void fixPasted(int source_codeBox);
int get_key(int key);
void clearTranslations(void);
int count_string_lines(char *string);
void line_search(app_widgets *app_wdgts);
void write_to_file(app_widgets *app_wdgts);
void create_new_tab(app_widgets *app_wdgts);
void write_feedback(app_widgets *app_wdgts, char *text);
bool recontain_node(a_nodezoneinfo* firstNodeZone, a_nodezoneinfo* secondNodeZone, int x, int y);
bool join_nodes(a_nodezoneinfo* firstNodeZone, a_nodezoneinfo* secondNodeZone);
bool check_not_descendent(int main_node, int desc_node);
void check_obscured(int node);
void check_obscuring(int node);
void clear_obscured_by(int node);
void clear_obscuring(int node);
void splice_out_node(int *array, int inner_node);
void draw_libre_arc(cairo_t *cr, int node, int frame_x, int frame_y, float scale_factor);
void mark_unsaved(app_widgets *app_wdgts);
bool check_unsaved(void);

GtkWidget *pmenu;

int screenWidth = 4095;
int screenHeight = 2047;

int grid_size = 20; // number of pixels
int major_grid = 5; //number of minor grids

int current_codeBox = 0;
a_node_group_context *codeBox_list;

int node_being_edited = -1;
int node_group_being_edited = -1;
int node_being_dragged = -1;
int node_being_resized = -1;
int node_displacement_count = 1;

int *rendered_data;
int *rendered_counts;
int line_count = 0;
a_nodezone_array node_zones;

a_nodezoneinfo tempNodeZone;
bool no_node_selected = true;

int current_x;
int current_y;

a_nodezone rightClickCoord;
a_nodezone codeboxWindowPos;

bool nodeHeadFound = false;
int group_nodeHeadFound = -1;
bool nodeBodyFlankFound = false;
int group_nodeBodyFlankFound = -1;
bool nodeWidthAdjust = false;
int group_nodeWidthFound = -1;
char *output_text;

bool mousePressed = false;
bool mouse_down = false;

bool adminOp = false;
int focusNode_x;
int focusNode_y;
bool sessionMode = false;
bool fileSaveAsMode = false;
bool first_time_codebox = true;
char default_destination[] = "nete_out.txt";

GTimer *last_refresh_time;
toolingmodes toolmode = MOVE;
int node_head_width = 100;
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
gint on_ntb_nete_tabs_switch_page(__attribute__((unused)) GtkButton *widget, __attribute__((unused)) GtkWidget *page, guint page_num, app_widgets *app_wdgts, __attribute__((unused)) gpointer data)
{
    int old_nodegroup_id;
    unsigned int new_page_num = page_num;

    old_nodegroup_id = current_codeBox;
    current_codeBox = new_page_num;

    if(current_nodegroup.topNode != -1) {
        gtk_label_set_text (app_wdgts->l_lbl_codebox_focus, current_nodegroup.node_group[current_nodegroup.topNode].name);
        gtk_widget_show(app_wdgts->l_lbl_codebox_focus);
    } else {
        gtk_widget_hide(app_wdgts->l_lbl_codebox_focus);
    }
    no_node_selected = true;

    g_object_ref(app_wdgts->l_lbl_codebox_focus);
    g_object_ref(app_wdgts->w_darea_scroll);
    gtk_container_remove (codeBox_list[old_nodegroup_id].codebox_container, app_wdgts->w_darea_scroll);
    gtk_container_remove (codeBox_list[old_nodegroup_id].codebox_container, app_wdgts->l_lbl_codebox_focus);
    gtk_container_add (current_nodegroup.codebox_container, app_wdgts->l_lbl_codebox_focus);
    gtk_container_add (current_nodegroup.codebox_container, app_wdgts->w_darea_scroll);

    gtk_widget_queue_draw(app_wdgts->drawingArea);

    return TRUE;
}

gint on_drawingarea_button_press_event(GtkWidget *widget, GdkEventButton *event, app_widgets *app_wdgts, gpointer data) {
    const guint RIGHT_CLICK = 3;

    if (event->type == GDK_BUTTON_PRESS) {

        GdkEventButton *bevent = (GdkEventButton *) event;

        if (bevent->button == RIGHT_CLICK) {
            rightClickCoord.x = (guint)event->x;
            rightClickCoord.y = (guint)event->y;
            //printf("right click ... (%d,%d)\n", (guint)event->x, (guint)event->y);
            gtk_menu_popup_at_pointer(GTK_MENU(pmenu), event);
            //dArea = widget;
        } else {
            mouse_down = true;

            doMouseDown2((guint)event->x, (guint)event->y, data);
        }
        return TRUE;
    }

    if(event->type == GDK_DOUBLE_BUTTON_PRESS) {
        // Show the "Open Text File" dialog box
        doDoubleClick((guint)event->x, (guint)event->y, data, app_wdgts);
    }

    gtk_widget_queue_draw( widget);
    mark_unsaved(app_wdgts);

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
    printf("%s called\n",__FUNCTION__);
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

        if(refresh_msec > 20000) {
            gtk_widget_queue_draw( widget);
            g_timer_start (last_refresh_time);
        }
    }
    return TRUE;
}
void on_btn_codebox_submit_clicked(__attribute__((unused)) GtkButton *widget, app_widgets *app_wdgts, __attribute__((unused)) gpointer data) {
    const gchar *name;
    const gchar *description;
    const gchar *head;
    const gchar *priority;
    const gchar *real_code;
    const gchar *tail;

    int priority_num = 0;

    GtkTextIter start;
    GtkTextIter end;

//printf("submitting\n");
    if(node_being_edited != -1 && node_group_being_edited != -1) {
        gtk_text_buffer_get_start_iter (app_wdgts->e_txt_codebox_code, &start);
        gtk_text_buffer_get_end_iter (app_wdgts->e_txt_codebox_code, &end);

        name = gtk_entry_get_text (app_wdgts->e_txt_codebox_name);
        description = gtk_entry_get_text (app_wdgts->e_txt_codebox_description);
        head = gtk_entry_get_text (app_wdgts->e_txt_codebox_head);
        priority = gtk_entry_get_text (app_wdgts->e_txt_codebox_priority);
        priority_num = atoi(priority);
        if(priority_num < 0) {
            priority_num = 0;
        }
        if(priority_num > 9) {
            priority_num = 9;
        }

        real_code = gtk_text_buffer_get_text(app_wdgts->e_txt_codebox_code, &start, &end, FALSE);
        tail = gtk_entry_get_text (app_wdgts->e_txt_codebox_tail);

        node_group_update_id(
            node_being_edited,
            node_group_being_edited,
            (char *) name,
            (char *) description,
            (char *) head,
            priority_num,
            (char *) real_code,
            (char *) tail);

        node_being_edited = -1;
        node_group_being_edited = -1;
    }
    gtk_window_get_position(GTK_WINDOW(app_wdgts->w_dlg_codebox), &codeboxWindowPos.x, &codeboxWindowPos.y);
    first_time_codebox = false;
//printf("new position is: x: %i  y:%i\n", codeboxWindowPos.x, codeboxWindowPos.y);
    gtk_widget_hide(app_wdgts->w_dlg_codebox);

    gtk_widget_queue_draw(app_wdgts->drawingArea);
    deactivate_quit();
    mark_unsaved(app_wdgts);
}
gboolean on_window_codebox_delete_event( GtkWidget *widget, __attribute__((unused)) GdkEvent *event, __attribute__((unused)) gpointer data) {
//gint x, y;
    gtk_window_get_position(GTK_WINDOW(widget), &codeboxWindowPos.x, &codeboxWindowPos.y);
    first_time_codebox = false;
//printf("new position is: x: %i  y:%i\n", codeboxWindowPos.x, codeboxWindowPos.y);
    gtk_widget_hide(widget);
    return TRUE;
}
int nete_new_node(__attribute__((unused)) GtkWidget *widget, GtkWidget *ddarea) {
    int* temp_contained_array = dynarray_create(int);

    int* primary_nodes;
    a_nodezoneinfo newNodeZone;
    a_nodezoneinfo newnodeNodeZone;

    newnodeNodeZone.node = node_group_push(
                               (rightClickCoord.x / current_nodegroup.zoomScaleFactor),
                               (rightClickCoord.y / current_nodegroup.zoomScaleFactor),
                               100,
                               20,
                               -1,
                               -1,
                               -1,
                               current_nodegroup.topNode,
                               5,
                               temp_contained_array,
                               "New Node",
                               "",
                               "",
                               "",
                               "",
                               -1
                           );

    if(current_nodegroup.topNode != -1 ) {
        dynarray_push(current_nodegroup.node_group[current_nodegroup.topNode].contained, newnodeNodeZone.node);
    }

    primary_nodes = getPrimaryNodes();

    getNodeAndZone(&newNodeZone, primary_nodes, rightClickCoord.x, rightClickCoord.y, 0, 0, current_nodegroup.zoomScaleFactor, -1);

    if(newNodeZone.node != current_nodegroup.topNode && (newNodeZone.zone == 1 || newNodeZone.zone == 2 || newNodeZone.zone == 3)) {
        recontain_node(&newnodeNodeZone.node, &newNodeZone, rightClickCoord.x, rightClickCoord.y);
    }

    check_obscuring(newnodeNodeZone.node);
    check_obscured(newnodeNodeZone.node);

    dynarray_destroy(temp_contained_array);
    dynarray_destroy(primary_nodes);

    gtk_widget_queue_draw(ddarea);
    deactivate_quit();

    return TRUE;
}
int nete_copy_node(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) GtkWidget *ddarea) {
    mousePressed = false;

    int* primary_nodes;

    primary_nodes = getPrimaryNodes();

    getNodeAndZone(&tempNodeZone, primary_nodes, rightClickCoord.x, rightClickCoord.y, 0, 0, current_nodegroup.zoomScaleFactor, -1);
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

    pastedNodeZone.node = pasteNode(current_nodegroup.topNode, node_to_copy.node, node_to_copy.source_codeBox);
    fixPasted(node_to_copy.source_codeBox);
    clearTranslations();

    primary_nodes = getPrimaryNodes();

    getNodeAndZone(&newNodeZone, primary_nodes, rightClickCoord.x, rightClickCoord.y, 0, 0, current_nodegroup.zoomScaleFactor, -1);

    if(newNodeZone.node != current_nodegroup.topNode && (newNodeZone.zone == 1 || newNodeZone.zone == 2 || newNodeZone.zone == 3)) {
        recontain_node(&pastedNodeZone, &newNodeZone, rightClickCoord.x, rightClickCoord.y);
    }
    check_obscuring(pastedNodeZone.node);
    check_obscured(pastedNodeZone.node);
    gtk_widget_queue_draw(ddarea);
    deactivate_quit();
//printf("new node created\n");
    dynarray_destroy(primary_nodes);

    return 0;
}
int nete_focus_node(__attribute__((unused)) GtkWidget *widget, app_widgets *app_wdgts) {
    mousePressed = false;

    int* primary_nodes;

    primary_nodes = getPrimaryNodes();

    getNodeAndZone(&tempNodeZone, primary_nodes, rightClickCoord.x, rightClickCoord.y, 0, 0, current_nodegroup.zoomScaleFactor, -1);

    dynarray_destroy(primary_nodes);

    if (tempNodeZone.node != -1) {
        if (tempNodeZone.zone == head_zone) {

            current_nodegroup.topNode = tempNodeZone.node;
            gtk_label_set_text (app_wdgts->l_lbl_codebox_focus, current_nodegroup.node_group[tempNodeZone.node].name);
            gtk_widget_show(app_wdgts->l_lbl_codebox_focus);
            gtk_widget_queue_draw(app_wdgts->drawingArea);
        }
    }
    return 0;
}
int nete_restore_all(__attribute__((unused)) GtkWidget *widget, app_widgets *app_wdgts) {
    current_nodegroup.topNode = -1;

    gtk_widget_hide(app_wdgts->l_lbl_codebox_focus);

    gtk_widget_queue_draw(app_wdgts->drawingArea);

    return 0;
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
gboolean on_msg_file_save_warn_delete_event (GtkWidget *widget, GdkEvent  *event, gpointer   user_data) {
    gtk_widget_hide(widget);

    return TRUE;
}
void on_window_delete_event(GtkWidget *object, app_widgets *app_wdgts, gpointer user_data) {
    if(check_unsaved()) {
        gtk_widget_show(user_data);
    } else {
        gtk_main_quit();
    }
}
void on_msg_file_save_warn_close (GtkDialog *dialog, gpointer   user_data) {

}
void on_msg_file_save_warn_response (GtkDialog *dialog, gint       response_id, gpointer   user_data) {
    if(response_id == -9) {
        gtk_widget_hide(dialog);
    } else if(response_id == -8) {
        gtk_main_quit();
    }
}
gint on_btn_line_submit_clicked(__attribute__((unused)) GtkButton *widget,
                                app_widgets *app_wdgts)
{
    line_search(app_wdgts);

    return TRUE;
}
void on_txt_search_line_activate (GtkEntry *entry, app_widgets *app_wdgts, gpointer  user_data) {
    printf("find line enter\n");
    line_search(app_wdgts);

    return TRUE;
}
void on_mnu_item_open_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gchar *file_name = NULL;        // Name of file to open from dialog box
    gchar *file_contents = NULL;    // For reading contents of file
    gboolean file_success = FALSE;  // File read status
    char *last_slash;

    char *tab_name;
    // Show the "Open Text File" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);
    // Check return value from Open Text File dialog box to see if user clicked the Open button
    if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_choose)) == GTK_RESPONSE_OK) {
        // Get the file name from the dialog box
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose));
        if (file_name != NULL) {
            // Copy the contents of the file to dynamically allocated memory
            file_success = g_file_get_contents(file_name, &file_contents, NULL, NULL);
            if (file_success) {
                create_new_tab(app_wdgts);

                load_codeBox(file_contents);

                last_slash = strrchr(file_name, '/');
                string_init(&tab_name);
                string_set(&tab_name, last_slash + 1);

                gtk_notebook_set_tab_label_text (app_wdgts->t_ntb_nete_tabs,
                                                 gtk_notebook_get_nth_page (app_wdgts->t_ntb_nete_tabs, current_codeBox),
                                                 tab_name);

                string_set(&(current_nodegroup.name), tab_name);
                string_set(&(current_nodegroup.fileName), file_name);

                string_free(&tab_name) ;

                gtk_notebook_set_current_page (app_wdgts->t_ntb_nete_tabs,
                                               current_codeBox);

                current_nodegroup.fileSaveAsMode = false;

                gtk_widget_queue_draw(app_wdgts->drawingArea);
            }
            g_free(file_contents);
        }
        g_free(file_name);
    }
    // Finished with the "Open Text File" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);
}
void on_mnu_item_save_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    write_to_file(app_wdgts);
}
void on_mnu_item_render_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    FILE *fp;

    char feedback_date[30];
    char *feedback_text;

    string_init(&feedback_text);
    clearRenderedData();
    renderData();
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
}
void on_mnu_item_saveas_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    current_nodegroup.fileSaveAsMode = true;
    write_to_file(app_wdgts);
}
void on_mnu_item_new_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    create_new_tab(app_wdgts);

    gtk_notebook_set_current_page (app_wdgts->t_ntb_nete_tabs,
                                   current_codeBox);
}
void on_mnu_nete_quit_activate(__attribute__((unused)) GtkMenuItem *menuitem,  gpointer user_data) {
    if(check_unsaved()) {
        gtk_widget_show(user_data);
    } else {
        gtk_main_quit();
    }
}
void on_cmnu_wrap_toggled (GtkCheckMenuItem *checkmenuitem, app_widgets *app_wdgts, gpointer user_data) {
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->b_mnu_wrap))) {
        gtk_text_view_set_wrap_mode (app_wdgts->s_source_view, GTK_WRAP_WORD_CHAR);
    }
    else {
        gtk_text_view_set_wrap_mode(app_wdgts->s_source_view, GTK_WRAP_NONE);
    }
}
void on_rbtn_language_toggled (GtkCheckMenuItem *checkmenuitem, app_widgets *app_wdgts, gpointer user_data) {
    GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
    GtkSourceLanguage *language;

    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_c_cpp))) {
        language = gtk_source_language_manager_get_language(manager, "c");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_html))) {
        language = gtk_source_language_manager_get_language(manager, "html");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_java))) {
        language = gtk_source_language_manager_get_language(manager, "java");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_javascript))) {
        language = gtk_source_language_manager_get_language(manager, "javascript");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_latex))) {
        language = gtk_source_language_manager_get_language(manager, "latex");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_php))) {
        language = gtk_source_language_manager_get_language(manager, "php");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_python))) {
        language = gtk_source_language_manager_get_language(manager, "python");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_verilog))) {
        language = gtk_source_language_manager_get_language(manager, "verilog");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
    }
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(app_wdgts->language_buttons.b_rbtn_plain_text))) {
        language = gtk_source_language_manager_get_language(manager, "text");
        gtk_source_buffer_set_language(app_wdgts->e_txt_codebox_code, language);
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
void on_mnu_findline_item_activate(__attribute__((unused)) GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    printf("find line\n");
    gtk_entry_grab_focus_without_selecting(app_wdgts->e_txt_search_line);
}
void on_btn_options_submit_clicked(__attribute__((unused)) GtkButton *widget, app_widgets *app_wdgts, __attribute__((unused)) gpointer data) {
    current_nodegroup.destination = (char *)gtk_entry_get_text (app_wdgts->e_txt_options_destinations);
    gtk_widget_hide(app_wdgts->w_dlg_options);
}
gboolean on_window_options_delete_event(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) GdkEvent *event, __attribute__((unused)) gpointer data) {
    gtk_widget_hide(widget);
    return TRUE;
}
void on_mnu_options_item_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts) {
    gtk_entry_set_text (app_wdgts->e_txt_options_destinations, current_nodegroup.destination);

    gtk_widget_show(app_wdgts->w_dlg_options);
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
void on_btn_codebox_tocodebox_clicked(__attribute__((unused)) GtkButton *widget, app_widgets *app_wdgts, __attribute__((unused)) gpointer data) {
    gchar *selected_text;
    int* temp_contained_array = dynarray_create(int);
    int top_node;
    int newnodeID;

    GtkTextIter start;
    GtkTextIter end;

    if(gtk_text_buffer_get_selection_bounds (app_wdgts->e_txt_codebox_code, &start, &end)) {

        selected_text = gtk_text_buffer_get_text(app_wdgts->e_txt_codebox_code, &start, &end, FALSE);

        gtk_text_buffer_delete_selection (app_wdgts->e_txt_codebox_code, FALSE, FALSE);

        top_node = current_nodegroup.node_group[node_being_edited].cont_by;

        newnodeID = node_group_push(
                        current_nodegroup.node_group[node_being_edited].x + node_displacement_count * 5,
                        current_nodegroup.node_group[node_being_edited].y + node_displacement_count * 5,
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
                        -1
                    );

        if(top_node != -1 ) {
            dynarray_push(current_nodegroup.node_group[top_node].contained, newnodeID);
        }

        check_obscuring(newnodeID);
        check_obscured(newnodeID);

        dynarray_destroy(temp_contained_array);
        g_free(selected_text);

        gtk_widget_queue_draw(app_wdgts->drawingArea);
        deactivate_quit();

        node_displacement_count ++;

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

    if(gtk_text_buffer_get_selection_bounds (app_wdgts->e_txt_codebox_code, &start, &end)) {
        string_init(&final_text);

        selected_text = gtk_text_buffer_get_text(app_wdgts->e_txt_codebox_code, &start, &end, FALSE);

        gtk_text_buffer_delete_selection (app_wdgts->e_txt_codebox_code, FALSE, FALSE);

        head_text = gtk_entry_get_text (app_wdgts->e_txt_codebox_head);
        string_set(&final_text, head_text);
        string_add(&final_text, " ");
        string_add(&final_text, selected_text);

        gtk_entry_set_text (app_wdgts->e_txt_codebox_head, final_text);

        string_free(&final_text);
        g_free(selected_text);
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


    if(gtk_text_buffer_get_selection_bounds (app_wdgts->e_txt_codebox_code, &start, &end)) {
        string_init(&final_text);

        selected_text = gtk_text_buffer_get_text(app_wdgts->e_txt_codebox_code, &start, &end, FALSE);

        gtk_text_buffer_delete_selection (app_wdgts->e_txt_codebox_code, FALSE, FALSE);

        tail_text = gtk_entry_get_text (app_wdgts->e_txt_codebox_tail);

        string_set(&final_text, selected_text);
        string_add(&final_text, " ");
        string_add(&final_text, tail_text);

        gtk_entry_set_text (app_wdgts->e_txt_codebox_tail, final_text);

        string_free(&final_text);
        g_free(selected_text);

    }
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

    app_widgets     *widgets = g_slice_new(app_widgets);

    gtk_init(&argc,&argv);

    pango_attr_list_insert(attrlist, attr);

    builder = gtk_builder_new ();
    if( gtk_builder_add_from_resource (builder,"/org/nete/ui/nete.gui",&error) == 0)
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
    widgets->e_txt_codebox_head = GTK_ENTRY(gtk_builder_get_object(builder, "txt_codebox_head"));
    widgets->e_txt_codebox_priority = GTK_ENTRY(gtk_builder_get_object(builder, "txt_codebox_priority"));
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

    gtk_entry_set_attributes(GTK_ENTRY(widgets->e_txt_codebox_name), attrlist);
    pango_attr_list_unref(attrlist);

    GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
    GtkSourceLanguage *language = gtk_source_language_manager_get_language(manager, "c");
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

    popup_separator2 = gtk_separator_menu_item_new ();
    gtk_widget_show(popup_separator2);
    gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), popup_separator2);

    g_signal_connect(G_OBJECT(pastenodeMi), "activate",
                     G_CALLBACK(nete_paste_node), widgets->drawingArea);
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

    gtk_main();
    g_slice_free(app_widgets, widgets);
    return EXIT_SUCCESS;
}
void redraw(cairo_t *cr) {
    RGB tempcolour;
    unsigned int i;
    tempcolour = colorConverter(0x222222);

    int *obscured_nodes;
    obscured_nodes = dynarray_create(int);

    node_mode_flags node_modes;

    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_rectangle(cr, 0, 0, screenWidth, screenHeight);
    cairo_fill(cr);

    drawGrid(cr);

    for (i = 0; i < (dynarray_length(current_nodegroup.node_group) ); i++) {
        if(current_nodegroup.node_group[i].cont_by == current_nodegroup.topNode) {
            if(!current_nodegroup.node_group[i].obscured) {
                node_modes.last_touched = false;
                node_modes.last_read = false;
                node_modes.libre = false;
                node_modes.obscured = false;
                node_modes.being_dragged = false;
                node_modes.being_resized = false;

                if(i == current_nodegroup.last_node_read) {
                    node_modes.last_read = true;
                }
                if(i == node_being_dragged) {
                    node_modes.being_dragged = true;
                }
                if(i == node_being_resized) {
                    node_modes.being_resized = true;
                }
                if(!no_node_selected && i == tempNodeZone.node) {
                    node_modes.last_touched = true;
                }
                drawNode(cr, i, 0, 0, 4095, 2047, current_nodegroup.zoomScaleFactor, node_modes);
                if (current_nodegroup.node_group[i].next != -1) {
                    draw_arc(cr, i, current_nodegroup.node_group[i].next, 0, 0, current_nodegroup.zoomScaleFactor);
                }
            } else {

                dynarray_push(obscured_nodes, i);
            }
        }
    }
    for (i = 0; i < dynarray_length(obscured_nodes); i++) {
        node_modes.last_touched = false;
        node_modes.last_read = false;
        node_modes.libre = false;
        node_modes.obscured = true;
        node_modes.being_dragged = false;
        node_modes.being_resized = false;

        if(obscured_nodes[i] == current_nodegroup.last_node_read) {
            node_modes.last_read = true;
        }
        if(obscured_nodes[i] == node_being_dragged) {
            node_modes.being_dragged = true;
        }
        if(obscured_nodes[i] == node_being_resized) {
            node_modes.being_resized = true;
        }
        if(!no_node_selected && obscured_nodes[i] == tempNodeZone.node) {
            node_modes.last_touched = true;
        }
        if(current_nodegroup.node_group[obscured_nodes[i]].x < 0 || current_nodegroup.node_group[obscured_nodes[i]].y < 0) {
            node_modes.libre = true;
        }
        drawNode(cr, obscured_nodes[i], 0, 0, 4095, 2047, current_nodegroup.zoomScaleFactor, node_modes);

        if (current_nodegroup.node_group[obscured_nodes[i]].next != -1) {
            draw_arc(cr, obscured_nodes[i], current_nodegroup.node_group[obscured_nodes[i]].next, 0, 0, current_nodegroup.zoomScaleFactor);
        }
    }
    dynarray_destroy(obscured_nodes);
}
void draw_arc(cairo_t *cr, int node1, int node2, int frame_x, int frame_y, float scale_factor) {
    RGB tempcolour;
    int arc_overhang = 10;
    tempcolour = colorConverter(0xcccccc);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_move_to (cr, frame_x + (current_nodegroup.node_group[node1].x + current_nodegroup.node_group[node1].width)*scale_factor + 2, frame_y + (current_nodegroup.node_group[node1].y + (node_head_height + node_spacer_height + node_descript_height + node_spacer_height) + current_nodegroup.node_group[node1].height / 2)*scale_factor);
    cairo_line_to (cr, frame_x + (current_nodegroup.node_group[node1].x + current_nodegroup.node_group[node1].width + arc_overhang)*scale_factor + 2, frame_y + (current_nodegroup.node_group[node1].y + (node_head_height + node_spacer_height + node_descript_height + node_spacer_height) + current_nodegroup.node_group[node1].height / 2)*scale_factor);
    cairo_line_to (cr, frame_x + (current_nodegroup.node_group[node2].x - arc_overhang)*scale_factor - 2, frame_y + (current_nodegroup.node_group[node2].y + (node_head_height + node_spacer_height + node_descript_height + node_spacer_height) + current_nodegroup.node_group[node2].height / 2)*scale_factor);
    cairo_line_to (cr, frame_x + current_nodegroup.node_group[node2].x*scale_factor - 2, frame_y + (current_nodegroup.node_group[node2].y + (node_head_height + node_spacer_height + node_descript_height + node_spacer_height) + current_nodegroup.node_group[node2].height / 2)*scale_factor);
    cairo_set_line_width (cr, 4);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke (cr);
    cairo_set_line_width (cr, 1);
}
void drawNode(cairo_t *cr, int node, int frame_x, int frame_y, __attribute__((unused)) int frame_width, __attribute__((unused)) int frame_height, float scale_factor, node_mode_flags node_modes) {
    RGB tempcolour;

    int contained_node;

    float x = current_nodegroup.node_group[node].x * scale_factor + frame_x;
    float y = current_nodegroup.node_group[node].y * scale_factor + frame_y;
    int height = current_nodegroup.node_group[node].height;
    int width = current_nodegroup.node_group[node].width;
    char name[128];
    strcpy(name, current_nodegroup.node_group[node].name);

    static const double dashed1[] = {5.0, 3.0};
    static int len1  = sizeof(dashed1) / sizeof(dashed1[0]);
    static const double dashnone[] = {};

    node_mode_flags inner_node_modes;

    float head_width  = node_head_width * scale_factor;
    float head_height  = node_head_height * scale_factor;
    float descript_height = node_descript_height * scale_factor;
    float descript_width = node_descript_width * scale_factor;
    float body_height = height * scale_factor;
    float body_width = width * scale_factor;
    float spacer  = node_spacer_height *scale_factor;
    float children_scale_factor = scale_factor * calculateScale(node, scale_factor, head_height +spacer + descript_height + spacer);
    int text_size = node_text_size;
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
    cairo_set_line_width (cr, 2);
    tempcolour = colorConverter(0x222222);
    cairo_set_source_rgba (cr, tempcolour.r, tempcolour.g, tempcolour.b, 0.5);
    cairo_rectangle(cr, x, y + head_height +spacer + descript_height + spacer, body_width + 2, body_height + 2);
    cairo_stroke(cr);
//====================================================

//========== body outline params ========================
    if(node_modes.last_read) {
        cairo_set_dash(cr, dashed1, len1, 0);
        cairo_set_line_width (cr, 3);
    }

//=============== body =======================================
    tempcolour = colorConverter(0x444444);
    cairo_set_source_rgba (cr, tempcolour.r, tempcolour.g, tempcolour.b, 0.75);
    cairo_rectangle(cr, x, y + head_height +spacer + descript_height + spacer, body_width, body_height);
    cairo_fill_preserve(cr);
    tempcolour = colorConverter(priority_colour[current_nodegroup.node_group[node].priority]);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_rectangle(cr, x, y + head_height +spacer + descript_height + spacer, body_width, body_height);
    cairo_stroke(cr);
    cairo_set_line_width (cr, 1);
    tempcolour = colorConverter(0x444444);
    cairo_set_dash(cr, dashnone, 0, 0);
//===================================================

//============  flanks =================================
    tempcolour = colorConverter(0x555555);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_rectangle(cr, x + 2, y + head_height +spacer + descript_height + spacer + 2, 13, body_height - 4);
    cairo_fill(cr);
    cairo_rectangle(cr, x + body_width - 15, y + head_height +spacer + descript_height + spacer + 2, 13, body_height - 4);
    cairo_fill(cr);
//========================================================

// ============ resize tab ======================================
    tempcolour = colorConverter(0x777777);

    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_rectangle(cr, x + body_width - 15*scale_factor, y + body_height + head_height +spacer + descript_height + spacer - 15*scale_factor, 15*scale_factor - 2, 15*scale_factor - 2);
// use the resize box without scaling instead
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

    if(current_nodegroup.node_group[node].width > 75 && current_nodegroup.node_group[node].height > 35) {
        tempcolour = colorConverter(0xcccccc);
        cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);

        cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size (cr, text_size);

        draw_text_in_box(cr, current_nodegroup.node_group[node].real_code, x + spacer, y + head_height+spacer, body_width, body_height);
    }
    tempcolour = colorConverter(0xffffff);

    cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, text_size);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);
    cairo_move_to (cr, x + spacer, y + spacer*8);
    cairo_show_text (cr, name);
    unsigned long int i = 0;

    int *obscured_nodes;
    obscured_nodes = dynarray_create(int);

    for (i = 0; i < dynarray_length(current_nodegroup.node_group[node].contained); i++) {
        contained_node = current_nodegroup.node_group[node].contained[i];
        if(!current_nodegroup.node_group[contained_node].obscured) {

            inner_node_modes.last_touched = false;
            inner_node_modes.last_read = false;
            inner_node_modes.libre = false;
            inner_node_modes.obscured = false;
            inner_node_modes.being_dragged = false;
            inner_node_modes.being_resized = false;

            if(contained_node == current_nodegroup.last_node_read) {
                inner_node_modes.last_read = true;
            }
            if(contained_node == node_being_dragged) {
                inner_node_modes.being_dragged = true;
            }
            if(contained_node == node_being_resized) {
                inner_node_modes.being_resized = true;
            }
            if(!no_node_selected && contained_node == tempNodeZone.node) {
                inner_node_modes.last_touched = true;
            }
            if(current_nodegroup.node_group[contained_node].x < 0 || current_nodegroup.node_group[contained_node].y < 0) {
                inner_node_modes.libre = true;
                draw_libre_arc(cr, contained_node,x, y+ head_height +spacer + descript_height + spacer, children_scale_factor);
            }
            drawNode(cr, contained_node, x, y+ head_height +spacer + descript_height + spacer, current_nodegroup.node_group[node].width, current_nodegroup.node_group[node].height, children_scale_factor, inner_node_modes);
            if (current_nodegroup.node_group[contained_node].next != -1) {
                draw_arc(cr, contained_node, current_nodegroup.node_group[contained_node].next, x, y+ head_height +spacer + descript_height + spacer, children_scale_factor);
            }
        } else {
            dynarray_push(obscured_nodes, contained_node);
        }
    }
    for (i = 0; i < dynarray_length(obscured_nodes); i++) {
        inner_node_modes.last_touched = false;
        inner_node_modes.last_read = false;
        inner_node_modes.libre = false;
        inner_node_modes.obscured = true;
        inner_node_modes.being_dragged = false;
        inner_node_modes.being_resized = false;

        if(obscured_nodes[i] == current_nodegroup.last_node_read) {
            inner_node_modes.last_read = true;
        }
        if(obscured_nodes[i] == node_being_dragged) {
            inner_node_modes.being_dragged = true;
        }
        if(obscured_nodes[i] == node_being_resized) {
            inner_node_modes.being_resized = true;
        }
        if(!no_node_selected && obscured_nodes[i] == tempNodeZone.node) {
            inner_node_modes.last_touched = true;
        }
        if(current_nodegroup.node_group[obscured_nodes[i]].x < 0 || current_nodegroup.node_group[obscured_nodes[i]].y < 0) {
            inner_node_modes.libre = true;
            draw_libre_arc(cr, obscured_nodes[i],x, y+ head_height +spacer + descript_height + spacer, children_scale_factor);
        }
        drawNode(cr, obscured_nodes[i], x, y+ head_height +spacer + descript_height + spacer, current_nodegroup.node_group[node].width, current_nodegroup.node_group[node].height, children_scale_factor, inner_node_modes);
        if (current_nodegroup.node_group[obscured_nodes[i]].next != -1) {
            draw_arc(cr, obscured_nodes[i], current_nodegroup.node_group[obscured_nodes[i]].next, x, y+ head_height +spacer + descript_height + spacer, children_scale_factor);
        }
    }
    dynarray_destroy(obscured_nodes);
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
        // printf("%s\n", oneline);
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
void draw_libre_arc(cairo_t *cr, int node, int frame_x, int frame_y, float scale_factor) {
    RGB tempcolour;

    tempcolour = colorConverter(0x339933);
    cairo_set_source_rgb (cr, tempcolour.r, tempcolour.g, tempcolour.b);

    cairo_move_to(cr, frame_x + (current_nodegroup.node_group[node].x + current_nodegroup.node_group[node].width)*scale_factor + 2, frame_y + (current_nodegroup.node_group[node].y + (node_head_height + node_spacer_height + node_descript_height + node_spacer_height) + current_nodegroup.node_group[node].height / 2)*scale_factor);
    cairo_line_to(cr, frame_x, frame_y );

    cairo_set_line_width (cr, 1);
    cairo_stroke (cr);
}
void doMouseDown2(int x, int y, __attribute__((unused)) gpointer data) {
    mousePressed = true;

    int* primary_nodes;
    a_nodezoneinfo newNodeZone;

    primary_nodes = getPrimaryNodes();

    if(no_node_selected) {
        getNodeAndZone(&tempNodeZone, primary_nodes, x, y, 0, 0, current_nodegroup.zoomScaleFactor, -1);
        if(tempNodeZone.node != -1) {
            if(tempNodeZone.zone == head_zone) {
                node_being_dragged = tempNodeZone.node;
            }
            if(tempNodeZone.zone == resize_zone) {
                node_being_resized = tempNodeZone.node;
            }
        }
        no_node_selected = false;
    } else {
        switch(toolmode) {
        case EXITENTER:
            getNodeAndZone(&newNodeZone, primary_nodes, x, y, 0, 0, current_nodegroup.zoomScaleFactor, tempNodeZone.node);
            recontain_node(&tempNodeZone, &newNodeZone, x, y);
            check_obscuring(tempNodeZone.node);
            check_obscured(tempNodeZone.node);
            no_node_selected = true;
            break;
        case JOIN:
            getNodeAndZone(&newNodeZone, primary_nodes, x, y, 0, 0, current_nodegroup.zoomScaleFactor, tempNodeZone.node);
            if(newNodeZone.node != -1) {
                join_nodes(&tempNodeZone, &newNodeZone);
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

    if(toolmode == MOVE) {
        if (mousePressed && tempNodeZone.zone == 0) {

            xDifference = snapGrid((x / tempNodeZone.scale_factor))*tempNodeZone.scale_factor - tempNodeZone.x;
            yDifference = snapGrid((y / tempNodeZone.scale_factor))*tempNodeZone.scale_factor - tempNodeZone.y;

            current_nodegroup.node_group[tempNodeZone.node].x = snapGrid(tempNodeZone.node_x + xDifference/tempNodeZone.scale_factor);
            current_nodegroup.node_group[tempNodeZone.node].y = snapGrid(tempNodeZone.node_y + yDifference/tempNodeZone.scale_factor);
            no_node_selected = true;
        }

        if (mousePressed && tempNodeZone.zone == 4) {
            xDifference = snapGrid((x / tempNodeZone.scale_factor))*tempNodeZone.scale_factor - tempNodeZone.x;
            yDifference = snapGrid((y / tempNodeZone.scale_factor))*tempNodeZone.scale_factor - tempNodeZone.y;

            current_nodegroup.node_group[tempNodeZone.node].width = snapGrid(tempNodeZone.node_width + xDifference/tempNodeZone.scale_factor);
            current_nodegroup.node_group[tempNodeZone.node].height = snapGrid(tempNodeZone.node_height + yDifference/tempNodeZone.scale_factor);
            no_node_selected = true;
        }
    }
}
void doDoubleClick(int x, int y, __attribute__((unused)) gpointer data, app_widgets *app_wdgts) {
    int* primary_nodes;

    primary_nodes = getPrimaryNodes();

    mousePressed = false;

    getNodeAndZone(&tempNodeZone, primary_nodes, x, y, 0, 0, current_nodegroup.zoomScaleFactor, -1);

    if (tempNodeZone.node != -1 && tempNodeZone.zone == 0) {
        current_nodegroup.last_node_read = tempNodeZone.node;

        launchBox(tempNodeZone.node, 0,  app_wdgts);
    }
}
void doMouseUp2(int x, int y, __attribute__((unused)) gpointer data) {
    int* primary_nodes;
    a_nodezoneinfo newNodeZone;

    if (tempNodeZone.zone == 0) {
        primary_nodes = getPrimaryNodes();

        getNodeAndZone(&newNodeZone, primary_nodes, x, y, 0, 0, current_nodegroup.zoomScaleFactor, tempNodeZone.node);

        dynarray_destroy(primary_nodes);

        if(toolmode == MOVE) {
            check_obscured(tempNodeZone.node);
            check_obscuring(tempNodeZone.node);
        }
        if ((newNodeZone.node != current_nodegroup.topNode && (newNodeZone.zone == 1 || newNodeZone.zone == 2 || newNodeZone.zone == 3 ) && newNodeZone.node != tempNodeZone.node) || newNodeZone.node == current_nodegroup.topNode ) {
        }
    }
    if (tempNodeZone.zone == 4) {
        if(toolmode == MOVE) {
            check_obscuring(tempNodeZone.node);
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
    node_being_dragged = -1;
    node_being_resized = -1;

    mousePressed = false;
    nodeHeadFound = false;
    nodeBodyFlankFound = false;
    nodeWidthAdjust = false;
}
int * getPrimaryNodes() {
    int* return_nodes = dynarray_create(int);
    unsigned int i;
    for(i = 0; i < dynarray_length(current_nodegroup.node_group); i++) {
        if(current_nodegroup.node_group[i].cont_by == current_nodegroup.topNode) {
            dynarray_push(return_nodes, i);
        }
    }
    return return_nodes;
}
float calculateScale(int node, __attribute__((unused)) int y_displacement, float scale_factor) {
    unsigned long int i;
    int max_x = 0;
    int max_y = 0;
    int current_x;
    int current_y;
    int current_node;
    float node_max_x = current_nodegroup.node_group[node].width * scale_factor - 17 * scale_factor;
    float node_max_y = current_nodegroup.node_group[node].height * scale_factor  - 5*scale_factor;
    float scale_x = 1;
    float scale_y = 1;

    for(i = 0; i < dynarray_length(current_nodegroup.node_group[node].contained); i++) {
        current_node = current_nodegroup.node_group[node].contained[i];
        current_x = (current_nodegroup.node_group[current_node].x + current_nodegroup.node_group[current_node].width) * scale_factor;
        current_y = (current_nodegroup.node_group[current_node].y + current_nodegroup.node_group[current_node].height + (node_head_height +node_spacer_height + node_descript_height + node_spacer_height)) * scale_factor;
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
void  getNodeAndZone(a_nodezoneinfo *return_nodezone, int *nodes, int x, int y, int frame_x, int frame_y, float scale_factor, int omit) {
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

    for(obsc_ind = 0; obsc_ind < 2; obsc_ind++) {
        for(i = 0; i < dynarray_length(nodes); i++) {
            current_node = nodes[i];
            if((obsc_ind == 0 && current_nodegroup.node_group[current_node].obscured) || (obsc_ind == 1 && !current_nodegroup.node_group[current_node].obscured)) {
                head_height  = node_head_height * scale_factor;
                descript_height = node_descript_height * scale_factor;
                spacer  = node_spacer_height *scale_factor;

                children_scale_factor = scale_factor * calculateScale(current_node, scale_factor, head_height +spacer + descript_height + spacer);

                if(dynarray_length(current_nodegroup.node_group[current_node].contained) != 0) {
                    getNodeAndZone(return_nodezone, (current_nodegroup.node_group[current_node].contained), x, y, current_nodegroup.node_group[current_node].x * scale_factor + frame_x, current_nodegroup.node_group[current_node].y * scale_factor + frame_y +  ( head_height +spacer + descript_height + spacer), children_scale_factor, omit);
                    if(return_nodezone->node != current_nodegroup.topNode)
                        return;
                }
                fixZones(current_node);
                for(j = 0; j < node_zones.length; j++) {
                    x_min = frame_x + (current_nodegroup.node_group[current_node].x+node_zones.array[j].x) * scale_factor;
                    x_max = frame_x + (current_nodegroup.node_group[current_node].x+node_zones.array[j].x + node_zones.array[j].width) * scale_factor;
                    y_min = frame_y + (current_nodegroup.node_group[current_node].y+node_zones.array[j].y) * scale_factor;
                    y_max = frame_y + (current_nodegroup.node_group[current_node].y+node_zones.array[j].y + node_zones.array[j].height) * scale_factor;
                    if(current_node != omit && x > x_min && x < x_max && y > y_min && y < y_max) {
                        return_nodezone->node = current_node;
                        return_nodezone->zone = j;
                        return_nodezone->scale_factor = scale_factor;
                        return_nodezone->x = x;
                        return_nodezone->y = y;
                        return_nodezone->node_x = current_nodegroup.node_group[current_node].x;
                        return_nodezone->node_y = current_nodegroup.node_group[current_node].y;
                        return_nodezone->node_width = current_nodegroup.node_group[current_node].width;
                        return_nodezone->node_height = current_nodegroup.node_group[current_node].height;
                        return_nodezone->frame_x = frame_x;
                        return_nodezone->frame_y = frame_y;

                        return;
                    }
                }
            }
        }
    }
    return_nodezone->node = current_nodegroup.topNode;
    return_nodezone->zone = body_zone;
    return_nodezone->scale_factor = current_nodegroup.zoomScaleFactor;
    return_nodezone->x = 0;
    return_nodezone->y = 0;
    return_nodezone->node_x = 0;
    return_nodezone->node_y = 0;
    return_nodezone->node_width = 0;
    return_nodezone->node_height = 0;
    return_nodezone->frame_x = 0;
    return_nodezone->frame_y = 0;
}
void fixZones(int node) {
    node_zones.array[body_zone].x = node_flank_width;
    node_zones.array[body_zone].y = node_head_height + node_spacer_height + node_descript_height + node_spacer_height;
    node_zones.array[body_zone].width = current_nodegroup.node_group[node].width - node_flank_width * 2;
    node_zones.array[body_zone].height = current_nodegroup.node_group[node].height;

    node_zones.array[flank1_zone].x = 0;
    node_zones.array[flank1_zone].y =  node_head_height + node_spacer_height + node_descript_height + node_spacer_height;
    node_zones.array[flank1_zone].width = node_flank_width;
    node_zones.array[flank1_zone].height = current_nodegroup.node_group[node].height;

    node_zones.array[flank2_zone].x = current_nodegroup.node_group[node].width - node_flank_width;
    node_zones.array[flank2_zone].y = node_head_height + node_spacer_height + node_descript_height + node_spacer_height;
    node_zones.array[flank2_zone].width = node_flank_width;
    node_zones.array[flank2_zone].height = current_nodegroup.node_group[node].height - node_resize_width;

    node_zones.array[resize_zone].x = current_nodegroup.node_group[node].width - node_resize_width;
    node_zones.array[resize_zone].y = node_head_height + node_spacer_height + node_descript_height + node_spacer_height + current_nodegroup.node_group[node].height - node_resize_width;
    node_zones.array[resize_zone].width = node_resize_width;
    node_zones.array[resize_zone].height = node_resize_width;
}
bool check_not_descendent(int main_node, int desc_node) {
    unsigned int i = 0;
    for (i = 0; i < dynarray_length(current_nodegroup.node_group[main_node].contained); i++) {
        if(current_nodegroup.node_group[main_node].contained[i] == desc_node)
            return false;

        if(!check_not_descendent(current_nodegroup.node_group[main_node].contained[i], desc_node))
            return false;
    }
    return true;
}
bool check_not_circular(int main_node, int susp_node) {
    int tempNode = current_nodegroup.node_group[main_node].previous;

    while (tempNode != -1) {
        if(tempNode == susp_node)
            return false;

        if(!check_not_descendent(tempNode, susp_node))
            return false;

        tempNode = current_nodegroup.node_group[tempNode].previous;
    }
    tempNode = current_nodegroup.node_group[main_node].next;

    while (tempNode != -1) {
        if(tempNode == susp_node)
            return false;

        if(!check_not_descendent(tempNode, susp_node))
            return false;

        tempNode = current_nodegroup.node_group[tempNode].next;
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
void get_absolutePosition(a_container_pos *container_pos, int node) {
    float node_x = current_nodegroup.node_group[node].x;
    float node_y = current_nodegroup.node_group[node].y;
    float current_scale;
    a_container_pos upper_position;

    if(current_nodegroup.node_group[node].cont_by == current_nodegroup.topNode) {
        container_pos->x = (node_x*current_nodegroup.zoomScaleFactor);
        container_pos->y = (node_y + node_head_height + node_spacer_height)*current_nodegroup.zoomScaleFactor;
        container_pos->scaleFactor = current_nodegroup.zoomScaleFactor;
    } else {
        get_absolutePosition(&upper_position, current_nodegroup.node_group[node].cont_by);
        current_scale = calculateScale(current_nodegroup.node_group[node].cont_by, 0, upper_position.scaleFactor) * upper_position.scaleFactor;

        container_pos->x = ((node_x*current_scale) + upper_position.x);
        container_pos->y = (((node_y )*current_scale) + (node_head_height + node_spacer_height)*upper_position.scaleFactor + upper_position.y );
        container_pos->scaleFactor = current_scale;
    }
}
void launchBox(int node, int line_start, app_widgets *app_wdgts) {
    char num_buf[3];
    int codebox_width, codebox_height;

    gtk_entry_set_text (app_wdgts->e_txt_codebox_name, current_nodegroup.node_group[node].name);
    gtk_entry_set_text (app_wdgts->e_txt_codebox_description, current_nodegroup.node_group[node].description);
    gtk_entry_set_text (app_wdgts->e_txt_codebox_head, current_nodegroup.node_group[node].head);
    sprintf(num_buf, "%d", current_nodegroup.node_group[node].priority);
    gtk_entry_set_text (app_wdgts->e_txt_codebox_priority, num_buf);
    gtk_text_buffer_set_text(app_wdgts->e_txt_codebox_code, current_nodegroup.node_group[node].real_code, -1);
    gtk_entry_set_text (app_wdgts->e_txt_codebox_tail, current_nodegroup.node_group[node].tail);

    node_being_edited = node;
    node_group_being_edited = current_codeBox;

    gtk_window_present (app_wdgts->w_dlg_codebox);
    gtk_window_get_size (app_wdgts->w_dlg_codebox, &codebox_width, &codebox_height);

    if(first_time_codebox) {
        gtk_window_move (app_wdgts->w_dlg_codebox, gdk_screen_width() - codebox_width, 10);
    } else {
        gtk_window_move (app_wdgts->w_dlg_codebox, codeboxWindowPos.x, codeboxWindowPos.y);
    }
    node_displacement_count = 1;
}
void renderData() {
    clearRenderedData();
    string_set(&output_text, "");
    int node_priority;
    unsigned long int i;
    int p;

    for(p = 0; p <= 9; p++) {
        for (i = 0; i < dynarray_length(current_nodegroup.node_group); i++) {
            node_priority = current_nodegroup.node_group[i].priority;
            if(node_priority == -1) {
                node_priority = 5;
            }
            if (current_nodegroup.node_group[i].cont_by == -1 && current_nodegroup.node_group[i].previous == -1 && node_priority == p) {
                if(strlen(current_nodegroup.node_group[i].head) > 0) {
                    string_add(&output_text, current_nodegroup.node_group[i].head);
                    string_add(&output_text, "\r\n");
                }
                if (dynarray_length(current_nodegroup.node_group[i].contained) == 0) {
                    dynarray_push(rendered_data, i);
                    if(strlen(current_nodegroup.node_group[i].real_code) > 0) {
                        string_add(&output_text, current_nodegroup.node_group[i].real_code);
                        string_add(&output_text, "\r\n");
                    }
                } else {
                    renderDataNodes(current_nodegroup.node_group[i].contained);
                }
                if(strlen(current_nodegroup.node_group[i].tail) > 0) {
                    string_add(&output_text, current_nodegroup.node_group[i].tail);
                    string_add(&output_text, "\r\n");
                }
                if (current_nodegroup.node_group[i].next != -1) {
                    renderANode(current_nodegroup.node_group[i].next);
                }
            }
        }
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
void renderDataNodes(int* nodes) {
    int node_priority;
    int p;
    unsigned long int j;
    for(p = 0; p <= 9; p++) {
        for (j = 0; j < dynarray_length(nodes); j++) {
            node_priority = current_nodegroup.node_group[nodes[j]].priority;
            if(node_priority == -1)
                node_priority = 5;
            if (current_nodegroup.node_group[nodes[j]].previous == -1 && node_priority == p) {
                if(strlen(current_nodegroup.node_group[nodes[j]].head) > 0) {
                    string_add(&output_text, current_nodegroup.node_group[nodes[j]].head);
                    string_add(&output_text, "\r\n");
                }
                if (dynarray_length(current_nodegroup.node_group[nodes[j]].contained) == 0) {
                    dynarray_push(rendered_data, nodes[j]);
                    if(strlen(current_nodegroup.node_group[nodes[j]].real_code) > 0) {
                        string_add(&output_text, current_nodegroup.node_group[nodes[j]].real_code);
                        string_add(&output_text, "\r\n");
                    }
                } else {
                    renderDataNodes(current_nodegroup.node_group[nodes[j]].contained);
                }
                if(strlen(current_nodegroup.node_group[nodes[j]].tail) > 0) {
                    string_add(&output_text, current_nodegroup.node_group[nodes[j]].tail);
                    string_add(&output_text, "\r\n");
                }
                if (current_nodegroup.node_group[nodes[j]].next != -1) {
                    renderANode(current_nodegroup.node_group[nodes[j]].next);
                }
            }
        }
    }
}
void renderANode(int node) {
    if(strlen(current_nodegroup.node_group[node].head) > 0) {
        string_add(&output_text, current_nodegroup.node_group[node].head);
        string_add(&output_text, "\r\n");
    }
    if (dynarray_length(current_nodegroup.node_group[node].contained) == 0) {
        if(strlen(current_nodegroup.node_group[node].real_code) > 0) {
            dynarray_push(rendered_data, node);
            string_add(&output_text, current_nodegroup.node_group[node].real_code);
            string_add(&output_text, "\r\n");
        }
    } else {
        renderDataNodes(current_nodegroup.node_group[node].contained);
    }
    if(strlen(current_nodegroup.node_group[node].tail) > 0) {
        string_add(&output_text, current_nodegroup.node_group[node].tail);
        string_add(&output_text, "\r\n");
    }
    if (current_nodegroup.node_group[node].next != -1) {
        renderANode(current_nodegroup.node_group[node].next);
    }
}
void renderLineData() {
    clearRenderedData();

    int node_priority;
    line_count = 1;
    unsigned int i;
    int p;

    for(p = 0; p <= 9; p++) {
        for (i = 0; i < dynarray_length(current_nodegroup.node_group); i++) {
            node_priority = current_nodegroup.node_group[i].priority;
            if(node_priority == -1) {
                node_priority = 5;
            }
            if (current_nodegroup.node_group[i].cont_by == -1 && current_nodegroup.node_group[i].previous == -1 && node_priority == p) {
                if(strlen(current_nodegroup.node_group[i].head) > 0) {
                    dynarray_push(rendered_data, i);
                    dynarray_push(rendered_counts, line_count);
                    line_count++;
                }
                if (dynarray_length(current_nodegroup.node_group[i].contained) == 0) {
                    if(strlen(current_nodegroup.node_group[i].real_code) > 0) {
                        dynarray_push(rendered_data, i);
                        dynarray_push(rendered_counts, line_count);
                        line_count += count_string_lines(current_nodegroup.node_group[i].real_code);
                    }
                } else {
                    renderLineDataNodes(current_nodegroup.node_group[i].contained);
                }
                if(strlen(current_nodegroup.node_group[i].tail) > 0) {
                    dynarray_push(rendered_data, i);
                    dynarray_push(rendered_counts, line_count);
                    line_count++;
                }
                if (current_nodegroup.node_group[i].next != -1) {
                    renderLineANode(current_nodegroup.node_group[i].next);
                }
            }
        }
    }
}
void clearRenderedLineData() {
    int temp_id = 0;
    while (dynarray_length(rendered_data) > 0) {
        dynarray_pop(rendered_data, &temp_id);
    }
    while (dynarray_length(rendered_counts) > 0) {
        dynarray_pop(rendered_counts, &temp_id);
    }
}
void renderLineDataNodes(int* nodes) {
    int node_priority;
    unsigned int j;
    int p;
    for(p = 0; p <= 9; p++) {
        for (j = 0; j < dynarray_length(nodes); j++) {
            node_priority = current_nodegroup.node_group[nodes[j]].priority;
            if(node_priority == -1)
                node_priority = 5;
            if (current_nodegroup.node_group[nodes[j]].previous == -1 && node_priority == p) {
                if(strlen(current_nodegroup.node_group[nodes[j]].head) > 0) {
                    dynarray_push(rendered_data, nodes[j]);
                    dynarray_push(rendered_counts, line_count);
                    line_count++;
                }
                if (dynarray_length(current_nodegroup.node_group[nodes[j]].contained) == 0) {
                    if(strlen(current_nodegroup.node_group[nodes[j]].real_code) > 0) {
                        dynarray_push(rendered_data, nodes[j]);
                        dynarray_push(rendered_counts, line_count);
                        line_count += count_string_lines(current_nodegroup.node_group[nodes[j]].real_code);
                    }
                } else {
                    renderLineDataNodes(current_nodegroup.node_group[nodes[j]].contained);
                }
                if(strlen(current_nodegroup.node_group[nodes[j]].tail) > 0) {
                    dynarray_push(rendered_data, nodes[j]);
                    dynarray_push(rendered_counts, line_count);
                    line_count++;
                }
                if (current_nodegroup.node_group[nodes[j]].next != -1) {
                    renderLineANode(current_nodegroup.node_group[nodes[j]].next);
                }
            }
        }
    }
}
void renderLineANode(int node) {
    if(strlen(current_nodegroup.node_group[node].head) > 0) {
        dynarray_push(rendered_data, node);
        dynarray_push(rendered_counts, line_count);
        line_count ++;
    }
    if (dynarray_length(current_nodegroup.node_group[node].contained) == 0) {
        if(strlen(current_nodegroup.node_group[node].real_code) > 0) {
            dynarray_push(rendered_data, node);
            dynarray_push(rendered_counts, line_count);
            line_count += count_string_lines(current_nodegroup.node_group[node].real_code);
        }
    } else {
        renderLineDataNodes(current_nodegroup.node_group[node].contained);
    }
    if(strlen(current_nodegroup.node_group[node].tail) > 0) {
        dynarray_push(rendered_data, node);
        dynarray_push(rendered_counts, line_count);
        line_count ++;
    }
    if (current_nodegroup.node_group[node].next != -1) {
        renderLineANode(current_nodegroup.node_group[node].next);
    }
}
int pasteNode(int parent, int node, int source_codeBox) {
    int newID;
    unsigned int i;
    int *temp_contained;
    a_translate_vector temp_translate;

    float x = codeBox_list[source_codeBox].node_group[node].x;
    float y = codeBox_list[source_codeBox].node_group[node].y;

    if(parent == current_nodegroup.topNode) {
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

    temp_contained = dynarray_create(int);

    newID = node_group_push(
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
                0
            );

    temp_translate.original = node;
    temp_translate.newone = newID;
    dynarray_push(node_translation, temp_translate);

    if(current_nodegroup.topNode != -1 && parent == current_nodegroup.topNode) {
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
bool recontain_node(a_nodezoneinfo* firstNodeZone, a_nodezoneinfo* secondNodeZone, int x, int y) {
    int tempNode;
    int old_tempNode;

    int x_difference;
    int y_difference;

    float better_scale;
    a_container_pos container_pos;
    container_pos.x = 0;
    container_pos.y = 0;
    container_pos.scaleFactor = current_nodegroup.zoomScaleFactor;
    if (current_nodegroup.node_group[firstNodeZone->node].cont_by != secondNodeZone->node  && check_not_descendent(firstNodeZone->node, secondNodeZone->node) && check_not_circular(firstNodeZone->node, secondNodeZone->node)) {
        if(current_nodegroup.node_group[firstNodeZone->node].cont_by != -1) {
            containerRemoveNode(current_nodegroup.node_group[firstNodeZone->node].cont_by, firstNodeZone->node);
        }
//we add the current node to the node body we've found
        current_nodegroup.node_group[firstNodeZone->node].cont_by = secondNodeZone->node;

        better_scale = current_nodegroup.zoomScaleFactor;

        container_pos.x = 0;
        container_pos.y = 0;
        container_pos.scaleFactor = current_nodegroup.zoomScaleFactor;

        if(secondNodeZone->node != current_nodegroup.topNode) {
            get_absolutePosition(&container_pos, secondNodeZone->node);
            better_scale = calculateScale(secondNodeZone->node, 0, container_pos.scaleFactor) * container_pos.scaleFactor;

        }

        if(secondNodeZone->node != -1) {
            dynarray_push(current_nodegroup.node_group[secondNodeZone->node].contained, firstNodeZone->node);
        }
//we set the coordinates of the newly added node in proportion to the size of node they're going into

        current_nodegroup.node_group[firstNodeZone->node].x = (x - container_pos.x) / better_scale;
        current_nodegroup.node_group[firstNodeZone->node].y = (y - container_pos.y) / better_scale;

//we follow up with all the previous nodes in the list
        tempNode = current_nodegroup.node_group[firstNodeZone->node].previous;

        while (tempNode != -1) {
            if(current_nodegroup.node_group[tempNode].cont_by != -1) {
                containerRemoveNode(current_nodegroup.node_group[tempNode].cont_by, tempNode);
            }

            if(secondNodeZone->node != -1) {
                dynarray_push(current_nodegroup.node_group[secondNodeZone->node].contained, tempNode);
            }
            current_nodegroup.node_group[tempNode].cont_by = secondNodeZone->node;

            x_difference = current_nodegroup.node_group[tempNode].x - firstNodeZone->node_x;
            y_difference = current_nodegroup.node_group[tempNode].y - firstNodeZone->node_y;

            current_nodegroup.node_group[tempNode].x = (x - container_pos.x +x_difference) / better_scale;
            current_nodegroup.node_group[tempNode].y = (y - container_pos.y +y_difference) / better_scale;

            old_tempNode = tempNode;
            tempNode = current_nodegroup.node_group[tempNode].previous;
        }
        if(secondNodeZone->node != current_nodegroup.topNode) {
            if (old_tempNode != current_nodegroup.topNode && current_nodegroup.node_group[secondNodeZone->node].cont_head != -1) {
                current_nodegroup.node_group[secondNodeZone->node].cont_head = old_tempNode;
            }
        }
        tempNode = current_nodegroup.node_group[firstNodeZone->node].next;
        while (tempNode != -1) {
            if(current_nodegroup.node_group[tempNode].cont_by != -1) {
                containerRemoveNode(current_nodegroup.node_group[tempNode].cont_by, tempNode);
            }
            if(secondNodeZone->node != -1) {
                dynarray_push(current_nodegroup.node_group[secondNodeZone->node].contained, tempNode);
            }
            current_nodegroup.node_group[tempNode].cont_by = secondNodeZone->node;

            x_difference = current_nodegroup.node_group[tempNode].x - firstNodeZone->node_x;
            y_difference = current_nodegroup.node_group[tempNode].y - firstNodeZone->node_y;

            current_nodegroup.node_group[tempNode].x = (x - container_pos.x +x_difference) / better_scale;
            current_nodegroup.node_group[tempNode].y = (y - container_pos.y +y_difference) / better_scale;

            tempNode = current_nodegroup.node_group[tempNode].next;
        }
        deactivate_quit();

        return true;
    }
    return true;
}
bool join_nodes(a_nodezoneinfo* firstNodeZone, a_nodezoneinfo* secondNodeZone) {
    if (firstNodeZone->zone == 1 || firstNodeZone->zone == 2 || firstNodeZone->zone == 3) {
        if (firstNodeZone->node != current_nodegroup.topNode && (secondNodeZone->zone == 1 || secondNodeZone->zone == 2 || secondNodeZone->zone == 3) && secondNodeZone->node != firstNodeZone->node) {
            if(current_nodegroup.node_group[firstNodeZone->node].cont_by == current_nodegroup.node_group[secondNodeZone->node].cont_by ) {
                if(current_nodegroup.node_group[firstNodeZone->node].next == -1 && current_nodegroup.node_group[secondNodeZone->node].previous == -1) {
                    current_nodegroup.node_group[firstNodeZone->node].next = secondNodeZone->node;
                    current_nodegroup.node_group[secondNodeZone->node].previous = firstNodeZone->node;
                } else if (current_nodegroup.node_group[firstNodeZone->node].next == secondNodeZone->node) {
                    current_nodegroup.node_group[firstNodeZone->node].next = -1;
                    current_nodegroup.node_group[secondNodeZone->node].previous = -1;
                }
                deactivate_quit();
            }
        }
    }
    return true;
}
void check_obscured(int node) {
    int i;
    int contained_node;
    current_nodegroup.node_group[node].obscured = false;

    clear_obscured_by(node);

    if(current_nodegroup.node_group[node].cont_by == -1) {
        for(i = 0; i < dynarray_length(current_nodegroup.node_group); i++) {
            if(current_nodegroup.node_group[i].cont_by == -1) {
                if( (current_nodegroup.node_group[node].x + node_head_width/2) > current_nodegroup.node_group[i].x &&
                        current_nodegroup.node_group[node].x < (current_nodegroup.node_group[i].x + current_nodegroup.node_group[i].width) &&
                        current_nodegroup.node_group[node].y > current_nodegroup.node_group[i].y &&
                        current_nodegroup.node_group[node].y < (current_nodegroup.node_group[i].y + current_nodegroup.node_group[i].height + node_head_height)
                  ) {
                    current_nodegroup.node_group[node].obscured = true;
                    dynarray_push(current_nodegroup.node_group[node].obscured_by, i);
                    dynarray_push(current_nodegroup.node_group[i].obscuring, node);
                }
            }
        }
    } else {
        for(i = 0; i < dynarray_length(current_nodegroup.node_group[current_nodegroup.node_group[node].cont_by].contained); i++) {
            contained_node = current_nodegroup.node_group[current_nodegroup.node_group[node].cont_by].contained[i];
            if( (current_nodegroup.node_group[node].x + node_head_width/2) > current_nodegroup.node_group[contained_node].x &&
                    current_nodegroup.node_group[node].x < (current_nodegroup.node_group[contained_node].x + current_nodegroup.node_group[contained_node].width) &&
                    current_nodegroup.node_group[node].y > current_nodegroup.node_group[contained_node].y &&
                    current_nodegroup.node_group[node].y < (current_nodegroup.node_group[contained_node].y + current_nodegroup.node_group[contained_node].height + node_head_height)
              ) {
                current_nodegroup.node_group[node].obscured = true;
                dynarray_push(current_nodegroup.node_group[node].obscured_by, contained_node);
                dynarray_push(current_nodegroup.node_group[contained_node].obscuring, node);
            }
        }
    }
}
void clear_obscured_by(int node) {
    int i;
    int j;
    int obscuring_node;
    int temp_node;

    for(i = 0; i < dynarray_length(current_nodegroup.node_group[node].obscured_by); i++) {
        obscuring_node = current_nodegroup.node_group[node].obscured_by[i];

        splice_out_node(current_nodegroup.node_group[obscuring_node].obscuring, node);
    }
    while(dynarray_length(current_nodegroup.node_group[node].obscured_by) > 0) {
        dynarray_pop(current_nodegroup.node_group[node].obscured_by, &temp_node);
    }
}
void check_obscuring(int node) {
    int i;
    int contained_node;

    clear_obscuring(node);

    if(current_nodegroup.node_group[node].cont_by == -1) {
        for(i = 0; i < dynarray_length(current_nodegroup.node_group); i++) {
            if(current_nodegroup.node_group[i].cont_by == -1) {
                if( (current_nodegroup.node_group[i].x + node_head_width/2) > current_nodegroup.node_group[node].x &&
                        current_nodegroup.node_group[i].x < (current_nodegroup.node_group[node].x + current_nodegroup.node_group[node].width) &&
                        current_nodegroup.node_group[i].y > current_nodegroup.node_group[node].y &&
                        current_nodegroup.node_group[i].y < (current_nodegroup.node_group[node].y + current_nodegroup.node_group[node].height + node_head_height)
                  ) {
                    current_nodegroup.node_group[i].obscured = true;
                    dynarray_push(current_nodegroup.node_group[i].obscured_by, node);
                    dynarray_push(current_nodegroup.node_group[node].obscuring, i);
                }
            }
        }
    } else {
        for(i = 0; i < dynarray_length(current_nodegroup.node_group[current_nodegroup.node_group[node].cont_by].contained); i++) {
            contained_node = current_nodegroup.node_group[current_nodegroup.node_group[node].cont_by].contained[i];
            if( (current_nodegroup.node_group[contained_node].x + node_head_width/2) > current_nodegroup.node_group[node].x &&
                    current_nodegroup.node_group[contained_node].x < (current_nodegroup.node_group[node].x + current_nodegroup.node_group[node].width) &&
                    current_nodegroup.node_group[contained_node].y > current_nodegroup.node_group[node].y &&
                    current_nodegroup.node_group[contained_node].y < (current_nodegroup.node_group[node].y + current_nodegroup.node_group[node].height + node_head_height)
              ) {
                current_nodegroup.node_group[contained_node].obscured = true;
                dynarray_push(current_nodegroup.node_group[contained_node].obscured_by, node);
                dynarray_push(current_nodegroup.node_group[node].obscuring, contained_node);
            }
        }
    }
}
void clear_obscuring(int node) {
    int i;
    int j;
    int obscured_node;
    int temp_node;

    for(i = 0; i < dynarray_length(current_nodegroup.node_group[node].obscuring); i++) {
        obscured_node = current_nodegroup.node_group[node].obscuring[i];

        splice_out_node(current_nodegroup.node_group[obscured_node].obscured_by, node);

        if(dynarray_length(current_nodegroup.node_group[obscured_node].obscured_by) == 0) {
            current_nodegroup.node_group[obscured_node].obscured = false;
        }
    }

    while(dynarray_length(current_nodegroup.node_group[node].obscuring) > 0) {
        dynarray_pop(current_nodegroup.node_group[node].obscuring, &temp_node);
    }
}
int node_group_push(int x, int y, int width, int height, int next, int previous, int cont_head, int cont_by, int priority, int* contained, char *name, char *description, char *head, char *tail, char *real_code, int node_id) {
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

    dynarray_push(current_nodegroup.node_group, temp_node);

    node_pos = dynarray_length(current_nodegroup.node_group) - 1;

    current_nodegroup.node_group[node_pos].node_id = node_pos;
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
int node_group_push_id(int x, int y, int width, int height, int next, int previous, int cont_head, int cont_by, int priority, int* contained, char *name, char *description, char *head, char *tail, char *real_code, int node_id) {
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
    dynarray_push(current_nodegroup.node_group, temp_node);
    node_pos = dynarray_length(current_nodegroup.node_group) - 1;
    current_nodegroup.node_group[node_pos].node_id = node_pos;
    return node_pos;
}
void node_group_update_id(int node_id, int group_id, char *name, char *description, char *head, int priority,  char *real_code, char *tail) {
    codeBox_list[group_id].node_group[node_id].priority = priority;

    string_set(&codeBox_list[group_id].node_group[node_id].name, name);
    string_set(&codeBox_list[group_id].node_group[node_id].description, description);
    string_set(&codeBox_list[group_id].node_group[node_id].head, head);
    string_set(&codeBox_list[group_id].node_group[node_id].tail, tail);
    string_set(&codeBox_list[group_id].node_group[node_id].real_code, real_code);

}
void init_nodes() {
    int* temp_contained_array;

    temp_contained_array = dynarray_create(int);
// node zones define zones on a node that get activated upon clicking. Note
// that some zones change size depending on context and so must be modified on the fly;

    printf("init nodes\n");

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
        0
    );
    dynarray_destroy(temp_contained_array);
}
void containerRemoveNode(int containing_node, int  inner_node) {
    unsigned int i = 0;
    unsigned int j = 0;

    int temp_node;

    unsigned int array_length = dynarray_length(current_nodegroup.node_group[containing_node].contained);

    while(i < array_length) {
        if(current_nodegroup.node_group[containing_node].contained[i] == inner_node) {
            // we splice the node
            j = i;
            while(j < array_length) {
                current_nodegroup.node_group[containing_node].contained[j] = current_nodegroup.node_group[containing_node].contained[j + 1];
                j++;
            }
            if(array_length > 0) {
                //  node_group[containing_node].contained.length --; //reduce array length
                dynarray_pop(current_nodegroup.node_group[containing_node].contained, &temp_node);
            }
        }
        i++;
    }
    current_nodegroup.node_group[inner_node].cont_by = current_nodegroup.topNode;

    deactivate_quit();
}

void string_init(char** string_ptr) {
    char *ptr = malloc(16);
    // Dereference our pointer and set its address to the new contiguous block of memory
    *string_ptr = ptr;
}
void string_free(char** string_ptr) {

    free(*string_ptr);
}
void string_set(char** destination, char* value) {

    int new_size = strlen(value);

    // Add 1 to account for '\0' null terminator
    *destination = realloc(*destination, sizeof(char)*new_size + 1);

    if(*destination == NULL) {
        printf("Unable to realloc() ptr!");
    } else {
        strcpy(*destination, value);
    }
}
void string_add(char** destination, char* value) {
    int new_size = strlen(*destination) + strlen(value);
    // Add 1 to account for '\0' null terminator
    *destination = realloc(*destination, sizeof(char)*new_size + 1);

    if(*destination == NULL) {
        printf("Unable to realloc() ptr!");
    } else {
        strcat(*destination, value);
    }
}
void init_arrays() {
    node_zones.length = 0;
    a_node_group_context temp_group_list;

    string_init(&output_text);
    rendered_data = dynarray_create(int);
    rendered_counts = dynarray_create(int);
    node_translation = dynarray_create(a_translate_vector);

    codeBox_list = dynarray_create(a_node_group_context);

    init_node_group_context(&temp_group_list);
    dynarray_push(codeBox_list, temp_group_list);

}
void init_node_group_context(a_node_group_context *temp_group_list) {
    string_init(&(temp_group_list->name));
    string_init(&(temp_group_list->destination));
    string_init(&(temp_group_list->fileName));
    temp_group_list->zoomScaleFactor = 1.0;
    temp_group_list->topNode = -1;
    temp_group_list->last_node_read = -1;
    temp_group_list->node_group_altered = false;
    temp_group_list->fileSaveAsMode = true;

    string_set(&(temp_group_list->destination), default_destination);
    string_set(&(temp_group_list->name), "new_file");
    string_set(&(temp_group_list->fileName), "");

    temp_group_list->node_group = dynarray_create(a_node);
}
void load_codeBox(char *nete_string) {

    cJSON *incoming_node_group = NULL;
    cJSON *code_node = NULL;
    cJSON *contained_member;
    int* temp_contained;

    cJSON *nete_json;

    int real_priority;
    unsigned int i;

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

    cJSON *destination;
    cJSON *zoomScaleFactor;
    cJSON *topNode;

    nete_json = cJSON_Parse(nete_string);

    zoomScaleFactor = cJSON_GetObjectItem(nete_json, "zoomScaleFactor");
    topNode = cJSON_GetObjectItem(nete_json, "topNode");
    destination = cJSON_GetObjectItem(nete_json, "destination");

    current_nodegroup.zoomScaleFactor = zoomScaleFactor->valuedouble;
    current_nodegroup.topNode = topNode->valueint;
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

        if (!cJSON_IsNumber(priority)) {
            real_priority = atoi(priority->valuestring);

        } else {
            real_priority = priority->valueint;
        }

        node_group_push_id(
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
            0
        );

        dynarray_destroy(temp_contained);
    }
    for(i = 0; i < dynarray_length(current_nodegroup.node_group); i++) {
        check_obscured(i);
    }

    cJSON_Delete(nete_json);
}
char* stringify_nodes(void) {
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

    json_add_string(name, current_nodegroup.name, output_code, end);
    json_add_string(fileName, "", output_code, end);
    json_add_string(destination, current_nodegroup.destination, output_code, end);
    json_add_number(topNode, current_nodegroup.topNode, output_code, end);
    json_add_number(zoomScaleFactor, current_nodegroup.zoomScaleFactor, output_code, end);

    output_node_group = cJSON_CreateArray();
    if (output_node_group == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(output_code, "node_group", output_node_group);

    for (index = 0; index < (dynarray_length(current_nodegroup.node_group)); ++index)
    {
        codebox = cJSON_CreateObject();
        if (codebox == NULL)
        {
            goto end;
        }
        cJSON_AddItemToArray(output_node_group, codebox);

        json_add_number(x, current_nodegroup.node_group[index].x, codebox, end);
        json_add_number(y, current_nodegroup.node_group[index].y, codebox, end);
        json_add_number(width, current_nodegroup.node_group[index].width, codebox, end);
        json_add_number(height, current_nodegroup.node_group[index].height, codebox, end);
        json_add_number(next, current_nodegroup.node_group[index].next, codebox, end);
        json_add_number(previous, current_nodegroup.node_group[index].previous, codebox, end);
        json_add_number(cont_head, current_nodegroup.node_group[index].cont_head, codebox, end);
        json_add_number(cont_by, current_nodegroup.node_group[index].cont_by, codebox, end);

        json_add_array(contained, codebox, end);

        for(i = 0; i < dynarray_length(current_nodegroup.node_group[index].contained); i++) {
            json_add_number_arr(contained_item, current_nodegroup.node_group[index].contained[i], contained, end);
        }

        json_add_number(priority, current_nodegroup.node_group[index].priority, codebox, end);
        json_add_string(name, current_nodegroup.node_group[index].name, codebox, end);
        json_add_string(description, current_nodegroup.node_group[index].description, codebox, end);
        json_add_string(head, current_nodegroup.node_group[index].head, codebox, end);
        json_add_string(tail, current_nodegroup.node_group[index].tail, codebox, end);
        json_add_string(real_code, current_nodegroup.node_group[index].real_code, codebox, end);
        json_add_number(node_id, current_nodegroup.node_group[index].node_id, codebox, end);

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
void write_to_file(app_widgets *app_wdgts) {
    gchar *file_name = NULL;
    char *output_filename;
    char *last_slash;
    char *tab_name;

    char feedback_text[30];

    char *string_nodegroup;

    FILE *fp;
    bool write_to_file = false;

    string_init(&output_filename);

    if (strlen(current_nodegroup.fileName) > 5 && !(current_nodegroup.fileSaveAsMode)) {
        string_set(&output_filename, current_nodegroup.fileName);
        write_to_file = true;

    } else {
        gtk_widget_show(app_wdgts->w_dlg_file_save);

        if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_save)) == GTK_RESPONSE_OK) {

            file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save));

            string_set(&output_filename, file_name);

            if(strncmp(".nete", (file_name + strlen(file_name) - 5), 5) == 0 || strncmp(".json", (file_name + strlen(file_name) - 5), 5) == 0) {
                // printf("nete file\n");
            } else {
                string_add(&output_filename, ".nete");
            }

// ====== set up the tab's name =========================
            last_slash = strrchr(file_name, '/');
            string_init(&tab_name);
            string_set(&tab_name, last_slash + 1);

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
        fp = fopen(output_filename, "w");

        string_nodegroup = stringify_nodes();

        fputs(string_nodegroup, fp);
        fclose(fp);

        current_nodegroup.node_group_altered = false;
        current_nodegroup.fileSaveAsMode = false;
        free(string_nodegroup);
        string_free(&output_filename);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(feedback_text, "file saved: %d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        write_feedback(app_wdgts, feedback_text);

        current_nodegroup.node_group_altered = false;
        mark_unsaved(app_wdgts);
    }
}
void write_feedback(app_widgets *app_wdgts, char *text) {
    gtk_label_set_text (app_wdgts->l_lbl_main_feedback, text);
}
void create_new_tab(app_widgets *app_wdgts) {
    GtkWidget	*tab_label;
    GtkWidget	*a_box1;

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

    current_nodegroup.codebox_container = a_box1;

    g_object_ref(app_wdgts->l_lbl_codebox_focus);
    g_object_ref(app_wdgts->w_darea_scroll);
    gtk_container_remove (codeBox_list[old_nodegroup_id].codebox_container, app_wdgts->w_darea_scroll);
    gtk_container_remove (codeBox_list[old_nodegroup_id].codebox_container, app_wdgts->l_lbl_codebox_focus);
    gtk_container_add (current_nodegroup.codebox_container, app_wdgts->l_lbl_codebox_focus);
    gtk_container_add (current_nodegroup.codebox_container, app_wdgts->w_darea_scroll);
    g_object_unref(app_wdgts->w_darea_scroll);
    g_object_unref(app_wdgts->l_lbl_codebox_focus);
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

    line_number = atoi(line_number_string);

    if(line_number == 0) {
        write_feedback(app_wdgts, "please input a number");
    } else {
        renderLineData();
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
            current_nodegroup.last_node_read = node_with_line;
            launchBox(node_with_line, first_line, app_wdgts);
        }
    }
}
void deactivate_quit() {
    current_nodegroup.node_group_altered = true;
}
RGB colorConverter(int hexValue) {
    RGB rgbColor;
    rgbColor.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
    rgbColor.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
    rgbColor.b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte

    return rgbColor;
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
                                     gtk_notebook_get_nth_page (app_wdgts->t_ntb_nete_tabs, current_codeBox), tab_name);
    string_free(&tab_name);
}

