
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "algorithms.h"

static GtkWidget *output_view;

static void show_output(const char *message)
{
    GtkTextBuffer *buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));

    gtk_text_buffer_set_text(buffer, message, -1);
}

static void show_system(GtkWidget *widget, gpointer data)
{
    (void)widget;
    (void)data;

    char text[8192] = "";
    char line[256];

    calculate_need();

    g_strlcat(text, "DEADLOCK SIMULATION SYSTEM\n", sizeof(text));
    g_strlcat(text, "==========================\n\n", sizeof(text));

    g_strlcat(text, "AVAILABLE RESOURCES:\n", sizeof(text));

    for (int j = 0; j < resource_count; j++) {
        snprintf(line, sizeof(line), "%c: %d\n",
                 'A' + j, available[j]);
        g_strlcat(text, line, sizeof(text));
    }

    g_strlcat(text, "\nALLOCATION MATRIX:\n", sizeof(text));

    for (int i = 0; i < process_count; i++) {
        snprintf(line, sizeof(line), "P%d: ", i);
        g_strlcat(text, line, sizeof(text));

        for (int j = 0; j < resource_count; j++) {
            snprintf(line, sizeof(line), "%d ", allocation[i][j]);
            g_strlcat(text, line, sizeof(text));
        }

        g_strlcat(text, "\n", sizeof(text));
    }

    g_strlcat(text, "\nMAXIMUM MATRIX:\n", sizeof(text));

    for (int i = 0; i < process_count; i++) {
        snprintf(line, sizeof(line), "P%d: ", i);
        g_strlcat(text, line, sizeof(text));

        for (int j = 0; j < resource_count; j++) {
            snprintf(line, sizeof(line), "%d ", maximum[i][j]);
            g_strlcat(text, line, sizeof(text));
        }

        g_strlcat(text, "\n", sizeof(text));
    }

    g_strlcat(text, "\nNEED MATRIX:\n", sizeof(text));

    for (int i = 0; i < process_count; i++) {
        snprintf(line, sizeof(line), "P%d: ", i);
        g_strlcat(text, line, sizeof(text));

        for (int j = 0; j < resource_count; j++) {
            snprintf(line, sizeof(line), "%d ", need[i][j]);
            g_strlcat(text, line, sizeof(text));
        }

        g_strlcat(text, "\n", sizeof(text));
    }

    show_output(text);
}

static void run_banker(GtkWidget *widget, gpointer data)
{
    (void)widget;
    (void)data;

    int sequence[MAX_PROCESSES];
    char text[2048] = "";
    char line[128];

    if (banker_algorithm(sequence)) {
        g_strlcat(text, "SYSTEM STATUS: SAFE\n\n",
                  sizeof(text));
        g_strlcat(text, "Safe sequence:\n",
                  sizeof(text));

        for (int i = 0; i < process_count; i++) {
            snprintf(line, sizeof(line), "P%d%s",
                     sequence[i],
                     i == process_count - 1 ? "\n" : " -> ");
            g_strlcat(text, line, sizeof(text));
        }
    } else {
        g_strlcat(text,
                  "SYSTEM STATUS: UNSAFE\n"
                  "No complete safe sequence exists.\n",
                  sizeof(text));
    }

    show_output(text);
}

static void detect_system_deadlock(GtkWidget *widget, gpointer data)
{
    (void)widget;
    (void)data;

    int deadlocked[MAX_PROCESSES];
    int count = detect_deadlock(deadlocked);
    char text[1024] = "";
    char line[128];

    if (count == 0) {
        g_strlcat(text,
                  "DEADLOCK DETECTION RESULT\n\n"
                  "No deadlocked processes detected.\n",
                  sizeof(text));
    } else {
        g_strlcat(text,
                  "DEADLOCK DETECTION RESULT\n\n"
                  "Processes requiring attention:\n",
                  sizeof(text));

        for (int i = 0; i < count; i++) {
            snprintf(line, sizeof(line), "P%d\n", deadlocked[i]);
            g_strlcat(text, line, sizeof(text));
        }
    }

    show_output(text);
}

static void activate(GtkApplication *app, gpointer data)
{
    (void)data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),
                         "Deadlock Simulation System");
    gtk_window_set_default_size(GTK_WINDOW(window), 720, 540);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 18);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span size='x-large' weight='bold'>"
        "Deadlock Simulation System</span>");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 4);

    GtkWidget *subtitle = gtk_label_new(
        "Operating Systems Lab | Banker’s Algorithm and Deadlock Detection");
    gtk_label_set_line_wrap(GTK_LABEL(subtitle), TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), subtitle, FALSE, FALSE, 2);

    GtkWidget *button_box =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

    GtkWidget *system_button =
        gtk_button_new_with_label("View System Matrices");
    GtkWidget *banker_button =
        gtk_button_new_with_label("Run Banker’s Algorithm");
    GtkWidget *detect_button =
        gtk_button_new_with_label("Detect Deadlock");

    gtk_box_pack_start(GTK_BOX(button_box), system_button,
                       TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), banker_button,
                       TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), detect_button,
                       TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), button_box,
                       FALSE, FALSE, 4);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);

    output_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(output_view),
                                GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(output_view), TRUE);

    gtk_container_add(GTK_CONTAINER(scroll), output_view);
    gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);

    g_signal_connect(system_button, "clicked",
                     G_CALLBACK(show_system), NULL);
    g_signal_connect(banker_button, "clicked",
                     G_CALLBACK(run_banker), NULL);
    g_signal_connect(detect_button, "clicked",
                     G_CALLBACK(detect_system_deadlock), NULL);

    show_system(NULL, NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new(
        "org.example.deadlocksimulator",
        G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
